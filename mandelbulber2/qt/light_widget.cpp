/*
 * light_widget.cpp
 *
 *  Created on: 13 mar 2021
 *      Author: krzysztof
 */

#include "light_widget.h"
#include "src/system_data.hpp"

#include <QEvent>
#include <QPainter>
#include <QImage>
#include <QPaintEvent>

cLightWidget::cLightWidget(QWidget *parent) : QWidget(parent)
{
	size = systemData.GetPreferredThumbnailSize()*0.7;
	SetLightParameters(CVector3(0.1, 0.1, 0.0), sRGB8(200, 200, 200));
	setMinimumWidth(size);
	setMinimumHeight(size);
	updateGeometry();
}

cLightWidget::~cLightWidget()
{
	// TODO Auto-generated destructor stub
}

void cLightWidget::SetLightParameters(CVector3 _angle, sRGB8 _color)
{
	lightAngle = _angle;
	lightColor = _color;

	image8.resize(size * size);

	double center = double(size) / 2.0;
	double radius = center * 0.9;

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			CVector3 sphereVect;
			sphereVect.x = double(x) - center;
			sphereVect.y = double(y) - center;
			double r2D = sqrt(sphereVect.x * sphereVect.x + sphereVect.y * sphereVect.y);

			double brightness = 0.0;
			if (r2D < radius)
			{
				sphereVect.z = sqrt(radius * radius - r2D * r2D);

				sphereVect.Normalize();
				brightness = sphereVect.z;
			}

			int address = y * size + x;
			image8[address] = sRGB8(brightness * 255, brightness * 255, brightness * 255);
		}
	}
}

void cLightWidget::paintEvent(QPaintEvent *event)
{
	if (image8.size() > 0)
	{
		event->accept();

		QPainter painter(this);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

		QImage qImage(reinterpret_cast<const quint8 *>(image8.data()), size, size,
			int(size * sizeof(sRGB8)), QImage::Format_RGB888);
		painter.drawImage(QRect(0, 0, size, size), qImage, QRect(0, 0, size, size));
	}
}
