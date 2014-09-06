/*
 * rendered_image_widget.cpp
 *
 *  Created on: Aug 15, 2014
 *      Author: krzysztof
 */

#include "rendered_image_widget.hpp"
#include <QtCore>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QApplication>
#include <QPainter>

using namespace Qt;

RenderedImage::RenderedImage(QWidget *parent) :
		QWidget(parent)
{
	// makes RenderedImage focusable to catch keyboard events
	setFocusPolicy(Qt::StrongFocus);

	image = NULL;
	params = NULL;
	cursorVisible = true; //TODO changing cursor visibility
	setMouseTracking(true);
	clickMode = clickMoveCamera;
	smoothLastZMouse = 0.0;
	redrawed = false;
}

void RenderedImage::paintEvent(QPaintEvent *event)
{
	(void) event;

	if (image)
	{
		image->RedrawInWidget();

		if(cursorVisible && isFocus)
		{
			QPainter painter(this);
			painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

			QString text;
			switch (clickMode)
			{
				case clickMoveCamera:
					text = QString("Move camera");
					break;

				case clickFogVisibility:
					text = QString("Change fog visibility");
					break;

				case clickDOFFocus:
					text = QString("Change DOF focus");
					break;
			}
			QRect textRect = painter.boundingRect(QRect(), Qt::AlignLeft, text);
			textRect.setHeight(textRect.height() + 2);
			textRect.moveBottomLeft(QPoint(lastMousePosition.x + 30, lastMousePosition.y - 3));

			QPen pen(Qt::white, 1, Qt::SolidLine);
			QBrush brush(Qt::gray);
			painter.setOpacity(0.5);
			painter.setPen(pen);
			painter.setBrush(brush);
			painter.drawRoundedRect(textRect, 3, 3);
			painter.drawText(QPoint(lastMousePosition.x + 30, lastMousePosition.y - 4), text);
			redrawed = true;
		}
	}
	else
	{
		qCritical() << "RenderedImage::mouseMoveEvent(QMouseEvent * event): image not assigned";
	}

}

void RenderedImage::mouseMoveEvent(QMouseEvent * event)
{
	//TODO add displaying of coordinates by the cursor
	CVector2<int> screenPoint(event->x(), event->y());

	//remember last mouse position
	lastMousePosition = screenPoint;

	if(params)
	{
		if(cursorVisible && isFocus && redrawed)
		{
			CVector2<int> point = screenPoint / image->GetPreviewScale();
			double z = image->GetPixelZBuffer(point.x, point.y);

			if(z < 1e10)
			{
				redrawed = false;
				smoothLastZMouse = smoothLastZMouse + (z - smoothLastZMouse) * 0.01;

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

				double boxWidth2 = boxWidth * z * fov;
				double boxHeigth2 = boxHeight * z * fov;
				double boxDepth2 = boxHeight * z * fov;

				double n = 3.0;

				for (int iz = -n; iz <= n; iz++)
				{
					double yy1 = ((p.y + n * boxHeight) / (1.0 - boxDepth * iz * fov) + 0.5) * sh;
					double yy2 = ((p.y - n * boxHeight) / (1.0 - boxDepth * iz * fov) + 0.5) * sh;
					for (int ix = -3.0; ix <= 3.0; ix++)
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

						/*
						if (mode >= 5 && mode < 10)
						{
							double r = 1.5 * (boxWidth * n / aspectRatio);
							if (r > 1.0) r = 1.0;
							image->CircleBorder(screenPoint.x, screenPoint.y, z, r * sw, (sRGB8 ) { 0, 100, 255 }, r * 0.1 * sw, 1.0, 1);
						}
						*/
					}
				}


			}
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
	emit singleClick(event->x(), event->y(), event->button());
}

void RenderedImage::mouseReleaseEvent(QMouseEvent * event)
{
}

void RenderedImage::enterEvent(QEvent * event)
{
	setFocus();
	isFocus = true;
}

void RenderedImage::leaveEvent(QEvent * event)
{
	isFocus = false;
	update();
}

void RenderedImage::keyPressEvent(QKeyEvent * event)
{
	emit keyPress((Qt::Key) event->key());
}

void RenderedImage::keyReleaseEvent(QKeyEvent * event)
{
	emit keyRelease((Qt::Key) event->key());
}
