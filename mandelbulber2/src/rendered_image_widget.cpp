/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
#include <QStaticText>
#include <QVariant>

#include "animation_path_data.hpp"
#include "camera_movement_modes.h"
#include "cimage.hpp"
#include "common_math.h"
#include "fractparams.hpp"
#include "light.h"
#include "nine_fractals.hpp"
#include "parameters.hpp"
#include "primitives.h"
#include "primitive.hpp"
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
	lightsVisible = false;
	primitivesVisible = false;
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
	clickModesEnables = true;
	draggingStarted = false;
	draggingInitStarted = false;
	buttonsPressed = 0;
	currentLightIndex = 1;

	QList<QVariant> mode;
	mode.append(int(RenderedImage::clickDoNothing));
	clickModeData = mode;
	cameraMovementMode = cameraMovementEnums::fixedDistance;

	// timer to refresh image
	timerRefreshImage = new QTimer(this);
	timerRefreshImage->setInterval(40);
	connect(timerRefreshImage, SIGNAL(timeout()), this, SLOT(update()));
}

void RenderedImage::paintEvent(QPaintEvent *event)
{
	(void)event;

	if (image)
	{
		CVector2<int> point = lastMousePosition / image->GetPreviewScale();
		float z;
		if (point.x >= 0 && point.y >= 0 && point.x < int(image->GetWidth())
				&& point.y < int(image->GetHeight()))
			z = image->GetPixelZBuffer(point.x, point.y);
		else
			z = float(1e20);

		if (params)
		{
			if ((cursorVisible && isFocus) || gridType != gridTypeCrosshair)
			{
				if (!anaglyphMode) DisplayCrosshair();
			}

			if (lightsVisible)
			{
				DisplayAllLights();
			}

			if (primitivesVisible)
			{
				DisplayAllPrimitives();
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
			double dpiScale = image->GetDpiScale();
			Compass(rotation,
				QPointF(
					image->GetPreviewWidth() * 0.9 / dpiScale, image->GetPreviewHeight() * 0.9 / dpiScale),
				image->GetPreviewHeight() * 0.05 / dpiScale);
		}

		if (params)
		{
			if (clickMode == clickFlightSpeedControl)
			{
				double dpiScale = image->GetDpiScale();
				Compass(flightData.rotation,
					QPointF(
						image->GetPreviewWidth() * 0.5 / dpiScale, image->GetPreviewHeight() * 0.5 / dpiScale),
					image->GetPreviewHeight() * 0.2 / dpiScale);
			}
		}

		PaintLastRenderedTilesInfo();

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
			text += tr("\nCtrl + Mouse wheel - light fwd/bkwd ");
			text += tr("\nAlt + Mouse wheel - placement fwd/bkwd ");
			break;
		case clickPlacePrimitive:
			text = tr("Place ")
						 + cPrimitives::PrimitiveNames(fractal::enumObjectType(clickModeData.at(1).toInt()))
						 + QString(" #") + QString::number(clickModeData.at(2).toInt());
			break;
		case clickGetJuliaConstant: text = tr("Get Julia constant"); break;
		case clickFlightSpeedControl:
			text = tr("LMB - increase speed");
			text += tr("\nRMB - decrease speed");
			text += tr("\narrow keys - sidewards");
			text += tr("\nz, x keys - roll");
			text += tr("\nspacebar - pause");
			text += tr("\nhold shift key - orthogonal move");
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
		case clickWrapLimitsAroundObject: text = tr("Wrap limits\naround object"); break;
	}

	if (clickMode != clickDoNothing)
	{
		QRect textRect = painter.boundingRect(QRect(), Qt::AlignTop | Qt::AlignLeft, text);
		textRect.setHeight(textRect.height() + 2);

		double dpiScale = image->GetDpiScale();

		textRect.moveBottomLeft(
			QPoint(lastMousePosition.x / dpiScale + 30, lastMousePosition.y / dpiScale - 3));

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

	double dpiScale = image->GetDpiScale();
	textRect2.moveTopLeft(
		QPoint(lastMousePosition.x / dpiScale + 30, lastMousePosition.y / dpiScale + 3));
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
		smoothLastZMouse += diff * 0.1;
	}
	else
	{
		double delta = sqrt(fabs(diff)) * 0.1;
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
		double distanceLimit = params->Get<double>("view_distance_max");

		params::enumPerspectiveType perspType =
			params::enumPerspectiveType(params->Get<int>("perspective_type"));
		CVector3 camera = params->Get<CVector3>("camera");

		CRotationMatrix mRot;
		mRot.RotateZ(rotation.x);
		mRot.RotateX(rotation.y);
		mRot.RotateY(rotation.z);
		mRot.RotateZ(-sweetSpotHAngle);
		mRot.RotateX(sweetSpotVAngle);

		double fov = CalcFOV(params->Get<double>("fov"), perspType);

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
				double distanceBehind = traceBehindFractal(
					params, fractals, frontDist, viewVector, z, 1.0 / image->GetHeight(), distanceLimit);
				z += distanceBehind;
			}
		}

		lastCoordinates = point;

		if (anaglyphMode)
		{
			CVector2<float> p1, p2;
			p1.x = p.x;
			p1.y = p.y;
			Draw3DBox(scale, fov, p1, z, cStereo::eyeLeft);
			if (perspType == params::perspThreePoint || perspType == params::perspFishEye
					|| perspType == params::perspFishEyeCut)
			{
				p2.x = p.x - 2.0 * (stereoEyeDistance / z - stereoInfiniteCorrection / 10.0) / fov;
			}
			else
			{
				p2.x = p.x - 2.0 * (stereoEyeDistance / z - stereoInfiniteCorrection / 10.0) / fov * 2.0;
			}
			p2.y = p.y;
			Draw3DBox(scale, fov, p2, z, cStereo::eyeRight);
		}
		else
		{
			Draw3DBox(scale, fov, CVector2<float>(p.x, p.y), z, cStereo::eyeNone);
		}
	}
	else if (clickMode == clickFlightSpeedControl)
	{
		// draw small cross
		image->AntiAliasedLine(screenPoint.x - 20, screenPoint.y - 20, screenPoint.x + 20,
			screenPoint.y + 20, -1, -1, sRGB8(255, 255, 255), sRGBFloat(0.3f, 0.3f, 0.3f), 1.0f, 1);
		image->AntiAliasedLine(screenPoint.x + 20, screenPoint.y - 20, screenPoint.x - 20,
			screenPoint.y + 20, -1, -1, sRGB8(255, 255, 255), sRGBFloat(0.3f, 0.3f, 0.3f), 1.0f, 1);
	}
	lastDepth = z;
}

void RenderedImage::Draw3DBox(
	float scale, float fov, CVector2<float> p, float z, cStereo::enumEye eye) const
{
	float sw = image->GetPreviewWidth();
	float sh = image->GetPreviewHeight();

	float aspectRatio = sw / sh;

	float boxWidth = 10.0f / sw * scale;
	float boxHeight = 10.0f / sw * scale;
	float boxDepth = 10.0f / sw * scale;

	float boxDepth2 = boxHeight * z * fov;

	float n = 3.0;
	int in = int(n);

	sRGBFloat opacity;
	switch (eye)
	{
		case cStereo::eyeNone: opacity = sRGBFloat(0.8f, 0.8f, 0.8f); break;
		case cStereo::eyeLeft: opacity = sRGBFloat(0.8f, 0.0f, 0.0f); break;
		case cStereo::eyeRight: opacity = sRGBFloat(0.0f, 0.8f, 0.8f); break;
	}

	unsigned char R, G, B;
	for (int iz = -in; iz <= in; iz++)
	{
		float yy1 = ((p.y + n * boxHeight) / (1.0f - boxDepth * iz * fov) + 0.5f) * sh;
		float yy2 = ((p.y - n * boxHeight) / (1.0f - boxDepth * iz * fov) + 0.5f) * sh;
		for (int ix = -in; ix <= in; ix++)
		{
			float xx1 = ((p.x + boxWidth * ix) / (1.0f - boxDepth * iz * fov) / aspectRatio + 0.5f) * sw;
			if (eye == cStereo::eyeNone)
			{
				R = uchar(128 + iz * 40);
				G = uchar(128 - iz * 40);
				B = 0;
			}
			else
			{
				R = uchar(128 + iz * 40);
				G = uchar(128 + iz * 40);
				B = uchar(128 + iz * 40);
			}
			if (iz == 0 && ix == 0)
			{
				R = G = B = 255;
				// opacity = 1.0;
			}
			image->AntiAliasedLine(xx1, yy1, xx1, yy2, z - iz * boxDepth2, z - iz * boxDepth2,
				sRGB8(R, G, B), opacity, 1.0f, 1);
		}

		float xx1 = ((p.x + n * boxWidth) / (1.0f - boxDepth * iz * fov) / aspectRatio + 0.5f) * sw;
		float xx2 = ((p.x - n * boxWidth) / (1.0f - boxDepth * iz * fov) / aspectRatio + 0.5f) * sw;
		for (int iy = -in; iy <= in; iy++)
		{
			float yyn1 = ((p.y + boxWidth * iy) / (1.0f - boxDepth * iz * fov) + 0.5f) * sh;

			if (eye == cStereo::eyeNone)
			{
				R = uchar(128 + iz * 40);
				G = uchar(128 - iz * 40);
				B = 0;
			}
			else
			{
				R = uchar(128 + iz * 40);
				G = uchar(128 + iz * 40);
				B = uchar(128 + iz * 40);
			}

			if (iz == 0 && iy == 0)
			{
				R = G = B = 255;
				// opacity = 1.0;
			}

			image->AntiAliasedLine(xx1, yyn1, xx2, yyn1, z - iz * boxDepth2, z - iz * boxDepth2,
				sRGB8(R, G, B), opacity, 1.0f, 1);
		}

		if (iz < n)
		{
			for (int ix = -in; ix <= in; ix++)
			{
				for (int iy = -in; iy <= in; iy++)
				{
					float xxn1 =
						((p.x + boxWidth * ix) / (1.0f - boxDepth * iz * fov) / aspectRatio + 0.5f) * sw;
					float yyn1 = ((p.y + boxWidth * iy) / (1.0f - boxDepth * iz * fov) + 0.5f) * sh;
					float xxn2 =
						((p.x + boxWidth * ix) / (1.0f - boxDepth * (iz + 1) * fov) / aspectRatio + 0.5f) * sw;
					float yyn2 = ((p.y + boxWidth * iy) / (1.0f - boxDepth * (iz + 1) * fov) + 0.5f) * sh;

					if (eye == cStereo::eyeNone)
					{
						R = uchar(128 + iz * 40);
						G = uchar(128 - iz * 40);
						B = 0;
					}
					else
					{
						R = uchar(128 + iz * 40);
						G = uchar(128 + iz * 40);
						B = uchar(128 + iz * 40);
					}

					if (ix == 0 && iy == 0)
					{
						R = G = B = 255;
						// opacity = 1.0;
					}

					image->AntiAliasedLine(xxn1, yyn1, xxn2, yyn2, z - iz * boxDepth2,
						z - (iz + 1) * boxDepth2, sRGB8(R, G, B), opacity, 1.0f, 1);
				}
			}
		}
		if (iz == 0)
		{
			CVector2<float> sPoint((p.x / aspectRatio + 0.5f) * sw, (p.y + 0.5f) * sh);
			image->AntiAliasedLine(sPoint.x - sw * 0.3f, sPoint.y, sPoint.x + sw * 0.3f, sPoint.y, z, z,
				sRGB8(255, 255, 255), opacity, 1.0f, 1);
			image->AntiAliasedLine(sPoint.x, sPoint.y - sh * 0.3f, sPoint.x, sPoint.y + sh * 0.3f, z, z,
				sRGB8(255, 255, 255), opacity, 1.0f, 1);
			if (anaglyphMode)
			{
				image->AntiAliasedLine(sPoint.x - sw * 0.05f, sPoint.y - sh * 0.05f, sPoint.x + sw * 0.05f,
					sPoint.y - sh * 0.05f, z, z, sRGB8(0, 0, 0), opacity, 1.0f, 1);
				image->AntiAliasedLine(sPoint.x + sw * 0.05f, sPoint.y - sh * 0.05f, sPoint.x + sw * 0.05f,
					sPoint.y + sh * 0.05f, z, z, sRGB8(0, 0, 0), opacity, 1.0f, 1);
				image->AntiAliasedLine(sPoint.x + sw * 0.05f, sPoint.y + sh * 0.05f, sPoint.x - sw * 0.05f,
					sPoint.y + sh * 0.05f, z, z, sRGB8(0, 0, 0), opacity, 1.0f, 1);
				image->AntiAliasedLine(sPoint.x - sw * 0.05f, sPoint.y + sh * 0.05f, sPoint.x - sw * 0.05f,
					sPoint.y - sh * 0.05f, z, z, sRGB8(0, 0, 0), opacity, 1.0f, 1);
			}

			if (clickMode == clickPlaceLight)
			{
				float r = 1.5f * (boxWidth * n / aspectRatio);
				if (r > 1.0f) r = 1.0f;
				image->CircleBorder(
					sPoint.x, sPoint.y, z, r * sw, sRGB8(0, 100, 255), r * 0.1f * sw, opacity, 1);
			}
		}
	}
}

void RenderedImage::mouseMoveEvent(QMouseEvent *event)
{
	double dpiScale = image->GetDpiScale();

	CVector2<int> screenPoint(event->x() * dpiScale, event->y() * dpiScale);

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

	if (draggingInitStarted)
	{
		if (abs(screenPoint.x - dragStartPosition.x) > 1
				|| abs(screenPoint.y - dragStartPosition.y) > 1)
		{
			draggingInitStarted = false;
			draggingStarted = true;
			emit mouseDragStart(dragStartPosition.x, dragStartPosition.y, dragButtons);
		}
	}

	if (draggingStarted)
	{
		int dx = screenPoint.x - dragStartPosition.x;
		int dy = screenPoint.y - dragStartPosition.y;
		emit mouseDragDelta(dx, dy);
	}
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
		if (clickModesEnables)
		{
			draggingInitStarted = true;
			double dpiScale = image->GetDpiScale();
			dragStartPosition = CVector2<int>(event->x() * dpiScale, event->y() * dpiScale);
			dragButtons = event->buttons();
		}
	}
	buttonsPressed++;
}

void RenderedImage::mouseReleaseEvent(QMouseEvent *event)
{
	if (!draggingStarted && enumClickMode(clickModeData.at(0).toInt()) != clickFlightSpeedControl)
	{
		if (clickModesEnables)
		{
			double dpiScale = image->GetDpiScale();
			emit singleClick(event->x() * dpiScale, event->y() * dpiScale, event->button());
		}
	}

	if (buttonsPressed == 1)
	{
		draggingStarted = false;
		draggingInitStarted = false;
		emit mouseDragFinish();
	}
	buttonsPressed--;

	// in case if some release event was missed
	if (event->buttons() == Qt::NoButton)
	{
		buttonsPressed = 0;
		draggingStarted = false;
		draggingInitStarted = false;
	}
}

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
void RenderedImage::enterEvent(QEnterEvent *event)
#else
void RenderedImage::enterEvent(QEvent *event)
#endif
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
	event->ignore(); // pass pressed key event to parents
}

void RenderedImage::keyReleaseEvent(QKeyEvent *event)
{
	if (event->isAutoRepeat())
	{
		event->ignore();
		// emit keyPress(event);
		// emit keyRelease(event);
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
	event->ignore(); // pass pressed key event to parents
}

void RenderedImage::wheelEvent(QWheelEvent *event)
{
	event->accept();

	if (clickModesEnables || enumClickMode(clickModeData.at(0).toInt()) == clickFlightSpeedControl)
	{

		event->accept(); // do not propagate event to parent widgets - prevents from scrolling

		double dpiScale = image->GetDpiScale();

#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
		emit mouseWheelRotatedWithKey(
			event->x() * dpiScale, event->y() * dpiScale, event->delta(), event->modifiers());
#else
		emit mouseWheelRotatedWithKey(int(event->position().x() * dpiScale),
			int(event->position().y() * dpiScale), event->angleDelta().y() + event->angleDelta().x(),
			event->modifiers());
		// with alt key there is modified delta.x
#endif
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
				qCritical()
					<< "RenderedImage::mouseMoveEvent(QMouseEvent * event): parameters not assigned";
		}
	}
}

void RenderedImage::DisplayCrosshair() const
{
	// calculate crosshair center point according to sweet point

	double sweetSpotHAngle = params->Get<double>("sweet_spot_horizontal_angle") / 180.0 * M_PI;
	double sweetSpotVAngle = params->Get<double>("sweet_spot_vertical_angle") / 180.0 * M_PI;
	params::enumPerspectiveType perspType =
		params::enumPerspectiveType(params->Get<int>("perspective_type"));

	double fov = CalcFOV(params->Get<double>("fov"), perspType);

	float sw = image->GetPreviewWidth();
	float sh = image->GetPreviewHeight();

	double aspectRatio = sw / sh;

	CVector2<float> crossShift;

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
				double r2 = asin(r) * 2.;
				crossShift.x = (forward.x / fov) * r2 / r / 2.0 / aspectRatio;
				crossShift.y = (forward.y / fov) * r2 / r / 2.0;
			}
			else
			{
				crossShift = CVector2<float>(0, 0);
			}
			break;
		}

		case params::perspEquirectangular:
		{
			CVector3 forward(0.0, 0.0, 1.0);
			forward = forward.RotateAroundVectorByAngle(CVector3(0.0, 1.0, 0.0), -sweetSpotHAngle);
			forward = forward.RotateAroundVectorByAngle(CVector3(1.0, 0.0, 0.0), -sweetSpotVAngle);
			crossShift.x = asin(forward.x / cos(asin(forward.y))) / 0.5 / fov / aspectRatio;
			if (forward.z < 0 && crossShift.x > 0) crossShift.x = fov / aspectRatio - crossShift.x;
			if (forward.z < 0 && crossShift.x < 0) crossShift.x = -fov / aspectRatio - crossShift.x;
			crossShift.y = asin(forward.y) / 0.5 / fov;
			break;
		}
	}

	CVector2<float> crossCenter;
	crossCenter.x = (sw * 0.5f) * (1.0f + 2.0f * crossShift.x);
	crossCenter.y = (sh * 0.5f) * (1.0f + 2.0f * crossShift.y);

	double crossHairThickness = clamp(sw / 1000.0, 1.0, 10.0);
	int blackShift = crossHairThickness * 3;

	if (params->Get<bool>("stereo_enabled")
			&& params->Get<bool>("stereo_mode") == cStereo::stereoLeftRight)
	{
		image->AntiAliasedLine(crossCenter.x / 2, 0, crossCenter.x / 2, sh, 0, 0, sRGB8(255, 255, 255),
			sRGBFloat(0.3f, 0.3f, 0.3f), 1.0f, 1);
		image->AntiAliasedLine(crossCenter.x * 1.5f, 0, crossCenter.x * 1.5f, sh, 0, 0,
			sRGB8(255, 255, 255), sRGBFloat(0.3f, 0.3f, 0.3f), 1.0f, 1);
		image->AntiAliasedLine(0, crossCenter.y, sw, crossCenter.y, 0, 0, sRGB8(255, 255, 255),
			sRGBFloat(0.3f, 0.3f, 0.3f), 1.0f, 1);
	}
	else
	{
		switch (gridType)
		{
			case gridTypeCrosshair:
				image->AntiAliasedLine(crossCenter.x, 0, crossCenter.x, sh, 0, 0, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);
				image->AntiAliasedLine(0, crossCenter.y, sw, crossCenter.y, 0, 0, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);

				image->AntiAliasedLine(crossCenter.x + blackShift, crossCenter.y - sh / 10,
					crossCenter.x + blackShift, crossCenter.y - blackShift, 0, 0, sRGB8(0, 0, 0),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);
				image->AntiAliasedLine(crossCenter.x + blackShift, crossCenter.y + sh / 10,
					crossCenter.x + blackShift, crossCenter.y + blackShift, 0, 0, sRGB8(0, 0, 0),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);
				image->AntiAliasedLine(crossCenter.x - blackShift, crossCenter.y - sh / 10,
					crossCenter.x - blackShift, crossCenter.y - blackShift, 0, 0, sRGB8(0, 0, 0),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);
				image->AntiAliasedLine(crossCenter.x - blackShift, crossCenter.y + sh / 10,
					crossCenter.x - blackShift, crossCenter.y + blackShift, 0, 0, sRGB8(0, 0, 0),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);

				image->AntiAliasedLine(crossCenter.x - sw / 10, crossCenter.y + blackShift,
					crossCenter.x - blackShift, crossCenter.y + blackShift, 0, 0, sRGB8(0, 0, 0),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);
				image->AntiAliasedLine(crossCenter.x + sw / 10, crossCenter.y + blackShift,
					crossCenter.x + blackShift, crossCenter.y + blackShift, 0, 0, sRGB8(0, 0, 0),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);
				image->AntiAliasedLine(crossCenter.x - sw / 10, crossCenter.y - blackShift,
					crossCenter.x - blackShift, crossCenter.y - blackShift, 0, 0, sRGB8(0, 0, 0),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);
				image->AntiAliasedLine(crossCenter.x + sw / 10, crossCenter.y - blackShift,
					crossCenter.x + blackShift, crossCenter.y - blackShift, 0, 0, sRGB8(0, 0, 0),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);
				break;

			case gridTypeThirds:
				image->AntiAliasedLine(sw * 0.3333f, 0, sw * 0.3333f, sh, 0, 0, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);
				image->AntiAliasedLine(sw * 0.6666f, 0, sw * 0.6666f, sh, 0, 0, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);
				image->AntiAliasedLine(0, sh * 0.3333f, sw, sh * 0.3333f, 0, 0, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);
				image->AntiAliasedLine(0, sh * 0.6666f, sw, sh * 0.6666f, 0, 0, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);
				break;

			case gridTypeGolden:
				float goldenRatio = (1.0f + sqrtf(5.0f)) / 2.0f;
				float ratio1 = goldenRatio - 1.0f;
				float ratio2 = 1.0f - ratio1;
				image->AntiAliasedLine(sw * ratio1, 0, sw * ratio1, sh, 0, 0, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);
				image->AntiAliasedLine(sw * ratio2, 0, sw * ratio2, sh, 0, 0, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);
				image->AntiAliasedLine(0, sh * ratio1, sw, sh * ratio1, 0, 0, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);
				image->AntiAliasedLine(0, sh * ratio2, sw, sh * ratio2, 0, 0, sRGB8(255, 255, 255),
					sRGBFloat(0.3f, 0.3f, 0.3f), crossHairThickness, 1);
				break;
		}
	}
}

void RenderedImage::Compass(CVector3 rotation, QPointF center, double size)
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
		QPointF(point1.x() - textZ.size().width() * 0.5, point1.y() - textZ.size().height() * 0.5);
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
		QPointF(point1.x() - textY.size().width() * 0.5, point1.y() - textY.size().height() * 0.5);
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
	int numberOfFrames = animationPathData.numberOfFrames;

	CVector3 camera = params->Get<CVector3>("camera");
	CVector3 rotation = params->Get<CVector3>("camera_rotation");
	params::enumPerspectiveType perspectiveType =
		static_cast<params::enumPerspectiveType>(params->Get<int>("perspective_type"));
	double fov = CalcFOV(params->Get<double>("fov"), perspectiveType);
	int width = image->GetPreviewWidth();
	int height = image->GetPreviewHeight();

	CRotationMatrix mRotInv;
	mRotInv.RotateY(-rotation.z / 180.0 * M_PI);
	mRotInv.RotateX(-rotation.y / 180.0 * M_PI);
	mRotInv.RotateZ(-rotation.x / 180.0 * M_PI);

	int frameIndex = 0;
	for (int key = 0; key < numberOfKeyframes; key++)
	{
		int numberOfSubframes = animationPathData.framesPeyKey.at(key);

		for (int subframe = 0; subframe < numberOfSubframes; subframe++)
		{
			if (frameIndex >= numberOfFrames - 1) break;

			CVector3 pointTarget1, pointTarget2, pointCamera1, pointCamera2;

			double percent = double(frameIndex) / numberOfFrames;

			if (animationPathData.targetPathEnable)
			{
				CVector3 target1 = animationPathData.animationPath[frameIndex].target;
				CVector3 target2 = animationPathData.animationPath[frameIndex + 1].target;

				pointTarget1 =
					InvProjection3D(target1, camera, mRotInv, perspectiveType, fov, width, height);
				pointTarget2 =
					InvProjection3D(target2, camera, mRotInv, perspectiveType, fov, width, height);

				sRGB8 color(255 - percent * 128, 0, percent * 255);
				if (pointTarget1.z > 0)
				{
					if (subframe == 0)
					{
						image->CircleBorder(pointTarget1.x, pointTarget1.y, pointTarget1.z, 5.0, color, 2.0,
							sRGBFloat(1.0, 1.0, 1.0), 1);
					}
				}
				if (pointTarget1.z > 0 && pointTarget2.z > 0)
				{
					image->AntiAliasedLine(pointTarget1.x, pointTarget1.y, pointTarget2.x, pointTarget2.y,
						pointTarget1.z, pointTarget2.z, color, sRGBFloat(1.0, 1.0, 1.0), 1.0f, 1);
				}
			}

			if (animationPathData.cameraPathEnable)
			{
				CVector3 camera1 = animationPathData.animationPath[frameIndex].camera;
				CVector3 camera2 = animationPathData.animationPath[frameIndex + 1].camera;
				pointCamera1 =
					InvProjection3D(camera1, camera, mRotInv, perspectiveType, fov, width, height);
				pointCamera2 =
					InvProjection3D(camera2, camera, mRotInv, perspectiveType, fov, width, height);

				sRGB8 color(0, 255 - percent * 128, percent * 255);
				if (pointCamera1.z > 0)
				{
					if (subframe == 0)
					{
						image->CircleBorder(pointCamera1.x, pointCamera1.y, pointCamera1.z, 5.0, color, 2.0,
							sRGBFloat(1.0, 1.0, 1.0), 1);
					}
				}

				if (pointCamera1.z > 0 && pointCamera2.z > 0)
				{
					image->AntiAliasedLine(pointCamera1.x, pointCamera1.y, pointCamera2.x, pointCamera2.y,
						pointCamera1.z, pointCamera2.z, color, sRGBFloat(1.0, 1.0, 1.0), 1.0f, 1);
				}
			}

			if (animationPathData.cameraPathEnable && animationPathData.targetPathEnable)
			{
				if (pointCamera1.z > 0 && pointTarget1.z > 0)
				{
					if (numberOfSubframes > 4)
					{
						if (subframe % (numberOfSubframes / 4) == 0)
						{
							image->AntiAliasedLine(pointCamera1.x, pointCamera1.y, pointTarget1.x, pointTarget1.y,
								pointCamera1.z, pointTarget1.z, sRGB8(255, 255, 0), sRGBFloat(0.2f, 0.2f, 0.2f),
								1.0f, 1);
						}
					}
				}
			}

			// lights
			for (int i = 0; i < 4; i++)
			{
				if (animationPathData.lightPathEnable[i])
				{
					CVector3 target1 = animationPathData.animationPath[frameIndex].lights[i];
					CVector3 target2 = animationPathData.animationPath[frameIndex + 1].lights[i];

					pointTarget1 =
						InvProjection3D(target1, camera, mRotInv, perspectiveType, fov, width, height);
					pointTarget2 =
						InvProjection3D(target2, camera, mRotInv, perspectiveType, fov, width, height);

					sRGB8 color = animationPathData.animationPath[frameIndex].lightColor[i];

					if (subframe % 8 >= 4)
					{
						color.R = 255 - color.R;
						color.G = 255 - color.G;
						color.B = 255 - color.B;
					}
					if (pointTarget1.z > 0)
					{
						if (subframe == 0)
						{
							image->CircleBorder(pointTarget1.x, pointTarget1.y, pointTarget1.z, 5.0, color, 2.0,
								sRGBFloat(1.0, 1.0, 1.0), 1);
						}
					}
					if (pointTarget1.z > 0 && pointTarget2.z > 0)
					{
						image->AntiAliasedLine(pointTarget1.x, pointTarget1.y, pointTarget2.x, pointTarget2.y,
							pointTarget1.z, pointTarget2.z, color, sRGBFloat(1.0, 1.0, 1.0), 1.0f, 1);
					}
				}
			}

			frameIndex++;
		}
	}
}

void RenderedImage::showRenderedTilesList(QList<sRenderedTileData> listOfRenderedTiles)
{
	listOfRenderedTilesData.append(listOfRenderedTiles);
}

void RenderedImage::PaintLastRenderedTilesInfo()
{
	QPainter painter(this);
	painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

	QPen penRed(Qt::red, 1.0, Qt::SolidLine);
	QPen penGreen(Qt::green, 1.0, Qt::SolidLine);
	painter.setOpacity(0.5);

	QList<QPair<int, int>> listOfPaintedTiles;

	double dpiScale = image->GetDpiScale();

	for (sRenderedTileData &tile : listOfRenderedTilesData)
	{
		if (!listOfPaintedTiles.contains(QPair<int, int>(tile.x, tile.y)))
		{
			listOfPaintedTiles.append(QPair<int, int>(tile.x, tile.y));

			QRect r(tile.x * image->GetPreviewScale() / dpiScale,
				tile.y * image->GetPreviewScale() / dpiScale,
				tile.width * image->GetPreviewScale() / dpiScale,
				tile.height * image->GetPreviewScale() / dpiScale);

			painter.setOpacity(0.5);
			painter.setPen(penRed);

			painter.drawLine(r.x(), r.y(), r.x() + r.width() / 4, r.y());
			painter.drawLine(r.x(), r.y(), r.x(), r.y() + r.height() / 4);
			painter.drawLine(r.right(), r.bottom(), r.right() - r.width() / 4, r.bottom());
			painter.drawLine(r.right(), r.bottom(), r.right(), r.bottom() - r.height() / 4);

			painter.setPen(penGreen);

			painter.drawLine(r.right(), r.y(), r.right() - r.width() / 4, r.y());
			painter.drawLine(r.right(), r.y(), r.right(), r.y() + r.height() / 4);
			painter.drawLine(r.left(), r.bottom(), r.left() + r.width() / 4, r.bottom());
			painter.drawLine(r.left(), r.bottom(), r.left(), r.bottom() - r.height() / 4);

			QPoint center = r.center();

			painter.setOpacity(1.0);
			if (tile.noiseLevel > 0)
			{
				QStaticText text(QString("%1").arg(tile.noiseLevel * 100.0, 0, 'f', 1));
				QPointF point =
					QPointF(center.x() - text.size().width() * 0.5, center.y() - text.size().height() * 0.5);
				painter.drawStaticText(point, text);
			}
		}
	}
	listOfRenderedTilesData.clear();
}

void RenderedImage::DisplayAllLights()
{
	CVector3 camera = params->Get<CVector3>("camera");
	CVector3 target = params->Get<CVector3>("target");
	CVector3 rotation = params->Get<CVector3>("camera_rotation");
	params::enumPerspectiveType perspectiveType =
		static_cast<params::enumPerspectiveType>(params->Get<int>("perspective_type"));
	double fov = CalcFOV(params->Get<double>("fov"), perspectiveType);
	int width = image->GetPreviewWidth();
	int height = image->GetPreviewHeight();

	CRotationMatrix mRotInv;
	mRotInv.RotateY(-rotation.z / 180.0 * M_PI);
	mRotInv.RotateX(-rotation.y / 180.0 * M_PI);
	mRotInv.RotateZ(-rotation.x / 180.0 * M_PI);

	QList<QString> listOfParameters = params->GetListOfParameters();
	for (auto &parameterName : listOfParameters)
	{
		const int lengthOfPrefix = 5;
		if (parameterName.left(lengthOfPrefix) == "light")
		{
			int positionOfDash = parameterName.indexOf('_');
			int lightIndex = parameterName.mid(lengthOfPrefix, positionOfDash - lengthOfPrefix).toInt();
			if (parameterName.mid(positionOfDash + 1) == "is_defined")
			{
				const cLight light(lightIndex, params, false, true, false);

				if (light.enabled)
				{
					bool bold = lightIndex == currentLightIndex;
					double thickness = (bold) ? 2.0 : 1.0;

					if (light.type != cLight::lightDirectional)
					{
						CVector3 lightCenter =
							InvProjection3D(light.position, camera, mRotInv, perspectiveType, fov, width, height);

						if (lightCenter.z < 0.0) continue;

						sRGB8 color = toRGB8(light.color);

						image->CircleBorder(lightCenter.x, lightCenter.y, lightCenter.z, 10.0, color,
							thickness * 4.0, sRGBFloat(1.0, 1.0, 1.0), 1);

						if (light.type == cLight::lightPoint)
						{
							double visibleSize =
								sqrt(light.intensity) * light.size / lightCenter.z / fov * height;

							visibleSize = clamp(visibleSize, 0.0, double(width / 2.0));

							image->CircleBorder(lightCenter.x, lightCenter.y, lightCenter.z, visibleSize, color,
								thickness * 2.0, sRGBFloat(0.5, 0.5, 0.5), 1);

							double sizeFactor = sqrt(light.intensity) * light.size * 20.0;

							line3D(light.position - CVector3(sizeFactor, 0.0, 0.0),
								light.position + CVector3(sizeFactor, 0.0, 0.0), camera, target, mRotInv,
								perspectiveType, fov, width, height, color, thickness, sRGBFloat(0.7, 0.7, 0.7), 20,
								1);

							line3D(light.position - CVector3(0.0, sizeFactor, 0.0),
								light.position + CVector3(0.0, sizeFactor, 0.0), camera, target, mRotInv,
								perspectiveType, fov, width, height, color, thickness, sRGBFloat(0.7, 0.7, 0.7), 20,
								1);

							line3D(light.position - CVector3(0.0, 0.0, sizeFactor),
								light.position + CVector3(0.0, 0.0, sizeFactor), camera, target, mRotInv,
								perspectiveType, fov, width, height, color, thickness, sRGBFloat(0.7, 0.7, 0.7), 20,
								1);
						}

						if (light.type == cLight::lightConical)
						{
							double sizeFactor = sqrt(light.intensity) * light.size * 2.0;
							for (int s = 0; s < 2; s++)
							{

								double coneRatio =
									sin((s == 0) ? light.coneAngle : light.coneAngle + light.coneSoftAngle);

								sRGBFloat opacity =
									((s == 0) ? sRGBFloat(0.7, 0.7, 0.7) : sRGBFloat(0.2, 0.2, 0.2));

								for (int i = 0; i < 8; i++)
								{
									double r1 = (i + 1) * light.size * coneRatio * sizeFactor;
									double r2 = i * light.size * coneRatio * sizeFactor;

									CVector3 previousPoint;

									for (int j = 0; j <= 16; j++)
									{
										double angle = j / 16.0 * 2.0 * M_PI;

										CVector3 dx1 = r1 * cos(angle) * light.lightRightVector;
										CVector3 dy1 = r1 * sin(angle) * light.lightTopVector;
										CVector3 dz1 =
											(-1.0) * (light.size * (i + 1) * sizeFactor) * light.lightDirection;

										CVector3 point1 = light.position + dx1 + dy1 + dz1;

										// draw lines
										if (j % 4 == 0)
										{
											CVector3 dx2 = r2 * cos(angle) * light.lightRightVector;
											CVector3 dy2 = r2 * sin(angle) * light.lightTopVector;
											CVector3 dz2 = (-1.0) * (light.size * i * sizeFactor) * light.lightDirection;

											CVector3 point2 = light.position + dx2 + dy2 + dz2;

											line3D(point1, point2, camera, target, mRotInv, perspectiveType, fov, width,
												height, color, thickness, opacity, 10, 1);
										}

										// draw circles
										if (j > 0)
										{
											line3D(point1, previousPoint, camera, target, mRotInv, perspectiveType, fov,
												width, height, color, thickness, opacity, 10, 1);
										}

										previousPoint = point1;
									} // for j
								}		// for i
							}			// for s
						}				// if conical

						if (light.type == cLight::lightProjection)
						{
							double sizeFactor = sqrt(light.intensity) * light.size * 2.0;

							for (int i = 1; i <= 8; i++)
							{
								double w = i * light.size * light.projectionHorizontalRatio * sizeFactor * 0.5;
								double h = i * light.size * light.projectionVerticalRatio * sizeFactor * 0.5;

								CVector3 dx = w * light.lightRightVector;
								CVector3 dy = h * light.lightTopVector;
								CVector3 dz = (-1.0) * light.size * i * sizeFactor * light.lightDirection;

								CVector3 point1 = light.position + dx + dy + dz;
								CVector3 point2 = light.position - dx + dy + dz;
								CVector3 point3 = light.position - dx - dy + dz;
								CVector3 point4 = light.position + dx - dy + dz;

								line3D(point1, point2, camera, target, mRotInv, perspectiveType, fov, width, height,
									color, thickness, sRGBFloat(0.7, 0.7, 0.7), 10, 1);
								line3D(point2, point3, camera, target, mRotInv, perspectiveType, fov, width, height,
									color, thickness, sRGBFloat(0.7, 0.7, 0.7), 10, 1);
								line3D(point3, point4, camera, target, mRotInv, perspectiveType, fov, width, height,
									color, thickness, sRGBFloat(0.7, 0.7, 0.7), 10, 1);
								line3D(point4, point1, camera, target, mRotInv, perspectiveType, fov, width, height,
									color, thickness, sRGBFloat(0.7, 0.7, 0.7), 10, 1);

								if (i == 8)
								{
									line3D(point1, light.position, camera, target, mRotInv, perspectiveType, fov,
										width, height, color, thickness, sRGBFloat(0.7, 0.7, 0.7), 100, 1);
									line3D(point2, light.position, camera, target, mRotInv, perspectiveType, fov,
										width, height, color, thickness, sRGBFloat(0.7, 0.7, 0.7), 100, 1);
									line3D(point3, light.position, camera, target, mRotInv, perspectiveType, fov,
										width, height, color, thickness, sRGBFloat(0.7, 0.7, 0.7), 100, 1);
									line3D(point4, light.position, camera, target, mRotInv, perspectiveType, fov,
										width, height, color, thickness, sRGBFloat(0.7, 0.7, 0.7), 100, 1);
								}

							} // for i
						}		// if projection
					}			// if not directional
				}				// if enabled
			}					// if is defined
		}						// if parameter is light
	}							// for parameterName
}

void RenderedImage::DrawWireframeTorus(const std::shared_ptr<sPrimitiveTorus> &torus,
	const CVector3 &camera, const CVector3 &target, const CRotationMatrix &mRotInv,
	params::enumPerspectiveType perspectiveType, double fov, int width, int height, sRGB8 color,
	double thickness)
{
	// primitive torus cannot be scaled

	double angleStep = 2.0 * M_PI / sPrimitiveBasic::wireframeSegments;
	for (double alpha = 0; alpha < 2.0 * M_PI; alpha += angleStep * 2.0)
	{
		double tx1 = cos(alpha) * torus->tubeRadius + torus->radius;
		double tz1 = sin(alpha) * torus->tubeRadius;
		double tx2 = cos(alpha + angleStep * 2.0) * torus->tubeRadius + torus->radius;
		double tz2 = sin(alpha + angleStep * 2.0) * torus->tubeRadius;
		for (double beta = 0.0; beta < 2.0 * M_PI; beta += angleStep)
		{
			double x1 = cos(beta) * tx1;
			double y1 = sin(beta) * tx1;
			double x2 = cos(beta + angleStep) * tx1;
			double y2 = sin(beta + angleStep) * tx1;
			double x3 = cos(beta) * tx2;
			double y3 = sin(beta) * tx2;
			{
				CVector3 p1(x1, y1, tz1);
				CVector3 p2(x2, y2, tz1);
				CVector3 point1 = torus->rotationMatrix.Transpose().RotateVector(p1);
				point1 = point1 + torus->position;
				CVector3 point2 = torus->rotationMatrix.Transpose().RotateVector(p2);
				point2 = point2 + torus->position;
				line3D(point1, point2, camera, target, mRotInv, perspectiveType, fov, width, height, color,
					thickness, sRGBFloat(0.7, 0.7, 0.7), 10, 1);
			}
			{
				CVector3 p1(x1, y1, tz1);
				CVector3 p2(x3, y3, tz2);
				CVector3 point1 = torus->rotationMatrix.Transpose().RotateVector(p1);
				point1 = point1 + torus->position;
				CVector3 point2 = torus->rotationMatrix.Transpose().RotateVector(p2);
				point2 = point2 + torus->position;
				line3D(point1, point2, camera, target, mRotInv, perspectiveType, fov, width, height, color,
					thickness, sRGBFloat(0.7, 0.7, 0.7), 10, 1);
			}
		}
	}
}

void RenderedImage::DisplayAllPrimitives()
{
	cPrimitives primitives(params, nullptr);
	QStringList listOfNames = primitives.GetListOfPrimitiveNames();

	CVector3 camera = params->Get<CVector3>("camera");
	CVector3 target = params->Get<CVector3>("target");
	CVector3 rotation = params->Get<CVector3>("camera_rotation");
	params::enumPerspectiveType perspectiveType =
		static_cast<params::enumPerspectiveType>(params->Get<int>("perspective_type"));
	double fov = CalcFOV(params->Get<double>("fov"), perspectiveType);
	int width = image->GetPreviewWidth();
	int height = image->GetPreviewHeight();

	CRotationMatrix mRotInv;
	mRotInv.RotateY(-rotation.z / 180.0 * M_PI);
	mRotInv.RotateX(-rotation.y / 180.0 * M_PI);
	mRotInv.RotateZ(-rotation.x / 180.0 * M_PI);

	for (int index = 0; index < primitives.GetNumberOfPrimivives(); index++)
	{
		const std::shared_ptr<sPrimitiveBasic> primitive = primitives.GetPrimitive(index);
		const sPrimitiveBasic::tWireframeShape primitiveShape = primitive->GetWireFrameShape();
		const QString primitiveName = listOfNames.at(index);

		sRGB8 color(0, 255, 0);
		double thickness = (primitiveName == currentPrimitiveItem.fullName) ? 2.0 : 1.0;

		// primitive torus cannot be scaled
		if (auto torus = std::dynamic_pointer_cast<sPrimitiveTorus>(primitive))
		{
			DrawWireframeTorus(
				torus, camera, target, mRotInv, perspectiveType, fov, width, height, color, thickness);
		}

		for (const sPrimitiveBasic::sPrimitiveWireLine &line : primitiveShape)
		{
			double sizeMultiplier = 1.0;

			if (std::dynamic_pointer_cast<sPrimitivePlane>(primitive))
			{
				sizeMultiplier = (camera - primitive->position).Length();
			}

			CVector3 point1 = primitive->rotationMatrix.Transpose().RotateVector(
				line.p1 * primitive->size * sizeMultiplier);
			point1 = point1 + primitive->position;

			CVector3 point2 = primitive->rotationMatrix.Transpose().RotateVector(
				line.p2 * primitive->size * sizeMultiplier);
			point2 = point2 + primitive->position;

			line3D(point1, point2, camera, target, mRotInv, perspectiveType, fov, width, height, color,
				thickness, sRGBFloat(0.7, 0.7, 0.7), 10, 1);
		}
	}
}

void RenderedImage::line3D(const CVector3 &p1, const CVector3 &p2, const CVector3 &camera,
	const CVector3 &target, const CRotationMatrix &mRotInv,
	params::enumPerspectiveType perspectiveType, double fov, double imgWidth, double imgHeight,
	sRGB8 color, double thickness, sRGBFloat opacity, int numberOfSegments, int layer)
{
	CVector3 cameraVector = target - camera;
	cameraVector.Normalize();

	// ref:
	// calculation of clipping plane:
	// https://gabrielgambetta.com/computer-graphics-from-scratch/11-clipping.html

	double clip = 1e-6;

	for (int i = 0; i < numberOfSegments; i++)
	{
		double k1 = double(i) / numberOfSegments;
		double kn1 = 1.0 - k1;

		double k2 = double(i + 1) / numberOfSegments;
		double kn2 = 1.0 - k2;

		CVector3 point1 = p1 * kn1 + p2 * k1;
		CVector3 point2 = p1 * kn2 + p2 * k2;

		double distFromClipPlane1 = (point1 - camera).Dot(cameraVector);
		double distFromClipPlane2 = (point2 - camera).Dot(cameraVector);

		if (distFromClipPlane1 >= clip || distFromClipPlane2 >= clip)
		{
			if (distFromClipPlane1 < clip)
			{
				double t = (clip - cameraVector.Dot(point1 - camera)) / (cameraVector.Dot(point2 - point1));
				point1 = point1 + t * (point2 - point1);
			}

			if (distFromClipPlane2 < clip)
			{
				double t = (clip - cameraVector.Dot(point2 - camera)) / (cameraVector.Dot(point1 - point2));
				point2 = point2 + t * (point1 - point2);
			}

			CVector3 p1Projected =
				InvProjection3D(point1, camera, mRotInv, perspectiveType, fov, imgWidth, imgHeight);

			CVector3 p2Projected =
				InvProjection3D(point2, camera, mRotInv, perspectiveType, fov, imgWidth, imgHeight);

			image->AntiAliasedLine(p1Projected.x, p1Projected.y, p2Projected.x, p2Projected.y,
				p1Projected.z, p2Projected.z, color, opacity, thickness, layer);
		}
	}
}
