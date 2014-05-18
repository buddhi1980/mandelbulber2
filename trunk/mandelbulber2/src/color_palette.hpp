/*
 * palette.hpp
 *
 *  Created on: May 18, 2014
 *      Author: krzysztof
 */

#ifndef COLOR_PALETTE_HPP_
#define COLOR_PALETTE_HPP_

#include "color_structures.hpp"

class cColorPalette
{
public:
	cColorPalette();
	cColorPalette(int randomSeed, double saturation);
	sRGB IndexToColour(int index) const;
	sRGB GetColor(int index) const;

private:
	sRGB palette[256];
	bool isIntialized;
};



#endif /* COLOR_PALETTE_HPP_ */
