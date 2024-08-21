/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * file image class to store different image file formats
 *
 * Each image file type derives ImageFileSave and implements the SaveImage
 * method to store the image data with the corresponding file format
 */

#include "file_image.hpp"

// custom includes
#ifdef USE_EXR
#include <ImfAttribute.h>
#include <ImfChannelList.h>
#include <ImfFrameBuffer.h>
#include <ImfInputFile.h>
#include <ImfHeader.h>
#include <ImfOutputFile.h>
#include <ImfStringAttribute.h>
#include <half.h>
#endif // USE_EXR
// #include <libpng16/pngconf.h>
#include <QByteArray>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QImage>
#include <QIODevice>
#include <QMap>
#include <QStringList>
#include <QVector>
#include <memory>
#include "common_math.h"
#include "cimage.hpp"
#include "error_message.hpp"
#include "files.h"
#include "initparameters.hpp"
#include "parameters.hpp"
#include "write_log.hpp"
// custom includes
#ifdef USE_TIFF
#include "tiff.h"
#include "tiffio.h"
#endif // USE_TIFF

#define PNG_DEBUG 3

const uint64_t ImageFileSave::SAVE_CHUNK_SIZE;

ImageFileSave::ImageFileSave(
	QString filename, std::shared_ptr<cImage> image, ImageConfig imageConfig)
{
	this->filename = filename;
	this->image = image;
	this->imageConfig = imageConfig;
	currentChannel = 0;
	totalChannel = 0;
	currentChannelKey = IMAGE_CONTENT_COLOR;
}

std::shared_ptr<ImageFileSave> ImageFileSave::create(QString filename, enumImageFileType fileType,
	std::shared_ptr<cImage> image, ImageConfig imageConfig)
{
	switch (fileType)
	{
		case IMAGE_FILE_TYPE_PNG:
			return std::shared_ptr<ImageFileSave>(new ImageFileSavePNG(filename, image, imageConfig));
		case IMAGE_FILE_TYPE_JPG:
			return std::shared_ptr<ImageFileSave>(new ImageFileSaveJPG(filename, image, imageConfig));
#ifdef USE_TIFF
		case IMAGE_FILE_TYPE_TIFF:
			return std::shared_ptr<ImageFileSave>(new ImageFileSaveTIFF(filename, image, imageConfig));
#endif /* USE_TIFF */
#ifdef USE_EXR
		case IMAGE_FILE_TYPE_EXR:
			return std::shared_ptr<ImageFileSave>(new ImageFileSaveEXR(filename, image, imageConfig));
#endif /* USE_EXR */
		default: qCritical() << "fileType " << ImageFileExtension(fileType) << " not supported!";
	}

	return nullptr;
}

QString ImageFileSave::ImageFileExtension(enumImageFileType imageFileType)
{
	switch (imageFileType)
	{
		case IMAGE_FILE_TYPE_JPG: return "jpg";
		case IMAGE_FILE_TYPE_PNG: return "png";
		case IMAGE_FILE_TYPE_EXR: return "exr";
		case IMAGE_FILE_TYPE_TIFF: return "tiff";
	}
	return "";
}

QString ImageFileSave::ImageChannelName(enumImageContentType imageContentType)
{
	switch (imageContentType)
	{
		case IMAGE_CONTENT_COLOR: return "color";
		case IMAGE_CONTENT_ALPHA: return "alpha";
		case IMAGE_CONTENT_ZBUFFER: return "zbuffer";
		case IMAGE_CONTENT_NORMAL: return "normal";
		case IMAGE_CONTENT_NORMAL_WORLD: return "normalWorld";
		case IMAGE_CONTENT_SPECULAR: return "specular";
		case IMAGE_CONTENT_DIFFUSE: return "diffuse";
		case IMAGE_CONTENT_WORLD_POSITION: return "world";
		case IMAGE_CONTENT_SHADOWS: return "shadow_channel";
		case IMAGE_CONTENT_GLOBAL_ILLUMINATION: return "gi_channel";
		case IMAGE_CONTENT_NOT_DENOISED: return "not_denoised_channel";
	}
	return "";
}

QStringList ImageFileSave::ImageChannelNames()
{
	return QStringList({"color", "alpha", "zbuffer", "normal", "specular", "diffuse", "world",
		"normalWorld", "shadow_channel", "gi_channel", "not_denoised_channel"});
}

ImageFileSave::enumImageFileType ImageFileSave::ImageFileType(QString imageFileExtension)
{
	if (imageFileExtension == "jpg" || imageFileExtension == "jpeg")
		return IMAGE_FILE_TYPE_JPG;
	else if (imageFileExtension == "png")
		return IMAGE_FILE_TYPE_PNG;
	else if (imageFileExtension == "exr")
		return IMAGE_FILE_TYPE_EXR;
	else if (imageFileExtension == "tiff" || imageFileExtension == "tif")
		return IMAGE_FILE_TYPE_TIFF;
	else
		return IMAGE_FILE_TYPE_JPG;
}

QString ImageFileSave::ImageNameWithoutExtension(QString path)
{
	QFileInfo fi(path);
	QString fileName = fi.completeBaseName();
	if (fi.suffix() != ""
			&& !QStringList({"jpg", "jpeg", "png", "exr", "tiff", "tif"}).contains(fi.suffix()))
	{
		fileName += "." + fi.suffix();
	}
	return fi.path() + QDir::separator() + fileName;
}

void ImageFileSave::updateProgressAndStatusChannel(double progress)
{
	emit updateProgressAndStatus(getJobName(),
		QObject::tr("Saving channel: %1").arg(ImageChannelName(currentChannelKey)),
		(1.0 * currentChannel / totalChannel) + (progress / totalChannel));
}
void ImageFileSave::updateProgressAndStatusStarted()
{
	emit updateProgressAndStatus(getJobName(), QObject::tr("Started"), 0.0);
}
void ImageFileSave::updateProgressAndStatusFinished()
{
	emit updateProgressAndStatus(getJobName(), QObject::tr("Finished"), 1.0);
}

QString ImageFileSave::CreateFullFileNameAndMakeDir(const QString &filename,
	enumImageContentType contentType, const QString &postfix, const QString extension)
{
	QString fullFilename;
	if (gPar->Get<bool>("save_channels_in_separate_folders") && contentType != IMAGE_CONTENT_COLOR)
	{
		QFileInfo fileInfo(filename);
		QDir dir = fileInfo.absoluteDir();
		QString onlyFileName = fileInfo.fileName();

		if (dir.exists())
		{
			QString newDirName =
				QDir::toNativeSeparators(dir.absolutePath() + QDir::separator() + postfix);
			if (QDir(newDirName).exists())
			{
				fullFilename =
					QDir::toNativeSeparators(newDirName + QDir::separator() + onlyFileName + "." + extension);
			}
			else
			{
				if (dir.mkdir(newDirName))
				{
					fullFilename = QDir::toNativeSeparators(
						newDirName + QDir::separator() + onlyFileName + "." + extension);
				}
				else
				{
					cErrorMessage::showMessage(
						QObject::tr("Cannot create directory for image!\n") + newDirName,
						cErrorMessage::errorMessage);
				}
			}
		}
		else
		{
			cErrorMessage::showMessage(
				QObject::tr("Directory for new image is not accessible!\n") + dir.absolutePath(),
				cErrorMessage::errorMessage);
		}
	}
	else
	{
		fullFilename = QDir::toNativeSeparators(filename + postfix + "." + extension);
	}
	return fullFilename;
}

QStringList ImageFileSavePNG::SaveImage()
{
	updateProgressAndStatusStarted();

	QStringList listOfSavedFiles;

	bool appendAlpha = gPar->Get<bool>("append_alpha_png")
										 && imageConfig.contains(IMAGE_CONTENT_COLOR)
										 && imageConfig.contains(IMAGE_CONTENT_ALPHA);
	if (hasAppendAlphaCustom) appendAlpha = appendAlphaCustom;

	currentChannel = 0;
	totalChannel = imageConfig.size();
	for (ImageConfig::iterator channel = imageConfig.begin(); channel != imageConfig.end(); ++channel)
	{
		currentChannelKey = channel.key();

		QString fullFilename =
			CreateFullFileNameAndMakeDir(filename, currentChannelKey, channel.value().postfix, "png");

		emit updateProgressAndStatus(getJobName(),
			QObject::tr("Saving channel: %1").arg(ImageChannelName(currentChannelKey)),
			1.0 * currentChannel / totalChannel);

		switch (currentChannelKey)
		{
			case IMAGE_CONTENT_COLOR:
			{
				SavePNG(fullFilename, image, channel.value(), appendAlpha);
				listOfSavedFiles.append(fullFilename);
				break;
			}
			case IMAGE_CONTENT_ALPHA:
			{
				if (!appendAlpha)
				{
					SavePNG(fullFilename, image, channel.value());
					listOfSavedFiles.append(fullFilename);
				}
				break;
			}
			case IMAGE_CONTENT_ZBUFFER:
			case IMAGE_CONTENT_NORMAL:
			case IMAGE_CONTENT_NORMAL_WORLD:
			case IMAGE_CONTENT_SPECULAR:
			case IMAGE_CONTENT_DIFFUSE:
			case IMAGE_CONTENT_WORLD_POSITION:
			case IMAGE_CONTENT_SHADOWS:
			case IMAGE_CONTENT_GLOBAL_ILLUMINATION:
			case IMAGE_CONTENT_NOT_DENOISED:
			default:
			{
				SavePNG(fullFilename, image, channel.value());
				listOfSavedFiles.append(fullFilename);
				break;
			}
		}
		currentChannel++;
	}
	updateProgressAndStatusFinished();

	return listOfSavedFiles;
}

QStringList ImageFileSaveJPG::SaveImage()
{
	updateProgressAndStatusStarted();

	QStringList listOfSavedFiles;

	currentChannel = 0;
	totalChannel = imageConfig.size();
	for (ImageConfig::iterator channel = imageConfig.begin(); channel != imageConfig.end(); ++channel)
	{
		currentChannelKey = channel.key();

		QString fullFilename =
			CreateFullFileNameAndMakeDir(filename, currentChannelKey, channel.value().postfix, "jpg");

		listOfSavedFiles.append(fullFilename);

		emit updateProgressAndStatus(getJobName(),
			QObject::tr("Saving channel: %1").arg(ImageChannelName(currentChannelKey)),
			1.0 * currentChannel / totalChannel);

		switch (currentChannelKey)
		{
			case IMAGE_CONTENT_COLOR:
				SaveJPEGQt(fullFilename, image->ConvertTo8bitChar(), int(image->GetWidth()),
					int(image->GetHeight()), gPar->Get<int>("jpeg_quality"), image->getMeta());
				break;
			case IMAGE_CONTENT_ALPHA:
				SaveJPEGQtGreyscale(fullFilename, image->ConvertAlphaTo8bit().data(),
					int(image->GetWidth()), int(image->GetHeight()), gPar->Get<int>("jpeg_quality"),
					image->getMeta());
				break;
			case IMAGE_CONTENT_ZBUFFER:
			{
				std::vector<float> &zbuffer = image->GetZBuffer();
				quint64 size = image->GetWidth() * image->GetHeight();

				bool invertZ = gPar->Get<bool>("zbuffer_invert");
				bool logarithmicScale = gPar->Get<bool>("zbuffer_logarithmic");
				bool constRange = gPar->Get<bool>("zbuffer_constant_range");

				std::vector<unsigned char> zBuffer8Bit(size);
				float minZ = float(1.0e50);
				float maxZ = 0.0;

				if (constRange)
				{
					minZ = gPar->Get<double>("zbuffer_min_depth");
					maxZ = gPar->Get<double>("zbuffer_max_depth");
				}
				else
				{
					for (quint64 i = 0; i < size; i++)
					{
						float z = zbuffer[i];
						if (z > maxZ && z < 1e19f) maxZ = z;
						if (z < minZ) minZ = z;
					}
				}

				float kZ = log(maxZ / minZ);

				for (quint64 y = 0; y < image->GetHeight(); y++)
				{
					for (quint64 x = 0; x < image->GetWidth(); x++)
					{
						quint64 ptr = (x + y * image->GetWidth());
						float z = image->GetPixelZBuffer(x, y);
						float z1;
						if (logarithmicScale)
						{
							z1 = log(z / minZ) / kZ;
						}
						else
						{
							z1 = (z - minZ) / (maxZ - minZ);
						}
						if (z1 < 0) z1 = 0.0;
						if (z1 > 1.0) z1 = 1.0;
						int intZ = int(z1 * 254);
						if (z > 1e19f) intZ = 255;
						zBuffer8Bit[ptr] = (invertZ) ? uchar(255 - intZ) : uchar(intZ);
					}
				}
				SaveJPEGQtGreyscale(fullFilename, zBuffer8Bit.data(), image->GetWidth(), image->GetHeight(),
					gPar->Get<int>("jpeg_quality"), image->getMeta());

				break;
			}
			case IMAGE_CONTENT_NORMAL:
			case IMAGE_CONTENT_NORMAL_WORLD:
			case IMAGE_CONTENT_SPECULAR:
			case IMAGE_CONTENT_DIFFUSE:
			case IMAGE_CONTENT_WORLD_POSITION:
			case IMAGE_CONTENT_SHADOWS:
			case IMAGE_CONTENT_GLOBAL_ILLUMINATION:
			case IMAGE_CONTENT_NOT_DENOISED:
				SaveJPEGQt32(fullFilename, channel.value(), int(image->GetWidth()), int(image->GetHeight()),
					gPar->Get<int>("jpeg_quality"), image->getMeta());
				break;
			default: qWarning() << "Unknown channel for JPG"; break;
		}
		currentChannel++;
	}
	updateProgressAndStatusFinished();

	return listOfSavedFiles;
}

#ifdef USE_TIFF
QStringList ImageFileSaveTIFF::SaveImage()
{
	updateProgressAndStatusStarted();

	QStringList listOfSavedFiles;

	bool appendAlpha = gPar->Get<bool>("append_alpha_png")
										 && imageConfig.contains(IMAGE_CONTENT_COLOR)
										 && imageConfig.contains(IMAGE_CONTENT_ALPHA);

	currentChannel = 0;
	totalChannel = imageConfig.size();
	for (ImageConfig::iterator channel = imageConfig.begin(); channel != imageConfig.end(); ++channel)
	{
		currentChannelKey = channel.key();

		QString fullFilename =
			CreateFullFileNameAndMakeDir(filename, currentChannelKey, channel.value().postfix, "tiff");

		listOfSavedFiles.append(fullFilename);

		emit updateProgressAndStatus(getJobName(),
			QObject::tr("Saving channel: %1").arg(ImageChannelName(currentChannelKey)),
			1.0 * currentChannel / totalChannel);

		switch (currentChannelKey)
		{
			case IMAGE_CONTENT_COLOR: SaveTIFF(fullFilename, image, channel.value(), appendAlpha); break;
			case IMAGE_CONTENT_ALPHA:
				if (!appendAlpha) SaveTIFF(fullFilename, image, channel.value());
				break;
			case IMAGE_CONTENT_ZBUFFER:
			case IMAGE_CONTENT_NORMAL:
			case IMAGE_CONTENT_NORMAL_WORLD:
			case IMAGE_CONTENT_SPECULAR:
			case IMAGE_CONTENT_DIFFUSE:
			case IMAGE_CONTENT_WORLD_POSITION:
			case IMAGE_CONTENT_SHADOWS:
			case IMAGE_CONTENT_GLOBAL_ILLUMINATION:
			case IMAGE_CONTENT_NOT_DENOISED:
			default: SaveTIFF(fullFilename, image, channel.value()); break;
		}
		currentChannel++;
	}
	updateProgressAndStatusFinished();

	return listOfSavedFiles;
}
#endif /* USE_TIFF */

#ifdef USE_EXR
QStringList ImageFileSaveEXR::SaveImage()
{
	updateProgressAndStatusStarted();

	QStringList listOfSavedFiles;

	QString fullFilename = filename + ".exr";
	listOfSavedFiles.append(fullFilename);

	SaveEXR(fullFilename, image, imageConfig);
	updateProgressAndStatusFinished();

	return listOfSavedFiles;
}
#endif /* USE_EXR */

void ImageFileSavePNG::SavePNG(QString filenameInput, std::shared_ptr<cImage> image,
	structSaveImageChannel imageChannel, bool appendAlpha)
{
	uint64_t width = image->GetWidth();
	uint64_t height = image->GetHeight();

	/* create file */
	FILE *fp = fopen(filenameInput.toLocal8Bit().constData(), "wb");
	png_structp png_ptr = nullptr;
	png_info *info_ptr = nullptr;

	try
	{
		if (!fp) throw QString("[write_png_file] File %s could not be opened for writing.");

		/* initialize stuff */
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

		if (!png_ptr) throw QString("[write_png_file] png_create_write_struct failed");

		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) throw QString("[write_png_file] png_create_info_struct failed");

		if (setjmp(png_jmpbuf(png_ptr))) throw QString("[write_png_file] Error during init_io");

		png_init_io(png_ptr, fp);

		/* write header */
		if (setjmp(png_jmpbuf(png_ptr))) throw QString("[write_png_file] Error during writing header");

		if (imageChannel.channelQuality != IMAGE_CHANNEL_QUALITY_8
				&& imageChannel.channelQuality != IMAGE_CHANNEL_QUALITY_16)
		{
			// for PNG no more than 16 bit per channel possible
			imageChannel.channelQuality = IMAGE_CHANNEL_QUALITY_16;
		}

		int qualitySize;
		switch (imageChannel.channelQuality)
		{
			case IMAGE_CHANNEL_QUALITY_8: qualitySize = 8; break;
			case IMAGE_CHANNEL_QUALITY_16: qualitySize = 16; break;
			default: qualitySize = 8; break;
		}
		int qualitySizeByte = qualitySize / 8;

		int colorType;
		switch (imageChannel.contentType)
		{
			case IMAGE_CONTENT_COLOR:
				colorType = appendAlpha ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB;
				break;
			case IMAGE_CONTENT_ALPHA:
			case IMAGE_CONTENT_ZBUFFER: colorType = PNG_COLOR_TYPE_GRAY; break;
			case IMAGE_CONTENT_NORMAL: colorType = PNG_COLOR_TYPE_RGB; break;
			case IMAGE_CONTENT_NORMAL_WORLD: colorType = PNG_COLOR_TYPE_RGB; break;
			case IMAGE_CONTENT_SPECULAR: colorType = PNG_COLOR_TYPE_RGB; break;
			case IMAGE_CONTENT_DIFFUSE: colorType = PNG_COLOR_TYPE_RGB; break;
			case IMAGE_CONTENT_WORLD_POSITION: colorType = PNG_COLOR_TYPE_RGB; break;
			case IMAGE_CONTENT_SHADOWS: colorType = PNG_COLOR_TYPE_RGB; break;
			case IMAGE_CONTENT_GLOBAL_ILLUMINATION: colorType = PNG_COLOR_TYPE_RGB; break;
			case IMAGE_CONTENT_NOT_DENOISED: colorType = PNG_COLOR_TYPE_RGB; break;
			default: colorType = PNG_COLOR_TYPE_RGB; break;
		}

		png_set_IHDR(png_ptr, info_ptr, width, height, qualitySize, colorType, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		png_write_info(png_ptr, info_ptr);
		png_set_swap(png_ptr);

		/* write bytes */
		if (setjmp(png_jmpbuf(png_ptr))) throw QString("[write_png_file] Error during writing bytes");

		std::vector<png_bytep> row_pointers(height);

		uint64_t pixelSize = qualitySizeByte;

		switch (imageChannel.contentType)
		{
			case IMAGE_CONTENT_COLOR: pixelSize *= appendAlpha ? 4 : 3; break;
			case IMAGE_CONTENT_ALPHA: pixelSize *= 1; break;
			case IMAGE_CONTENT_ZBUFFER: pixelSize *= 1; break;
			case IMAGE_CONTENT_NORMAL: pixelSize *= 3; break;
			case IMAGE_CONTENT_NORMAL_WORLD: pixelSize *= 3; break;
			case IMAGE_CONTENT_SPECULAR: pixelSize *= 3; break;
			case IMAGE_CONTENT_DIFFUSE: pixelSize *= 3; break;
			case IMAGE_CONTENT_WORLD_POSITION: pixelSize *= 3; break;
			case IMAGE_CONTENT_SHADOWS: pixelSize *= 3; break;
			case IMAGE_CONTENT_GLOBAL_ILLUMINATION: pixelSize *= 3; break;
			case IMAGE_CONTENT_NOT_DENOISED: pixelSize *= 3; break;
		}

		bool directOnBuffer = false;
		if (imageChannel.contentType == IMAGE_CONTENT_COLOR && !appendAlpha) directOnBuffer = true;
		if (imageChannel.contentType == IMAGE_CONTENT_ALPHA) directOnBuffer = true;

		std::vector<char> colorPtr;

		if (directOnBuffer)
		{
			char *directPointer = nullptr;
			switch (imageChannel.contentType)
			{
				case IMAGE_CONTENT_COLOR:
				{
					if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
					{
						directPointer = reinterpret_cast<char *>(image->GetImage16().data());
					}
					else
					{
						directPointer = reinterpret_cast<char *>(image->ConvertTo8bitChar());
					}
				}
				break;
				case IMAGE_CONTENT_ALPHA:
				{
					if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
					{
						directPointer = reinterpret_cast<char *>(image->GetAlphaBuf().data());
					}
					else
					{
						directPointer = reinterpret_cast<char *>(image->ConvertAlphaTo8bit().data());
					}
				}
				break;
				case IMAGE_CONTENT_ZBUFFER:
				case IMAGE_CONTENT_NORMAL:
				case IMAGE_CONTENT_NORMAL_WORLD:
				case IMAGE_CONTENT_SPECULAR:
				case IMAGE_CONTENT_DIFFUSE:
				case IMAGE_CONTENT_WORLD_POSITION:
				case IMAGE_CONTENT_SHADOWS:
				case IMAGE_CONTENT_GLOBAL_ILLUMINATION:
				case IMAGE_CONTENT_NOT_DENOISED:
					// zbuffer and normals are float, so direct buffer write is not applicable
					break;
			}

			for (uint64_t y = 0; y < height; y++)
			{
				row_pointers[y] = reinterpret_cast<png_byte *>(&directPointer[y * width * pixelSize]);
			}
		}
		else
		{
			colorPtr.resize(uint64_t(width) * height * pixelSize);

			// calculate min / max values from zbuffer range
			float minZ = float(1.0e50);
			float maxZ = 0.0;
			if (imageChannel.contentType == IMAGE_CONTENT_ZBUFFER)
			{
				std::vector<float> &zbuffer = image->GetZBuffer();
				uint64_t size = width * height;

				bool constRange = gPar->Get<bool>("zbuffer_constant_range");

				if (constRange)
				{
					minZ = gPar->Get<double>("zbuffer_min_depth");
					maxZ = gPar->Get<double>("zbuffer_max_depth");
				}
				else
				{
					for (uint64_t i = 0; i < size; i++)
					{
						float z = zbuffer[i];
						if (z > maxZ && z < 1e19) maxZ = z;
						if (z < minZ) minZ = z;
					}
				}
			}
			float kZ = log(maxZ / minZ);
			bool zLogarithmicScale = gPar->Get<bool>("zbuffer_logarithmic");
			bool invertZ = gPar->Get<bool>("zbuffer_invert");

			for (uint64_t y = 0; y < height; y++)
			{
				for (uint64_t x = 0; x < width; x++)
				{
					uint64_t ptr = (x + y * width) * pixelSize;
					switch (imageChannel.contentType)
					{
						case IMAGE_CONTENT_COLOR:
						{
							if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
							{
								if (appendAlpha)
								{
									sRGBA16 *typedColorPtr = reinterpret_cast<sRGBA16 *>(&colorPtr[ptr]);
									*typedColorPtr = sRGBA16(image->GetPixelImage16(x, y));
									typedColorPtr->A = image->GetPixelAlpha(x, y);
								}
							}
							else
							{
								if (appendAlpha)
								{
									if (x == 0 && y == 0)
									{
										image->ConvertAlphaTo8bit();
										image->ConvertTo8bitChar();
									}
									sRGBA8 *typedColorPtr = reinterpret_cast<sRGBA8 *>(&colorPtr[ptr]);
									*typedColorPtr = sRGBA8(image->GetPixelImage8(x, y));
									typedColorPtr->A = image->GetPixelAlpha8(x, y);
								}
							}
						}
						break;
						case IMAGE_CONTENT_ALPHA:
							// all alpha savings to PNG happen directly on buffers in cimage
							break;
						case IMAGE_CONTENT_ZBUFFER:
						{
							if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
							{
								float z = image->GetPixelZBuffer(x, y);
								float z1;
								if (zLogarithmicScale)
								{
									z1 = log(z / minZ) / kZ;
								}
								else
								{
									z1 = (z - minZ) / (maxZ - minZ);
								}
								if (z1 < 0) z1 = 0.0;
								if (z1 > 1.0) z1 = 1.0;
								int intZ = int(z1 * 65534);
								if (z > 1e19f) intZ = 65535;

								if (invertZ) intZ = 65535 - intZ;

								unsigned short *typedColorPtr = reinterpret_cast<unsigned short *>(&colorPtr[ptr]);
								*typedColorPtr = static_cast<unsigned short>(intZ);
							}
							else
							{
								float z = image->GetPixelZBuffer(x, y);
								float z1;
								if (zLogarithmicScale)
								{
									z1 = log(z / minZ) / kZ;
								}
								else
								{
									z1 = (z - minZ) / (maxZ - minZ);
								}
								if (z1 < 0) z1 = 0.0;
								if (z1 > 1.0) z1 = 1.0;
								int intZ = int(z1 * 254);
								if (z > 1e19) intZ = 255;

								if (invertZ) intZ = 255 - intZ;

								unsigned char *typedColorPtr = reinterpret_cast<unsigned char *>(&colorPtr[ptr]);
								*typedColorPtr = static_cast<unsigned char>(intZ);
							}
						}
						break;
						case IMAGE_CONTENT_NORMAL:
							SavePngRgbPixel(imageChannel, &colorPtr[ptr], image->GetPixelNormal(x, y), false);
							break;
						case IMAGE_CONTENT_NORMAL_WORLD:
							SavePngRgbPixel(imageChannel, &colorPtr[ptr], image->GetPixelNormalWorld(x, y), true);
							break;
						case IMAGE_CONTENT_SPECULAR:
							SavePngRgbPixel(imageChannel, &colorPtr[ptr], image->GetPixelSpecular(x, y), false);
							break;
						case IMAGE_CONTENT_DIFFUSE:
							SavePngRgbPixel(imageChannel, &colorPtr[ptr], image->GetPixelDiffuse(x, y), false);
							break;
						case IMAGE_CONTENT_WORLD_POSITION:
							SavePngRgbPixel(imageChannel, &colorPtr[ptr], image->GetPixelWorld(x, y), false);
							break;
						case IMAGE_CONTENT_SHADOWS:
							SavePngRgbPixel(imageChannel, &colorPtr[ptr], image->GetPixelShadows(x, y), false);
							break;
						case IMAGE_CONTENT_GLOBAL_ILLUMINATION:
							SavePngRgbPixel(
								imageChannel, &colorPtr[ptr], image->GetPixelGlobalIllumination(x, y), false);
							break;
						case IMAGE_CONTENT_NOT_DENOISED:
							SavePngRgbPixel(
								imageChannel, &colorPtr[ptr], image->GetPixelNotDenoised(x, y), false);
							break;
					}
				}
				row_pointers[y] = reinterpret_cast<png_byte *>(&colorPtr[y * width * pixelSize]);
			}
		}

		// png_write_image(png_ptr, row_pointers);
		for (uint64_t r = 0; r < height; r += SAVE_CHUNK_SIZE)
		{
			uint64_t currentChunkSize = std::min(height - r, SAVE_CHUNK_SIZE);
			png_write_rows(png_ptr, png_bytepp(&row_pointers[r]), currentChunkSize);
			updateProgressAndStatusChannel(1.0 * r / height);
		}

		/* end write */
		if (setjmp(png_jmpbuf(png_ptr))) throw QString("[write_png_file] Error during end of write");

		png_write_end(png_ptr, info_ptr);
		png_destroy_write_struct(&png_ptr, &info_ptr);

		fclose(fp);
	}
	catch (QString &status)
	{
		if (png_ptr)
		{
			if (info_ptr)
			{
				png_destroy_write_struct(&png_ptr, &info_ptr);
			}
			else
			{
				png_destroy_write_struct(&png_ptr, nullptr);
			}
		}
		if (fp) fclose(fp);
		cErrorMessage::showMessage(
			QObject::tr("Can't save image to PNG file!\n") + filenameInput + "\n" + status,
			cErrorMessage::errorMessage);
	}
}

void ImageFileSavePNG::SavePNG16(QString filename, int width, int height, sRGB16 *image16)
{
	/* create file */
	FILE *fp = fopen(filename.toLocal8Bit().constData(), "wb");
	png_structp png_ptr = nullptr;
	png_info *info_ptr = nullptr;

	try
	{
		if (!fp) throw QString("[write_png_file] File %s could not be opened for writing.");

		/* initialize stuff */
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

		if (!png_ptr) throw QString("[write_png_file] png_create_write_struct failed");

		info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr) throw QString("[write_png_file] png_create_info_struct failed");

		if (setjmp(png_jmpbuf(png_ptr))) throw QString("[write_png_file] Error during init_io");

		png_init_io(png_ptr, fp);

		/* write header */
		if (setjmp(png_jmpbuf(png_ptr))) throw QString("[write_png_file] Error during writing header");

		png_set_IHDR(png_ptr, info_ptr, width, height, 16, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		png_write_info(png_ptr, info_ptr);
		png_set_swap(png_ptr);

		/* write bytes */
		if (setjmp(png_jmpbuf(png_ptr))) throw QString("[write_png_file] Error during writing bytes");

		std::vector<png_bytep> row_pointers(height);
		for (int y = 0; y < height; y++)
		{
			row_pointers[y] = reinterpret_cast<png_byte *>(&image16[y * width]);
		}

		png_write_image(png_ptr, row_pointers.data());

		/* end write */
		if (setjmp(png_jmpbuf(png_ptr))) throw QString("[write_png_file] Error during end of write");

		png_write_end(png_ptr, info_ptr);
		png_destroy_write_struct(&png_ptr, &info_ptr);

		fclose(fp);
	}
	catch (QString &status)
	{
		if (png_ptr)
		{
			if (info_ptr)
			{
				png_destroy_write_struct(&png_ptr, &info_ptr);
			}
			else
			{
				png_destroy_write_struct(&png_ptr, nullptr);
			}
		}

		if (fp) fclose(fp);
		cErrorMessage::showMessage(
			QObject::tr("Can't save image to PNG file!\n") + filename + "\n" + status,
			cErrorMessage::errorMessage);
	}
}

void ImageFileSavePNG::SaveFromTilesPNG16(const char *filename, int width, int height, int tiles)
{
	/* create file */
	std::string filenamePNG(filename);
	filenamePNG += "_fromTiles.png";
	FILE *fp = fopen(filenamePNG.c_str(), "wb");
	if (!fp)
	{
		fprintf(stderr, "[write_png_file] File %s could not be opened for writing", filename);
		return;
	}

	/* initialize stuff */
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

	if (!png_ptr)
	{
		fprintf(stderr, "[write_png_file] png_create_write_struct failed");
		fclose(fp);
		return;
	}

	png_info *info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		fprintf(stderr, "[write_png_file] png_create_info_struct failed");
		fclose(fp);
		return;
	}

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during init_io");
		fclose(fp);
		return;
	}

	png_init_io(png_ptr, fp);

	/* write header */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during writing header");
		fclose(fp);
		return;
	}

	png_set_IHDR(png_ptr, info_ptr, width * tiles, height * tiles, 16, PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

	png_write_info(png_ptr, info_ptr);
	png_set_swap(png_ptr);

	/* write bytes */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during writing bytes");
		fclose(fp);
		return;
	}

	FILE **files = new FILE *[tiles];

	sRGB16 *rowBuffer = new sRGB16[width * tiles];

	for (int tileRow = 0; tileRow < tiles; tileRow++)
	{
		printf("Compiling image from tiles, row %d\n", tileRow);
		for (int tile = 0; tile < tiles; tile++)
		{
			int fileNumber = tile + tileRow * tiles;
			std::string filename2 = IndexFilename(filename, "tile", fileNumber);
			files[tile] = fopen(filename2.c_str(), "rb");
		}

		for (int y = 0; y < height; y++)
		{
			for (int tile = 0; tile < tiles; tile++)
			{
				size_t result = fread(&rowBuffer[tile * width], 1, sizeof(sRGB16) * width, files[tile]);
				if (result != sizeof(sRGB16) * width)
				{
					printf("Reading error of image tile files");
					for (int tile2 = 0; tile2 < tiles; tile2++)
						fclose(files[tile2]);
					delete[] rowBuffer;
					delete[] files;
					return;
				}
			}
			png_write_rows(png_ptr, reinterpret_cast<png_bytep *>(&rowBuffer), 1);
		}

		for (int tile = 0; tile < tiles; tile++)
		{
			fclose(files[tile]);
			int fileNumber = tile + tileRow * tiles;
			std::string filename2 = IndexFilename(filename, "tile", fileNumber);
			remove(filename2.c_str());
		}
	}

	/* end write */
	if (setjmp(png_jmpbuf(png_ptr)))
	{
		fprintf(stderr, "[write_png_file] Error during end of write");
		delete[] rowBuffer;
		delete[] files;
		fclose(fp);
		return;
	}

	png_write_end(png_ptr, info_ptr);
	png_destroy_write_struct(&png_ptr, &info_ptr);
	delete[] rowBuffer;
	delete[] files;

	fclose(fp);
}

void ImageFileSavePNG::SavePngRgbPixel(
	structSaveImageChannel imageChannel, char *colorPtr, sRGBFloat pixel, bool signedInput)
{
	if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16
			|| imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_32)
	{
		sRGB16 *typedColorPtr = reinterpret_cast<sRGB16 *>(colorPtr);
		if (signedInput)
		{
			*typedColorPtr = sRGB16(ushort(clamp((1.0f + pixel.R) * 0.5f * 65535.0f, 0.0f, 65535.0f)),
				ushort(clamp((1.0f + pixel.G) * 0.5f * 65535.0f, 0.0f, 65535.0f)),
				ushort(clamp((1.0f + pixel.B) * 0.5f * 65535.0f, 0.0f, 65535.0f)));
		}
		else
		{
			*typedColorPtr = sRGB16(ushort(clamp(pixel.R * 65535.0f, 0.0f, 65535.0f)),
				ushort(clamp(pixel.G * 65535.0f, 0.0f, 65535.0f)),
				ushort(clamp(pixel.B * 65535.0f, 0.0f, 65535.0f)));
		}
	}
	else
	{
		sRGB8 *typedColorPtr = reinterpret_cast<sRGB8 *>(colorPtr);
		if (signedInput)
		{
			*typedColorPtr = sRGB8(uchar(clamp((1.0f + pixel.R) * 0.5f * 255.0f, 0.0f, 255.0f)),
				uchar(clamp((1.0f + pixel.G) * 0.5f * 255.0f, 0.0f, 255.0f)),
				uchar(clamp((1.0f + pixel.B) * 0.5f * 255.0f, 0.0f, 255.0f)));
		}
		else
		{
			*typedColorPtr = sRGB8(uchar(clamp(pixel.R * 255.0f, 0.0f, 255.0f)),
				uchar(clamp(pixel.G * 255.0f, 0.0f, 255.0f)), uchar(clamp(pixel.B * 255.0f, 0.0f, 255.0f)));
		}
	}
}

bool ImageFileSaveJPG::SaveJPEGQt(QString filename, unsigned char *image, int width, int height,
	int quality, QMap<QString, QString> meta)
{
	if (!image)
	{
		WriteLog(
			"ImageFileSaveJPG::SaveJPEGQt(): "
			"the image is a null pointer, this might be the case for optional channel(s). "
			"If this is the case, just rerender the image with enabled channel(s).",
			1);
		return false;
	}

	std::unique_ptr<QImage> qImage(new QImage(width, height, QImage::Format_RGB888));
	QMapIterator<QString, QString> i(meta);
	while (i.hasNext())
	{
		i.next();
		qImage->setText(i.key(), i.value());
	}

	for (int line = 0; line < height; line++)
	{
		unsigned char *linePointer = &image[line * width * 3];
		unsigned char *qScanLine = qImage->scanLine(line);
		memcpy(qScanLine, linePointer, sizeof(unsigned char) * 3 * width);
	}

	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	bool result = qImage->save(&file, "JPEG", quality);
	if (!result)
	{
		cErrorMessage::showMessage(
			QObject::tr("Can't save image to JPEG file!\n") + filename + "\n" + file.errorString(),
			cErrorMessage::errorMessage);
	}
	file.close();

	return result;
}

bool ImageFileSaveJPG::SaveJPEGQt32(QString filename, structSaveImageChannel imageChannel,
	int width, int height, int quality, QMap<QString, QString> meta)
{

	if (!image)
	{
		WriteLog(
			"ImageFileSaveJPG::SaveJPEGQt(): "
			"the image is a null pointer, this might be the case for optional channel(s). "
			"If this is the case, just rerender the image with enabled channel(s).",
			1);
		return false;
	}

	std::unique_ptr<QImage> qImage(new QImage(width, height, QImage::Format_RGB888));
	QMapIterator<QString, QString> i(meta);
	while (i.hasNext())
	{
		i.next();
		qImage->setText(i.key(), i.value());
	}
	for (int y = 0; y < height; y++)
	{
		unsigned char *qScanLine = qImage->scanLine(y);

		for (int x = 0; x < width; x++)
		{
			sRGBAFloat pixel;
			switch (imageChannel.contentType)
			{
				case IMAGE_CONTENT_NORMAL: pixel = image->GetPixelNormal(x, y); break;
				case IMAGE_CONTENT_NORMAL_WORLD: pixel = image->GetPixelNormalWorld(x, y); break;
				case IMAGE_CONTENT_SPECULAR: pixel = image->GetPixelSpecular(x, y); break;
				case IMAGE_CONTENT_DIFFUSE: pixel = image->GetPixelDiffuse(x, y); break;
				case IMAGE_CONTENT_WORLD_POSITION: pixel = image->GetPixelWorld(x, y); break;
				case IMAGE_CONTENT_SHADOWS: pixel = image->GetPixelShadows(x, y); break;
				case IMAGE_CONTENT_GLOBAL_ILLUMINATION:
					pixel = image->GetPixelGlobalIllumination(x, y);
					break;
				case IMAGE_CONTENT_NOT_DENOISED: pixel = image->GetPixelNotDenoised(x, y); break;
				default: pixel = sRGBAFloat(); break;
			}
			sRGB8 pixel8 = sRGB8(uchar(clamp(pixel.R * 255.0f, 0.0f, 255.0f)),
				uchar(clamp(pixel.G * 255.0f, 0.0f, 255.0f)), uchar(clamp(pixel.B * 255.0f, 0.0f, 255.0f)));
			qScanLine[3 * x + 0] = pixel8.R;
			qScanLine[3 * x + 1] = pixel8.G;
			qScanLine[3 * x + 2] = pixel8.B;
		}
	}
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	bool result = qImage->save(&file, "JPEG", quality);
	if (!result)
	{
		cErrorMessage::showMessage(
			QObject::tr("Can't save image to JPEG file!\n") + filename + "\n" + file.errorString(),
			cErrorMessage::errorMessage);
	}
	file.close();

	return result;
}

bool ImageFileSaveJPG::SaveJPEGQtGreyscale(QString filename, unsigned char *image, int width,
	int height, int quality, QMap<QString, QString> meta)
{
	if (!image)
	{
		WriteLog(
			"ImageFileSaveJPG::SaveJPEGQtGreyscale(): "
			"the image is a null pointer, this might be the case for optional channel(s). "
			"If this is the case, just rerender the image with enabled channel(s).",
			1);
		return false;
	}
	std::unique_ptr<QImage> qImage(new QImage(width, height, QImage::Format_Indexed8));
	QMapIterator<QString, QString> i(meta);
	while (i.hasNext())
	{
		i.next();
		qImage->setText(i.key(), i.value());
	}
	QVector<QRgb> my_table;
	for (int i = 0; i < 256; i++)
		my_table.push_back(qRgb(i, i, i));
	qImage->setColorTable(my_table);

	for (int line = 0; line < height; line++)
	{
		unsigned char *linePointer = &image[line * width];
		unsigned char *qScanLine = qImage->scanLine(line);
		memcpy(qScanLine, linePointer, sizeof(unsigned char) * width);
	}

	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	bool result = qImage->save(&file, "JPEG", quality);
	if (!result)
	{
		cErrorMessage::showMessage(
			QObject::tr("Can't save image to JPEG file!\n") + filename + "\n" + file.errorString(),
			cErrorMessage::errorMessage);
	}
	file.close();

	return result;
}

bool ImageFileSavePNG::SavePNGQtBlackAndWhite(
	QString filename, const unsigned char *image, int width, int height)
{
	std::unique_ptr<QImage> qImage(new QImage(width, height, QImage::Format_Mono));
	QVector<QRgb> my_table;
	my_table.push_back(qRgb(0, 0, 0));
	my_table.push_back(qRgb(255, 255, 255));
	qImage->setColorTable(my_table);

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			qImage->setPixel(i, j, image[i + j * width]);
		}
	}
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	bool result = qImage->save(&file, "PNG");
	if (!result)
	{
		cErrorMessage::showMessage(
			QObject::tr("Can't save image to JPEG file!\n") + filename + "\n" + file.errorString(),
			cErrorMessage::errorMessage);
	}
	file.close();
	return result;
}

bool ImageFileSavePNG::SavePNGQtGreyscale(
	QString filename, const unsigned char *image, int width, int height)
{
	std::unique_ptr<QImage> qImage(new QImage(width, height, QImage::Format_Grayscale8));

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			int val = image[i + j * width];
			qImage->setPixel(i, j, qRgb(val, val, val));
		}
	}
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	bool result = qImage->save(&file, "PNG");
	if (!result)
	{
		cErrorMessage::showMessage(
			QObject::tr("Can't save image to JPEG file!\n") + filename + "\n" + file.errorString(),
			cErrorMessage::errorMessage);
	}
	file.close();

	return result;
}

#ifdef USE_EXR
void ImageFileSaveEXR::SaveEXR(QString filename, std::shared_ptr<cImage> image,
	QMap<enumImageContentType, structSaveImageChannel> imageConfig)
{
	uint64_t width = image->GetWidth();
	uint64_t height = image->GetHeight();

	Imf::Header header(width, height);
	Imf::FrameBuffer frameBuffer;
	std::vector<char> bufferRGB;
	std::vector<char> bufferA;
	std::vector<char> bufferZ;
	std::vector<char> bufferNormal;
	std::vector<char> bufferWorld;
	std::vector<char> bufferSpecular;
	std::vector<char> bufferDiffuse;
	std::vector<char> bufferWorldPos;
	std::vector<char> bufferShadow;
	std::vector<char> bufferGI;
	std::vector<char> bufferNotDenoised;

	// compress each scan line on its own. This gives a good compression / read performance tradeoff
	header.compression() = Imf::ZIPS_COMPRESSION;

	bool linear = gPar->Get<bool>("linear_colorspace");

	if (imageConfig.contains(IMAGE_CONTENT_COLOR))
	{
		// add rgb channel header
		Imf::PixelType imfQuality =
			imageConfig[IMAGE_CONTENT_COLOR].channelQuality == IMAGE_CHANNEL_QUALITY_32 ? Imf::FLOAT
																																									: Imf::HALF;

		header.channels().insert("R", Imf::Channel(imfQuality, 1, 1, linear));
		header.channels().insert("G", Imf::Channel(imfQuality, 1, 1, linear));
		header.channels().insert("B", Imf::Channel(imfQuality, 1, 1, linear));

		uint64_t pixelSize = sizeof(tsRGB<half>);
		if (imfQuality == Imf::FLOAT) pixelSize = sizeof(tsRGB<float>);
		bufferRGB.resize(uint64_t(width) * height * pixelSize);
		tsRGB<half> *halfPointer = reinterpret_cast<tsRGB<half> *>(bufferRGB.data());
		tsRGB<float> *floatPointer = reinterpret_cast<tsRGB<float> *>(bufferRGB.data());

		for (uint64_t y = 0; y < height; y++)
		{
			for (uint64_t x = 0; x < width; x++)
			{
				uint64_t ptr = x + y * width;
				if (imfQuality == Imf::FLOAT)
				{
					sRGBFloat pixel = image->GetPixelImage(x, y);
					floatPointer[ptr].R = pixel.R;
					floatPointer[ptr].G = pixel.G;
					floatPointer[ptr].B = pixel.B;
				}
				else
				{
					sRGBFloat pixel = image->GetPixelImage(x, y);
					halfPointer[ptr].R = pixel.R;
					halfPointer[ptr].G = pixel.G;
					halfPointer[ptr].B = pixel.B;
				}
			}
		}

		// point EXR frame buffer to rgb
		size_t compSize = (imfQuality == Imf::FLOAT ? sizeof(float) : sizeof(half));
		frameBuffer.insert(
			"R", Imf::Slice(imfQuality, static_cast<char *>(bufferRGB.data()) + 0 * compSize,
						 3 * compSize, 3 * width * compSize));
		frameBuffer.insert(
			"G", Imf::Slice(imfQuality, static_cast<char *>(bufferRGB.data()) + 1 * compSize,
						 3 * compSize, 3 * width * compSize));
		frameBuffer.insert(
			"B", Imf::Slice(imfQuality, static_cast<char *>(bufferRGB.data()) + 2 * compSize,
						 3 * compSize, 3 * width * compSize));
	}

	if (imageConfig.contains(IMAGE_CONTENT_ALPHA))
	{
		// add alpha channel header
		Imf::PixelType imfQuality =
			imageConfig[IMAGE_CONTENT_ALPHA].channelQuality == IMAGE_CHANNEL_QUALITY_32 ? Imf::FLOAT
																																									: Imf::HALF;

		header.channels().insert("A", Imf::Channel(imfQuality, 1, 1, linear));

		uint64_t pixelSize = sizeof(half);
		if (imfQuality == Imf::FLOAT) pixelSize = sizeof(float);
		bufferA.resize(uint64_t(width) * height * pixelSize);
		half *halfPointer = reinterpret_cast<half *>(bufferA.data());
		float *floatPointer = reinterpret_cast<float *>(bufferA.data());

		for (uint64_t y = 0; y < height; y++)
		{
			for (uint64_t x = 0; x < width; x++)
			{
				uint64_t ptr = x + y * width;

				if (imfQuality == Imf::FLOAT)
				{
					floatPointer[ptr] = image->GetPixelAlpha(x, y) / 65536.0;
				}
				else
				{
					halfPointer[ptr] = image->GetPixelAlpha(x, y) / 65536.0;
				}
			}
		}
		// point EXR frame buffer to alpha
		size_t compSize = (imfQuality == Imf::FLOAT ? sizeof(float) : sizeof(half));
		frameBuffer.insert(
			"A", Imf::Slice(imfQuality, static_cast<char *>(bufferA.data()), compSize, width * compSize));
	}

	if (imageConfig.contains(IMAGE_CONTENT_ZBUFFER))
	{
		// add z Buffer channel header
		Imf::PixelType imfQuality =
			imageConfig[IMAGE_CONTENT_ZBUFFER].channelQuality == IMAGE_CHANNEL_QUALITY_32 ? Imf::FLOAT
																																										: Imf::HALF;

		header.channels().insert("Z", Imf::Channel(imfQuality, 1, 1, linear));

		// point EXR frame buffer to z buffer
		if (imfQuality == Imf::FLOAT)
		{
			// direct on buffer
			std::vector<float> &zBuffer = image->GetZBuffer();
			frameBuffer.insert("Z", Imf::Slice(Imf::FLOAT, reinterpret_cast<char *>(zBuffer.data()),
																sizeof(float), width * sizeof(float)));
		}
		else
		{
			uint64_t pixelSize = sizeof(half);
			bufferZ.resize(uint64_t(width) * height * pixelSize);
			half *halfPointer = reinterpret_cast<half *>(bufferZ.data());

			for (uint64_t y = 0; y < height; y++)
			{
				for (uint64_t x = 0; x < width; x++)
				{
					uint64_t ptr = x + y * width;
					halfPointer[ptr] = image->GetPixelZBuffer(x, y);
				}
			}
			frameBuffer.insert("Z", Imf::Slice(imfQuality, static_cast<char *>(bufferZ.data()),
																sizeof(half), width * sizeof(half)));
		}
	}

	if (imageConfig.contains(IMAGE_CONTENT_NORMAL))
	{
		SaveExrRgbChannel(QStringList{"n.X", "n.Y", "n.Z"}, imageConfig[IMAGE_CONTENT_NORMAL], &header,
			&frameBuffer, width, height, &bufferNormal);
	}

	if (imageConfig.contains(IMAGE_CONTENT_NORMAL_WORLD))
	{
		SaveExrRgbChannel(QStringList{"nW.X", "nW.Y", "nW.Z"}, imageConfig[IMAGE_CONTENT_NORMAL_WORLD],
			&header, &frameBuffer, width, height, &bufferWorld);
	}

	if (imageConfig.contains(IMAGE_CONTENT_SPECULAR))
	{
		SaveExrRgbChannel(QStringList{"s.R", "s.G", "s.B"}, imageConfig[IMAGE_CONTENT_SPECULAR],
			&header, &frameBuffer, width, height, &bufferSpecular);
	}

	if (imageConfig.contains(IMAGE_CONTENT_DIFFUSE))
	{
		SaveExrRgbChannel(QStringList{"d.R", "d.G", "d.B"}, imageConfig[IMAGE_CONTENT_DIFFUSE], &header,
			&frameBuffer, width, height, &bufferDiffuse);
	}

	if (imageConfig.contains(IMAGE_CONTENT_WORLD_POSITION))
	{
		SaveExrRgbChannel(QStringList{"p.X", "p.Y", "p.Z"}, imageConfig[IMAGE_CONTENT_WORLD_POSITION],
			&header, &frameBuffer, width, height, &bufferWorldPos);
	}

	if (imageConfig.contains(IMAGE_CONTENT_SHADOWS))
	{
		SaveExrRgbChannel(QStringList{"sh.R", "sh.G", "sh.B"}, imageConfig[IMAGE_CONTENT_SHADOWS],
			&header, &frameBuffer, width, height, &bufferShadow);
	}

	if (imageConfig.contains(IMAGE_CONTENT_GLOBAL_ILLUMINATION))
	{
		SaveExrRgbChannel(QStringList{"gi.R", "gi.G", "gi.B"},
			imageConfig[IMAGE_CONTENT_GLOBAL_ILLUMINATION], &header, &frameBuffer, width, height,
			&bufferGI);
	}

	if (imageConfig.contains(IMAGE_CONTENT_NOT_DENOISED))
	{
		SaveExrRgbChannel(QStringList{"nd.R", "nd.G", "nd.B"}, imageConfig[IMAGE_CONTENT_NOT_DENOISED],
			&header, &frameBuffer, width, height, &bufferNotDenoised);
	}

	// insert meta data
	QMapIterator<QString, QString> i(image->getMeta());
	while (i.hasNext())
	{
		i.next();
		header.insert(
			i.key().toStdString().c_str(), Imf::StringAttribute(i.value().toStdString().c_str()));
	}

	Imf::OutputFile file(filename.toStdString().c_str(), header);
	file.setFrameBuffer(frameBuffer);

	for (uint64_t r = 0; r < height; r += SAVE_CHUNK_SIZE)
	{
		uint64_t currentChunkSize = std::min(height - r, SAVE_CHUNK_SIZE);
		file.writePixels(currentChunkSize);
		emit updateProgressAndStatus(getJobName(), QString("Saving all channels"), 1.0 * r / height);
	}
}

void ImageFileSaveEXR::SaveExrRgbChannel(QStringList names, structSaveImageChannel imageChannel,
	Imf::Header *header, Imf::FrameBuffer *frameBuffer, uint64_t width, uint64_t height,
	std::vector<char> *imageBuffer)
{
	bool linear = gPar->Get<bool>("linear_colorspace");
	// add rgb channel header
	Imf::PixelType imfQuality =
		imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_32 ? Imf::FLOAT : Imf::HALF;

	header->channels().insert(
		names.at(0).toStdString().c_str(), Imf::Channel(imfQuality, 1, 1, linear));
	header->channels().insert(
		names.at(1).toStdString().c_str(), Imf::Channel(imfQuality, 1, 1, linear));
	header->channels().insert(
		names.at(2).toStdString().c_str(), Imf::Channel(imfQuality, 1, 1, linear));

	int pixelSize = sizeof(tsRGB<half>);
	if (imfQuality == Imf::FLOAT) pixelSize = sizeof(tsRGB<float>);
	imageBuffer->resize(uint64_t(width) * height * pixelSize);
	tsRGB<half> *halfPointer = reinterpret_cast<tsRGB<half> *>(imageBuffer->data());
	tsRGB<float> *floatPointer = reinterpret_cast<tsRGB<float> *>(imageBuffer->data());

	for (uint64_t y = 0; y < height; y++)
	{
		for (uint64_t x = 0; x < width; x++)
		{
			uint64_t ptr = (x + y * width);
			sRGBFloat pixel;
			switch (imageChannel.contentType)
			{
				case IMAGE_CONTENT_NORMAL: pixel = image->GetPixelNormal(x, y); break;
				case IMAGE_CONTENT_NORMAL_WORLD: pixel = image->GetPixelNormalWorld(x, y); break;
				case IMAGE_CONTENT_SPECULAR: pixel = image->GetPixelSpecular(x, y); break;
				case IMAGE_CONTENT_DIFFUSE: pixel = image->GetPixelDiffuse(x, y); break;
				case IMAGE_CONTENT_WORLD_POSITION: pixel = image->GetPixelWorld(x, y); break;
				case IMAGE_CONTENT_SHADOWS: pixel = image->GetPixelShadows(x, y); break;
				case IMAGE_CONTENT_GLOBAL_ILLUMINATION:
					pixel = image->GetPixelGlobalIllumination(x, y);
					break;
				case IMAGE_CONTENT_NOT_DENOISED: pixel = image->GetPixelNotDenoised(x, y); break;

				default: pixel = sRGBFloat(); break;
			}
			if (imfQuality == Imf::FLOAT)
			{
				floatPointer[ptr] = pixel;
			}
			else
			{
				halfPointer[ptr].R = pixel.R;
				halfPointer[ptr].G = pixel.G;
				halfPointer[ptr].B = pixel.B;
			}
		}
	}

	// point EXR frame buffer to rgb
	size_t compSize = (imfQuality == Imf::FLOAT ? sizeof(float) : sizeof(half));
	frameBuffer->insert(names.at(0).toStdString().c_str(),
		Imf::Slice(imfQuality, static_cast<char *>(imageBuffer->data()) + 0 * compSize, 3 * compSize,
			3 * width * compSize));
	frameBuffer->insert(names.at(1).toStdString().c_str(),
		Imf::Slice(imfQuality, static_cast<char *>(imageBuffer->data()) + 1 * compSize, 3 * compSize,
			3 * width * compSize));
	frameBuffer->insert(names.at(2).toStdString().c_str(),
		Imf::Slice(imfQuality, static_cast<char *>(imageBuffer->data()) + 2 * compSize, 3 * compSize,
			3 * width * compSize));
}

#endif /* USE_EXR */

#ifdef USE_TIFF
bool ImageFileSaveTIFF::SaveTIFF(QString filenameInput, std::shared_ptr<cImage> image,
	structSaveImageChannel imageChannel, bool appendAlpha)
{
	uint64_t width = image->GetWidth();
	uint64_t height = image->GetHeight();

	TIFF *tiff = TIFFOpen(filenameInput.toLocal8Bit().constData(), "w");
	if (!tiff)
	{
		qCritical() << "SaveTiff() cannot open file";
		return false;
	}

	int qualitySize;
	int sampleFormat;
	switch (imageChannel.channelQuality)
	{
		case IMAGE_CHANNEL_QUALITY_8:
			qualitySize = 8;
			sampleFormat = SAMPLEFORMAT_UINT;
			break;
		case IMAGE_CHANNEL_QUALITY_16:
			qualitySize = 16;
			sampleFormat = SAMPLEFORMAT_UINT;
			break;
		default:
			qualitySize = 32;
			sampleFormat = SAMPLEFORMAT_IEEEFP;
			break;
	}

	int colorType;
	switch (imageChannel.contentType)
	{
		case IMAGE_CONTENT_COLOR: colorType = PHOTOMETRIC_RGB; break;
		case IMAGE_CONTENT_ALPHA:
		case IMAGE_CONTENT_ZBUFFER: colorType = PHOTOMETRIC_MINISBLACK; break;
		case IMAGE_CONTENT_NORMAL: colorType = PHOTOMETRIC_RGB; break;
		case IMAGE_CONTENT_NORMAL_WORLD: colorType = PHOTOMETRIC_RGB; break;
		case IMAGE_CONTENT_SPECULAR: colorType = PHOTOMETRIC_RGB; break;
		case IMAGE_CONTENT_DIFFUSE: colorType = PHOTOMETRIC_RGB; break;
		case IMAGE_CONTENT_WORLD_POSITION: colorType = PHOTOMETRIC_RGB; break;
		case IMAGE_CONTENT_SHADOWS: colorType = PHOTOMETRIC_RGB; break;
		case IMAGE_CONTENT_GLOBAL_ILLUMINATION: colorType = PHOTOMETRIC_RGB; break;
		case IMAGE_CONTENT_NOT_DENOISED: colorType = PHOTOMETRIC_RGB; break;
		default: colorType = PHOTOMETRIC_RGB; break;
	}

	int samplesPerPixel = IMAGE_CONTENT_COLOR;
	switch (imageChannel.contentType)
	{
		case IMAGE_CONTENT_COLOR: samplesPerPixel = appendAlpha ? 4 : 3; break;
		case IMAGE_CONTENT_ALPHA: samplesPerPixel = 1; break;
		case IMAGE_CONTENT_ZBUFFER: samplesPerPixel = 1; break;
		case IMAGE_CONTENT_NORMAL: samplesPerPixel = 3; break;
		case IMAGE_CONTENT_NORMAL_WORLD: samplesPerPixel = 3; break;
		case IMAGE_CONTENT_SPECULAR: samplesPerPixel = 3; break;
		case IMAGE_CONTENT_DIFFUSE: samplesPerPixel = 3; break;
		case IMAGE_CONTENT_WORLD_POSITION: samplesPerPixel = 3; break;
		case IMAGE_CONTENT_SHADOWS: samplesPerPixel = 3; break;
		case IMAGE_CONTENT_GLOBAL_ILLUMINATION: samplesPerPixel = 3; break;
		case IMAGE_CONTENT_NOT_DENOISED: samplesPerPixel = 3; break;
	}

	TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height);
	TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, qualitySize);
	TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, samplesPerPixel);
	TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, SAVE_CHUNK_SIZE);

	TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
	TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, colorType);
	TIFFSetField(tiff, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
	TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, sampleFormat);

	uint64_t pixelSize = samplesPerPixel * qualitySize / 8;
	std::vector<char> colorPtr(uint64_t(width) * height * pixelSize);

	// calculate min / max values from zbuffer range
	float minZ = float(1.0e50);
	float maxZ = 0.0;
	float rangeZ = 0.0;
	if (imageChannel.contentType == IMAGE_CONTENT_ZBUFFER)
	{
		std::vector<float> &zbuffer = image->GetZBuffer();
		uint64_t size = width * height;

		bool constRange = gPar->Get<bool>("zbuffer_constant_range");

		if (constRange)
		{
			minZ = gPar->Get<double>("zbuffer_min_depth");
			maxZ = gPar->Get<double>("zbuffer_max_depth");
		}
		else
		{
			for (uint64_t i = 0; i < size; i++)
			{
				float z = zbuffer[i];
				if (z > maxZ && z < 1e19) maxZ = z;
				if (z < minZ) minZ = z;
			}
		}
		rangeZ = maxZ - minZ;
	}

	bool zLogarithmicScale = gPar->Get<bool>("zbuffer_logarithmic");
	bool invertZ = gPar->Get<bool>("zbuffer_invert");
	float kZ = log(maxZ / minZ);

	for (uint64_t y = 0; y < height; y++)
	{
		for (uint64_t x = 0; x < width; x++)
		{
			uint64_t ptr = (x + y * width) * pixelSize;
			switch (imageChannel.contentType)
			{
				case IMAGE_CONTENT_COLOR:
				{
					if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_32)
					{
						if (appendAlpha)
						{
							sRGBAFloat *typedColorPtr = reinterpret_cast<sRGBAFloat *>(&colorPtr[ptr]);
							sRGB16 rgbPointer = image->GetPixelImage16(x, y);
							typedColorPtr->R = rgbPointer.R / 65536.0f;
							typedColorPtr->G = rgbPointer.G / 65536.0f;
							typedColorPtr->B = rgbPointer.B / 65536.0f;
							typedColorPtr->A = image->GetPixelAlpha(x, y) / 65536.0f;
						}
						else
						{
							sRGBFloat *typedColorPtr = reinterpret_cast<sRGBFloat *>(&colorPtr[ptr]);
							sRGB16 rgbPointer = image->GetPixelImage16(x, y);
							typedColorPtr->R = rgbPointer.R / 65536.0f;
							typedColorPtr->G = rgbPointer.G / 65536.0f;
							typedColorPtr->B = rgbPointer.B / 65536.0f;
						}
					}
					else if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
					{
						if (appendAlpha)
						{
							sRGBA16 *typedColorPtr = reinterpret_cast<sRGBA16 *>(&colorPtr[ptr]);
							*typedColorPtr = sRGBA16(image->GetPixelImage16(x, y));
							typedColorPtr->A = image->GetPixelAlpha(x, y);
						}
						else
						{
							sRGB16 *typedColorPtr = reinterpret_cast<sRGB16 *>(&colorPtr[ptr]);
							*typedColorPtr = sRGB16(image->GetPixelImage16(x, y));
						}
					}
					else
					{
						if (appendAlpha)
						{
							if (x == 0 && y == 0)
							{
								image->ConvertAlphaTo8bit();
								image->ConvertTo8bitChar();
							}
							sRGBA8 *typedColorPtr = reinterpret_cast<sRGBA8 *>(&colorPtr[ptr]);
							*typedColorPtr = sRGBA8(image->GetPixelImage8(x, y));
							typedColorPtr->A = image->GetPixelAlpha8(x, y);
						}
						else
						{
							if (x == 0 && y == 0)
							{
								image->ConvertTo8bitChar();
							}
							sRGB8 *typedColorPtr = reinterpret_cast<sRGB8 *>(&colorPtr[ptr]);
							*typedColorPtr = sRGB8(image->GetPixelImage8(x, y));
						}
					}
				}
				break;
				case IMAGE_CONTENT_ALPHA:
				{
					if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_32)
					{
						float *typedColorPtr = reinterpret_cast<float *>(&colorPtr[ptr]);
						*typedColorPtr = image->GetPixelAlpha(x, y) / 65536.0f;
					}
					if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
					{
						unsigned short *typedColorPtr = reinterpret_cast<unsigned short *>(&colorPtr[ptr]);
						*typedColorPtr = image->GetPixelAlpha(x, y);
					}
					else
					{
						if (x == 0 && y == 0)
						{
							image->ConvertAlphaTo8bit();
						}
						unsigned char *typedColorPtr = reinterpret_cast<unsigned char *>(&colorPtr[ptr]);
						*typedColorPtr = image->GetPixelAlpha8(x, y);
					}
				}
				break;
				case IMAGE_CONTENT_ZBUFFER:
				{
					if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_32)
					{
						float *typedColorPtr = reinterpret_cast<float *>(&colorPtr[ptr]);
						*typedColorPtr = (image->GetPixelZBuffer(x, y) - minZ) / rangeZ;
					}
					if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
					{
						float z = image->GetPixelZBuffer(x, y);
						float z1;
						if (zLogarithmicScale)
						{
							z1 = log(z / minZ) / kZ;
						}
						else
						{
							z1 = (z - minZ) / (maxZ - minZ);
						}
						if (z1 < 0) z1 = 0.0;
						if (z1 > 1.0) z1 = 1.0;
						int intZ = int(z1 * 65534);
						if (z > 1e19f) intZ = 65535;

						if (invertZ) intZ = 65535 - intZ;

						unsigned short *typedColorPtr = reinterpret_cast<unsigned short *>(&colorPtr[ptr]);
						*typedColorPtr = static_cast<unsigned short>(intZ);
					}
					else
					{
						float z = image->GetPixelZBuffer(x, y);
						float z1;
						if (zLogarithmicScale)
						{
							z1 = log(z / minZ) / kZ;
						}
						else
						{
							z1 = (z - minZ) / (maxZ - minZ);
						}
						if (z1 < 0) z1 = 0.0;
						if (z1 > 1.0) z1 = 1.0;
						int intZ = int(z1 * 254);
						if (z > 1e19) intZ = 255;

						if (invertZ) intZ = 255 - intZ;

						unsigned char *typedColorPtr = reinterpret_cast<unsigned char *>(&colorPtr[ptr]);
						*typedColorPtr = static_cast<unsigned char>(intZ);
					}
				}
				break;
				case IMAGE_CONTENT_NORMAL:
					SaveTiffRgbPixel(imageChannel, &colorPtr[ptr], image->GetPixelNormal(x, y));
					break;
				case IMAGE_CONTENT_NORMAL_WORLD:
					SaveTiffRgbPixel(imageChannel, &colorPtr[ptr], image->GetPixelNormalWorld(x, y));
					break;
				case IMAGE_CONTENT_SPECULAR:
					SaveTiffRgbPixel(imageChannel, &colorPtr[ptr], image->GetPixelSpecular(x, y));
					break;
				case IMAGE_CONTENT_DIFFUSE:
					SaveTiffRgbPixel(imageChannel, &colorPtr[ptr], image->GetPixelDiffuse(x, y));
					break;
				case IMAGE_CONTENT_WORLD_POSITION:
					SaveTiffRgbPixel(imageChannel, &colorPtr[ptr], image->GetPixelWorld(x, y));
					break;
				case IMAGE_CONTENT_SHADOWS:
					SaveTiffRgbPixel(imageChannel, &colorPtr[ptr], image->GetPixelShadows(x, y));
					break;
				case IMAGE_CONTENT_GLOBAL_ILLUMINATION:
					SaveTiffRgbPixel(imageChannel, &colorPtr[ptr], image->GetPixelGlobalIllumination(x, y));
					break;
				case IMAGE_CONTENT_NOT_DENOISED:
					SaveTiffRgbPixel(imageChannel, &colorPtr[ptr], image->GetPixelNotDenoised(x, y));
					break;
			}
		}
	}

	// TIFFWriteEncodedStrip(
	//	tiff, 0, static_cast<void *>(colorPtr), tsize_t(width * height * pixelSize));
	for (uint64_t r = 0; r < height; r += SAVE_CHUNK_SIZE)
	{
		uint64_t currentChunkSize = std::min(height - r, SAVE_CHUNK_SIZE);
		// needs buffer with offset position
		char *buf = static_cast<char *>(colorPtr.data()) + r * pixelSize * width;
		tsize_t size = tsize_t(currentChunkSize * pixelSize * width);
		TIFFWriteEncodedStrip(tiff, r / SAVE_CHUNK_SIZE, buf, size);
		updateProgressAndStatusChannel(1.0 * r / height);
	}
	TIFFClose(tiff);

	return true;
}

void ImageFileSaveTIFF::SaveTiffRgbPixel(
	structSaveImageChannel imageChannel, char *colorPtr, sRGBFloat pixel)
{
	if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_32)
	{
		sRGBFloat *typedColorPtr = reinterpret_cast<sRGBFloat *>(colorPtr);
		*typedColorPtr = pixel;
	}
	else if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
	{
		sRGB16 *typedColorPtr = reinterpret_cast<sRGB16 *>(colorPtr);
		*typedColorPtr =
			sRGB16(ushort(pixel.R * 65535.0f), ushort(pixel.G * 65535.0f), ushort(pixel.B * 65535.0f));
	}
	else
	{
		sRGB8 *typedColorPtr = reinterpret_cast<sRGB8 *>(colorPtr);
		*typedColorPtr =
			sRGB8(uchar(pixel.R * 255.0f), uchar(pixel.G * 255.0f), uchar(pixel.B * 255.0f));
	}
}

#endif /* USE_TIFF */
