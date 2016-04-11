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

#ifndef FILE_IMAGE_H_
#define FILE_IMAGE_H_

#include "cimage.hpp"
#include "files.h"
#include <QtCore>

class ImageFileSave : public QObject
{
Q_OBJECT
	protected:
	enum enumImageFileType
	{
		IMAGE_FILE_TYPE_PNG = 0,
		IMAGE_FILE_TYPE_JPG = 1,
		IMAGE_FILE_TYPE_EXR = 2,
		IMAGE_FILE_TYPE_TIFF = 3
	};

	enum enumImageContentType
	{
		// rgb color
		IMAGE_CONTENT_COLOR = 0,

		// alpha channel / opacity
		IMAGE_CONTENT_ALPHA = 1,

		// depth of the image pixels, distance to the spectator
		IMAGE_CONTENT_ZBUFFER = 2,

		// direction vector of the surface normal with vector (x,y,z) ~> (r,g,b)
		// x E[-1,1], y E[-1,1], z E[0,1] and rgb channels xyz mapped to full range of selected quality
		// see reference Normal maps at
		// https://www.blender.org/manual/render/blender_render/textures/influence/material/bump_and_normal.html
		IMAGE_CONTENT_NORMAL = 3
	};

	enum enumImageChannelQualityType
	{
		IMAGE_CHANNEL_QUALITY_8 = 0,
		IMAGE_CHANNEL_QUALITY_16 = 1,
		IMAGE_CHANNEL_QUALITY_32 = 2
	};

	struct structSaveImageChannel
	{
		structSaveImageChannel() :
						contentType((enumImageContentType) 0),
						channelQuality((enumImageChannelQualityType) 0),
						postfix("")
		{
		}
		structSaveImageChannel(enumImageContentType _contentType,
				enumImageChannelQualityType _channelQuality, QString _postfix) :
				contentType(_contentType), channelQuality(_channelQuality), postfix(_postfix)
		{
		}

		enumImageContentType contentType;
		enumImageChannelQualityType channelQuality;
		QString postfix;
	};

	QString filename;
	cImage* image;
	typedef QMap<enumImageContentType, structSaveImageChannel> ImageConfig;
	ImageConfig imageConfig;

	static ImageFileSave* create(QString filename,
		enumImageFileType filetype, cImage *image,
		ImageConfig imageConfig);

	ImageFileSave(QString filename, cImage *image,
		ImageConfig imageConfig)
	{
		this->filename = filename;
		this->image = image;
		this->imageConfig = imageConfig;
	}

	virtual void SaveImage() = 0;
};

class ImageFileSavePNG : public ImageFileSave
{
public:
	ImageFileSavePNG(QString filename, cImage *image,
		ImageConfig imageConfig)
		: ImageFileSave(filename, image, imageConfig){}
	void SaveImage();
};

class ImageFileSaveJPG : public ImageFileSave
{
public:
	ImageFileSaveJPG(QString filename, cImage *image,
		ImageConfig imageConfig)
		: ImageFileSave(filename, image, imageConfig){}
	void SaveImage();
};

#ifdef USE_TIFF
class ImageFileSaveTIFF : public ImageFileSave
{
public:
	ImageFileSaveTIFF(QString filename, cImage *image,
		ImageConfig imageConfig)
		: ImageFileSave(filename, image, imageConfig){}
	void SaveImage();
};
#endif /* USE_TIFF */

#ifdef USE_EXR
class ImageFileSaveEXR : public ImageFileSave
{
public:
	ImageFileSaveEXR(QString filename, cImage *image,
		ImageConfig imageConfig)
		: ImageFileSave(filename, image, imageConfig){}
	void SaveImage();
};
#endif /* USE_EXR */


#endif /* FILE_IMAGE_H_ */

