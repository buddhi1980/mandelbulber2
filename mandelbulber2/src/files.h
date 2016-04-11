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

#include <string>
#include "cimage.hpp"
#include "system.hpp"
#include "file_image.hpp"
#include <QtCore>

extern std::string logfileName;

std::string IndexFilename(const char* filename, const char* extension, int number);
bool FileIfExists(const char *filename);
int fcopy(const char *source, const char *dest);
std::string removeFileExtension(const std::string &filename);
void BufferNormalize16(sRGB16 *buffer, unsigned int size);
//void SaveAllImageLayers(const char *filename, cImage *image);
void SaveImage(QString filename, ImageFileSave::enumImageFileType filetype, cImage *image);

#endif /* FILES_H_ */

