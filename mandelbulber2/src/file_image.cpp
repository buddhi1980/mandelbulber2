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
#include "error_message.hpp"
#include "global_data.hpp"
#include "cimage.hpp"

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
	bool appendAlpha = gPar->Get<bool>("append_alpha_png")
			&& imageConfig.contains(IMAGE_CONTENT_COLOR) && imageConfig.contains(IMAGE_CONTENT_ALPHA);

	if (imageConfig.contains(IMAGE_CONTENT_COLOR))
	{
		QString fullFilename = filename + imageConfig[IMAGE_CONTENT_COLOR].postfix + ".png";
		SavePNG(fullFilename, image, imageConfig[IMAGE_CONTENT_COLOR], appendAlpha);
	}
	if (imageConfig.contains(IMAGE_CONTENT_ALPHA) && !appendAlpha)
	{
		QString fullFilename = filename + imageConfig[IMAGE_CONTENT_ALPHA].postfix + ".png";
		SavePNG(fullFilename, image, imageConfig[IMAGE_CONTENT_ALPHA]);
	}
	if (imageConfig.contains(IMAGE_CONTENT_ZBUFFER))
	{
		QString fullFilename = filename + imageConfig[IMAGE_CONTENT_ZBUFFER].postfix + ".png";
		SavePNG(fullFilename, image, imageConfig[IMAGE_CONTENT_ZBUFFER]);
	}
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


void ImageFileSavePNG::SavePNG(QString filename, cImage* image, structSaveImageChannel imageChannel, bool appendAlpha)
{
	int width = image->GetWidth();
	int height = image->GetHeight();

	/* create file */
	FILE *fp = fopen(filename.toLocal8Bit().constData(), "wb");
	try
	{
		if (!fp)
			throw("[write_png_file] File %s could not be opened for writing.");

		/* initialize stuff */
		png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

		if (!png_ptr)
			throw("[write_png_file] png_create_write_struct failed");

		png_info *info_ptr = png_create_info_struct(png_ptr);
		if (!info_ptr)
			throw("[write_png_file] png_create_info_struct failed");

		if (setjmp(png_jmpbuf(png_ptr)))
			throw("[write_png_file] Error during init_io");

		png_init_io(png_ptr, fp);

		/* write header */
		if (setjmp(png_jmpbuf(png_ptr)))
			throw("[write_png_file] Error during writing header");

		if (imageChannel.channelQuality != IMAGE_CHANNEL_QUALITY_8
				&& imageChannel.channelQuality != IMAGE_CHANNEL_QUALITY_16)
		{
			// for PNG no more than 16 bit per channel possible
			imageChannel.channelQuality = IMAGE_CHANNEL_QUALITY_16;
		}

		int qualitySize;
		switch (imageChannel.channelQuality)
		{
			case IMAGE_CHANNEL_QUALITY_8:
				qualitySize = 8;
				break;
			case IMAGE_CHANNEL_QUALITY_16:
				qualitySize = 16;
				break;
			default:
				qualitySize = 8;
				break;
		}
		int qualitySizeByte = qualitySize / 8;

		int colorType;
		switch (imageChannel.contentType)
		{
			case IMAGE_CONTENT_COLOR:
				colorType = appendAlpha ? PNG_COLOR_TYPE_RGB_ALPHA : PNG_COLOR_TYPE_RGB;
				break;
			case IMAGE_CONTENT_ALPHA:
				colorType = PNG_COLOR_TYPE_GRAY;
				break;
			case IMAGE_CONTENT_ZBUFFER:
				colorType = PNG_COLOR_TYPE_GRAY;
				break;
			default:
				colorType = PNG_COLOR_TYPE_RGB;
				break;
		}

		png_set_IHDR(png_ptr,
								 info_ptr,
								 width,
								 height,
								 qualitySize,
								 colorType,
								 PNG_INTERLACE_NONE,
								 PNG_COMPRESSION_TYPE_BASE,
								 PNG_FILTER_TYPE_BASE);

		png_write_info(png_ptr, info_ptr);
		png_set_swap(png_ptr);

		/* write bytes */
		if (setjmp(png_jmpbuf(png_ptr)))
			throw("[write_png_file] Error during writing bytes");

		png_bytep *row_pointers = new png_bytep[height];
		char *colorPtr = NULL;

		int pixelSize = qualitySizeByte;

		switch (imageChannel.contentType)
		{
			case IMAGE_CONTENT_COLOR:
				pixelSize *= appendAlpha ? 4 : 3;
				break;
			case IMAGE_CONTENT_ALPHA:
				pixelSize *= 1;
				break;
			case IMAGE_CONTENT_ZBUFFER:
				pixelSize *= 1;
				break;
			case IMAGE_CONTENT_NORMAL:
				pixelSize *= 3;
				break;
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
						directPointer = (char*) image->GetImage16Ptr();
					}
					else
					{
						directPointer = (char*) image->ConvertTo8bit();
					}
				}
					break;
				case IMAGE_CONTENT_ALPHA:
				{
					if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
					{
						directPointer = (char*) image->GetAlphaBufPtr();
					}
					else
					{
						directPointer = (char*) image->ConvertAlphaTo8bit();
					}
				}
					break;
				case IMAGE_CONTENT_ZBUFFER:
				{
					// zbuffer is float, so direct buffer write is not applicable
				}
					break;
			}

			for (int y = 0; y < height; y++)
			{
				row_pointers[y] = (png_byte*) &directPointer[y * width * pixelSize];
			}
		}
		else
		{
			colorPtr = new char[(unsigned long int) width * height * pixelSize];

			// calculate min / max values from zbuffer range
			float minZ = 1.0e50;
			float maxZ = 0.0;
			if (imageChannel.contentType == IMAGE_CONTENT_ZBUFFER)
			{
				float *zbuffer = image->GetZBufferPtr();
				unsigned int size = width * height;
				for (unsigned int i = 0; i < size; i++)
				{
					float z = zbuffer[i];
					if (z > maxZ && z < 1e19) maxZ = z;
					if (z < minZ) minZ = z;
				}
			}
			double kZ = log(maxZ / minZ);

			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					unsigned long int ptr = (x + y * width) * pixelSize;
					switch (imageChannel.contentType)
					{
						case IMAGE_CONTENT_COLOR:
						{
							if (imageChannel.channelQuality == IMAGE_CHANNEL_QUALITY_16)
							{
								if (appendAlpha)
								{
									sRGBA16* typedColorPtr = (sRGBA16*) &colorPtr[ptr];
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
									sRGBA8* typedColorPtr = (sRGBA8*) &colorPtr[ptr];
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

								unsigned short* typedColorPtr = (unsigned short*) &colorPtr[ptr];
								*typedColorPtr = (unsigned short) (intZ);
							}
							else
							{
								float z = image->GetPixelZBuffer(x, y);
								float z1 = log(z / minZ) / kZ;
								int intZ = z1 * 240;
								if (z > 1e19) intZ = 255;

								unsigned char* typedColorPtr = (unsigned char*) &colorPtr[ptr];
								*typedColorPtr = (unsigned char) (intZ);
							}
						}
							break;
					}
				}
				row_pointers[y] = (png_byte*) &colorPtr[y * width * pixelSize];
			}
		}

		png_write_image(png_ptr, row_pointers);

		/* end write */
		if (setjmp(png_jmpbuf(png_ptr)))
			throw("[write_png_file] Error during end of write");

		png_write_end(png_ptr, NULL);
		delete[] row_pointers;
		if (colorPtr) delete[] colorPtr;
		fclose(fp);
	}
	catch (QString &status)
	{
		if (fp) fclose(fp);
		cErrorMessage::showMessage(QObject::tr("Can't save image to PNG file!\n")
															 + filename + "\n" + status, cErrorMessage::errorMessage);
	}
}
