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
#include "common_math.h"

//constructor
cTexture::cTexture(QString filename, bool beQuiet)
{
	bitmap = NULL;
	QImage qimage;
	qimage.load(filename);
	qimage = qimage.convertToFormat(QImage::Format_RGB888);

	if (!qimage.isNull())
	{
		width = qimage.width();
		height = qimage.height();
		bitmap = new sRGB8[width * height];

		for(int y = 0; y < height; y++)
		{
			memcpy(&bitmap[y * width], qimage.scanLine(y), sizeof(sRGB8) * width);
		}

		//qDebug() << "cTexture::cTexture(QString filename, bool beQuiet): (sRGB8*)(qimage.bits());:" << width * height * sizeof(sRGB8);
		loaded = true;
		originalFileName = filename;
	}
	else
	{
		if (!beQuiet) cErrorMessage::showMessage(QObject::tr("Can't load texture!\n") + filename,
																						 cErrorMessage::errorMessage);
		width = 100;
		height = 100;
		loaded = false;
		bitmap = new sRGB8[100 * 100];
		memset(bitmap, 255, sizeof(sRGB8) * 100 * 100);
		//qDebug() << "cTexture::cTexture(QString filename, bool beQuiet): new sRGB8[100 * 100];:" << width * height * sizeof(sRGB8);
	}
}

//copy constructor
cTexture::cTexture(const cTexture &tex)
{
	width = tex.width;
	height = tex.height;
	loaded = tex.loaded;
	originalFileName = tex.originalFileName;
	bitmap = new sRGB8[width * height];
	//qDebug() << "cTexture::cTexture(const cTexture &tex):  new sRGB8[width * height]:" << width * height * sizeof(sRGB8);
	memcpy(bitmap, tex.bitmap, sizeof(sRGB8) * width * height);
}

cTexture& cTexture::operator=(const cTexture &tex)
{
	if (bitmap)
	{
		delete[] bitmap;
		bitmap = NULL;
		//qDebug() << "cTexture& cTexture::operator=(const cTexture &tex): 	delete[] bitmap;:" << width * height * sizeof(sRGB8);
	}
	width = tex.width;
	height = tex.height;
	loaded = tex.loaded;
	originalFileName = tex.originalFileName;
	bitmap = new sRGB8[width * height];
	//qDebug() << "cTexture& cTexture::operator=(const cTexture &tex): 	new sRGB8[width * height];:" << width * height * sizeof(sRGB8);
	memcpy(bitmap, tex.bitmap, sizeof(sRGB8) * width * height);

	return *this;
}

void cTexture::FromQByteArray(QByteArray buffer)
{
	if (bitmap)
	{
		//qDebug() << "void cTexture::FromQByteArray(QByteArray buffer): delete[] bitmap;:" << width * height * sizeof(sRGB8);
		delete[] bitmap;
	}

	bitmap = NULL;
	QImage qimage(buffer);
	qimage.loadFromData(buffer);
	qimage = qimage.convertToFormat(QImage::Format_RGB888);
	if (!qimage.isNull())
	{
		width = qimage.width();
		height = qimage.height();
		bitmap = new sRGB8[width * height];
		memcpy(bitmap, qimage.constBits(), sizeof(sRGB8) * width * height);
		//qDebug() << "void cTexture::FromQByteArray(QByteArray buffer): (sRGB8*)(qimage.bits()):" << width * height * sizeof(sRGB8);
		loaded = true;
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("Can't load texture from QByteArray!\n"),
															 cErrorMessage::errorMessage);
	}
}

cTexture::cTexture(void)
{
	width = 100;
	height = 100;
	loaded = false;
	bitmap = new sRGB8[100 * 100];
	memset(bitmap, 255, sizeof(sRGB8) * 100 * 100);
	//qDebug() << "cTexture::cTexture(void): new sRGB8[100 * 100]" << width * height * sizeof(sRGB8);
}

//destructor
cTexture::~cTexture(void)
{
	if (bitmap)
	{
		delete[] bitmap;
		bitmap = NULL;
		//qDebug() << "cTexture::~cTexture(void): delete[] bitmap:" << width * height * sizeof(sRGB8);
	}
	else
	{
		qCritical() << "cTexture::~cTexture(void): bitmap was not allocated before";
	}
}

//read pixel
sRGBfloat cTexture::Pixel(double x, double y)
{
	if (x >= 0 && x < width && y >= 0 && y < height - 1.0)
	{
		return BicubicInterpolation(x, y);
	}
	else
	{
		return sRGBfloat(0.0, 0.0, 0.0);
	}
}

sRGBfloat cTexture::Pixel(CVector2<double> point) const
{
	int intX = point.x;
	int intY = point.y;
	point.x = point.x - intX;
	point.y = point.y - intY;
	if(point.x < 0.0) point.x += 1.0;
	if(point.y < 0.0) point.y += 1.0;
	point.x *= (double)width;
	point.y *= (double)height;
	return BicubicInterpolation(point.x, point.y);
}

sRGB8 cTexture::LinearInterpolation(double x, double y)
{
	sRGB8 color;
	int ix = x;
	int iy = y;
	double rx = (x - (int) x);
	double ry = (y - (int) y);
	sRGB8 k1 = bitmap[iy * width + ix];
	sRGB8 k2 = bitmap[iy * width + ix + 1];
	sRGB8 k3 = bitmap[(iy + 1) * width + ix];
	sRGB8 k4 = bitmap[(iy + 1) * width + ix + 1];
	color.R = (k1.R * (1.0 - rx) * (1.0 - ry) + k2.R * (rx) * (1.0 - ry) + k3.R * (1.0 - rx) * ry
			+ k4.R * (rx * ry));
	color.G = (k1.G * (1.0 - rx) * (1.0 - ry) + k2.G * (rx) * (1.0 - ry) + k3.G * (1.0 - rx) * ry
			+ k4.G * (rx * ry));
	color.B = (k1.B * (1.0 - rx) * (1.0 - ry) + k2.B * (rx) * (1.0 - ry) + k3.B * (1.0 - rx) * ry
			+ k4.B * (rx * ry));
	return color;
}

sRGBfloat cTexture::BicubicInterpolation(double x, double y) const
{
	int ix = x;
	int iy = y;
	double rx = (x - ix);
	double ry = (y - iy);

	double R[4][4], G[4][4], B[4][4];

	for(int yy = 0; yy < 4; yy++)
	{
		for(int xx = 0; xx < 4; xx++)
		{
			int ixx = ix + xx - 1;
			int iyy = iy + yy - 1;
			ixx = (ixx + width) % width;
			iyy = (iyy + height) % height;
			int addess2 = ixx + iyy * width;
			sRGB8 pixel = bitmap[addess2];
			R[xx][yy] = pixel.R;
			G[xx][yy] = pixel.G;
			B[xx][yy] = pixel.B;
		}
	}

	double dR = bicubicInterpolate(R, rx, ry);
	double dG = bicubicInterpolate(G, rx, ry);
	double dB = bicubicInterpolate(B, rx, ry);
	if(dR < 0) dR = 0; if(dR > 255) dR = 255;
	if(dG < 0) dG = 0; if(dG > 255) dG = 255;
	if(dB < 0) dB = 0; if(dB > 255) dB = 255;

	return sRGBfloat(dR/256.0, dG/256.0, dB/256.0);
}

sRGB8 cTexture::FastPixel(int x, int y)
{
	return bitmap[x + y * width];
}

CVector3 cTexture::NormalMapFromBumpMap(CVector2<double> point, double bump) const
{
	int intX = point.x;
	int intY = point.y;
	point.x = point.x - intX;
	point.y = point.y - intY;
	if(point.x < 0.0) point.x += 1.0;
	if(point.y < 0.0) point.y += 1.0;

	double m[3][3];
	for(int y = 0; y<=2; y++)
	{
		for(int x = 0; x<=2; x++)
		{
			m[x][y] = BicubicInterpolation(point.x*width + x - 1.0, point.y*height + y - 1.0).R;
		}
	}
	CVector3 normal;
	normal.x = bump * (m[2][2]-m[0][2]+2*(m[2][1]-m[0][1])+m[2][0]-m[0][0]);
	normal.y = bump * (m[0][0]-m[0][2]+2*(m[1][0]-m[1][2])+m[2][0]-m[2][2]);
	normal.z = 1.0;
	normal.Normalize();
	return normal;
}

CVector3 cTexture::NormalMap(CVector2<double> point, double bump) const
{
	int intX = point.x;
	int intY = point.y;
	point.x = point.x - intX;
	point.y = point.y - intY;
	if(point.x < 0.0) point.x += 1.0;
	if(point.y < 0.0) point.y += 1.0;
	sRGBfloat normalPixel = BicubicInterpolation(point.x * width, point.y * height);
	CVector3 normal(normalPixel.R * 2.0 - 1.0, normalPixel.G * 2.0 - 1.0, normalPixel.B);
	normal.x *= -bump;
	normal.y *= -bump;
	normal.Normalize();

	return normal;
}


