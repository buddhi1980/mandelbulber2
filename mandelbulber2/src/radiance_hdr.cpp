/*
 * radiance_hdr.cpp
 *
 *  Created on: 9 lip 2020
 *      Author: krzysztof
 */

#include "radiance_hdr.h"

#define STB_IMAGE_IMPLEMENTATION
#define STBI_ONLY_HDR
#include "third-party/stb/stb_image.h"

cRadianceHDR::cRadianceHDR()
{
	// Just for testing
	int x, y, n;

	// check if it is radiance hdr image

	unsigned char *data = stbi_load("test.hdr", &x, &y, &n, 0);
	//    // ... process data if not NULL ...
	//    // ... x = width, y = height, n = # 8-bit components per pixel ...
	//    // ... replace '0' with '1'..'4' to force that many components per pixel
	//    // ... but 'n' will always be the number that it would have been if you said 0
	stbi_image_free(data);
}

cRadianceHDR::~cRadianceHDR()
{
	// TODO Auto-generated destructor stub
}

bool cRadianceHDR::Init(const QString filename, int *width, int *height)
{
	int w = 0;
	int h = 0;
	int bitsPerPixel = 0;

	if (stbi_is_hdr(filename.toLocal8Bit().constData()))
	{
		if (stbi_info(filename.toLocal8Bit().constData(), &w, &h, &bitsPerPixel))
		{
			*width = w;
			*height = h;
			actualFileName = filename;
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

void cRadianceHDR::Load(std::vector<sRGBFloat> *fBitmap)
{
	int w = 0;
	int h = 0;
	int comp = 0;
	float *data = stbi_loadf(actualFileName.toLocal8Bit().constData(), &w, &h, &comp, 0);
	if (data)
	{
		quint64 numberOFPixels = w * h;
		for (quint64 i = 0; i < numberOFPixels; i++)
		{
			fBitmap->data()[i] = sRGBFloat(data[i * 3], data[i * 3 + 1], data[i * 3 + 2]);
		}
		stbi_image_free(data);
	}
}
