/*
 * color_gradient.cpp
 *
 *  Created on: 1 cze 2019
 *      Author: krzysztof
 */

#include "color_gradient.h"

cColorGradient::cColorGradient()
{
	sColor positionedColor = {sRGB16(65535, 65535, 65535), 0.0};
	colors.append(positionedColor);
	positionedColor.position = 1.0;
	colors.append(positionedColor);
}

cColorGradient::~cColorGradient() {}

int cColorGradient::AddColor(sRGB16 color, double position)
{
	if (position >= 0.0 && position <= 1.0)
	{
		sColor positionedColor = {color, position};
		colors.append(positionedColor);
		return colors.size();
	}
	else
	{
		qCritical() << "Wrong color posotion in gradient!" << position;
		return 0;
	}
}

void cColorGradient::ModifyColor(int index, sRGB16 color)
{
	if (index < colors.size())
	{
		colors[index].color = color;
	}
	else
	{
		qCritical() << "color index is too high";
	}
}

void cColorGradient::ModifyPosition(int index, double position)
{
	if (position >= 0.0 && position <= 1.0)
	{
		if (index < colors.size())
		{
			colors[index].position = position;
		}
		else
		{
			qCritical() << "wrong color index";
		}
	}
	else
	{
		qCritical() << "Wrong color posotion in gradient!" << position;
	}
}

void cColorGradient::RemoveColor(int index)
{
	if (colors.size() > 2)
	{
		if (index < colors.size())
		{
		}
		else
		{
			qCritical() << "color index is too high";
		}
	}
	else
	{
		qCritical() << "there must be at least two colors";
	}
}

sRGB16 cColorGradient::GetColor(double position) {}

QVector<sRGB16> cColorGradient::GetGradient(int length) {}

QList<cColorGradient::sColor> cColorGradient::GetListOColors() {}

void cColorGradient::SortGradient() {}
