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

#ifndef COLOR_PALETTE_HPP_
#define COLOR_PALETTE_HPP_

#include "color_structures.hpp"
#include <QtCore>

class cColorPalette
{
public:
	cColorPalette();
	cColorPalette(int size, int randomSeed, double saturation);
	void AppendColor(const sRGB &color);
	void ChangeColor(int index, const sRGB &color);
	sRGB IndexToColour(int index) const;
	sRGB GetColor(int index) const;
	int GetSize() {return paletteSize;}
	bool IsInitialized() {return isInitialized;}

private:
	QVector<sRGB> palette;
	bool isInitialized;
	int paletteSize;
};



#endif /* COLOR_PALETTE_HPP_ */
