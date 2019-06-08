/*
 * color_gradient.cpp
 *
 *  Created on: 1 cze 2019
 *      Author: krzysztof
 */

#include "color_gradient.h"
#include <algorithm> // std::sort

cColorGradient::cColorGradient()
{
	sColor positionedColor = {sRGB16(65535, 65535, 65535), 0.0};
	colors.append(positionedColor);

	sColor positionedColor2 = {sRGB16(65535, 65535, 65535), 1.0};
	colors.append(positionedColor2);

	// third for testing
	sColor positionedColor3 = {sRGB16(65535, 0, 0), 0.3};
	colors.append(positionedColor3);

	// fourth for testing
	sColor positionedColor4 = {sRGB16(0, 0, 0), 0.25};
	colors.append(positionedColor4);

	sorted = false;
}

cColorGradient::~cColorGradient() {}

int cColorGradient::AddColor(sRGB16 color, double position)
{
	if (position >= 0.0 && position <= 1.0)
	{
		sorted = false;
		sColor positionedColor = {color, position};
		colors.append(positionedColor);
		return colors.size();
	}
	else
	{
		qCritical() << "Wrong color position in gradient!" << position;
		return 0;
	}
}

void cColorGradient::ModifyColor(int index, sRGB16 color)
{
	if (index < colors.size())
	{
		sorted = false;
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
			sorted = false;
			colors[index].position = position;
		}
		else
		{
			qCritical() << "wrong color index";
		}
	}
	else
	{
		qCritical() << "Wrong color position in gradient!" << position;
	}
}

void cColorGradient::RemoveColor(int index)
{
	if (colors.size() > 2)
	{
		if (index < colors.size())
		{
			sorted = false;
			colors.removeAt(index);
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

int cColorGradient::PaletteIterator(int paletteIndex, double colorPosition)
{
	int newIndex = paletteIndex;
	while (newIndex < sortedColors.size() - 1 && colorPosition > sortedColors[newIndex + 1].position)
	{
		newIndex++;
	}
	return newIndex;
}

sRGB16 cColorGradient::GetColor(double position)
{
	SortGradient();
	int paletteIndex = PaletteIterator(0, position);
	return Interpolate(paletteIndex, position);
}

sRGB16 cColorGradient::Interpolate(int paletteIndex, double pos)
{
	sRGB16 color;
	// if last element then just copy color value (no interpolation)
	if (paletteIndex == sortedColors.size() - 1)
	{
		color = sortedColors[paletteIndex - 1].color;
	}
	else
	{
		// interpolation
		sRGB16 color1 = sortedColors[paletteIndex].color;
		sRGB16 color2 = sortedColors[paletteIndex + 1].color;
		double pos1 = sortedColors[paletteIndex].position;
		double pos2 = sortedColors[paletteIndex + 1].position;
		// relative delta
		if (pos2 - pos1 > 0.0)
		{
			double delta = (pos - pos1) / (pos2 - pos1);
			double nDelta = 1.0 - delta;
			color.R = color1.R * nDelta + color2.R * delta;
			color.G = color1.G * nDelta + color2.G * delta;
			color.B = color1.B * nDelta + color2.B * delta;
		}
		else
		{
			qCritical() << "Wrong sequence of colors";
			color = color1;
		}
	}
	return color;
}

QVector<sRGB16> cColorGradient::GetGradient(int length)
{
	QVector<sRGB16> gradient;
	if (length >= 2)
	{
		SortGradient();

		gradient.reserve(length);

		int paletteIndex = 0;

		double step = 1.0 / (length - 1);
		for (int i = 0; i < length; i++)
		{
			double pos = i * step;
			paletteIndex = PaletteIterator(paletteIndex, pos);
			sRGB16 color = Interpolate(paletteIndex, pos);
			gradient.append(color);
		}
	}
	else
	{
		qCritical() << "Wrong length of gradient";
	}
	return gradient;
}

QList<cColorGradient::sColor> cColorGradient::GetListOfColors()
{
	return colors;
}

void cColorGradient::SortGradient()
{
	if (!sorted)
	{
		sortedColors = colors;
		std::sort(sortedColors.begin(), sortedColors.end(), sColor::lessCompare);
		sorted = true;
	}
}
