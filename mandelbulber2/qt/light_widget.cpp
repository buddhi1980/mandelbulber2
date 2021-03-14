/*
 * light_widget.cpp
 *
 *  Created on: 13 mar 2021
 *      Author: krzysztof
 */

#include "light_widget.h"
#include "src/system_data.hpp"
#include "src/common_math.h"

#include <QEvent>
#include <QPainter>
#include <QImage>
#include <QPaintEvent>

cLightWidget::cLightWidget(QWidget *parent) : QWidget(parent)
{
	size = systemData.GetPreferredThumbnailSize() * 0.7;
	//SetLightAngle(CVector3(0.0, 0.5, 0.5), sRGBFloat(0.9, 0.4, 0.1));
	SetLightColor(sRGBFloat(0.9, 0.4, 0.1));
	setMinimumWidth(size);
	setMinimumHeight(size);
	updateGeometry();
}

cLightWidget::~cLightWidget()
{
	// TODO Auto-generated destructor stub
}

void cLightWidget::SetLightAngle(CVector3 _angle)
{
	lightAngle = _angle;
	Render();
}

void cLightWidget::SetLightColor(sRGBFloat _color)
{
	lightColor = _color;
	Render();
}

void cLightWidget::paintEvent(QPaintEvent *event)
{
	if (image8.size() > 0)
	{
		event->accept();

		QPainter painter(this);
		painter.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);

		QImage qImage(reinterpret_cast<const quint8 *>(image8.data()), size, size,
			int(size * sizeof(sRGBA8)), QImage::Format_RGBA8888);
		painter.drawImage(QRect(0, 0, size, size), qImage, QRect(0, 0, size, size));
	}
}

void cLightWidget::Render()
{
	image8.resize(size * size);

	double center = double(size) / 2.0;
	double radius = center * 0.9;

	CRotationMatrix rotMatrix;
	rotMatrix.SetRotation(lightAngle);
	CVector3 lightDirection = rotMatrix.RotateVector(CVector3(0.0, 1.0, 0.0));

	for (int y = 0; y < size; y++)
	{
		for (int x = 0; x < size; x++)
		{
			CVector3 sphereVect;
			sphereVect.x = double(x) - center;
			sphereVect.z = double(y) - center;
			double r2D = sqrt(sphereVect.x * sphereVect.x + sphereVect.z * sphereVect.z);

			sRGBFloat color;
			int opacity = 0;
			if (r2D < radius)
			{
				sphereVect.y = sqrt(radius * radius - r2D * r2D);

				sphereVect.Normalize();

				// shading
				double shade = sphereVect.Dot(lightDirection);
				if (shade < 0.0) shade = 0.0;

				// specular highlight
				CVector3 halfVector = lightDirection - CVector3(0.0, -1.0, 0.0);
				halfVector.Normalize();
				double shade2 = sphereVect.Dot(halfVector);
				if (shade2 < 0.0f) shade2 = 0.0f;
				double specular = pow(shade2, 60.0) * 1.5;

				color.R = clamp((shade + specular) * lightColor.R, 0.0, 1.0);
				color.G = clamp((shade + specular) * lightColor.G, 0.0, 1.0);
				color.B = clamp((shade + specular) * lightColor.B, 0.0, 1.0);

				// antialiasing
				opacity = clamp(radius - r2D, 0.0, 1.0) * 255;
			}
			else
			{
				color = {0.5, 0.5, 0.5};
				opacity = 0;
			}

			int address = y * size + x;
			image8[address] = sRGBA8(color.R * 255, color.G * 255, color.B * 255, opacity);
		}
	}
	update();
}
