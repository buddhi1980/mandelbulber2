/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * RenderedImage class - extension for QWidget class. Widget prepared for displaying rendered image and 3D cursor
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "rendered_image_widget.hpp"
#include <QtCore>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QPainter>
#include "fractparams.hpp"
#include "common_math.h"
#include "global_data.hpp"

using namespace Qt;

RenderedImage::RenderedImage(QWidget *parent) :
		QWidget(parent)
{
	// makes RenderedImage focusable to catch keyboard events
	setFocusPolicy(Qt::StrongFocus);
	setMouseTracking(true);

	image = NULL;
	params = NULL;
	cursorVisible = true;
	smoothLastZMouse = 0.0;
	redrawed = true;
	isFocus = false;
	isOnObject = false;
	lastDepth = 0.0;
	frontDist = 0.0;
	flightRotationDirection = 0;

	QList<QVariant> mode;
	mode.append((int)RenderedImage::clickPlaceLight);
	clickModeData = mode;

	//timer to refresh image
	timerRefreshImage = new QTimer(this);
	timerRefreshImage->setInterval(40);
	this->connect(timerRefreshImage, SIGNAL(timeout()), this, SLOT(update()));
}

void RenderedImage::paintEvent(QPaintEvent *event)
{
	(void) event;

	if (image)
	{
		if(cursorVisible && isFocus)
		{
			CVector2<int> point = lastMousePosition / image->GetPreviewScale();
			double z = image->GetPixelZBuffer(point.x, point.y);

			DisplayCrosshair();

			if(z < 1e10 || (enumClickMode)clickModeData.at(0).toInt() == clickFlightSpeedControl)
			{
				redrawed = false;
				isOnObject = true;

				Display3DCursor(lastMousePosition, z);
			}
			else
			{
				isOnObject = false;
			}
		}

		image->RedrawInWidget();

		if(cursorVisible && isFocus && (isOnObject || (enumClickMode)clickModeData.at(0).toInt() == clickFlightSpeedControl))
		{
			DisplayCoordinates();
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
	enumClickMode clickMode = (enumClickMode)clickModeData.at(0).toInt();
	switch (clickMode)
	{
		case clickMoveCamera:
			text = tr("Move camera");
			break;
		case clickFogVisibility:
			text = tr("Change fog visibility");
			break;
		case clickDOFFocus:
			text = tr("Change DOF focus");
			break;
		case clickPlaceLight:
			text = tr("Place light #") + QString::number(clickModeData.at(1).toInt());
			text += tr("\nMouse wheel - light fov / bkw");
			break;
		case clickPlacePrimitive:
			text = tr("Place ") + PrimitiveNames((fractal::enumObjectType)clickModeData.at(1).toInt()) + QString(" #")+ QString::number(clickModeData.at(2).toInt());
			break;
		case clickGetJuliaConstant:
			text = tr("Get Julia constant");
			break;
		case clickFlightSpeedControl:
			text = tr("LMB - increase speed");
			text += tr("\nRMB - decrease speed");
			text += tr("\narrow keys - strafe");
			text += tr("\nz, x keys - roll");
			text += tr("\nspacebar - pause");
			break;
		case clickDoNothing:
			text  = "";
			break;
	}

	if(clickMode != clickDoNothing)
	{
		QRect textRect = painter.boundingRect(QRect(), Qt::AlignTop || Qt::AlignLeft, text);
		textRect.setHeight(textRect.height() + 2);
		textRect.moveBottomLeft(QPoint(lastMousePosition.x + 30, lastMousePosition.y - 3));

		painter.setOpacity(0.5);
		painter.setPen(penWhite);
		painter.setBrush(brushBrown);
		painter.drawRoundedRect(textRect, 3, 3);
		painter.drawText(textRect, Qt::AlignTop || Qt::AlignLeft, text);
	}

	QString textCoordinates;
	if(clickMode != clickFlightSpeedControl)
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

	QRect textRect2 = painter.boundingRect(QRect(), Qt::AlignTop || Qt::AlignLeft, textCoordinates);
	textRect2.setHeight(textRect2.height() + 2);
	textRect2.moveTopLeft(QPoint(lastMousePosition.x + 30, lastMousePosition.y + 3));
	painter.setOpacity(0.5);
	painter.setPen(penWhite);
	painter.setBrush(brushDarkBlue);
	painter.drawRoundedRect(textRect2, 3, 3);
	painter.drawText(textRect2, Qt::AlignTop || Qt::AlignLeft, textCoordinates);

}

void RenderedImage::Display3DCursor(CVector2<int> screenPoint, double z)
{
	enumClickMode clickMode = (enumClickMode)clickModeData.at(0).toInt();
	if (clickMode == clickPlaceLight)
	{
		z -= frontDist;
	}

	smoothLastZMouse = smoothLastZMouse + (z - smoothLastZMouse) * 0.01;

	if (z > 0 && clickMode != clickFlightSpeedControl)
	{
		if(smoothLastZMouse < 0.0) smoothLastZMouse = 0.0;

		bool legacyCoordinateSystem = gPar->Get<bool>("legacy_coordinate_system");
		double reverse = legacyCoordinateSystem ? -1.0 : 1.0;

		//preparing rotation matrix
		CVector3 rotation = params->Get<CVector3>("camera_rotation") / 180.0 * M_PI;
		CRotationMatrix mRot;
		mRot.RotateZ(rotation.x);
		mRot.RotateX(rotation.y);
		mRot.RotateY(rotation.z);

		double fov = params->Get<double>("fov");

		double sw = image->GetPreviewWidth();
		double sh = image->GetPreviewHeight();

		double aspectRatio = sw / sh;

		CVector2<double> p;
		p.x = (screenPoint.x / sw - 0.5) * aspectRatio;
		p.y = (screenPoint.y / sh - 0.5);

		double scale = smoothLastZMouse / z;

		double boxWidth = 10.0 / sw * scale;
		double boxHeight = 10.0 / sw * scale;
		double boxDepth = 10.0 / sw * scale;

		double boxDepth2 = boxHeight * z * fov;

		double n = 3.0;

		for (int iz = -n; iz <= n; iz++)
		{
			double yy1 = ((p.y + n * boxHeight) / (1.0 - boxDepth * iz * fov) + 0.5) * sh;
			double yy2 = ((p.y - n * boxHeight) / (1.0 - boxDepth * iz * fov) + 0.5) * sh;
			for (int ix = -n; ix <= n; ix++)
			{
				double xx1 = ((p.x + boxWidth * ix) / (1.0 - boxDepth * iz * fov) / aspectRatio + 0.5) * sw;
				unsigned char R = 128 + iz * 40;
				unsigned char G = 128 - iz * 40;
				unsigned char B = 0;
				double opacity = 0.8;
				if (iz == 0 && ix == 0)
				{
					R = G = B = 255;
					opacity = 1.0;
				}
				image->AntiAliasedLine(xx1, yy1, xx1, yy2, z - iz * boxDepth2, z - iz * boxDepth2, sRGB8(R, G, B), opacity, 1);
			}

			double xx1 = ((p.x + n * boxWidth) / (1.0 - boxDepth * iz * fov) / aspectRatio + 0.5) * sw;
			double xx2 = ((p.x - n * boxWidth) / (1.0 - boxDepth * iz * fov) / aspectRatio + 0.5) * sw;
			for (int iy = -n; iy <= n; iy++)
			{
				double yy1 = ((p.y + boxWidth * iy) / (1.0 - boxDepth * iz * fov) + 0.5) * sh;
				unsigned char R = 128 + iz * 40;
				unsigned char G = 128 - iz * 40;
				unsigned char B = 0;
				double opacity = 0.8;

				if (iz == 0 && iy == 0)
				{
					R = G = B = 255;
					opacity = 1.0;
				}

				image->AntiAliasedLine(xx1, yy1, xx2, yy1, z - iz * boxDepth2, z - iz * boxDepth2, sRGB8(R, G, B), opacity, 1);
			}

			if (iz < n)
			{
				for (int ix = -n; ix <= n; ix++)
				{
					for (int iy = -n; iy <= n; iy++)
					{
						double xx1 = ((p.x + boxWidth * ix) / (1.0 - boxDepth * iz * fov) / aspectRatio + 0.5) * sw;
						double yy1 = ((p.y + boxWidth * iy) / (1.0 - boxDepth * iz * fov) + 0.5) * sh;
						double xx2 = ((p.x + boxWidth * ix) / (1.0 - boxDepth * (iz + 1) * fov) / aspectRatio + 0.5) * sw;
						double yy2 = ((p.y + boxWidth * iy) / (1.0 - boxDepth * (iz + 1) * fov) + 0.5) * sh;

						double opacity = 0.8;
						unsigned char R = 128 + iz * 40;
						unsigned char G = 128 - iz * 40;
						unsigned char B = 0;

						if (ix == 0 && iy == 0)
						{
							R = G = B = 255;
							opacity = 1.0;
						}

						image->AntiAliasedLine(xx1, yy1, xx2, yy2, z - iz * boxDepth2, z - (iz + 1) * boxDepth2, sRGB8(R, G, B), opacity, 1);
					}
				}
			}
			if (iz == 0)
			{
				image->AntiAliasedLine(screenPoint.x - sw * 0.3, screenPoint.y, screenPoint.x + sw * 0.3, screenPoint.y, z, z, sRGB8(255, 255, 255), 1.0, 1);
				image->AntiAliasedLine(screenPoint.x, screenPoint.y - sh * 0.3, screenPoint.x, screenPoint.y + sh * 0.3, z, z, sRGB8(255, 255, 255), 1.0, 1);

				if (clickMode == clickPlaceLight)
				{
					double r = 1.5 * (boxWidth * n / aspectRatio);
					if (r > 1.0) r = 1.0;
					image->CircleBorder(screenPoint.x, screenPoint.y, z, r * sw, sRGB8(0, 100, 255), r * 0.1 * sw, 1.0, 1);
				}

			}
		}

		p.y *= -1.0 * reverse;
		params::enumPerspectiveType perspType = (params::enumPerspectiveType) params->Get<int>("perspective_type");
		CVector3 camera = params->Get<CVector3>("camera");
		CVector3 viewVector = CalculateViewVector(p, fov, perspType, mRot);
		CVector3 point = camera + viewVector * z;
		lastCoordinates = point;
	}
	else if(clickMode == clickFlightSpeedControl)
	{
		DrawHud(flightData.rotation);

		//draw small cross
		image->AntiAliasedLine(screenPoint.x - 20, screenPoint.y - 20, screenPoint.x + 20, screenPoint.y + 20, -1, -1, sRGB8(255,255,255), 0.3, 1);
		image->AntiAliasedLine(screenPoint.x + 20, screenPoint.y - 20, screenPoint.x - 20, screenPoint.y + 20, -1, -1, sRGB8(255,255,255), 0.3, 1);
	}
	lastDepth = z;
}

void RenderedImage::mouseMoveEvent(QMouseEvent * event)
{
	CVector2<int> screenPoint(event->x(), event->y());

	//remember last mouse position
	lastMousePosition = screenPoint;

	CVector2<double> yawAndPitch;
	yawAndPitch.x = ((double)lastMousePosition.x / image->GetPreviewWidth() - 0.5) * 2.0;
	yawAndPitch.y = ((double)lastMousePosition.y / image->GetPreviewHeight() - 0.5) * 2.0;
	emit flightYawAndPitch(yawAndPitch);

	if(params)
	{
		if(cursorVisible && isFocus && redrawed)
		{
			update();
		}
	}
	else
	{
		qCritical() << "RenderedImage::mouseMoveEvent(QMouseEvent * event): parameters not assigned";
	}

	emit mouseMoved(screenPoint.x, screenPoint.y);
}

void RenderedImage::mousePressEvent(QMouseEvent * event)
{
	if((enumClickMode)clickModeData.at(0).toInt() == clickFlightSpeedControl)
	{
		if(event->button() == Qt::LeftButton)
		{
			emit flightSpeedIncrease();
		}
		else if(event->button() == Qt::RightButton)
		{
			emit flightSpeedDecrease();
		}
	}
	else
	{
		emit singleClick(event->x(), event->y(), event->button());
	}

}

void RenderedImage::mouseReleaseEvent(QMouseEvent * event)
{
	(void)event;
}

void RenderedImage::enterEvent(QEvent * event)
{
	(void)event;
	setFocus();
	isFocus = true;
	timerRefreshImage->start();
}

void RenderedImage::leaveEvent(QEvent * event)
{
	(void)event;
	isFocus = false;
	update();
	timerRefreshImage->stop();
}

void RenderedImage::keyPressEvent(QKeyEvent * event)
{
	if(event->isAutoRepeat())
	{
		event->ignore();
	}
	else
	{
		if((enumClickMode)clickModeData.at(0).toInt() == clickFlightSpeedControl)
		{
			Qt::Key key = (Qt::Key) event->key();
			if(key == Qt::Key_Up)
			{
				keyArrows.y += 1;
				emit(flightStrafe(keyArrows));
			}
			else if(key == Qt::Key_Down)
			{
				keyArrows.y -= 1;
				emit(flightStrafe(keyArrows));
			}
			else if(key == Qt::Key_Left)
			{
				keyArrows.x -= 1;
				emit(flightStrafe(keyArrows));
			}
			else if(key == Qt::Key_Right)
			{
				keyArrows.x += 1;
				emit(flightStrafe(keyArrows));
			}
			else if(key == Qt::Key_Z)
			{
				flightRotationDirection = 1;
				emit(flightRotation(flightRotationDirection));
			}
			else if(key == Qt::Key_X)
			{
				flightRotationDirection = -1;
				emit(flightRotation(flightRotationDirection));
			}
			else if(key == Qt::Key_Space)
			{
				emit flightPause();
			}

		}
		else
		{
			emit keyPress((Qt::Key) event->key());
		}
	}
}

void RenderedImage::keyReleaseEvent(QKeyEvent * event)
{
	if(event->isAutoRepeat())
	{
		event->ignore();
	}
	else
	{
		if((enumClickMode)clickModeData.at(0).toInt() == clickFlightSpeedControl)
		{
			Qt::Key key = (Qt::Key) event->key();
			if(key == Qt::Key_Up)
			{
				keyArrows.y -= 1;
				emit(flightStrafe(keyArrows));
			}
			else if(key == Qt::Key_Down)
			{
				keyArrows.y += 1;
				emit(flightStrafe(keyArrows));
			}
			else if(key == Qt::Key_Left)
			{
				keyArrows.x += 1;
				emit(flightStrafe(keyArrows));
			}
			else if(key == Qt::Key_Right)
			{
				keyArrows.x -= 1;
				emit(flightStrafe(keyArrows));
			}
			else if(key == Qt::Key_Z)
			{
				flightRotationDirection = 0;
				emit(flightRotation(flightRotationDirection));
			}
			else if(key == Qt::Key_X)
			{
				flightRotationDirection = 0;
				emit(flightRotation(flightRotationDirection));
			}

		}
		else
		{
			emit keyRelease((Qt::Key) event->key());
		}
	}
}

void RenderedImage::wheelEvent(QWheelEvent * event)
{
	emit mouseWheelRotated(event->delta());
	if(params)
	{
		if(cursorVisible && isFocus && redrawed)
		{
			update();
		}
	}
	else
	{
		qCritical() << "RenderedImage::mouseMoveEvent(QMouseEvent * event): parameters not assigned";
	}
}

void RenderedImage::DisplayCrosshair()
{
	image->AntiAliasedLine(image->GetPreviewWidth()/2, 0, image->GetPreviewWidth()/2, image->GetPreviewHeight(), -1, -1, sRGB8(255,255,255), 0.3, 1);
	image->AntiAliasedLine(0, image->GetPreviewHeight()/2, image->GetPreviewWidth(), image->GetPreviewHeight()/2, -1, -1, sRGB8(255,255,255), 0.3, 1);
}

void RenderedImage::DrawHud(CVector3 rotation)
{
	CRotationMatrix mRotInv;
	mRotInv.RotateY(-rotation.z);
	mRotInv.RotateX(-rotation.y);
	mRotInv.RotateZ(-rotation.x);
	double width = image->GetPreviewWidth();
	double height = image->GetPreviewHeight();
	CVector3 center = CVector3(width * 0.5, height * 0.5, 0.0);

	const int steps = 100;
	const double persp = 0.4;
	CVector3 circlePoint1[steps];
	CVector3 circlePoint2[steps];
	CVector3 circlePoint3[steps];

	for(int i = 0; i<steps; i++)
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
	for(int i = 0; i<steps; i++)
	{
		CVector3 point1, point2;
		int index1 = i;
		int index2 = (i + 1) % steps;
		point1 = CalcPointPersp(circlePoint1[index1], mRotInv, persp) * (height * 0.2) + center;
		point2 = CalcPointPersp(circlePoint1[index2], mRotInv, persp) * (height * 0.2) + center;
		image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (255, 0, 0), 0.5, 1);

		point1 = CalcPointPersp(circlePoint2[index1], mRotInv, persp) * (height * 0.2) + center;
		point2 = CalcPointPersp(circlePoint2[index2], mRotInv, persp) * (height * 0.2) + center;
		image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (0, 255, 0), 0.5, 1);

		point1 = CalcPointPersp(circlePoint3[index1], mRotInv, persp) * (height * 0.2) + center;
		point2 = CalcPointPersp(circlePoint3[index2], mRotInv, persp) * (height * 0.2) + center;
		image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (0, 100, 255), 0.5, 1);
	}

	CVector3 point1, point2;
	point1 = CalcPointPersp(CVector3(1.0, 0.0, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(-1.0, 0.0, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (255, 0, 0), 0.5, 1);
	point1 = CalcPointPersp(CVector3(0.0, 1.0, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(0.0, -1.0, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (255, 0, 0), 0.5, 1);
	point1 = CalcPointPersp(CVector3(0.9, -0.05, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(1.0, 0.0, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (255, 0, 0), 0.5, 1);
	point1 = CalcPointPersp(CVector3(0.9, 0.05, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(1.0, 0.0, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (255, 0, 0), 0.5, 1);
	point1 = CalcPointPersp(CVector3(0.9, 0.0, 0.05), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(1.0, 0.0, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (255, 0, 0), 0.5, 1);
	point1 = CalcPointPersp(CVector3(0.9, 0.0, -0.05), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(1.0, 0.0, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (255, 0, 0), 0.5, 1);

	point1 = CalcPointPersp(CVector3(0.0, 0.0, 1.0), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(0.0, 0.0, -1.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (0, 255, 0), 0.5, 1);
	point1 = CalcPointPersp(CVector3(0.0, 1.0, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(0.0, -1.0, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (0, 255, 0), 0.5, 1);
	point1 = CalcPointPersp(CVector3(0.05, 0.9, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(0.0, 1.0, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (0, 255, 0), 0.5, 1);
	point1 = CalcPointPersp(CVector3(-0.05, 0.9, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(0.0, 1.0, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (0, 255, 0), 0.5, 1);
	point1 = CalcPointPersp(CVector3(0.0, 0.9, 0.05), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(0.0, 1.0, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (0, 255, 0), 0.5, 1);
	point1 = CalcPointPersp(CVector3(0.0, 0.9, -0.05), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(0.0, 1.0, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (0, 255, 0), 0.5, 1);

	point1 = CalcPointPersp(CVector3(1.0, 0.0, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(-1.0, 0.0, 0.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (0, 150, 255), 0.5, 1);
	point1 = CalcPointPersp(CVector3(0.0, 0.0, 1.0), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(0.0, 0.0, 1.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (0, 150, 255), 0.5, 1);
	point1 = CalcPointPersp(CVector3(0.05, 0.0, 0.9), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(0.0, 0.0, 1.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (0, 150, 255), 0.5, 1);
	point1 = CalcPointPersp(CVector3(-0.05, 0.0, 0.9), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(0.0, 0.0, 1.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (0, 150, 255), 0.5, 1);
	point1 = CalcPointPersp(CVector3(0.0, 0.05, 0.9), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(0.0, 0.0, 1.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (0, 150, 255), 0.5, 1);
	point1 = CalcPointPersp(CVector3(0.0, -0.05, 0.9), mRotInv, persp)  * (height * 0.2) + center;
	point2 = CalcPointPersp(CVector3(0.0, 0.0, 1.0), mRotInv, persp)  * (height * 0.2) + center;
	image->AntiAliasedLine(point1.x, point1.y, point2.x, point2.y, -1.0, -1.0, sRGB8 (0, 150, 255), 0.5, 1);
}

CVector3 RenderedImage::CalcPointPersp(const CVector3 &point, const CRotationMatrix &rot, double persp)
{
	CVector3 vect;
	CVector3 out;
	vect = rot.RotateVector(point);
	out.x = vect.x / (1.0 + vect.y * persp);
	out.y = -vect.z / (1.0 + vect.y * persp);
	return out;
}
