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
 */

#include "file_image.hpp"
#include "error_message.hpp"
#include "files.h"
#include "initparameters.hpp"
#include "parameters.hpp"
#include "cimage.hpp"

#define PNG_DEBUG 3

#ifdef USE_TIFF
#include "tiff.h"
#include "tiffio.h"
#endif // USE_TIFF

#ifdef USE_EXR
#include <ImfAttribute.h>
#include <ImfChannelList.h>
#include <ImfFrameBuffer.h>
#include <ImfInputFile.h>
#include <ImfOutputFile.h>
#include <half.h>
#endif // USE_EXR

ImageFileSave::ImageFileSave(QString filename, cImage *image, ImageConfig imageConfig)
{
	this->filename = filename;
	this->image = image;
	this->imageConfig = imageConfig;
	currentChannel = 0;
	totalChannel = 0;
	currentChannelKey = IMAGE_CONTENT_COLOR;
}

ImageFileSave *ImageFileSave::create(
	QString filename, enumImageFileType filetype, cImage *image, ImageConfig imageConfig)
{
	switch (filetype)
	{
		case IMAGE_FILE_TYPE_PNG: return new ImageFileSavePNG(filename, image, imageConfig);
		case IMAGE_FILE_TYPE_JPG: return new ImageFileSaveJPG(filename, image, imageConfig);
#ifdef USE_TIFF
		case IMAGE_FILE_TYPE_TIFF: return new ImageFileSaveTIFF(filename, image, imageConfig);
#endif /* USE_TIFF */
#ifdef USE_EXR
		case IMAGE_FILE_TYPE_EXR: return new ImageFileSaveEXR(filename, image, imageConfig);
#endif /* USE_EXR */
	}
	qCritical() << "filetype " << ImageFileExtension(filetype) << " not supported!";
	return NULL;
}

QString ImageFileSave::ImageFileExtension(enumImageFileType imageFileType)
{
	switch (imageFileType)
	{
		case IMAGE_FILE_TYPE_JPG: return "jpg"; break;
		case IMAGE_FILE_TYPE_PNG: return "png"; break;
		case IMAGE_FILE_TYPE_EXR: return "exr"; break;
		case IMAGE_FILE_TYPE_TIFF: return "tiff"; break;
	}
	return "";
}

QString ImageFileSave::ImageChannelName(enumImageContentType imageContentType)
{
	switch (imageContentType)
	{
		case IMAGE_CONTENT_COLOR: return "color"; break;
		case IMAGE_CONTENT_ALPHA: return "alpha"; break;
		case IMAGE_CONTENT_ZBUFFER: return "zbuffer"; break;
		case IMAGE_CONTENT_NORMAL: return "normal"; break;
	}
	return "";
}

ImageFileSave::enumImageFileType ImageFileSave::ImageFileType(QString imageFileExtension)
{
	if (imageFileExtension == "jpg")
		return IMAGE_FILE_TYPE_JPG;
	else if (imageFileExtension == "png")
		return IMAGE_FILE_TYPE_PNG;
	else if (imageFileExtension == "exr")
		return IMAGE_FILE_TYPE_EXR;
	else if (imageFileExtension == "tiff")
		return IMAGE_FILE_TYPE_TIFF;
	else
		return IMAGE_FILE_TYPE_JPG;
}

void ImageFileSavePNG::SaveImage()
{
	emit updateProgressAndStatus(getJobName(), QString("Started"), 0.0);

	bool appendAlpha = gPar->Get<bool>("append_alpha_png")
										 && imageConfig.contains(IMAGE_CONTENT_COLOR)
										 && imageConfig.contains(IMAGE_CONTENT_ALPHA);

	currentChannel = 0;
	totalChannel = imageConfig.size();
	for (ImageConfig::iterator channel = imageConfig.begin(); channel != imageConfig.end(); ++channel)
	{
		currentChannelKey = channel.key();
		QString fullFilename = filename + channel.value().postfix + ".png";
		emit updateProgressAndStatus(getJobName(),
			QString("Saving channel %1").arg(ImageChannelName(currentChannelKey)),
			1.0 * currentChannel / totalChannel);
		currentChannel++;

		switch (currentChannelKey)
		{
			case IMAGE_CONTENT_COLOR: SavePNG(fullFilename, image, channel.value(), appendAlpha); break;
			case IMAGE_CONTENT_ALPHA:
				if (!appendAlpha) SavePNG(fullFilename, image, channel.value());
				break;
			case IMAGE_CONTENT_ZBUFFER:
			case IMAGE_CONTENT_NORMAL:
			default: SavePNG(fullFilename, image, channel.value()); break;
		}
	}
	emit updateProgressAndStatus(getJobName(), QString("Finished"), 1.0);
}

void ImageFileSaveJPG::SaveImage()
{
	emit updateProgressAndStatus(getJobName(), QString("Started"), 0.0);

	currentChannel = 0;
	totalChannel = imageConfig.size();
	for (ImageConfig::iterator channel = imageConfig.begin(); channel != imageConfig.end(); ++channel)
	{
		currentChannelKey = channel.key();
		QString fullFilename = filename + channel.value().postfix + ".jpg";
		emit updateProgressAndStatus(getJobName(),
			QString("Saving channel %1").arg(ImageChannelName(currentChannelKey)),
			1.0 * currentChannel / totalChannel);
		currentChannel++;

		switch (currentChannelKey)
		{
			case IMAGE_CONTENT_COLOR:
				SaveJPEGQt(fullFilename, image->ConvertTo8bit(), image->GetWidth(), image->GetHeight(),
					gPar->Get<int>("jpeg_quality"));
				break;
			case IMAGE_CONTENT_ALPHA:
				SaveJPEGQtGreyscale(fullFilename, image->ConvertAlphaTo8bit(), image->GetWidth(),
					image->GetHeight(), gPar->Get<int>("jpeg_quality"));
				break;
			case IMAGE_CONTENT_ZBUFFER:
				qWarning() << "JPG cannot save zbuffer (loss of precision to strong)";
				break;
			case IMAGE_CONTENT_NORMAL:
				SaveJPEGQt(fullFilename, image->ConvertNormalto8Bit(), image->GetWidth(),
					image->GetHeight(), gPar->Get<int>("jpeg_quality"));
				break;
			default: qWarning() << "Unknown channel for JPG"; break;
		}
	}
	emit updateProgressAndStatus(getJobName(), QString("Finished"), 1.0);
}

#ifdef USE_TIFF
void ImageFileSaveTIFF::SaveImage()
{
	emit updateProgressAndStatus(getJobName(), QString("Started"), 0.0);

	bool appendAlpha = gPar->Get<bool>("append_alpha_png")
										 && imageConfig.contains(IMAGE_CONTENT_COLOR)
										 && imageConfig.contains(IMAGE_CONTENT_ALPHA);

	int currentChannel = 0;
	totalChannel = imageConfig.size();
	for (ImageConfig::iterator channel = imageConfig.begin(); channel != imageConfig.end(); ++channel)
	{
		currentChannelKey = channel.key();
		QString fullFilename = filename + channel.value().postfix + ".tiff";
		emit updateProgressAndStatus(getJobName(),
			QString("Saving channel %1").arg(ImageChannelName(currentChannelKey)),
			1.0 * currentChannel / totalChannel);
		currentChannel++;

		switch (currentChannelKey)
		{
			case IMAGE_CONTENT_COLOR: SaveTIFF(fullFilename, image, channel.value(), appendAlpha); break;
			case IMAGE_CONTENT_ALPHA:
				if (!appendAlpha) SaveTIFF(fullFilename, image, channel.value());
				break;
			case IMAGE_CONTENT_ZBUFFER:
			case IMAGE_CONTENT_NORMAL:
			default: SaveTIFF(fullFilename, image, channel.value()); break;
		}
	}
	emit updateProgressAndStatus(getJobName(), QString("Finished"), 1.0);
}
#endif /* USE_TIFF */

#ifdef USE_EXR
void ImageFileSaveEXR::SaveImage()
{
	emit updateProgressAndStatus(getJobName(), QString("Started"), 0.0);
	QString fullFilename = filename + ".exr";
	SaveEXR(fullFilename, image, imageConfig);
	emit updateProgressAndStatus(getJobName(), QString("Finished"), 1.0);
}
#endif /* USE_EXR */

void ImageFileSavePNG::SavePNG(
	QString filename, cImage *image, structSaveImageChannel imageChannel, bool appendAlpha)
{
	uint64_t width = image->GetWidth();
	uint64_t height = image->GetHeight();

	/* create file */
	FILE *fp = fopen(filename.toLocal8Bit().constData(), "wb");
	png_bytep *row_pointers = NULL;
	png_structp png_ptr = NULL;
	png_info *info_ptr = NULL;

	try
	{
		if (!fp) throw QString("[write_png_file] File %s could not be opened for writing.");

		/* initialize stuff */
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

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
			case IMAGE_CONTENT_ALPHA: colorType = PNG_COLOR_TYPE_GRAY; break;
			case IMAGE_CONTENT_ZBUFFER: colorType = PNG_COLOR_TYPE_GRAY; break;
			case IMAGE_CONTENT_NORMAL: colorType = PNG_COLOR_TYPE_RGB; break;
			default: colorType = PNG_COLOR_TYPE_RGB; break;
		}

		png_set_IHDR(png_ptr, info_ptr, width, height, qualitySize, colorType, PNG_INTERLACE_NONE,
			PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

		png_write_info(png_ptr, info_ptr);
		png_set_swap(png_ptr);

		char *colorPtr = NULL;

		/* write bytes */
		if (setjmp(png_jmpbuf(png_ptr))) throw QString("[write_png_file] Error during writing bytes");

		row_pointers = new png_bytep[height];

		uint64_t pixelSize = qualitySizeByte;

		switch (imageChannel.contentType)
		{
			case IMAGE_CONTENT_COLOR: pixelSize *= appendAlpha ? 4 : 3; break;
			case IMAGE_CONTENT_ALPHA: pixelSize *= 1; break;
			case IMAGE_CONTENT_ZBUFFER: pixelSize *= 1; break;
			case IMAGE_CONTENT_NORMAL: pixelSize *= 3; break;
		}

		bool directOnBuffer = false;
		if (imageChannel.contentType == IMAGE_CONTENT_COLOR && !appendAlpha) directOnBuffer = true;
		if (imageChannel.contentType == IMAGE_CONTENT_ALPHA) directOnBuffer = true;

		if (directOnBuffer)
		{
			char *directPointer = NULL;
			switch (imageChannel.contentType)
			{
				case IMAGE_CONTENT_COLOR:
				{
					if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
					{
						directPointer = (char *)image->GetImage16Ptr();
					}
					else
					{
						directPointer = (char *)image->ConvertTo8bit();
					}
				}
				break;
				case IMAGE_CONTENT_ALPHA:
				{
					if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
					{
						directPointer = (char *)image->GetAlphaBufPtr();
					}
					else
					{
						directPointer = (char *)image->ConvertAlphaTo8bit();
					}
				}
				break;
				case IMAGE_CONTENT_ZBUFFER:
				case IMAGE_CONTENT_NORMAL:
					// zbuffer and normals are float, so direct buffer write is not applicable
					break;
			}

			for (uint64_t y = 0; y < height; y++)
			{
				row_pointers[y] = (png_byte *)&directPointer[y * width * pixelSize];
			}
		}
		else
		{
			colorPtr = new char[(uint64_t)width * height * pixelSize];

			// calculate min / max values from zbuffer range
			float minZ = float(1.0e50);
			float maxZ = 0.0;
			if (imageChannel.contentType == IMAGE_CONTENT_ZBUFFER)
			{
				float *zbuffer = image->GetZBufferPtr();
				uint64_t size = width * height;
				for (uint64_t i = 0; i < size; i++)
				{
					float z = zbuffer[i];
					if (z > maxZ && z < 1e19) maxZ = z;
					if (z < minZ) minZ = z;
				}
			}
			double kZ = log(maxZ / minZ);

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
									sRGBA16 *typedColorPtr = (sRGBA16 *)&colorPtr[ptr];
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
										image->ConvertTo8bit();
									}
									sRGBA8 *typedColorPtr = (sRGBA8 *)&colorPtr[ptr];
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
								float z1 = log(z / minZ) / kZ;
								int intZ = z1 * 60000;
								if (z > 1e19) intZ = 65535;

								unsigned short *typedColorPtr = (unsigned short *)&colorPtr[ptr];
								*typedColorPtr = (unsigned short)(intZ);
							}
							else
							{
								float z = image->GetPixelZBuffer(x, y);
								float z1 = log(z / minZ) / kZ;
								int intZ = z1 * 240;
								if (z > 1e19) intZ = 255;

								unsigned char *typedColorPtr = (unsigned char *)&colorPtr[ptr];
								*typedColorPtr = (unsigned char)(intZ);
							}
						}
						break;
						case IMAGE_CONTENT_NORMAL:
						{
							if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
							{
								if (x == 0 && y == 0) image->ConvertNormalto16Bit();
								sRGB16 *typedColorPtr = (sRGB16 *)&colorPtr[ptr];
								*typedColorPtr = sRGB16(image->GetPixelNormal16(x, y));
							}
							else
							{
								if (x == 0 && y == 0) image->ConvertNormalto8Bit();
								sRGB8 *typedColorPtr = (sRGB8 *)&colorPtr[ptr];
								*typedColorPtr = sRGB8(image->GetPixelNormal8(x, y));
							}
						}
						break;
					}
				}
				row_pointers[y] = (png_byte *)&colorPtr[y * width * pixelSize];
			}
		}

		// png_write_image(png_ptr, row_pointers);
		uint64_t chunkSize = 100;
		for (uint64_t r = 0; r < height; r += chunkSize)
		{
			uint64_t leftToWrite = height - r;
			png_write_rows(png_ptr, (png_bytepp)&row_pointers[r], min(leftToWrite, chunkSize));
			/* TODO: make SavePNG private non static and rewrite direct accesses to static function
			 emit updateProgressAndStatus(getJobName(),
				QString("Saving channel %1").arg(ImageChannelName(currentChannelKey)),
				(1.0 * currentChannel / totalChannel) + (1.0 * r / (totalChannel * height)));*/
		}

		/* end write */
		if (setjmp(png_jmpbuf(png_ptr))) throw QString("[write_png_file] Error during end of write");

		png_write_end(png_ptr, info_ptr);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		delete[] row_pointers;
		if (colorPtr) delete[] colorPtr;
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
				png_destroy_write_struct(&png_ptr, NULL);
			}
		}
		if (row_pointers) delete[] row_pointers;
		if (fp) fclose(fp);
		cErrorMessage::showMessage(
			QObject::tr("Can't save image to PNG file!\n") + filename + "\n" + status,
			cErrorMessage::errorMessage);
	}
}

void ImageFileSavePNG::SavePNG16(QString filename, int width, int height, sRGB16 *image16)
{
	/* create file */
	FILE *fp = fopen(filename.toLocal8Bit().constData(), "wb");
	png_bytep *row_pointers = NULL;
	png_structp png_ptr = NULL;
	png_info *info_ptr = NULL;

	try
	{
		if (!fp) throw QString("[write_png_file] File %s could not be opened for writing.");

		/* initialize stuff */
		png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

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

		row_pointers = new png_bytep[height];
		for (int y = 0; y < height; y++)
		{
			row_pointers[y] = (png_byte *)&image16[y * width];
		}

		png_write_image(png_ptr, row_pointers);

		/* end write */
		if (setjmp(png_jmpbuf(png_ptr))) throw QString("[write_png_file] Error during end of write");

		png_write_end(png_ptr, info_ptr);
		png_destroy_write_struct(&png_ptr, &info_ptr);
		delete[] row_pointers;

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
				png_destroy_write_struct(&png_ptr, NULL);
			}
		}
		if (row_pointers) delete[] row_pointers;
		if (fp) fclose(fp);
		cErrorMessage::showMessage(
			QObject::tr("Can't save image to PNG file!\n") + filename + "\n" + status,
			cErrorMessage::errorMessage);
	}
}

void ImageFileSavePNG::SaveFromTilesPNG16(const char *filename, int width, int height, int tiles)
{
	/* create file */
	string filenamePNG(filename);
	filenamePNG += "_fromTiles.png";
	FILE *fp = fopen(filenamePNG.c_str(), "wb");
	if (!fp)
	{
		fprintf(stderr, "[write_png_file] File %s could not be opened for writing", filename);
		return;
	}

	/* initialize stuff */
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

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
			string filename2 = IndexFilename(filename, "tile", fileNumber);
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
			png_write_rows(png_ptr, (png_bytep *)&rowBuffer, 1);
		}

		for (int tile = 0; tile < tiles; tile++)
		{
			fclose(files[tile]);
			int fileNumber = tile + tileRow * tiles;
			string filename2 = IndexFilename(filename, "tile", fileNumber);
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

bool ImageFileSaveJPG::SaveJPEGQt(
	QString filename, unsigned char *image, int width, int height, int quality)
{
	if (!image)
	{
		qDebug() << "the image is a null pointer, this might be the case for optional channel(s). "
								"If this is the case, just rerender the image with enabled channel(s).";
		return false;
	}

	QImage *qimage = new QImage(width, height, QImage::Format_RGB888);

	for (int line = 0; line < height; line++)
	{
		unsigned char *linePointer = &image[line * width * 3];
		unsigned char *qScanLine = qimage->scanLine(line);
		memcpy(qScanLine, linePointer, sizeof(unsigned char) * 3 * width);
	}

	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	bool result = qimage->save(&file, "JPEG", quality);
	if (!result)
	{
		cErrorMessage::showMessage(
			QObject::tr("Can't save image to JPEG file!\n") + filename + "\n" + file.errorString(),
			cErrorMessage::errorMessage);
	}
	file.close();
	delete qimage;
	return result;
}

bool ImageFileSaveJPG::SaveJPEGQtGreyscale(
	QString filename, unsigned char *image, int width, int height, int quality)
{

	if (!image)
	{
		qDebug() << "the image is a null pointer, this might be the case for optional channel(s). "
								"If this is the case, just rerender the image with enabled channel(s).";
		return false;
	}
	QImage *qimage = new QImage(width, height, QImage::Format_Indexed8);
	QVector<QRgb> my_table;
	for (int i = 0; i < 256; i++)
		my_table.push_back(qRgb(i, i, i));
	qimage->setColorTable(my_table);

	for (int line = 0; line < height; line++)
	{
		unsigned char *linePointer = &image[line * width];
		unsigned char *qScanLine = qimage->scanLine(line);
		memcpy(qScanLine, linePointer, sizeof(unsigned char) * width);
	}

	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	bool result = qimage->save(&file, "JPEG", quality);
	if (!result)
	{
		cErrorMessage::showMessage(
			QObject::tr("Can't save image to JPEG file!\n") + filename + "\n" + file.errorString(),
			cErrorMessage::errorMessage);
	}
	file.close();
	delete qimage;
	return result;
}

bool ImageFileSavePNG::SavePNGQtBlackAndWhite(
	QString filename, unsigned char *image, int width, int height)
{
	QImage *qimage = new QImage(width, height, QImage::Format_Mono);
	QVector<QRgb> my_table;
	my_table.push_back(qRgb(0, 0, 0));
	my_table.push_back(qRgb(255, 255, 255));
	qimage->setColorTable(my_table);

	for (int i = 0; i < width; i++)
	{
		for (int j = 0; j < height; j++)
		{
			qimage->setPixel(i, j, image[i + j * width]);
		}
	}
	QFile file(filename);
	file.open(QIODevice::WriteOnly);
	bool result = qimage->save(&file, "PNG");
	if (!result)
	{
		cErrorMessage::showMessage(
			QObject::tr("Can't save image to JPEG file!\n") + filename + "\n" + file.errorString(),
			cErrorMessage::errorMessage);
	}
	file.close();
	delete qimage;
	return result;
}

#ifdef USE_EXR
void ImageFileSaveEXR::SaveEXR(
	QString filename, cImage *image, QMap<enumImageContentType, structSaveImageChannel> imageConfig)
{
	int width = image->GetWidth();
	int height = image->GetHeight();

	Imf::Header header(width, height);
	Imf::FrameBuffer frameBuffer;

	header.compression() = Imf::ZIP_COMPRESSION;

	if (imageConfig.contains(IMAGE_CONTENT_COLOR))
	{
		// add rgb channel header
		Imf::PixelType imfQuality =
			imageConfig[IMAGE_CONTENT_COLOR].channelQuality == IMAGE_CHANNEL_QUALITY_32 ? Imf::FLOAT
																																									: Imf::HALF;

		header.channels().insert("R", Imf::Channel(imfQuality));
		header.channels().insert("G", Imf::Channel(imfQuality));
		header.channels().insert("B", Imf::Channel(imfQuality));

		int pixelSize = sizeof(tsRGB<half>);
		if (imfQuality == Imf::FLOAT) pixelSize = sizeof(tsRGB<float>);
		char *buffer = new char[(uint64_t)width * height * pixelSize];
		tsRGB<half> *halfPointer = (tsRGB<half> *)buffer;
		tsRGB<float> *floatPointer = (tsRGB<float> *)buffer;

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				uint64_t ptr = (x + y * width);
				if (imfQuality == Imf::FLOAT)
				{
					sRGB16 pixel = image->GetPixelImage16(x, y);
					floatPointer[ptr].R = (1.0 / 65536.0) * pixel.R;
					floatPointer[ptr].G = (1.0 / 65536.0) * pixel.G;
					floatPointer[ptr].B = (1.0 / 65536.0) * pixel.B;
				}
				else
				{
					sRGB16 pixel = image->GetPixelImage16(x, y);
					halfPointer[ptr].R = (1.0 / 65536.0) * pixel.R;
					halfPointer[ptr].G = (1.0 / 65536.0) * pixel.G;
					halfPointer[ptr].B = (1.0 / 65536.0) * pixel.B;
				}
			}
		}

		// point EXR frame buffer to rgb
		size_t compSize = (imfQuality == Imf::FLOAT ? sizeof(float) : sizeof(half));
		frameBuffer.insert("R",
			Imf::Slice(imfQuality, (char *)buffer + 0 * compSize, 3 * compSize, 3 * width * compSize));
		frameBuffer.insert("G",
			Imf::Slice(imfQuality, (char *)buffer + 1 * compSize, 3 * compSize, 3 * width * compSize));
		frameBuffer.insert("B",
			Imf::Slice(imfQuality, (char *)buffer + 2 * compSize, 3 * compSize, 3 * width * compSize));
	}

	if (imageConfig.contains(IMAGE_CONTENT_ALPHA))
	{
		// add alpha channel header
		Imf::PixelType imfQuality =
			imageConfig[IMAGE_CONTENT_ALPHA].channelQuality == IMAGE_CHANNEL_QUALITY_32 ? Imf::FLOAT
																																									: Imf::HALF;

		header.channels().insert("A", Imf::Channel(imfQuality));

		int pixelSize = sizeof(half);
		if (imfQuality == Imf::FLOAT) pixelSize = sizeof(float);
		char *buffer = new char[(uint64_t)width * height * pixelSize];
		half *halfPointer = (half *)buffer;
		float *floatPointer = (float *)buffer;

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
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
		frameBuffer.insert("A", Imf::Slice(imfQuality, (char *)buffer, compSize, width * compSize));
	}

	if (imageConfig.contains(IMAGE_CONTENT_ZBUFFER))
	{
		// add z Buffer channel header
		Imf::PixelType imfQuality =
			imageConfig[IMAGE_CONTENT_ZBUFFER].channelQuality == IMAGE_CHANNEL_QUALITY_32 ? Imf::FLOAT
																																										: Imf::HALF;

		header.channels().insert("Z", Imf::Channel(imfQuality));

		// point EXR frame buffer to z buffer
		if (imfQuality == Imf::FLOAT)
		{
			// direct on buffer
			float *zBuffer = image->GetZBufferPtr();
			frameBuffer.insert(
				"Z", Imf::Slice(Imf::FLOAT, (char *)zBuffer, sizeof(float), width * sizeof(float)));
		}
		else
		{
			int pixelSize = sizeof(half);
			char *buffer = new char[(uint64_t)width * height * pixelSize];
			half *halfPointer = (half *)buffer;

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					uint64_t ptr = x + y * width;
					halfPointer[ptr] = image->GetPixelZBuffer(x, y);
				}
			}
			frameBuffer.insert(
				"Z", Imf::Slice(imfQuality, (char *)buffer, sizeof(half), width * sizeof(half)));
		}
	}

	if (imageConfig.contains(IMAGE_CONTENT_NORMAL))
	{
		// add rgb channel header
		Imf::PixelType imfQuality =
			imageConfig[IMAGE_CONTENT_NORMAL].channelQuality == IMAGE_CHANNEL_QUALITY_32 ? Imf::FLOAT
																																									 : Imf::HALF;

		header.channels().insert("n.X", Imf::Channel(imfQuality));
		header.channels().insert("n.Y", Imf::Channel(imfQuality));
		header.channels().insert("n.Z", Imf::Channel(imfQuality));

		int pixelSize = sizeof(tsRGB<half>);
		if (imfQuality == Imf::FLOAT) pixelSize = sizeof(tsRGB<float>);
		char *buffer = new char[(uint64_t)width * height * pixelSize];
		tsRGB<half> *halfPointer = (tsRGB<half> *)buffer;
		tsRGB<float> *floatPointer = (tsRGB<float> *)buffer;

		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				uint64_t ptr = (x + y * width);
				sRGBfloat pixel = image->GetPixelNormal(x, y);
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
		frameBuffer.insert("n.X",
			Imf::Slice(imfQuality, (char *)buffer + 0 * compSize, 3 * compSize, 3 * width * compSize));
		frameBuffer.insert("n.Y",
			Imf::Slice(imfQuality, (char *)buffer + 1 * compSize, 3 * compSize, 3 * width * compSize));
		frameBuffer.insert("n.Z",
			Imf::Slice(imfQuality, (char *)buffer + 2 * compSize, 3 * compSize, 3 * width * compSize));
	}

	Imf::OutputFile file(filename.toStdString().c_str(), header);
	file.setFrameBuffer(frameBuffer);
	file.writePixels(height);
}
#endif /* USE_EXR */

#ifdef USE_TIFF
bool ImageFileSaveTIFF::SaveTIFF(
	QString filename, cImage *image, structSaveImageChannel imageChannel, bool appendAlpha)
{
	uint64_t width = image->GetWidth();
	uint64_t height = image->GetHeight();

	TIFF *tiff = TIFFOpen(filename.toLocal8Bit().constData(), "w");
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
		case IMAGE_CONTENT_COLOR: colorType = appendAlpha ? PHOTOMETRIC_RGB : PHOTOMETRIC_RGB; break;
		case IMAGE_CONTENT_ALPHA: colorType = PHOTOMETRIC_MINISBLACK; break;
		case IMAGE_CONTENT_ZBUFFER: colorType = PHOTOMETRIC_MINISBLACK; break;
		case IMAGE_CONTENT_NORMAL: colorType = PHOTOMETRIC_RGB; break;
		default: colorType = PHOTOMETRIC_RGB; break;
	}

	int samplesPerPixel = IMAGE_CONTENT_COLOR;
	switch (imageChannel.contentType)
	{
		case IMAGE_CONTENT_COLOR: samplesPerPixel = appendAlpha ? 4 : 3; break;
		case IMAGE_CONTENT_ALPHA: samplesPerPixel = 1; break;
		case IMAGE_CONTENT_ZBUFFER: samplesPerPixel = 1; break;
		case IMAGE_CONTENT_NORMAL: samplesPerPixel = 3; break;
	}

	TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, width);
	TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, height);
	TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, qualitySize);
	TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, samplesPerPixel);
	TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, height);

	TIFFSetField(tiff, TIFFTAG_COMPRESSION, COMPRESSION_DEFLATE);
	TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, colorType);
	TIFFSetField(tiff, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
	TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
	TIFFSetField(tiff, TIFFTAG_SAMPLEFORMAT, sampleFormat);

	uint64_t pixelSize = samplesPerPixel * qualitySize / 8;
	char *colorPtr = new char[(uint64_t)width * height * pixelSize];

	// calculate min / max values from zbuffer range
	float minZ = float(1.0e50);
	float maxZ = 0.0;
	float rangeZ = 0.0;
	if (imageChannel.contentType == IMAGE_CONTENT_ZBUFFER)
	{
		float *zbuffer = image->GetZBufferPtr();
		uint64_t size = width * height;
		for (uint64_t i = 0; i < size; i++)
		{
			float z = zbuffer[i];
			if (z > maxZ && z < 1e19) maxZ = z;
			if (z < minZ) minZ = z;
		}
		rangeZ = maxZ - minZ;
	}
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
							sRGBAfloat *typedColorPtr = (sRGBAfloat *)&colorPtr[ptr];
							sRGB16 rgbPointer = image->GetPixelImage16(x, y);
							typedColorPtr->R = rgbPointer.R / 65536.0;
							typedColorPtr->G = rgbPointer.G / 65536.0;
							typedColorPtr->B = rgbPointer.B / 65536.0;
							typedColorPtr->A = image->GetPixelAlpha(x, y) / 65536.0;
						}
						else
						{
							sRGBfloat *typedColorPtr = (sRGBfloat *)&colorPtr[ptr];
							sRGB16 rgbPointer = image->GetPixelImage16(x, y);
							typedColorPtr->R = rgbPointer.R / 65536.0;
							typedColorPtr->G = rgbPointer.G / 65536.0;
							typedColorPtr->B = rgbPointer.B / 65536.0;
						}
					}
					else if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
					{
						if (appendAlpha)
						{
							sRGBA16 *typedColorPtr = (sRGBA16 *)&colorPtr[ptr];
							*typedColorPtr = sRGBA16(image->GetPixelImage16(x, y));
							typedColorPtr->A = image->GetPixelAlpha(x, y);
						}
						else
						{
							sRGB16 *typedColorPtr = (sRGB16 *)&colorPtr[ptr];
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
								image->ConvertTo8bit();
							}
							sRGBA8 *typedColorPtr = (sRGBA8 *)&colorPtr[ptr];
							*typedColorPtr = sRGBA8(image->GetPixelImage8(x, y));
							typedColorPtr->A = image->GetPixelAlpha8(x, y);
						}
						else
						{
							if (x == 0 && y == 0)
							{
								image->ConvertTo8bit();
							}
							sRGB8 *typedColorPtr = (sRGB8 *)&colorPtr[ptr];
							*typedColorPtr = sRGB8(image->GetPixelImage8(x, y));
						}
					}
				}
				break;
				case IMAGE_CONTENT_ALPHA:
				{
					if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_32)
					{
						float *typedColorPtr = (float *)&colorPtr[ptr];
						*typedColorPtr = image->GetPixelAlpha(x, y) / 65536.0;
					}
					if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
					{
						unsigned short *typedColorPtr = (unsigned short *)&colorPtr[ptr];
						*typedColorPtr = image->GetPixelAlpha(x, y);
					}
					else
					{
						if (x == 0 && y == 0)
						{
							image->ConvertAlphaTo8bit();
						}
						unsigned char *typedColorPtr = (unsigned char *)&colorPtr[ptr];
						*typedColorPtr = image->GetPixelAlpha8(x, y);
					}
				}
				break;
				case IMAGE_CONTENT_ZBUFFER:
				{
					if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_32)
					{
						float *typedColorPtr = (float *)&colorPtr[ptr];
						*typedColorPtr = (image->GetPixelZBuffer(x, y) - minZ) / rangeZ;
					}
					if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
					{
						unsigned short *typedColorPtr = (unsigned short *)&colorPtr[ptr];
						*typedColorPtr =
							(unsigned short)(((image->GetPixelZBuffer(x, y) - minZ) / rangeZ) * 65535);
					}
					else
					{
						unsigned char *typedColorPtr = (unsigned char *)&colorPtr[ptr];
						*typedColorPtr =
							(unsigned char)(((image->GetPixelZBuffer(x, y) - minZ) / rangeZ) * 255);
					}
				}
				break;
				case IMAGE_CONTENT_NORMAL:
				{
					if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_32)
					{
						sRGBfloat *typedColorPtr = (sRGBfloat *)&colorPtr[ptr];
						*typedColorPtr = sRGBfloat(image->GetPixelNormal(x, y));
					}
					else if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
					{
						if (x == 0 && y == 0) image->ConvertNormalto16Bit();
						sRGB16 *typedColorPtr = (sRGB16 *)&colorPtr[ptr];
						*typedColorPtr = sRGB16(image->GetPixelNormal16(x, y));
					}
					else
					{
						if (x == 0 && y == 0) image->ConvertNormalto8Bit();
						sRGB8 *typedColorPtr = (sRGB8 *)&colorPtr[ptr];
						*typedColorPtr = sRGB8(image->GetPixelNormal8(x, y));
					}
				}
				break;
			}
		}
	}

	TIFFWriteEncodedStrip(tiff, 0, (void *)colorPtr, tsize_t(width * height * pixelSize));
	TIFFClose(tiff);
	delete[] colorPtr;
	return true;
}

#endif /* USE_TIFF */
