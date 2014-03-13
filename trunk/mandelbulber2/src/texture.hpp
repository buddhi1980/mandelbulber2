/*********************************************************
 /                   MANDELBULBER
 / class for bitmap textures
 /
 /
 / author: Krzysztof Marczak
 / contact: buddhi1980@gmail.com
 / licence: GNU GPL v3.0
 /
 ********************************************************/

#ifndef TEXTURE_HPP_
#define TEXTURE_HPP_

#include "cimage.hpp"

class cTexture
{
public:
	cTexture(const char *filename);
	cTexture();
	~cTexture(void);
	int Height(void) {return height;}
	int Width(void) {return width;}
	sRGB8 Pixel(double x, double y);
	sRGB8 FastPixel(int x, int y);
	bool IsLoaded(void) {return loaded;}
private:
	sRGB8 Interpolation(double x, double y);
	sRGB8 *bitmap;
	int width;
	int height;
	bool loaded;
};

#endif /* TEXTURE_HPP_ */
