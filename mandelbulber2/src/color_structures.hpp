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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */


#ifndef COLOR_STRUCTURES_HPP_
#define COLOR_STRUCTURES_HPP_

struct sRGB8
{
	unsigned char R;
	unsigned char G;
	unsigned char B;
	sRGB8(unsigned char r, unsigned char g, unsigned char b) : R(r), G(g), B(b) {};
	sRGB8(){R = 0; G = 0; B = 0;}
};

struct sRGB16
{
	unsigned short R;
	unsigned short G;
	unsigned short B;
	sRGB16(unsigned short r, unsigned short g, unsigned short b) : R(r), G(g), B(b) {};
	sRGB16(){R = 0; G = 0; B = 0;}
};

struct sRGBfloat
{
	float R;
	float G;
	float B;
	sRGBfloat(float r, float g, float b) : R(r), G(g), B(b) {};
	sRGBfloat(){R = 0.0; G = 0.0; B = 0.0;}
};

struct sRGBAfloat
{
	float R;
	float G;
	float B;
	float A;
	sRGBAfloat(float r, float g, float b, float a) : R(r), G(g), B(b), A(a) {};
	sRGBAfloat(){R = 0.0; G = 0.0; B = 0.0; A = 1.0;}
};

struct sRGB
{
	int R;
	int G;
	int B;
	sRGB(int r, int g, int b) : R(r), G(g), B(b) {};
	sRGB(){R = 0; G = 0; B = 0;}
};

struct sImageRGBA16
{
	unsigned short R;
	unsigned short G;
	unsigned short B;
	unsigned short A;
};

struct sRGBA8
{
	unsigned char B;
	unsigned char G;
	unsigned char R;
	unsigned char A;
	sRGBA8(unsigned char r, unsigned char g, unsigned char b, unsigned char a) : B(b), G(g), R(r), A(a) {};
	sRGBA8(){R = 0; G = 0; B = 0; A = 255;}
};

#endif /* COLOR_STRUCTURES_HPP_ */
