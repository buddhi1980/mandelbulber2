/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cTexture class - simple bitmap container with pixel interpolation
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

#include <stdio.h>
#include <cstring>
#include "texture.hpp"
#include "files.h"
#include "system.hpp"
#include "error_message.hpp"

//constructor
cTexture::cTexture(QString filename)
{
	bitmap = NULL;
	qimage.load(filename);
	qimage = qimage.convertToFormat(QImage::Format_RGB888);

	if (!qimage.isNull())
	{
		bitmap = (sRGB8*)(qimage.bits());
		width = qimage.width();
		height = qimage.height();
		loaded = true;
		originalFileName = filename;
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("Can't load texture!\n") + filename, cErrorMessage::errorMessage);
		width = 100;
		height = 100;
		loaded = false;
		bitmap = new sRGB8[100 * 100];
		memset(bitmap,255,sizeof(sRGB8)*100*100);
	}
}

void cTexture::FromQByteArray(QByteArray buffer)
{
	// TODO
}

cTexture::cTexture(void)
{
	width = 100;
	height = 100;
	loaded = false;
	bitmap = new sRGB8[100 * 100];
	memset(bitmap,255,sizeof(sRGB8)*100*100);
}

//destructor
cTexture::~cTexture(void)
{
	if(qimage.isNull())
	{
		delete[] bitmap;
	}
}

//read pixel
sRGB8 cTexture::Pixel(double x, double y)
{
	sRGB8 black = sRGB8(0,0,0);
	if (x >= 0 && x < width && y >= 0 && y < height - 1.0)
	{
		return Interpolation(x, y);
	}
	else
	{
		return black;
	}
}

sRGB8 cTexture::Interpolation(double x, double y)
{
	sRGB8 kolor;
	int ix = x;
	int iy = y;
	double rx = (x - (int) x);
	double ry = (y - (int) y);
	sRGB8 k1 = bitmap[iy * width + ix];
	sRGB8 k2 = bitmap[iy * width + ix + 1];
	sRGB8 k3 = bitmap[(iy + 1) * width + ix];
	sRGB8 k4 = bitmap[(iy + 1) * width + ix + 1];
	kolor.R = (k1.R * (1.0 - rx) * (1.0 - ry) + k2.R * (rx) * (1.0 - ry) + k3.R * (1.0 - rx) * ry + k4.R * (rx * ry));
	kolor.G = (k1.G * (1.0 - rx) * (1.0 - ry) + k2.G * (rx) * (1.0 - ry) + k3.G * (1.0 - rx) * ry + k4.G * (rx * ry));
	kolor.B = (k1.B * (1.0 - rx) * (1.0 - ry) + k2.B * (rx) * (1.0 - ry) + k3.B * (1.0 - rx) * ry + k4.B * (rx * ry));
	return kolor;
}

sRGB8 cTexture::FastPixel(int x, int y)
{
	return bitmap[x + y * width];
}
