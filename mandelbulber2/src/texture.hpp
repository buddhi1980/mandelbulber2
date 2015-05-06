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

#include "color_structures.hpp"
#include <QtCore>
#include <QImage>

class cTexture
{
public:
	cTexture(QString filename, bool beQuiet = false);
	cTexture();
	cTexture(const cTexture &tex);
	cTexture& operator=(const cTexture &tex);

	~cTexture(void);
	int Height(void) {return height;}
	int Width(void) {return width;}
	sRGB8 Pixel(double x, double y);
	sRGB8 FastPixel(int x, int y);
	bool IsLoaded(void) {return loaded;}
	QString GetFileName() {return originalFileName;}
	void FromQByteArray(QByteArray buffer);

private:
	sRGB8 Interpolation(double x, double y);
	sRGB8 *bitmap;
	int width;
	int height;
	bool loaded;
	QImage qimage;
	QString originalFileName;
};

#endif /* TEXTURE_HPP_ */
