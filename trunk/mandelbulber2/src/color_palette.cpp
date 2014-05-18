/*
 * color_palette.cpp
 *
 *  Created on: May 18, 2014
 *      Author: krzysztof
 */

#include "color_palette.hpp"
#include <QtCore>
#include "common_math.h"

cColorPalette::cColorPalette()
{
	for(int i=0; i<256; i++)
	{
		palette[i] = sRGB(0,0,0);
	}
	isIntialized = false;
}

cColorPalette::cColorPalette(int randomSeed, double saturation)
{
	srand(randomSeed);
	int R, G, B, Y;

	for (int i = 0; i < 255; i++)
	{
		Y = (Random(255) - 128) / (1.0 + saturation);
		palette[i].R = R = Y + 128 + (Random(255) - 128) * saturation;
		palette[i].G = G = Y + 128 + (Random(255) - 128) * saturation;
		palette[i].B = B = Y + 128 + (Random(255) - 128) * saturation;
		if (R < 0) palette[i].R = 0;
		if (G < 0) palette[i].G = 0;
		if (B < 0) palette[i].B = 0;
		if (R > 255) palette[i].R = 255;
		if (G > 255) palette[i].G = 255;
		if (B > 255) palette[i].B = 255;
	}
	isIntialized = true;
}

sRGB cColorPalette::IndexToColour(int index) const
{
	sRGB colour(255, 255, 255);
	if (isIntialized)
	{
		double R1, R2, G1, G2, B1, B2;
		double RK, GK, BK;

		int kol, delta;
		if (index < 0)
		{
			colour = palette[255];
		}
		else
		{
			index = index % (255 * 256);
			kol = index / 256;
			if (kol < 255)
			{
				R1 = palette[kol].R;
				G1 = palette[kol].G;
				B1 = palette[kol].B;
				R2 = palette[kol + 1].R;
				G2 = palette[kol + 1].G;
				B2 = palette[kol + 1].B;
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
	if (isIntialized)
	{
		if(index >= 0 && index < 256)
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
