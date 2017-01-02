/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016 Krzysztof Marczak        §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Sebastian Jennen (sebastian.jennen@gmx.de)
 *
 * file image class to store different image file formats
 *
 * Each imagefiletype derives ImageFileSave and implements the SaveImage
 * method to store the image data with the corresponding file format
 *
 */

#ifndef MANDELBULBER2_SRC_FILE_IMAGE_HPP_
#define MANDELBULBER2_SRC_FILE_IMAGE_HPP_

#include <QtCore>
#include "color_structures.hpp"

extern "C" {
#include <png.h>
}

/**********************************************************/
// source of libraries:
// libpng: http://www.libpng.org/pub/png/libpng.html
/**********************************************************/

// forward declarations
class cImage;

class ImageFileSave : public QObject
{
	Q_OBJECT

public:
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
		structSaveImageChannel()
				: contentType(enumImageContentType(0)),
					channelQuality(enumImageChannelQualityType(0)),
					postfix("")
		{
		}
		structSaveImageChannel(enumImageContentType _contentType,
			enumImageChannelQualityType _channelQuality, QString _postfix)
				: contentType(_contentType), channelQuality(_channelQuality), postfix(_postfix)
		{
		}

		enumImageContentType contentType;
		enumImageChannelQualityType channelQuality;
		QString postfix;
	};

	typedef QMap<enumImageContentType, structSaveImageChannel> ImageConfig;
	static QString ImageFileExtension(enumImageFileType imageFileType);
	static QString ImageChannelName(enumImageContentType imageContentType);
	static enumImageFileType ImageFileType(QString imageFileExtension);
	static ImageFileSave *create(
		QString filename, enumImageFileType filetype, cImage *image, ImageConfig imageConfig);
	virtual void SaveImage() = 0;
	virtual QString getJobName() = 0;

protected:
	QString filename;
	cImage *image;
	ImageConfig imageConfig;
	enumImageContentType currentChannelKey;
	int currentChannel;
	int totalChannel;

	ImageFileSave(QString filename, cImage *image, ImageConfig imageConfig);

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
};

class ImageFileSavePNG : public ImageFileSave
{
	Q_OBJECT
public:
	ImageFileSavePNG(QString filename, cImage *image, ImageConfig imageConfig)
			: ImageFileSave(filename, image, imageConfig)
	{
	}
	void SaveImage() override;
	QString getJobName() override { return tr("Saving %1").arg("PNG"); }
	static void SavePNG(
		QString filename, cImage *image, structSaveImageChannel imageChannel, bool appendAlpha = false);
	static void SavePNG16(QString filename, int width, int height, sRGB16 *image16);
	static void SaveFromTilesPNG16(const char *filename, int width, int height, int tiles);
	static bool SavePNGQtBlackAndWhite(QString filename, unsigned char *image, int width, int height);
};

class ImageFileSaveJPG : public ImageFileSave
{
	Q_OBJECT
public:
	ImageFileSaveJPG(QString filename, cImage *image, ImageConfig imageConfig)
			: ImageFileSave(filename, image, imageConfig)
	{
	}
	void SaveImage() override;
	QString getJobName() override { return tr("Saving %1").arg("JPG"); }
	static bool SaveJPEGQt(
		QString filename, unsigned char *image, int width, int height, int quality);
	static bool SaveJPEGQtGreyscale(
		QString filename, unsigned char *image, int width, int height, int quality);
};

#ifdef USE_TIFF
class ImageFileSaveTIFF : public ImageFileSave
{
	Q_OBJECT
public:
	ImageFileSaveTIFF(QString filename, cImage *image, ImageConfig imageConfig)
			: ImageFileSave(filename, image, imageConfig)
	{
	}
	void SaveImage() override;
	QString getJobName() override { return tr("Saving %1").arg("TIFF"); }
	static bool SaveTIFF(
		QString filename, cImage *image, structSaveImageChannel imageChannel, bool appendAlpha = false);
};
#endif /* USE_TIFF */

#ifdef USE_EXR
class ImageFileSaveEXR : public ImageFileSave
{
	Q_OBJECT
public:
	ImageFileSaveEXR(QString filename, cImage *image, ImageConfig imageConfig)
			: ImageFileSave(filename, image, imageConfig)
	{
	}
	void SaveImage() override;
	QString getJobName() override { return tr("Saving %1").arg("EXR"); }
	static void SaveEXR(QString filename, cImage *image,
		QMap<enumImageContentType, structSaveImageChannel> imageConfig);
};
#endif /* USE_EXR */

#endif /* MANDELBULBER2_SRC_FILE_IMAGE_HPP_ */
