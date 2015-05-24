/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * file opration functions
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

#ifndef FILES_H_
#define FILES_H_

extern "C"
{
#include <png.h>
}


/**********************************************************/
//source of libraries:
//libpng: http://www.libpng.org/pub/png/libpng.html
/**********************************************************/

#include <string>
#include "cimage.hpp"
#include "system.hpp"
#include <QtCore>

extern std::string logfileName;

enum enumImageType
{
	IMAGE_TYPE_JPG,
	IMAGE_TYPE_PNG,
	IMAGE_TYPE_PNG_16,
	IMAGE_TYPE_PNG_16_WITH_ALPHA,
};


std::string IndexFilename(const char* filename, const char* extension, int number);
//METHODDEF(void) my_error_exit(j_common_ptr cinfo);
void SavePNG(QString filename, int width, int height, png_byte *image);
void SavePNG16(QString filename, int width, int height, sRGB16* image16);
void SavePNG16Alpha(QString filename, int width, int height, cImage *image);
void SaveFromTilesPNG16(const char *filename, int width, int height, int tiles);
bool FileIfExists(const char *filename);
int fcopy(const char *source, const char *dest);
void BufferNormalize16(sRGB16 *buffer, unsigned int size);
//void SaveAllImageLayers(const char *filename, cImage *image);
std::string removeFileExtension(const std::string &filename);
void SaveMainImage(QString filename, enumImageType imageType);
bool SaveJPEGQt(QString filename, unsigned char *image, int width, int height, int quality);
void SaveZBuffer(QString filename, cImage *image);

#endif /* FILES_H_ */
