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

#include <cstdio>
#include <string.h>

#include "files.h"
#include "error_message.hpp"
#include "initparameters.hpp"

using namespace std;

string logfileName;

//***************************** Index filename *************************
//funkcja numerująca pliki
//we:	filename - nazwa pliku bez rozszerzenia
//		extension - rozszerzenie
//		number - numer do dodania
//wy:	fullname - nazwa pliku z numerem i rozszerzeniem
//		return - ilość znaków

std::string IndexFilename(const char* filename, const char* extension, int number)
{
	char tmp[10];
	sprintf(tmp, "%.5i", number);
	return std::string(filename) + tmp + "." + extension;
}

bool FileIfExists(const char *filename)
{
	FILE *file;
	file = fopen(filename, "r");
	if (file)
	{
		fclose(file);
		return true;
	}
	else return false;
}

int fcopy(const char *source, const char *dest)
{
	// ------ file reading

	FILE * pFile;
	long int lSize;
	char *buffer;
	size_t result;

	pFile = fopen(source, "rb");
	if (pFile == NULL)
	{
		printf("Can't open source file for copying: %s\n", source);
		return 1;
	}

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	if (lSize > 0)
	{
		// allocate memory to contain the whole file:
		buffer = new char[lSize];

		// copy the file into the buffer:
		result = fread(buffer, 1, lSize, pFile);
		if (result != (size_t) lSize)
		{
			printf("Can't read source file for copying: %s\n", source);
			delete[] buffer;
			fclose(pFile);
			return 2;
		}
	}
	else
	{
		qCritical() << "Can't obtain file size: " << source;
		fclose(pFile);
		return 4;
	}
	fclose(pFile);

	// ----- file writing

	pFile = fopen(dest, "wb");
	if (pFile == NULL)
	{
		printf("Can't open destination file for copying: %s\n", dest);
		delete[] buffer;
		return 3;
	}
	fwrite(buffer, 1, lSize, pFile);
	fclose(pFile);

	delete[] buffer;
	return 0;
}

string removeFileExtension(const string &filename)
{
	size_t lastdot = filename.find_last_of(".");
	size_t lastSlash = filename.find_last_of("/");
	size_t lastBackSlash = filename.find_last_of("\\");
	if (lastdot == string::npos)
	{
		return filename;
	}
	else
	{
		if ((lastdot > lastSlash || lastSlash == string::npos)
				&& (lastdot > lastBackSlash || lastBackSlash == string::npos))
		{
			return filename.substr(0, lastdot);
		}
		else
		{
			return filename;
		}
	}
}

void BufferNormalize16(sRGB16 *buffer, unsigned int size)
{
	int maxR = 0;
	int maxG = 0;
	int maxB = 0;
	for (unsigned int i = 0; i < size; i++)
	{
		int R = buffer[i].R;
		if (R > maxR) maxR = R;
		int G = buffer[i].G;
		if (G > maxG) maxG = G;
		int B = buffer[i].B;
		if (B > maxB) maxB = B;
	}
	int max;
	max = (maxR > maxG) ? maxR : maxG;
	max = (maxB > max) ? maxB : max;
	double factor = 1.0;
	if (max > 0) factor = 65535.0 / max;
	for (unsigned int i = 0; i < size; i++)
	{
		buffer[i].R *= factor;
		buffer[i].G *= factor;
		buffer[i].B *= factor;
	}
}

/*
 void SaveAllImageLayers(const char *filename, cImage *image)
 {
 unsigned int width = image->GetWidth();
 unsigned int height = image->GetHeight();
 unsigned int size = width*height;
 sRGB16 *buffer16 = new sRGB16[width*height];
 sComplexImage *ci = image->GetComplexImagePtr();
 string file(filename);
 file = removeFileExtension(file);
 for(unsigned int i = 0; i<size; i++)	buffer16[i].R = buffer16[i].G = buffer16[i].B = ci[i].fogDensity16;
 BufferNormalize16(buffer16, size);
 SavePNG16((file+"_fog.png").c_str(), 100, width, height, buffer16);

 for(unsigned int i = 0; i<size; i++)	buffer16[i].R = buffer16[i].G = buffer16[i].B = ci[i].glowBuf16;
 BufferNormalize16(buffer16, size);
 SavePNG16((file+"_glow.png").c_str(), 100, width, height, buffer16);

 for(unsigned int i = 0; i<size; i++)	buffer16[i].R = buffer16[i].G = buffer16[i].B = ci[i].shadingBuf16;
 BufferNormalize16(buffer16, size);
 SavePNG16((file+"_shading.png").c_str(), 100, width, height, buffer16);

 for(unsigned int i = 0; i<size; i++)	buffer16[i].R = buffer16[i].G = buffer16[i].B = ci[i].shadowsBuf16;
 BufferNormalize16(buffer16, size);
 SavePNG16((file+"_shadow.png").c_str(), 100, width, height, buffer16);

 for(unsigned int i = 0; i<size; i++)	buffer16[i].R = buffer16[i].G = buffer16[i].B = ci[i].specularBuf16;
 BufferNormalize16(buffer16, size);
 SavePNG16((file+"_specular.png").c_str(), 100, width, height, buffer16);

 for(unsigned int i = 0; i<size; i++)	{buffer16[i].R = ci[i].ambientBuf16.R; buffer16[i].G = ci[i].ambientBuf16.G; buffer16[i].B = ci[i].ambientBuf16.B;}
 BufferNormalize16(buffer16, size);
 SavePNG16((file+"_ambient.png").c_str(), 100, width, height, buffer16);

 for(unsigned int i = 0; i<size; i++)	{buffer16[i].R = ci[i].auxLight.R; buffer16[i].G = ci[i].auxLight.G; buffer16[i].B = ci[i].auxLight.B;}
 BufferNormalize16(buffer16, size);
 SavePNG16((file+"_auxLights.png").c_str(), 100, width, height, buffer16);

 for(unsigned int i = 0; i<size; i++)	{buffer16[i].R = ci[i].auxSpecular.R; buffer16[i].G = ci[i].auxSpecular.G; buffer16[i].B = ci[i].auxSpecular.B;}
 BufferNormalize16(buffer16, size);
 SavePNG16((file+"_auxlightsSpec.png").c_str(), 100, width, height, buffer16);

 for(unsigned int i = 0; i<size; i++)	{buffer16[i].R = ci[i].backgroundBuf16.R; buffer16[i].G = ci[i].backgroundBuf16.G; buffer16[i].B = ci[i].backgroundBuf16.B;}
 BufferNormalize16(buffer16, size);
 SavePNG16((file+"_bkg.png").c_str(), 100, width, height, buffer16);

 for(unsigned int i = 0; i<size; i++)	{buffer16[i].R = ci[i].reflectBuf16.R; buffer16[i].G = ci[i].reflectBuf16.G; buffer16[i].B = ci[i].reflectBuf16.B;}
 BufferNormalize16(buffer16, size);
 SavePNG16((file+"_refl.png").c_str(), 100, width, height, buffer16);

 for(unsigned int i = 0; i<size; i++)	{buffer16[i].R = ci[i].volumetricLight.R; buffer16[i].G = ci[i].volumetricLight.G; buffer16[i].B = ci[i].volumetricLight.B;}
 BufferNormalize16(buffer16, size);
 SavePNG16((file+"_fogCol.png").c_str(), 100, width, height, buffer16);

 sImageAdjustments *imageAdjustments = image->GetImageAdjustments();
 unsigned short *colours = image->GetColourIndexBufPtr();
 for (unsigned int i = 0; i < size; i++)
 {
 int colorIndex = colours[i];
 int color_number;
 if (colorIndex >= 248 * 256)
 {
 color_number = colorIndex;
 }
 else
 {
 color_number = (int) (colorIndex * imageAdjustments->coloring_speed + 256 * imageAdjustments->paletteOffset) % 65536;
 }
 sRGB color = image->IndexToColour(color_number);
 buffer16[i].R = color.R; buffer16[i].G = color.G; buffer16[i].B = color.B;
 }
 BufferNormalize16(buffer16, size);
 SavePNG16((file+"_col.png").c_str(), 100, width, height, buffer16);

 unsigned short *alpha = image->GetAlphaBufPtr();
 for(unsigned int i = 0; i<size; i++)	{buffer16[i].R = alpha[i]; buffer16[i].G = alpha[i]; buffer16[i].B = alpha[i];}
 SavePNG16((file+"_alpha.png").c_str(), 100, width, height, buffer16);

 //normalize zBuffer
 double *zbuffer = image->GetZBufferPtr();
 float minZ = 1.0e50;
 float maxZ = 0.0;
 for (unsigned int i = 0; i < size; i++)
 {
 float z = zbuffer[i];
 if(z > maxZ && z < 1e19) maxZ = z;
 if(z < minZ) minZ = z;
 }
 float zRange = maxZ - minZ;
 float zFactor = 65000.0 / zRange;
 for(unsigned int i = 0; i<size; i++)
 {
 int z = (zbuffer[i] - minZ) * zFactor;
 if(zbuffer[i] > 1e19) z = 65535;
 buffer16[i].R = buffer16[i].G = buffer16[i].B = z;
 }
 SavePNG16((file+"_zBuffer.png").c_str(), 100, width, height, buffer16);

 SavePNG16((file+"_image16.png").c_str(), 100, width, height, image->GetImage16Ptr());

 delete [] buffer16;
 }
 */

void SaveImage(QString filename, ImageFileSave::enumImageFileType filetype, cImage *image)
{
	ImageFileSave::ImageConfig imageConfig;
	QStringList imageChannelNames;
	imageChannelNames << "color" << "alpha" << "zbuffer" << "normal";
	// read image config from preferences
	for (int i = 0; i < imageChannelNames.size(); i++)
	{
		QString imageChannelName = imageChannelNames.at(i);
		if (gPar->Get<bool>(imageChannelName + "_enabled"))
		{
			ImageFileSave::enumImageContentType contentType = (ImageFileSave::enumImageContentType) i;
			ImageFileSave::enumImageChannelQualityType channelQuality =
					(ImageFileSave::enumImageChannelQualityType) gPar->Get<int>(imageChannelName + "_quality");
			QString postfix = gPar->Get<QString>(imageChannelName + "_postfix");
			imageConfig.insert(contentType, ImageFileSave::structSaveImageChannel(contentType, channelQuality, postfix));
		}
	}
	QFileInfo fi(filename);
	QString fileWithoutExtension = fi.path() + QDir::separator() + fi.baseName();
	ImageFileSave *imageFileSave = ImageFileSave::create(fileWithoutExtension, filetype, image, imageConfig);
	imageFileSave->SaveImage();
	// return SaveImage(fileWithoutExtension, filetype, image, imageConfig);
}
