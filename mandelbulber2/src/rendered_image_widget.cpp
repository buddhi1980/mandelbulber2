/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * RenderedImage class - extension for QWidget class. Widget prepared for displaying rendered image
 * and 3D cursor
 */

#include "rendered_image_widget.hpp"

#include <QApplication>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QVariant>
#include <QtCore>
#include <QtGui>

#include "animation_path_data.hpp"
#include "camera_movement_modes.h"
#include "cimage.hpp"
#include "common_math.h"
#include "fractparams.hpp"
#include "nine_fractals.hpp"
#include "parameters.hpp"
#include "primitives.h"
#include "trace_behind.h"

using namespace Qt;

RenderedImage::RenderedImage(QWidget *parent) : QWidget(parent)
{
	// makes RenderedImage focusable to catch keyboard events
	setFocusPolicy(Qt::StrongFocus);
	setMouseTracking(true);

	image = nullptr;
	params = nullptr;
	fractals = nullptr;
	cursorVisible = true;
	smoothLastZMouse = 0.0;
	redrawed = true;
	isFocus = false;
	isOnObject = false;
	lastDepth = 0.0;
	frontDist = 0.0;
	flightRotationDirection = 0;
	clickMode = clickDoNothing;
	anaglyphMode = false;
	gridType = gridTypeCrosshair;
	placeLightBehind = false;

	QList<QVariant> mode;
	mode.append(int(RenderedImage::clickDoNothing));
	clickModeData = mode;
	cameraMovementMode = cameraMovementEnums::fixedDistance;

	// timer to refresh image
	timerRefreshImage = new QTimer(this);
	timerRefreshImage->setInterval(40);
	this->connect(timerRefreshImage, SIGNAL(timeout()), this, SLOT(update()));
}

void RenderedImage::paintEvent(QPaintEvent *event)
{
	(void)event;

	if (image)
	{
		CVector2<int> point = lastMousePosition / image->GetPreviewScale();
		double z;
		if (point.x >= 0 && point.y >= 0 && point.x < image->GetWidth() && point.y < image->GetHeight())
			z = image->GetPixelZBuffer(point.x, point.y);
		else
			z = float(1e20);

		if (params)
		{
			if ((cursorVisible && isFocus) || gridType != gridTypeCrosshair)
			{
				if (!anaglyphMode) DisplayCrosshair();
			}

			if (cursorVisible && isFocus)
			{
				if (z < 1e10 || enumClickMode(clickModeData.at(0).toInt()) == clickFlightSpeedControl)
				{
					redrawed = false;
					if (!isOnObject)
					{
						QApplication::setOverrideCursor(Qt::BlankCursor);
					}
					isOnObject = true;

					Display3DCursor(lastMousePosition, z);
				}
				else
				{
					if (isOnObject)
					{
						QApplication::restoreOverrideCursor();
					}
					isOnObject = false;
				}
			}
		}

		if (params && animationPathData.animationPath.length() > 0)
		{
			DrawAnimationPath();
		}

		image->RedrawInWidget();

		if (params)
		{
			if (cursorVisible && isFocus && !anaglyphMode
					&& (isOnObject || enumClickMode(clickModeData.at(0).toInt()) == clickFlightSpeedControl))
			{
				DisplayCoordinates();
			}
		}

		if (params && cursorVisible && clickMode != clickFlightSpeedControl)
		{
			CVector3 rotation = params->Get<CVector3>("camera_rotation") / 180.0 * M_PI;
			Compass(rotation, QPointF(image->GetPreviewWidth() * 0.9, image->GetPreviewHeight() * 0.9),
				image->GetPreviewHeight() * 0.05);
		}

		if (params)
		{
			if (clickMode == clickFlightSpeedControl)
			{
				Compass(flightData.rotation,
					QPointF(image->GetPreviewWidth() * 0.5, image->GetPreviewHeight() * 0.5),
					image->GetPreviewHeight() * 0.2);
			}
		}

		redrawed = true;
	}
	else
	{
		qCritical() << "RenderedImage::mouseMoveEvent(QMouseEvent * event): image not assigned";
	}
}

void RenderedImage::DisplayCoordinates()
{
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	QPen penWhite(Qt::white, 1, Qt::SolidLine);
	QBrush brushBrown(QColor(100, 50, 0));
	QBrush brushDarkBlue(QColor(0, 0, 100));

	QString text;
	enumClickMode clickMode = enumClickMode(clickModeData.at(0).toInt());
	switch (clickMode)
	{
		case clickMoveCamera:
		{
			switch (cameraMovementEnums::enumCameraMovementMode(cameraMovementMode))
			{
				case cameraMovementEnums::fixedDistance: text = tr("Move camera and target"); break;
				case cameraMovementEnums::moveCamera: text = tr("Move camera"); break;
				case cameraMovementEnums::moveTarget: text = tr("Move target"); break;
			}
			break;
		}
		case clickFogVisibility: text = tr("Change fog visibility"); break;
		case clickDOFFocus: text = tr("Change DOF focus"); break;
		case clickPlaceLight:
			text = tr("Place light #") + QString::number(clickModeData.at(1).toInt());
			text += tr("\nMouse wheel - light fov / bkw");
			break;
		case clickPlacePrimitive:
			text = tr("Place ") + PrimitiveNames(fractal::enumObjectType(clickModeData.at(1).toInt()))
						 + QString(" #") + QString::number(clickModeData.at(2).toInt());
			break;
		case clickGetJuliaConstant: text = tr("Get Julia constant"); break;
		case clickFlightSpeedControl:
			text = tr("LMB - increase speed");
			text += tr("\nRMB - decrease speed");
			text += tr("\narrow keys - strafe");
			text += tr("\nz, x keys - roll");
			text += tr("\nspacebar - pause");
			text += tr("\nhold shift key - orthogonal strafe");
			break;
		case clickDoNothing: text = ""; break;
		case clickPlaceRandomLightCenter:
			text = tr("Place center of random light");
			text += tr("\nalso calculates");
			text += tr("\ndistribution radius of lights to 50%,");
			text += tr("\nmax distance from fractal to 10%");
			text += tr("\nof distance [center to camera position]");
			break;
		case clickGetPoint:
			text = tr("Get coordinates");
			text += tr("\nand distance");
			break;
	}

	if (clickMode != clickDoNothing)
	{
		QRect textRect = painter.boundingRect(QRect(), Qt::AlignTop | Qt::AlignLeft, text);
		textRect.setHeight(textRect.height() + 2);
		textRect.moveBottomLeft(QPoint(lastMousePosition.x + 30, lastMousePosition.y - 3));

		painter.setOpacity(0.8);
		painter.setPen(penWhite);
		painter.setBrush(brushBrown);
		painter.drawRoundedRect(textRect, 3, 3);
		painter.drawText(textRect, Qt::AlignTop | Qt::AlignLeft, text);
	}

	QString textCoordinates;
	if (clickMode != clickFlightSpeedControl)
	{
		textCoordinates += "x: " + QString::number(lastCoordinates.x, 'g', 15);
		textCoordinates += "\ny: " + QString::number(lastCoordinates.y, 'g', 15);
		textCoordinates += "\nz: " + QString::number(lastCoordinates.z, 'g', 15);
		textCoordinates += "\ndist: " + QString::number(lastDepth, 'g', 15);
	}
	else
	{
		textCoordinates += "frame: " + QString::number(flightData.frame);
		textCoordinates += "\nx: " + QString::number(flightData.camera.x, 'g', 15);
		textCoordinates += "\ny: " + QString::number(flightData.camera.y, 'g', 15);
		textCoordinates += "\nz: " + QString::number(flightData.camera.y, 'z', 15);
		textCoordinates += "\ndist: " + QString::number(flightData.distance);
		textCoordinates += "\nspeed act: " + QString::number(flightData.speed);
		textCoordinates += "\nspeed set: " + QString::number(flightData.speedSp);
	}

	QRect textRect2 = painter.boundingRect(QRect(), Qt::AlignTop | Qt::AlignLeft, textCoordinates);
	textRect2.setHeight(textRect2.height() + 2);
	textRect2.moveTopLeft(QPoint(lastMousePosition.x + 30, lastMousePosition.y + 3));
	painter.setOpacity(0.8);
	painter.setPen(penWhite);
	painter.setBrush(brushDarkBlue);
	painter.drawRoundedRect(textRect2, 3, 3);
	painter.drawText(textRect2, Qt::AlignTop | Qt::AlignLeft, textCoordinates);
}

void RenderedImage::Display3DCursor(CVector2<int> screenPoint, double z)
{
	clickMode = enumClickMode(clickModeData.at(0).toInt());
	if (clickMode == clickPlaceLight)
	{
		if (!placeLightBehind)
		{
			z -= frontDist;
		}
	}

	double diff = z - smoothLastZMouse;
	if (fabs(diff) >= 1.0)
	{
		smoothLastZMouse += diff * 0.01;
	}
	else
	{
		double delta = sqrt(fabs(diff)) * 0.01;
		smoothLastZMouse += (diff > 0 ? 1.0 : -1.0) * fmin(delta, fabs(diff));
	}

	if (z > 0 && clickMode != clickFlightSpeedControl)
	{
		if (smoothLastZMouse < 0.0) smoothLastZMouse = 0.0;

		bool legacyCoordinateSystem = params->Get<bool>("legacy_coordinate_system");
		double reverse = legacyCoordinateSystem ? -1.0 : 1.0;

		// preparing rotation matrix
		CVector3 rotation = params->Get<CVector3>("camera_rotation") / 180.0 * M_PI;
		double sweetSpotHAngle = params->Get<double>("sweet_spot_horizontal_angle") / 180.0 * M_PI;
		double sweetSpotVAngle = params->Get<double>("sweet_spot_vertical_angle") / 180.0 * M_PI;

		bool stereoEnabled = params->Get<bool>("stereo_enabled");
		cStereo::enumStereoMode stereoMode = cStereo::enumStereoMode(params->Get<int>("stereo_mode"));
		anaglyphMode = stereoMode == cStereo::stereoRedCyan && stereoEnabled;
		double stereoEyeDistance = params->Get<double>("stereo_eye_distance");
		double stereoInfiniteCorrection = params->Get<double>("stereo_infinite_correction");

		params::enumPerspectiveType perspType =
			params::enumPerspectiveType(params->Get<int>("perspective_type"));
		CVector3 camera = params->Get<CVector3>("camera");

		CRotationMatrix mRot;
		mRot.RotateZ(rotation.x);
		mRot.RotateX(rotation.y);
		mRot.RotateY(rotation.z);
		mRot.RotateZ(-sweetSpotHAngle);
		mRot.RotateX(sweetSpotVAngle);

		double fov = params->Get<double>("fov");

		double sw = image->GetPreviewWidth();
		double sh = image->GetPreviewHeight();
		double aspectRatio = sw / sh;

		if (perspType == params::perspEquirectangular) aspectRatio = 2.0;

		CVector2<double> p;
		p.x = (screenPoint.x / sw - 0.5) * aspectRatio;
		p.y = (screenPoint.y / sh - 0.5);

		double scale = smoothLastZMouse / z;

		// calculate 3D point coordinates
		CVector2<double> pTemp = p;
		pTemp.y *= -1.0 * reverse;
		CVector3 viewVector = CalculateViewVector(pTemp, fov, perspType, mRot);
		CVector3 point = camera + viewVector * z;

		if (clickMode == clickPlaceLight)
		{
			if (placeLightBehind)
			{
				double distanceBehind =
					traceBehindFractal(params, fractals, frontDist, viewVector, z, 1.0 / image->GetHeight());
				z += distanceBehind;
			}
		}

		lastCoordinates = point;

		if (anaglyphMode)
		{
			CVector2<double> p1, p2;
			p1.x = p.x;
			p1.y = p.y;
			Draw3DBox(scale, fov, p1, z, cStereo::eyeLeft);
			if (perspType == params::perspThreePoint)
			{
				p2.x = p.x - 2.0 * (stereoEyeDistance / z - stereoInfiniteCorrection / 10.0) / fov;
			}
			else
			{
				p2.x = p.x - 2.0 * (stereoEyeDistance / z - stereoInfiniteCorrection / 10.0) / fov / M_PI;
			}
			p2.y = p.y;
			Draw3DBox(scale, fov, p2, z, cStereo::eyeRight);
		}
		else
		{
			Draw3DBox(scale, fov, p, z, cStereo::eyeNone);
		}
	}
	else if (clickMode == clickFlightSpeedControl)
	{
		// draw small cross
		image->AntiAliasedLine(screenPoint.x - 20, screenPoint.y - 20, screenPoint.x + 20,
			screenPoint.y + 20, -1, -1, sRGB8(255, 255, 255), sRGBFloat(0.3f, 0.3f, 0.3f), 1);
		image->AntiAliasedLine(screenPoint.x + 20, screenPoint.y - 20, screenPoint.x - 20,
			screenPoint.y + 20, -1, -1, sRGB8(255, 255, 255), sRGBFloat(0.3f, 0.3f, 0.3f), 1);
	}
	lastDepth = z;
}

void RenderedImage::Draw3DBox(
	double scale, double fov, CVector2<double> p, double z, cStereo::enumEye eye) const
{
	double sw = image->GetPreviewWidth();
	double sh = image->GetPreviewHeight();

	double aspectRatio = sw / sh;

	double boxWidth = 10.0 / sw * scale;
	double boxHeight = 10.0 / sw * scale;
	double boxDepth = 10.0 / sw * scale;

	double boxDepth2 = boxHeight * z * fov;

	double n = 3.0;

	sRGBFloat opacity;
	switch (eye)
	{
		case cStereo::eyeNone: opacity = sRGBFloat(0.8f, 0.8f, 0.8f); break;
		case cStereo::eyeLeft: opacity = sRGBFloat(0.8f, 0.0f, 0.0f); break;
		case cStereo::eyeRight: opacity = sRGBFloat(0.0f, 0.8f, 0.8f); break;
	}

	unsigned char R, G, B;
	for (int iz = -n; iz <= n; iz++)
	{
		double yy1 = ((p.y + n * boxHeight) / (1.0 - boxDepth * iz * fov) + 0.5) * sh;
		double yy2 = ((p.y - n * boxHeight) / (1.0 - boxDepth * iz * fov) + 0.5) * sh;
		for (int ix = -n; ix <= n; ix++)
		{
			double xx1 = ((p.x + boxWidth * ix) / (1.0 - boxDepth * iz * fov) / aspectRatio + 0.5) * sw;
			if (eye == cStereo::eyeNone)
			{
				R = 128 + iz * 40;
				G = 128 - iz * 40;
				B = 0;
			}
			else
			{
				R = 128 + iz * 40;
				G = 128 + iz * 40;
				B = 128 + iz * 40;
			}
			if (iz == 0 && ix == 0)
			{
				R = G = B = 255;
				// opacity = 1.0;
			}
			image->AntiAliasedLine(
				xx1, yy1, xx1, yy2, z - iz * boxDepth2, z - iz * boxDepth2, sRGB8(R, G, B), opacity, 1);
		}

		double xx1 = ((p.x + n * boxWidth) / (1.0 - boxDepth * iz * fov) / aspectRatio + 0.5) * sw;
		double xx2 = ((p.x - n * boxWidth) / (1.0 - boxDepth * iz * fov) / aspectRatio + 0.5) * sw;
		for (int iy = -n; iy <= n; iy++)
		{
			double yyn1 = ((p.y + boxWidth * iy) / (1.0 - boxDepth * iz * fov) + 0.5) * sh;

			if (eye == cStereo::eyeNone)
			{
				R = 128 + iz * 40;
				G = 128 - iz * 40;
				B = 0;
			}
			else
			{
				R = 128 + iz * 40;
				G = 128 + iz * 40;
				B = 128 + iz * 40;
			}

			if (iz == 0 && iy == 0)
			{
				R = G = B = 255;
				// opacity = 1.0;
			}

			image->AntiAliasedLine(
				xx1, yyn1, xx2, yyn1, z - iz * boxDepth2, z - iz * boxDepth2, sRGB8(R, G, B), opacity, 1);
		}

		if (iz < n)
		{
			for (int ix = -n; ix <= n; ix++)
			{
				for (int iy = -n; iy <= n; iy++)
				{
					double xxn1 =
						((p.x + boxWidth * ix) / (1.0 - boxDepth * iz * fov) / aspectRatio + 0.5) * sw;
					double yyn1 = ((p.y + boxWidth * iy) / (1.0 - boxDepth * iz * fov) + 0.5) * sh;
					double xxn2 =
						((p.x + boxWidth * ix) / (1.0 - boxDepth * (iz + 1) * fov) / aspectRatio + 0.5) * sw;
					double yyn2 = ((p.y + boxWidth * iy) / (1.0 - boxDepth * (iz + 1) * fov) + 0.5) * sh;

					if (eye == cStereo::eyeNone)
					{
						R = 128 + iz * 40;
						G = 128 - iz * 40;
						B = 0;
					}
					else
					{
						R = 128 + iz * 40;
						G = 128 + iz * 40;
						B = 128 + iz * 40;
					}

					if (ix == 0 && iy == 0)
					{
						R = G = B = 255;
						// opacity = 1.0;
					}

					image->AntiAliasedLine(xxn1, yyn1, xxn2, yyn2, z - iz * boxDepth2,
						z - (iz + 1) * boxDepth2, sRGB8(R, G, B), opacity, 1);
				}
			}
		}
		if (iz == 0)
		{
			CVector2<double> sPoint((p.x / aspectRatio + 0.5) * sw, (p.y + 0.5) * sh);
			image->AntiAliasedLine(sPoint.x - sw * 0.3, sPoint.y, sPoint.x + sw * 0.3, sPoint.y, z, z,
				sRGB8(255, 255, 255), opacity, 1);
			image->AntiAliasedLine(sPoint.x, sPoint.y - sh * 0.3, sPoint.x, sPoint.y + sh * 0.3, z, z,
				sRGB8(255, 255, 255), opacity, 1);
			if (anaglyphMode)
			{
				image->AntiAliasedLine(sPoint.x - sw * 0.05, sPoint.y - sh * 0.05, sPoint.x + sw * 0.05,
					sPoint.y - sh * 0.05, z, z, sRGB8(0, 0, 0), opacity, 1);
				image->AntiAliasedLine(sPoint.x + sw * 0.05, sPoint.y - sh * 0.05, sPoint.x + sw * 0.05,
					sPoint.y + sh * 0.05, z, z, sRGB8(0, 0, 0), opacity, 1);
				image->AntiAliasedLine(sPoint.x + sw * 0.05, sPoint.y + sh * 0.05, sPoint.x - sw * 0.05,
					sPoint.y + sh * 0.05, z, z, sRGB8(0, 0, 0), opacity, 1);
				image->AntiAliasedLine(sPoint.x - sw * 0.05, sPoint.y + sh * 0.05, sPoint.x - sw * 0.05,
					sPoint.y - sh * 0.05, z, z, sRGB8(0, 0, 0), opacity, 1);
			}

			if (clickMode == clickPlaceLight)
			{
				double r = 1.5 * (boxWidth * n / aspectRatio);
				if (r > 1.0) r = 1.0;
				image->CircleBorder(
					sPoint.x, sPoint.y, z, r * sw, sRGB8(0, 100, 255), r * 0.1 * sw, opacity, 1);
			}
		}
	}
}

void RenderedImage::mouseMoveEvent(QMouseEvent *event)
{
	CVector2<int> screenPoint(event->x(), event->y());

	// remember last mouse position
	lastMousePosition = screenPoint;

	CVector2<double> yawAndPitch;
	yawAndPitch.x = (double(lastMousePosition.x) / image->GetPreviewWidth() - 0.5) * 2.0;
	yawAndPitch.y = (double(lastMousePosition.y) / image->GetPreviewHeight() - 0.5) * 2.0;
	emit YawAndPitchChanged(yawAndPitch);

	if (params)
	{
		if (cursorVisible && isFocus && redrawed)
		{
			update();
		}
	}
	else
	{
		if (cursorVisible)
			qCritical() << "RenderedImage::mouseMoveEvent(QMouseEvent * event): parameters not assigned";
	}

	emit mouseMoved(screenPoint.x, screenPoint.y);
}

void RenderedImage::mousePressEvent(QMouseEvent *event)
{
	if (enumClickMode(clickModeData.at(0).toInt()) == clickFlightSpeedControl)
	{
		if (event->button() == Qt::LeftButton)
		{
			emit SpeedChanged(1.1);
		}
		else if (event->button() == Qt::RightButton)
		{
			emit SpeedChanged(0.9);
		}
	}
	else
	{
		emit singleClick(event->x(), event->y(), event->button());
	}
}

void RenderedImage::mouseReleaseEvent(QMouseEvent *event)
{
	(void)event;
}

void RenderedImage::enterEvent(QEvent *event)
{
	(void)event;

	if (!isFocus)
	{
		setFocus();
		QApplication::setOverrideCursor(Qt::CrossCursor);
	}
	isFocus = true;
	timerRefreshImage->start();
}

void RenderedImage::leaveEvent(QEvent *event)
{
	(void)event;
	isFocus = false;
	isOnObject = false;
	update();
	timerRefreshImage->stop();
	QApplication::restoreOverrideCursor();
	QApplication::restoreOverrideCursor();
}

void RenderedImage::keyPressEvent(QKeyEvent *event)
{
	if (event->isAutoRepeat())
	{
		event->ignore();
	}
	else
	{
		if (enumClickMode(clickModeData.at(0).toInt()) == clickFlightSpeedControl)
		{
			Qt::Key key = Qt::Key(event->key());
			if (key == Qt::Key_Up)
			{
				keyArrows.y += 1;
				emit StrafeChanged(keyArrows);
			}
			else if (key == Qt::Key_Down)
			{
				keyArrows.y -= 1;
				emit StrafeChanged(keyArrows);
			}
			else if (key == Qt::Key_Left)
			{
				keyArrows.x -= 1;
				emit StrafeChanged(keyArrows);
			}
			else if (key == Qt::Key_Right)
			{
				keyArrows.x += 1;
				emit StrafeChanged(keyArrows);
			}
			else if (key == Qt::Key_Z)
			{
				flightRotationDirection = 1;
				emit RotationChanged(flightRotationDirection);
			}
			else if (key == Qt::Key_X)
			{
				flightRotationDirection = -1;
				emit RotationChanged(flightRotationDirection);
			}
			else if (key == Qt::Key_Space)
			{
				emit Pause();
			}
			else if (key == Qt::Key_Shift)
			{
				emit ShiftModeChanged(true);
			}
		}
		else
		{
			emit keyPress(event);
		}
	}
}

void RenderedImage::keyReleaseEvent(QKeyEvent *event)
{
	if (event->isAutoRepeat())
	{
		event->ignore();
	}
	else
	{
		if (enumClickMode(clickModeData.at(0).toInt()) == clickFlightSpeedControl)
		{
			Qt::Key key = Qt::Key(event->key());
			if (key == Qt::Key_Up)
			{
				keyArrows.y -= 1;
				emit StrafeChanged(keyArrows);
			}
			else if (key == Qt::Key_Down)
			{
				keyArrows.y += 1;
				emit StrafeChanged(keyArrows);
			}
			else if (key == Qt::Key_Left)
			{
				keyArrows.x += 1;
				emit StrafeChanged(keyArrows);
			}
			else if (key == Qt::Key_Right)
			{
				keyArrows.x -= 1;
				emit StrafeChanged(keyArrows);
			}
			else if (key == Qt::Key_Z)
			{
				flightRotationDirection = 0;
				emit RotationChanged(flightRotationDirection);
			}
			else if (key == Qt::Key_X)
			{
				flightRotationDirection = 0;
				emit RotationChanged(flightRotationDirection);
			}
			else if (key == Qt::Key_Shift)
			{
				emit ShiftModeChanged(false);
			}
		}
		else
		{
			emit keyRelease(event);
		}
	}
}

void RenderedImage::wheelEvent(QWheelEvent *event)
{
	emit mouseWheelRotated(event->delta());
	if (params)
	{
		if (cursorVisible && isFocus && redrawed)
		{
			update();
		}
	}
	else
	{
		if (cursorVisible)
			qCritical() << "RenderedImage::mouseMoveEvent(QMouseEvent * event): parameters not assigned";
	}
}

void RenderedImage::DisplayCrosshair() const
{
	// calculate crosshair center point according to sweet point

	double sweetSpotHAngle = params->Get<double>("sweet_spot_horizontal_angle") / 180.0 * M_PI;
	double sweetSpotVAngle = params->Get<double>("sweet_spot_vertical_angle") / 180.0 * M_PI;
	params::enumPerspectiveType perspType =
		params::enumPerspectiveType(params->Get<int>("perspective_type"));

	double fov = params->Get<double>("fov");

	double sw = image->GetPreviewWidth();
	double sh = image->GetPreviewHeight();

	double aspectRatio = sw / sh;

	CVector2<double> crossShift;

	switch (perspType)
	{
		case params::perspThreePoint:
			crossShift.y = tan(sweetSpotVAngle) / fov;
			crossShift.x = tan(-sweetSpotHAngle) / fov / cos(sweetSpotVAngle) / aspectRatio;
			break;

		case params::perspFishEye:
		case params::perspFishEyeCut:
		{
			CVector3 forward(0.0, 0.0, 1.0);
			forward = forward.RotateAroundVectorByAngle(CVector3(0.0, 1.0, 0.0), -sweetSpotHAngle);
			forward = forward.RotateAroundVectorByAngle(CVector3(1.0, 0.0, 0.0), -sweetSpotVAngle);
			forward.Normalize();
			double r = sqrt(forward.x * forward.x + forward.y * forward.y);
			if (r > 0)
			{
				double r2 = asin(r) / (M_PI * 0.5);
				crossShift.x = (forward.x / fov) * r2 / r / 2.0 / aspectRatio;
				crossShift.y = (forward.y / fov) * r2 / r / 2.0;
			}
			else
			{
				crossShift = CVector2<double>(0, 0);
			}
			break;
		}

		case params::perspEquirectangular:
		{
			CVector3 forward(0.0, 0.0, 1.0);
			forward = forward.RotateAroundVectorByAngle(CVector3(0.0, 1.0, 0.0), -sweetSpotHAngle);
			forward = forward.RotateAroundVectorByAngle(CVector3(1.0, 0.0, 0.0), -sweetSpotVAngle);
			crossShift.x = asin(forward.x / cos(asin(forward.y))) / M_PI / fov / aspectRatio;
			if (forward.z < 0 && crossShift.x > 0) crossShift.x = fov / aspectRatio - crossShift.x;
			if (forward.z < 0 && crossShift.x < 0) crossShift.x = -fov / aspectRatio - crossShift.x;
			crossShift.y = asin(forward.y) / M_PI / fov;
			break;
		}
	}

	CVector2<double> crossCenter;
	crossCenter.x = (sw * 0.5) * (1.0 + 2.0 * crossShift.x);
	crossCenter.y = (sh * 0.5) * (1.0 + 2.0 * crossShift.y);

	if (params->Get<bool>("stereo_enabled")
			&& params->Get<bool>("stereo_mode") == cStereo::stereoLeftRight)
	{
		image->AntiAliasedLine(crossCenter.x / 2, 0, crossCenter.x / 2, sh, -1, -1,
			sRGB8(255, 255, 255), sRGBFloat(0.3f, 0.3f, 0.3f), 1);
		image->AntiAliasedLine(crossCenter.x * 1.5, 0, crossCenter.x * 1.5, sh, -1, -1,
			sRGB8(255, 255, 255), sRGBFloat(0.3f, 0.3f, 0.3f), 1);
		image->AntiAliasedLine(0, crossCenter.y, sw, crossCenter.y, -1, -1, sRGB8(255, 255, 255),
			sRGBFloat(0.3f, 0.3f, 0.3f), 1);
	}
	else
	{
		switch (gridType)
		{
			case gridTypeCrosshair:
				image->AntiAliasedLine(crossCenter.x, 0, crossCenter.x, sh, -1, -1, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), 1);
				image->AntiAliasedLine(0, crossCenter.y, sw, crossCenter.y, -1, -1, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), 1);
				break;

			case gridTypeThirds:
				image->AntiAliasedLine(sw * 0.3333, 0, sw * 0.3333, sh, -1, -1, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), 1);
				image->AntiAliasedLine(sw * 0.6666, 0, sw * 0.6666, sh, -1, -1, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), 1);
				image->AntiAliasedLine(0, sh * 0.3333, sw, sh * 0.3333, -1, -1, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), 1);
				image->AntiAliasedLine(0, sh * 0.6666, sw, sh * 0.6666, -1, -1, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), 1);
				break;

			case gridTypeGolden:
				double goldenRatio = (1.0 + sqrt(5.0)) / 2.0;
				double ratio1 = goldenRatio - 1.0;
				double ratio2 = 1.0 - ratio1;
				image->AntiAliasedLine(sw * ratio1, 0, sw * ratio1, sh, -1, -1, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), 1);
				image->AntiAliasedLine(sw * ratio2, 0, sw * ratio2, sh, -1, -1, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), 1);
				image->AntiAliasedLine(0, sh * ratio1, sw, sh * ratio1, -1, -1, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), 1);
				image->AntiAliasedLine(0, sh * ratio2, sw, sh * ratio2, -1, -1, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), 1);
				break;
		}
	}
}

void RenderedImage::Compass(CVector3 rotation, QPointF center, float size)
{
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	QPen penRed(Qt::red, 1.0, Qt::SolidLine);
	QPen penGreen(Qt::green, 1.0, Qt::SolidLine);
	QPen penBlue(Qt::blue, 1.0, Qt::SolidLine);
	QPen penYellow(Qt::yellow, 2.0, Qt::SolidLine);
	QPen penMagenta(Qt::magenta, 2.0, Qt::SolidLine);
	QPen penCyan(Qt::cyan, 2.0, Qt::SolidLine);

	CRotationMatrix mRotInv;
	mRotInv.RotateY(-rotation.z);
	mRotInv.RotateX(-rotation.y);
	mRotInv.RotateZ(-rotation.x);

	// Draw circles
	const int steps = 40;
	const double persp = 0.5;
	CVector3 circlePoint1[steps];
	CVector3 circlePoint2[steps];
	CVector3 circlePoint3[steps];

	for (int i = 0; i < steps; i++)
	{
		double angle = i * 2.0 * M_PI / steps;

		circlePoint1[i].x = cos(angle);
		circlePoint1[i].y = sin(angle);
		circlePoint1[i].z = 0.0;
		circlePoint2[i].x = 0;
		circlePoint2[i].y = sin(angle);
		circlePoint2[i].z = cos(angle);
		circlePoint3[i].x = cos(angle);
		circlePoint3[i].y = 0.0;
		circlePoint3[i].z = sin(angle);
	}

	QPolygonF polygon1(steps);
	QPolygonF polygon2(steps);
	QPolygonF polygon3(steps);

	for (int i = 0; i < steps; i++)
	{
		polygon1[i] = CalcPointPersp(circlePoint1[i], mRotInv, persp) * size + center;
		polygon2[i] = CalcPointPersp(circlePoint2[i], mRotInv, persp) * size + center;
		polygon3[i] = CalcPointPersp(circlePoint3[i], mRotInv, persp) * size + center;
	}

	painter.setOpacity(0.5);

	painter.setPen(penRed);
	painter.drawPolyline(polygon1);
	painter.setPen(penGreen);
	painter.drawPolyline(polygon2);
	painter.setPen(penBlue);
	painter.drawPolyline(polygon3);

	// Draw arrows
	QPointF point1, point2;
	painter.setOpacity(0.5);
	QFont font = painter.font();
	font.setBold(true);
	painter.setFont(font);
	// X axis
	painter.setPen(penMagenta);
	point1 = CalcPointPersp(CVector3(1.0, 0.0, 0.0), mRotInv, persp) * size + center;
	point2 = CalcPointPersp(CVector3(-1.0, 0.0, 0.0), mRotInv, persp) * size + center;
	painter.drawLine(point1, point2);

	QStaticText textX("X");
	point1 = CalcPointPersp(CVector3(1.2, 0.0, 0.0), mRotInv, persp) * size + center;
	point2 =
		QPointF(point1.x() - textX.size().width() * 0.5, point1.y() - textX.size().height() * 0.5);
	painter.drawStaticText(point2, textX);

	point1 = CalcPointPersp(CVector3(0.9, -0.05, 0.0), mRotInv, persp) * size + center;
	point2 = CalcPointPersp(CVector3(1.0, 0.0, 0.0), mRotInv, persp) * size + center;
	painter.drawLine(point1, point2);
	point1 = CalcPointPersp(CVector3(0.9, 0.05, 0.0), mRotInv, persp) * size + center;
	point2 = CalcPointPersp(CVector3(1.0, 0.0, 0.0), mRotInv, persp) * size + center;
	painter.drawLine(point1, point2);
	point1 = CalcPointPersp(CVector3(0.9, 0.0, 0.05), mRotInv, persp) * size + center;
	point2 = CalcPointPersp(CVector3(1.0, 0.0, 0.0), mRotInv, persp) * size + center;
	painter.drawLine(point1, point2);
	point1 = CalcPointPersp(CVector3(0.9, 0.0, -0.05), mRotInv, persp) * size + center;
	point2 = CalcPointPersp(CVector3(1.0, 0.0, 0.0), mRotInv, persp) * size + center;
	painter.drawLine(point1, point2);

	// Z axis
	painter.setPen(penCyan);
	point1 = CalcPointPersp(CVector3(0.0, 0.0, 1.0), mRotInv, persp) * size + center;
	point2 = CalcPointPersp(CVector3(0.0, 0.0, -1.0), mRotInv, persp) * size + center;
	painter.drawLine(point1, point2);

	QStaticText textZ("Z");
	point1 = CalcPointPersp(CVector3(0.0, 0.0, 1.2), mRotInv, persp) * size + center;
	point2 =
		QPointF(point1.x() - textX.size().width() * 0.5, point1.y() - textZ.size().height() * 0.5);
	painter.drawStaticText(point2, textZ);

	point1 = CalcPointPersp(CVector3(0.05, 0.0, 0.9), mRotInv, persp) * size + center;
	point2 = CalcPointPersp(CVector3(0.0, 0.0, 1.0), mRotInv, persp) * size + center;
	painter.drawLine(point1, point2);
	point1 = CalcPointPersp(CVector3(-0.05, 0.0, 0.9), mRotInv, persp) * size + center;
	point2 = CalcPointPersp(CVector3(0.0, 0.0, 1.0), mRotInv, persp) * size + center;
	painter.drawLine(point1, point2);
	point1 = CalcPointPersp(CVector3(0.0, 0.05, 0.9), mRotInv, persp) * size + center;
	point2 = CalcPointPersp(CVector3(0.0, 0.0, 1.0), mRotInv, persp) * size + center;
	painter.drawLine(point1, point2);
	point1 = CalcPointPersp(CVector3(0.0, -0.05, 0.9), mRotInv, persp) * size + center;
	point2 = CalcPointPersp(CVector3(0.0, 0.0, 1.0), mRotInv, persp) * size + center;
	painter.drawLine(point1, point2);

	// Y axis
	painter.setPen(penYellow);
	point1 = CalcPointPersp(CVector3(0.0, 1.0, 0.0), mRotInv, persp) * size + center;
	point2 = CalcPointPersp(CVector3(0.0, -1.0, 0.0), mRotInv, persp) * size + center;
	painter.drawLine(point1, point2);

	QStaticText textY("Y");
	point1 = CalcPointPersp(CVector3(0.0, 1.2, 0.0), mRotInv, persp) * size + center;
	point2 =
		QPointF(point1.x() - textX.size().width() * 0.5, point1.y() - textY.size().height() * 0.5);
	painter.drawStaticText(point2, textY);

	point1 = CalcPointPersp(CVector3(0.05, 0.9, 0.0), mRotInv, persp) * size + center;
	point2 = CalcPointPersp(CVector3(0.0, 1.0, 0.0), mRotInv, persp) * size + center;
	painter.drawLine(point1, point2);
	point1 = CalcPointPersp(CVector3(-0.05, 0.9, 0.0), mRotInv, persp) * size + center;
	point2 = CalcPointPersp(CVector3(0.0, 1.0, 0.0), mRotInv, persp) * size + center;
	painter.drawLine(point1, point2);
	point1 = CalcPointPersp(CVector3(0.0, 0.9, 0.05), mRotInv, persp) * size + center;
	point2 = CalcPointPersp(CVector3(0.0, 1.0, 0.0), mRotInv, persp) * size + center;
	painter.drawLine(point1, point2);
	point1 = CalcPointPersp(CVector3(0.0, 0.9, -0.05), mRotInv, persp) * size + center;
	point2 = CalcPointPersp(CVector3(0.0, 1.0, 0.0), mRotInv, persp) * size + center;
	painter.drawLine(point1, point2);
}

QPointF RenderedImage::CalcPointPersp(
	const CVector3 &point, const CRotationMatrix &rot, double persp)
{
	CVector3 vect;

	vect = rot.RotateVector(point);
	QPointF out(vect.x / (1.0 + vect.y * persp), -vect.z / (1.0 + vect.y * persp));
	return out;
}

void RenderedImage::setClickMode(QList<QVariant> _clickMode)
{
	if (_clickMode.size() > 0)
		clickModeData = _clickMode;
	else
		qWarning() << "_clickMode cannot be empty!";
}

void RenderedImage::slotSetMinimumSize(int width, int height)
{
	setMinimumSize(width, height);
}

void RenderedImage::SetGridType(enumGridType _gridType)
{
	gridType = _gridType;
	update();
}

void RenderedImage::SetAnimationPath(const sAnimationPathData &_animationPath)
{
	animationPathData = _animationPath;
}

void RenderedImage::DrawAnimationPath()
{
	int numberOfKeyframes = animationPathData.numberOfKeyframes;
	int framesPerKey = animationPathData.framesPeyKey;
	int numberOfFrames = numberOfKeyframes * framesPerKey;

	CVector3 camera = params->Get<CVector3>("camera");
	CVector3 rotation = params->Get<CVector3>("camera_rotation");
	params::enumPerspectiveType perspectiveType =
		static_cast<params::enumPerspectiveType>(params->Get<int>("perspective_type"));
	double fov = params->Get<double>("fov");
	int width = image->GetPreviewWidth();
	int height = image->GetPreviewHeight();

	CRotationMatrix mRotInv;
	mRotInv.RotateY(-rotation.z / 180.0 * M_PI);
	mRotInv.RotateX(-rotation.y / 180.0 * M_PI);
	mRotInv.RotateZ(-rotation.x / 180.0 * M_PI);

	for (int f = 1; f < numberOfFrames; f++)
	{
		CVector3 pointTarget1, pointTarget2, pointCamera1, pointCamera2;

		double percent = double(f) / numberOfFrames;

		if (animationPathData.targetPathEnable)
		{
			CVector3 target1 = animationPathData.animationPath[f - 1].target;
			CVector3 target2 = animationPathData.animationPath[f].target;

			pointTarget1 = InvProjection3D(target1, camera, mRotInv, perspectiveType, fov, width, height);
			pointTarget2 = InvProjection3D(target2, camera, mRotInv, perspectiveType, fov, width, height);

			sRGB8 color(255 - percent * 128, 0, percent * 255);
			if (pointTarget1.z > 0)
			{
				if (f % framesPerKey == 0)
				{
					image->CircleBorder(pointTarget1.x, pointTarget1.y, pointTarget1.z, 5.0, color, 2.0,
						sRGBFloat(1.0, 1.0, 1.0), 1);
				}
			}
			if (pointTarget1.z > 0 && pointTarget2.z > 0)
			{
				image->AntiAliasedLine(pointTarget1.x, pointTarget1.y, pointTarget2.x, pointTarget2.y,
					pointTarget1.z, pointTarget2.z, color, sRGBFloat(1.0, 1.0, 1.0), 1);
			}
		}

		if (animationPathData.cameraPathEnable)
		{
			CVector3 camera1 = animationPathData.animationPath[f - 1].camera;
			CVector3 camera2 = animationPathData.animationPath[f].camera;
			pointCamera1 = InvProjection3D(camera1, camera, mRotInv, perspectiveType, fov, width, height);
			pointCamera2 = InvProjection3D(camera2, camera, mRotInv, perspectiveType, fov, width, height);

			sRGB8 color(0, 255 - percent * 128, percent * 255);
			if (pointCamera1.z > 0)
			{
				if (f % framesPerKey == 0)
				{
					image->CircleBorder(pointCamera1.x, pointCamera1.y, pointCamera1.z, 5.0, color, 2.0,
						sRGBFloat(1.0, 1.0, 1.0), 1);
				}
			}

			if (pointCamera1.z > 0 && pointCamera2.z > 0)
			{
				image->AntiAliasedLine(pointCamera1.x, pointCamera1.y, pointCamera2.x, pointCamera2.y,
					pointCamera1.z, pointCamera2.z, color, sRGBFloat(1.0, 1.0, 1.0), 1);
			}
		}

		if (animationPathData.cameraPathEnable && animationPathData.targetPathEnable)
		{
			if (pointCamera1.z > 0 && pointTarget1.z > 0)
			{
				if (f * 4 % framesPerKey == 0)
				{
					image->AntiAliasedLine(pointCamera1.x, pointCamera1.y, pointTarget1.x, pointTarget1.y,
						pointCamera1.z, pointTarget1.z, sRGB8(255, 255, 0), sRGBFloat(0.2f, 0.2f, 0.2f), 1);
				}
			}
		}

		// lights
		for (int i = 0; i < 4; i++)
		{
			if (animationPathData.lightPathEnable[i])
			{
				CVector3 target1 = animationPathData.animationPath[f - 1].lights[i];
				CVector3 target2 = animationPathData.animationPath[f].lights[i];

				pointTarget1 =
					InvProjection3D(target1, camera, mRotInv, perspectiveType, fov, width, height);
				pointTarget2 =
					InvProjection3D(target2, camera, mRotInv, perspectiveType, fov, width, height);

				sRGB8 color = animationPathData.animationPath[f - 1].lightColor[i];

				if (f * 4 % framesPerKey > framesPerKey / 2)
				{
					color.R = 255 - color.R;
					color.G = 255 - color.G;
					color.B = 255 - color.B;
				}
				if (pointTarget1.z > 0)
				{
					if (f % framesPerKey == 0)
					{
						image->CircleBorder(pointTarget1.x, pointTarget1.y, pointTarget1.z, 5.0, color, 2.0,
							sRGBFloat(1.0, 1.0, 1.0), 1);
					}
				}
				if (pointTarget1.z > 0 && pointTarget2.z > 0)
				{
					image->AntiAliasedLine(pointTarget1.x, pointTarget1.y, pointTarget2.x, pointTarget2.y,
						pointTarget1.z, pointTarget2.z, color, sRGBFloat(1.0, 1.0, 1.0), 1);
				}
			}
		}
	}
}
