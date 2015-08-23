/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cColorPalette class - color palette container
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

#include "color_palette.hpp"
#include <QtCore>
#include "common_math.h"
#include "random.hpp"

cColorPalette::cColorPalette()
{
	palette.clear();
	isInitialized = false;
	paletteSize = 0;
}

void cColorPalette::AppendColor(const sRGB &color)
{
	palette.append(color);
	isInitialized = true;
	paletteSize = palette.size();
}

void cColorPalette::ChangeColor(int index, const sRGB &color)
{
	if (index < paletteSize && index >= 0)
	{
		palette[index] = color;
	}
	else
	{
		qCritical() << "cColorPalette::ChangeColor(int index, const sRGB &color): wrong color index:" << index;
	}
}

cColorPalette::cColorPalette(int size, int randomSeed, double saturation)
{
	palette.clear();
	cRandom random;;
	random.Initialize(randomSeed);

	for (int i = 0; i < size; i++)
	{
		int Y = (random.Random(255) - 128) / (1.0 + saturation);
		sRGB color;
		color.R = Y + 128 + (random.Random(255) - 128) * saturation;
		color.G = Y + 128 + (random.Random(255) - 128) * saturation;
		color.B = Y + 128 + (random.Random(255) - 128) * saturation;
		if (color.R < 0) color.R = 0;
		if (color.G < 0) color.G = 0;
		if (color.B < 0) color.B = 0;
		if (color.R > 255) color.R = 255;
		if (color.G > 255) color.G = 255;
		if (color.B > 255) color.B = 255;
		palette.append(color);
	}
	paletteSize = palette.size();
	isInitialized = true;
}

sRGB cColorPalette::IndexToColour(int index) const
{
	sRGB colour(255, 255, 255);
	if (isInitialized)
	{
		double R1, R2, G1, G2, B1, B2;
		double RK, GK, BK;

		int col, delta;
		if (index < 0)
		{
			colour = palette.last();
		}
		else
		{
			col = (index / 256) % paletteSize;
			int colplus1 = (col + 1) % paletteSize;
			if (col < paletteSize)
			{
				R1 = palette[col].R;
				G1 = palette[col].G;
				B1 = palette[col].B;
				R2 = palette[colplus1].R;
				G2 = palette[colplus1].G;
				B2 = palette[colplus1].B;
				RK = (R2 - R1) / 256.0;
				GK = (G2 - G1) / 256.0;
				BK = (B2 - B1) / 256.0;
				delta = index % 256;
				colour.R = R1 + (RK * delta);
				colour.G = G1 + (GK * delta);
				colour.B = B1 + (BK * delta);
			}
		}
	}
	else
	{
		qCritical() << "Palette is not initialized";
	}
	return colour;
}

sRGB cColorPalette::GetColor(int index) const
{
	sRGB colour(255, 255, 255);
	if (isInitialized)
	{
		if(index >= 0 && index < paletteSize)
		{
			colour = palette[index];
		}
		else
		{
			qCritical() << "Palette index out of range";
		}
	}
	else
	{
		qCritical() << "Palette is not initialized";
	}
	return colour;
}
