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

#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#include <qbytearray.h>
#include <qlist.h>
#include <qstring.h>

#include "algebra.hpp"
#include "color_structures.hpp"

class cTexture
{
public:
	enum enumUseMipmaps
	{
		doNotUseMipmaps,
		useMipmaps
	};

	cTexture(QString filename, enumUseMipmaps mode, bool beQuiet = false);
	cTexture();
	cTexture(const cTexture &tex);
	cTexture& operator=(const cTexture &tex);

	~cTexture(void);
	int Height(void) {return height;}
	int Width(void) {return width;}
	sRGBfloat Pixel(double x, double y, double pixelSize = 0.0);
	sRGBfloat Pixel(CVector2<double> point, double pixelSize = 0.0) const;
	sRGB8 FastPixel(int x, int y);
	bool IsLoaded(void) const {return loaded;}
	QString GetFileName() {return originalFileName;}
	void FromQByteArray(QByteArray *buffer, enumUseMipmaps mode);
	CVector3 NormalMapFromBumpMap(CVector2<double> point, double bump, double pixelSize = 0.0) const;
	CVector3 NormalMap(CVector2<double> point, double bump, double pixelSize = 0.0) const;
	void SetInvertGreen(bool invert) {invertGreen = invert;}

private:
	sRGB8 LinearInterpolation(double x, double y);
	sRGBfloat BicubicInterpolation(double x, double y, const sRGB8 *bitmap, int w, int h) const;
	sRGBfloat MipMap(double x, double y, double pixelSize) const;
	void CreateMipMaps();
	inline int WrapInt(int a, int size) {return (a + size) % size;}
	sRGB8 *bitmap;
	int width;
	int height;
	bool loaded;
	QString originalFileName;
	QList<QVector<sRGB8 >>mipmaps;
	QList<CVector2<int> >mipmapSizes;
	bool invertGreen;
};

#endif /* TEXTURE_HPP_ */


