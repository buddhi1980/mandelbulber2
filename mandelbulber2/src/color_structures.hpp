/*
 * color_structures.hpp
 *
 *  Created on: Apr 26, 2014
 *      Author: krzysztof
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
