/*
 * palette.hpp
 *
 *  Created on: May 18, 2014
 *      Author: krzysztof
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
	bool IsInitialized() {return isIntialized;}

private:
	QVector<sRGB> palette;
	bool isIntialized;
	int paletteSize;
};



#endif /* COLOR_PALETTE_HPP_ */
