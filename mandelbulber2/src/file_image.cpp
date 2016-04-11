/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * file image class to store different image file formats
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
 * Authors: Sebastian Jennen (sebastian.jennen@gmx.de)
 */

#include "file_image.hpp"

ImageFileSave* ImageFileSave::create(QString filename,
	enumImageFileType filetype, cImage *image,
	ImageConfig imageConfig)
{
	switch(filetype){
		case IMAGE_FILE_TYPE_PNG:
			return new ImageFileSavePNG(filename, image, imageConfig);
		case IMAGE_FILE_TYPE_JPG:
			return new ImageFileSaveJPG(filename, image, imageConfig);
#ifdef USE_TIFF
		case IMAGE_FILE_TYPE_TIFF:
			return new ImageFileSaveTIFF(filename, image, imageConfig);
#endif /* USE_TIFF */
#ifdef USE_EXR
		case IMAGE_FILE_TYPE_EXR:
			return new ImageFileSaveEXR(filename, image, imageConfig);
#endif /* USE_EXR */
	}
}

void ImageFileSavePNG::SaveImage()
{
	qDebug() << "TODO";
}

void ImageFileSaveJPG::SaveImage()
{
	qDebug() << "TODO";
}

#ifdef USE_TIFF
void ImageFileSaveTIFF::SaveImage()
{
	qDebug() << "TODO";
}
#endif /* USE_TIFF */

#ifdef USE_EXR
void ImageFileSaveEXR::SaveImage()
{
	qDebug() << "TODO";
}
#endif /* USE_EXR */
