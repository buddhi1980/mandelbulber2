/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2020 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * Wrapper for radiance HDR
 */

#include "radiance_hdr.h"

#include <QImage>
#include <QPixmap>

// custom includes
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
			imageWidth = w;
			imageHeight = h;
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
	fBitmap->resize(imageWidth * imageHeight);
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

void cRadianceHDR::LoadToQPixmap(QPixmap *pixmap)
{
	std::vector<sRGBFloat> fBitmap(imageWidth * imageHeight);
	Load(&fBitmap);

	std::vector<sRGB8> image8(imageWidth * imageHeight);
	for (size_t i = 0; i < imageWidth * imageHeight; i++)
	{
		sRGBFloat fPixel = fBitmap[i];
		quint8 R = uchar(qBound(0.0f, fPixel.R * 255.0f, 255.0f));
		quint8 G = uchar(qBound(0.0f, fPixel.G * 255.0f, 255.0f));
		quint8 B = uchar(qBound(0.0f, fPixel.B * 255.0f, 255.0f));

		image8[i] = sRGB8(R, G, B);
	}

	const QImage qImage(reinterpret_cast<const uchar *>(image8.data()), imageWidth, imageHeight,
		imageWidth * sizeof(sRGB8), QImage::Format_RGB888);

	pixmap->convertFromImage(qImage);
}
