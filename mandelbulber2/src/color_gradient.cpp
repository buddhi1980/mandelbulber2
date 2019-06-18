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
	sColor positionedColor = {sRGB(255, 255, 255), 0.0};
	colors.append(positionedColor);

	sColor positionedColor2 = {sRGB(255, 255, 255), 1.0};
	colors.append(positionedColor2);

	sorted = false;
}

cColorGradient::~cColorGradient() {}

int cColorGradient::AddColor(sRGB color, double position)
{
	sorted = false;
	position = CorrectPosition(position, -1);
	sColor positionedColor = {color, position};
	colors.append(positionedColor);
	return colors.size();
}

void cColorGradient::ModifyColor(int index, sRGB color)
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
	position = CorrectPosition(position, index);

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

sRGB cColorGradient::GetColor(double position, bool smooth)
{
	SortGradient();
	int paletteIndex = PaletteIterator(0, position);
	return Interpolate(paletteIndex, position, smooth);
}

sRGB cColorGradient::Interpolate(int paletteIndex, double pos, bool smooth)
{
	sRGB color;
	// if last element then just copy color value (no interpolation)
	if (paletteIndex == sortedColors.size() - 1)
	{
		color = sortedColors[paletteIndex - 1].color;
	}
	else
	{
		// interpolation
		sRGB color1 = sortedColors[paletteIndex].color;
		sRGB color2 = sortedColors[paletteIndex + 1].color;
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

QVector<sRGB> cColorGradient::GetGradient(int length, bool smooth)
{
	QVector<sRGB> gradient;
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
			sRGB color = Interpolate(paletteIndex, pos, smooth);
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
												 .arg(sortedColors[i].color.R, 2, 16, QChar('0'))
												 .arg(sortedColors[i].color.G, 2, 16, QChar('0'))
												 .arg(sortedColors[i].color.B, 2, 16, QChar('0'));

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
		sColor positionedColor = {sRGB(255, 255, 255), 0.0};
		colors.append(positionedColor);

		sColor positionedColor2 = {sRGB(255, 255, 255), 1.0};
		colors.append(positionedColor2);

		qCritical() << "Error! In gradient string shoud be at least two colors";
	}
	else
	{
		double position = 0.0;
		sRGB color;

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
					position = CorrectPosition(position, -1);
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

double cColorGradient::CorrectPosition(double position, int ignoreIndex)
{
	position = qBound(0.0, position, 1.0);
	bool positionIncorrect = false;

	bool rightDirection = (position < 0.5) ? true : false;

	do
	{
		for (int i = 0; i < colors.size(); i++)
		{
			if (i == ignoreIndex) continue;
			if (abs(position - colors[i].position) < 0.0001)
			{
				positionIncorrect = true;
				if (rightDirection)
				{
					position += 0.0001;
				}
				else
				{
					position -= 0.0001;
				}
			}
			else
			{
				positionIncorrect = false;
			}
		}
	} while (positionIncorrect);

	return position;
}
