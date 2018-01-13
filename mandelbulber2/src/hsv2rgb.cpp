/*
 * hsv2rgb.cpp
 *
 *  Created on: 13 sty 2018
 *      Author: krzysztof
 */

#include "hsv2rgb.h"

sRGBFloat Hsv2rgb(float hue, float sat, float val)
{
	sRGBFloat rgb;
	float h = hue / 60.0;
	int i = floor(hue / 60);
	float f = h - i;
	float p = val * (1 - sat);
	float q = val * (1 - (sat * f));
	float t = val * (1 - (sat * (1 - f)));
	switch (i)
	{
		case 0: rgb = sRGBFloat(val, t, p); break;
		case 1: rgb = sRGBFloat(q, val, p); break;
		case 2: rgb = sRGBFloat(p, val, t); break;
		case 3: rgb = sRGBFloat(p, q, val); break;
		case 4: rgb = sRGBFloat(t, p, val); break;
		case 5: rgb = sRGBFloat(val, p, q); break;
	}

	return rgb;
}
