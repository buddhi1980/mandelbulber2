/*
 * color_gradient.cpp
 *
 *  Created on: 1 cze 2019
 *      Author: krzysztof
 */

#include "color_gradient.h"
#include <algorithm> // std::sort
#include <QString>

cColorGradient::cColorGradient()
{
	sColor positionedColor = {sRGB8(255, 255, 255), 0.0};
	colors.append(positionedColor);

	sColor positionedColor2 = {sRGB8(255, 255, 255), 1.0};
	colors.append(positionedColor2);

	// third for testing
	sColor positionedColor3 = {sRGB8(255, 0, 0), 0.3};
	colors.append(positionedColor3);

	// fourth for testing
	sColor positionedColor4 = {sRGB8(0, 0, 0), 0.25};
	colors.append(positionedColor4);

	sorted = false;
}

cColorGradient::~cColorGradient() {}

int cColorGradient::AddColor(sRGB8 color, double position)
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

void cColorGradient::ModifyColor(int index, sRGB8 color)
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
	// FIXME prevent from having two colors at the same position;
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

sRGB8 cColorGradient::GetColor(double position, bool smooth)
{
	SortGradient();
	int paletteIndex = PaletteIterator(0, position);
	return Interpolate(paletteIndex, position, smooth);
}

sRGB8 cColorGradient::Interpolate(int paletteIndex, double pos, bool smooth)
{
	sRGB8 color;
	// if last element then just copy color value (no interpolation)
	if (paletteIndex == sortedColors.size() - 1)
	{
		color = sortedColors[paletteIndex - 1].color;
	}
	else
	{
		// interpolation
		sRGB8 color1 = sortedColors[paletteIndex].color;
		sRGB8 color2 = sortedColors[paletteIndex + 1].color;
		double pos1 = sortedColors[paletteIndex].position;
		double pos2 = sortedColors[paletteIndex + 1].position;
		// relative delta
		if (pos2 - pos1 > 0.0)
		{
			double delta = (pos - pos1) / (pos2 - pos1);

			if (smooth) delta = 0.5 * (1.0 - cos(delta * M_PI));

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

QVector<sRGB8> cColorGradient::GetGradient(int length, bool smooth)
{
	QVector<sRGB8> gradient;
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
			sRGB8 color = Interpolate(paletteIndex, pos, smooth);
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

QString cColorGradient::GetColorsAsString()
{
	SortGradient();
	QString string;

	// last color is not converted bcause is the same like first
	for (int i = 0; i < sortedColors.size() - 1; i++)
	{
		QString oneColor = QString("%1 %2%3%4")
												 .arg(int(sortedColors[i].position * 10000))
												 .arg(sortedColors[i].color.R / 256, 2, 16, QChar('0'))
												 .arg(sortedColors[i].color.G / 256, 2, 16, QChar('0'))
												 .arg(sortedColors[i].color.B / 256, 2, 16, QChar('0'));

		if (i > 0) string += " ";
		string += oneColor;
	}

	return string;
}

void cColorGradient::SetColorsFromString(const QString &string)
{
	QStringList split = string.split(" ");
	colors.clear();
	sorted = false;

	if (split.size() < 4)
	{
		sColor positionedColor = {sRGB8(255, 255, 255), 0.0};
		colors.append(positionedColor);

		sColor positionedColor2 = {sRGB8(255, 255, 255), 1.0};
		colors.append(positionedColor2);

		qCritical() << "Error! In gradient string shoud be at least two colors";
	}
	else
	{
		double position = 0.0;
		sRGB8 color;

		for (int i = 0; i < split.size(); i++)
		{
			if (split[i].size() > 0)
			{
				if (i % 2 == 0)
				{
					position = split[i].toInt() / 10000.0;
				}
				else
				{
					unsigned int colorHex = split[i].toInt(nullptr, 16);
					color.R = colorHex / 65536;
					color.G = (colorHex / 256) % 256;
					color.B = colorHex % 256;
					sColor colorPos = {color, position};
					colors.append(colorPos);

					if (i == 1)
					{
						sColor lastColor = colors.first();
						lastColor.position = 1.0;
						colors.append(lastColor);
					}
				}
			}
		}
	}
}
