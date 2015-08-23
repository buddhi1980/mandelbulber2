/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * data structures for color representation
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen
 */

#ifndef COLOR_STRUCTURES_HPP_
#define COLOR_STRUCTURES_HPP_

template <typename T>
struct tsRGB {
	T R, G, B;
	tsRGB(T r, T g, T b) : R(r), G(g), B(b) {};
	tsRGB(){R = 0; G = 0; B = 0;}
};


template <typename T>
struct tsRGBA
{
		T R, G, B, A;
		tsRGBA(T r, T g, T b, T a) : R(r), G(g), B(b), A(a) {}
		tsRGBA() {R = 0; G = 0; B = 0; A = 0;}
		tsRGBA(tsRGB<T> rgb) : R(rgb.R), G(rgb.G), B(rgb.B), A(0) {}
};

// explicitly define constructor for rgba with regards to max value of Opacity
template <> inline tsRGBA<unsigned char>::tsRGBA(){R = 0; G = 0; B = 0, A = (unsigned char)255;}
template <> inline tsRGBA<unsigned short>::tsRGBA(){R = 0; G = 0; B = 0, A = (unsigned short)65535;}
template <> inline tsRGBA<float>::tsRGBA(){R = 0.0; G = 0.0; B = 0.0, A = 1.0;}

typedef tsRGB<unsigned char> sRGB8;
typedef tsRGB<unsigned short> sRGB16;
typedef tsRGB<float> sRGBfloat;
typedef tsRGB<int> sRGB;

typedef tsRGBA<unsigned char> sRGBA8;
typedef tsRGBA<unsigned short> sRGBA16;
typedef tsRGBA<float> sRGBAfloat;

#endif /* COLOR_STRUCTURES_HPP_ */
