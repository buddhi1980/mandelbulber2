/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * file operation functions
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

enum enumImageFileType
{
	IMAGE_FILE_TYPE_PNG = 0,
	IMAGE_FILE_TYPE_JPG = 1,
	IMAGE_FILE_TYPE_EXR = 2
};

enum enumImageContentType
{
	IMAGE_CONTENT_COLOR = 0,
	IMAGE_CONTENT_ALPHA = 1,
	IMAGE_CONTENT_ZBUFFER = 2
};

enum enumImageChannelQualityType
{
	IMAGE_CHANNEL_QUALITY_8 = 0,
	IMAGE_CHANNEL_QUALITY_16 = 1,
	IMAGE_CHANNEL_QUALITY_32 = 2
};

struct structSaveImageChannel {
	structSaveImageChannel() : contentType((enumImageContentType)0), channelQuality((enumImageChannelQualityType)0), postfix("") {}
	structSaveImageChannel(
		enumImageContentType _contentType,
		enumImageChannelQualityType _channelQuality,
		QString _postfix):
		contentType(_contentType), channelQuality(_channelQuality), postfix(_postfix) {}

	enumImageContentType contentType;
	enumImageChannelQualityType channelQuality;
	QString postfix;
};

std::string IndexFilename(const char* filename, const char* extension, int number);
//METHODDEF(void) my_error_exit(j_common_ptr cinfo);
void SavePNG(QString filename, int width, int height, png_byte *image);
void SavePNG16(QString filename, int width, int height, sRGB16* image16);
void SavePNG16Alpha(QString filename, int width, int height, cImage *image);
void SavePNG(QString filename, cImage *image, structSaveImageChannel imageChannel, bool appendAlpha = false);
void SaveFromTilesPNG16(const char *filename, int width, int height, int tiles);
bool FileIfExists(const char *filename);
int fcopy(const char *source, const char *dest);
void BufferNormalize16(sRGB16 *buffer, unsigned int size);
//void SaveAllImageLayers(const char *filename, cImage *image);
std::string removeFileExtension(const std::string &filename);

bool SaveJPEGQt(QString filename, unsigned char *image, int width, int height, int quality);
bool SaveJPEGQtGreyscale(QString filename, unsigned char *image, int width, int height, int quality);
void SaveZBuffer(QString filename, cImage *image);
void SaveZBuffer(QString filename, cImage *image, float minZ, float maxZ);

void SaveImage(QString filename, enumImageFileType filetype, cImage *image);
void SaveImage(QString filename, enumImageFileType filetype, cImage *image, QMap<enumImageContentType, structSaveImageChannel> imageConfig);
#ifdef USE_EXR
void SaveEXR(QString filename, cImage* image, QMap<enumImageContentType, structSaveImageChannel> imageConfig);
#endif /* USE_EXR */

#endif /* FILES_H_ */

