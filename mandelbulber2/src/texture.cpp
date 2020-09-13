/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cTexture class - simple bitmap container with pixel interpolation
 *
 * This class holds an sRGB8 *bitmap to store texture information. The class
 * can be initialized by loading an image file, or by loading a QByteArray (network).
 * Pixel(...) gets the pixel at a given point. The image data is MipMap-ped and
 * bicubic interpolated to give a "smooth" result.
 * more information on Mipmaps:  https://en.wikipedia.org/wiki/Mipmap
 */

#include "texture.hpp"

#include <memory>

#include "common_math.h"
#include "error_message.hpp"
#include "files.h"
#include "netrender.hpp"
#include "qimage.h"
#include "radiance_hdr.h"
#include "resource_http_provider.hpp"
#include "write_log.hpp"

// constructor
cTexture::cTexture(
	QString filename, enumUseMipmaps mode, int frameNo, bool beQuiet, bool useNetRender)
{
	WriteLogString("Loading texture", filename, 2);

	if (gNetRender->IsClient() && useNetRender)
	{
		filename = gNetRender->GetFileFromNetRender(filename, frameNo);
	}
	else
	{
		WriteLogString("Loading texture - AnimatedFileName()", filename, 3);
		filename = AnimatedFileName(filename, frameNo);

		WriteLogString("Loading texture - FilePathHelperTextures()", filename, 3);
		filename = FilePathHelperTextures(filename);

		WriteLogString("Loading texture - httpProvider()", filename, 3);
		cResourceHttpProvider httpProvider(filename);
		if (httpProvider.IsUrl()) filename = httpProvider.cacheAndGetFilename();
	}

	// try to load image if it's PNG format (this one supports 16-bit depth images)
	WriteLogString("Loading texture - LoadPNG()", filename, 3);
	std::vector<sRGBA16> bitmap16 = LoadPNG(filename, width, height);
	if (!bitmap16.empty())
	{
		bitmap.resize(bitmap16.size());
		for (quint64 i = 0; i < bitmap16.size(); i++)
		{
			sRGBFloat pixel(bitmap16[i].R / 65536.0f, bitmap16[i].G / 65536.0f, bitmap16[i].B / 65536.0f);
			bitmap[i] = pixel;
		}
		bitmap16.clear();
	}

	// check if it is Radiance HDR image
	std::unique_ptr<cRadianceHDR> radiance(new cRadianceHDR());
	if (radiance->Init(filename, &width, &height))
	{
		radiance->Load(&bitmap);
		loaded = true;
	}

	// if not, try to use Qt image loader
	if (bitmap.empty())
	{
		WriteLogString("Loading texture - loading using QImage", filename, 3);
		QImage qImage;
		qImage.load(filename);
		qImage = qImage.convertToFormat(QImage::Format_RGB888);
		if (!qImage.isNull())
		{
			width = qImage.width();
			height = qImage.height();
			bitmap.resize(width * height);
			for (int y = 0; y < height; y++)
			{
				sRGB8 *line = reinterpret_cast<sRGB8 *>(qImage.scanLine(y));
				for (int x = 0; x < width; x++)
				{
					const sRGBFloat pixel(line[x].R / 256.0f, line[x].G / 256.0f, line[x].B / 256.0f);
					bitmap[x + y * width] = pixel;
				}
			}
		}
	}

	if (!bitmap.empty())
	{
		loaded = true;
		originalFileName = filename;
		if (mode == useMipmaps)
		{
			WriteLogString("Loading texture - CreateMipMaps()", filename, 3);
			CreateMipMaps();
		}
	}
	else
	{
		if (!beQuiet && !useNetRender)
			gErrorMessage->showMessageFromOtherThread(
				QObject::tr("Can't load texture!\n") + filename, cErrorMessage::errorMessage);
		width = 100;
		height = 100;
		loaded = false;
		bitmap.resize(100 * 100);
		std::fill(bitmap.begin(), bitmap.end(), sRGBFloat(1.0, 1.0, 1.0));
	}

	WriteLogString("Loading texture - finished", filename, 3);
}

/*
// copy constructor
cTexture::cTexture(const cTexture &tex)
{
	width = tex.width;
	height = tex.height;
	loaded = tex.loaded;
	originalFileName = tex.originalFileName;
	bitmap = new sRGBA16[width * height];
	memcpy(bitmap, tex.bitmap, sizeof(sRGBA16) * width * height);
	mipmaps = tex.mipmaps;
	mipmapSizes = tex.mipmapSizes;
}

cTexture &cTexture::operator=(const cTexture &tex)
{
	if (bitmap)
	{
		delete[] bitmap;
		bitmap = nullptr;
	}
	width = tex.width;
	height = tex.height;
	loaded = tex.loaded;
	originalFileName = tex.originalFileName;
	bitmap = new sRGBA16[width * height];
	memcpy(bitmap, tex.bitmap, sizeof(sRGBA16) * width * height);
	mipmaps = tex.mipmaps;
	mipmapSizes = tex.mipmapSizes;

	return *this;
}

cTexture &cTexture::operator=(cTexture &&tex)
{
	if (bitmap)
	{
		delete[] bitmap;
		bitmap = nullptr;
	}
	width = tex.width;
	height = tex.height;
	loaded = tex.loaded;
	originalFileName = tex.originalFileName;
	mipmaps = tex.mipmaps;
	mipmapSizes = tex.mipmapSizes;

	// move
	bitmap = tex.bitmap;
	tex.bitmap = nullptr;
	tex.loaded = false;

	return *this;
}
*/

void cTexture::FromQByteArray(QByteArray *buffer, enumUseMipmaps mode)
{
	QImage qImage(*buffer);
	qImage.loadFromData(*buffer);
	qImage = qImage.convertToFormat(QImage::Format_RGB888);

	if (!qImage.isNull())
	{
		width = qImage.width();
		height = qImage.height();
		bitmap.resize(width * height);
		for (int y = 0; y < height; y++)
		{
			sRGB8 *line = reinterpret_cast<sRGB8 *>(qImage.scanLine(y));
			for (int x = 0; x < width; x++)
			{
				const sRGBFloat pixel(line[x].R / 256.0f, line[x].G / 256.0f, line[x].B / 256.0f);
				bitmap[x + y * width] = pixel;
			}
		}

		loaded = true;

		if (mode == useMipmaps)
		{
			CreateMipMaps();
		}
	}
	else
	{
		cErrorMessage::showMessage(
			QObject::tr("Can't load texture from QByteArray!\n"), cErrorMessage::errorMessage);
		width = 100;
		height = 100;
		loaded = false;
		bitmap.resize(100 * 100);
		std::fill(bitmap.begin(), bitmap.end(), sRGBFloat(1.0, 1.0, 1.0));
	}
}

cTexture::cTexture()
{
	width = 100;
	height = 100;
	loaded = false;
	bitmap.resize(100 * 100);
	std::fill(bitmap.begin(), bitmap.end(), sRGBFloat(1.0, 1.0, 1.0));
}

// destructor
cTexture::~cTexture()
{
	//	if (bitmap)
	//	{
	//		delete[] bitmap;
	//		bitmap = nullptr;
	//	}
}

// read pixel
sRGBFloat cTexture::Pixel(float x, float y, float pixelSize) const
{
	if (x >= 0 && x < width && y >= 0 && y < height - 1.0)
	{
		return MipMap(x, y, pixelSize);
	}
	else
	{
		return sRGBFloat(0.0, 0.0, 0.0);
	}
}

sRGBFloat cTexture::Pixel(CVector2<float> point, float pixelSize) const
{
	if (point.x > 0)
		point.x = fmod(point.x, 1.0);
	else
		point.x = 1.0 + fmod(point.x, 1.0);

	if (point.y > 0)
		point.y = fmod(point.y, 1.0);
	else
		point.y = 1.0 + fmod(point.y, 1.0);

	point.x *= float(width);
	point.y *= float(height);
	return MipMap(point.x, point.y, pixelSize);
}

sRGBFloat cTexture::LinearInterpolation(float x, float y) const
{
	sRGBFloat color;
	const int ix = int(x);
	const int iy = int(y);
	const float rx = x - int(x);
	const float ry = y - int(y);
	const sRGBFloat k1 = bitmap[iy * width + ix];
	const sRGBFloat k2 = bitmap[iy * width + ix + 1];
	const sRGBFloat k3 = bitmap[(iy + 1) * width + ix];
	const sRGBFloat k4 = bitmap[(iy + 1) * width + ix + 1];
	color.R = (k1.R * (1.0f - rx) * (1.0f - ry) + k2.R * rx * (1.0f - ry) + k3.R * (1.0f - rx) * ry
						 + k4.R * (rx * ry));
	color.G = (k1.G * (1.0f - rx) * (1.0f - ry) + k2.G * rx * (1.0f - ry) + k3.G * (1.0f - rx) * ry
						 + k4.G * (rx * ry));
	color.B = (k1.B * (1.0f - rx) * (1.0f - ry) + k2.B * rx * (1.0f - ry) + k3.B * (1.0f - rx) * ry
						 + k4.B * (rx * ry));
	return color;
}

sRGBFloat cTexture::BicubicInterpolation(float x, float y, const sRGBFloat *bitm, int w, int h)
{
	const int ix = int(x);
	const int iy = int(y);
	const float rx = x - ix;
	const float ry = y - iy;

	float R[4][4], G[4][4], B[4][4];

	for (int yy = 0; yy < 4; yy++)
	{
		for (int xx = 0; xx < 4; xx++)
		{
			int ixx = ix + xx - 1;
			int iyy = iy + yy - 1;
			ixx = (ixx + w) % w;
			iyy = (iyy + h) % h;
			const int address2 = ixx + iyy * w;
			const sRGBFloat pixel = bitm[address2];
			R[xx][yy] = pixel.R;
			G[xx][yy] = pixel.G;
			B[xx][yy] = pixel.B;
		}
	}

	float dR = bicubicInterpolate(R, rx, ry);
	float dG = bicubicInterpolate(G, rx, ry);
	float dB = bicubicInterpolate(B, rx, ry);
	if (dR < 0.0f) dR = 0.0f;
	// if (dR > 65535) dR = 65535;
	if (dG < 0.0f) dG = 0.0f;
	// if (dG > 65535) dG = 65535;
	if (dB < 0.0f) dB = 0.0f;
	// if (dB > 65535) dB = 65535;

	return sRGBFloat(dR, dG, dB);
}

sRGBFloat cTexture::FastPixel(int x, int y) const
{
	return bitmap[x + y * width];
}

CVector3 cTexture::NormalMapFromBumpMap(CVector2<float> point, float bump, float pixelSize) const
{
	const int intX = int(point.x);
	const int intY = int(point.y);
	point.x = point.x - intX;
	point.y = point.y - intY;
	if (point.x < 0.0f) point.x += 1.0f;
	if (point.y < 0.0f) point.y += 1.0f;

	float m[3][3];
	for (int y = 0; y <= 2; y++)
	{
		for (int x = 0; x <= 2; x++)
		{
			m[x][y] = MipMap(point.x * width + x - 1.0f, point.y * height + y - 1.0f, pixelSize).R;
		}
	}
	CVector3 normal;
	normal.x = bump * (m[2][2] - m[0][2] + 2 * (m[2][1] - m[0][1]) + m[2][0] - m[0][0]);
	normal.y = bump * (m[0][0] - m[0][2] + 2 * (m[1][0] - m[1][2]) + m[2][0] - m[2][2]);
	normal.z = 1.0f;
	normal.Normalize();
	return normal;
}

CVector3 cTexture::NormalMap(
	CVector2<float> point, float bump, bool invertGreen, float pixelSize) const
{
	const int intX = int(point.x);
	const int intY = int(point.y);
	point.x = point.x - intX;
	point.y = point.y - intY;
	if (point.x < 0.0f) point.x += 1.0f;
	if (point.y < 0.0f) point.y += 1.0f;
	const sRGBFloat normalPixel = MipMap(point.x * width, point.y * height, pixelSize);
	CVector3 normal(normalPixel.R * 2.0f - 1.0f, normalPixel.G * 2.0f - 1.0f, normalPixel.B);
	normal.x *= -bump;
	normal.y *= -bump;
	if (invertGreen) normal.y *= -1.0f;
	normal.Normalize();

	return normal;
}

sRGBFloat cTexture::MipMap(float x, float y, float pixelSize) const
{
	pixelSize /= float(max(width, height));
	if (mipmaps.size() > 0 && pixelSize > 0)
	{
		if (pixelSize < 1e-20f) pixelSize = 1e-20f;
		float dMipLayer = -log(pixelSize) / log(2.0f);
		if (dMipLayer < 0) dMipLayer = 0;
		if (dMipLayer + 1 >= mipmaps.size() - 1) dMipLayer = mipmaps.size() - 1;

		const int layerBig = int(dMipLayer);
		const int layerSmall = int(dMipLayer + 1);
		const float sizeMultipleBig = pow(2.0f, float(layerBig));
		const float sizeMultipleSmall = pow(2.0f, float(layerSmall));
		const float trans = dMipLayer - layerBig;
		const float transN = 1.0f - trans;

		const sRGBFloat *bigBitmap, *smallBitmap;
		CVector2<int> bigBitmapSize, smallBitmapSize;
		if (layerBig >= 0 && layerBig <= mipmaps.length() && layerSmall >= 0
				&& layerSmall <= mipmaps.length())
		{
			if (layerBig == 0)
			{
				bigBitmap = bitmap.data();
				smallBitmap = mipmaps[layerSmall - 1].data();
				bigBitmapSize.x = width;
				bigBitmapSize.y = height;
				smallBitmapSize = mipmapSizes[layerSmall - 1];
			}
			else
			{
				bigBitmap = mipmaps[layerBig - 1].data();
				smallBitmap = mipmaps[layerSmall - 1].data();
				bigBitmapSize = mipmapSizes[layerBig - 1];
				smallBitmapSize = mipmapSizes[layerSmall - 1];
			}
			const sRGBFloat pixelFromBig = BicubicInterpolation(
				x / sizeMultipleBig, y / sizeMultipleBig, bigBitmap, bigBitmapSize.x, bigBitmapSize.y);
			const sRGBFloat pixelFromSmall = BicubicInterpolation(x / sizeMultipleSmall,
				y / sizeMultipleSmall, smallBitmap, smallBitmapSize.x, smallBitmapSize.y);

			sRGBFloat pixel;
			pixel.R = float(pixelFromSmall.R * trans + pixelFromBig.R * transN);
			pixel.G = float(pixelFromSmall.G * trans + pixelFromBig.G * transN);
			pixel.B = float(pixelFromSmall.B * trans + pixelFromBig.B * transN);
			return pixel;
		}
		else
		{
			return sRGBFloat();
		}
	}
	else
	{
		return BicubicInterpolation(x, y, bitmap.data(), width, height);
	}
}

void cTexture::CreateMipMaps()
{
	int prevW = width;
	int prevH = height;
	int w = width / 2;
	int h = height / 2;
	sRGBFloat *prevBitmap = bitmap.data();
	while (w > 0 && h > 0)
	{
		QVector<sRGBFloat> newMipmapV(w * h);
		sRGBFloat *newMipmap = newMipmapV.data();

		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				sRGBFloat newPixel;
				const sRGBFloat p1 = prevBitmap[WrapInt(x * 2, prevW) + WrapInt(y * 2, prevH) * prevW];
				const sRGBFloat p2 = prevBitmap[WrapInt(x * 2 + 1, prevW) + WrapInt(y * 2, prevH) * prevW];
				const sRGBFloat p3 = prevBitmap[WrapInt(x * 2, prevW) + WrapInt(y * 2 + 1, prevH) * prevW];
				const sRGBFloat p4 =
					prevBitmap[WrapInt(x * 2 + 1, prevW) + WrapInt(y * 2 + 1, prevH) * prevW];
				newPixel.R = (p1.R + p2.R + p3.R + p4.R) / 4.0f;
				newPixel.G = (p1.G + p2.G + p3.G + p4.G) / 4.0f;
				newPixel.B = (p1.B + p2.B + p3.B + p4.B) / 4.0f;
				newMipmap[x + y * w] = newPixel;
			}
		}
		mipmapSizes.append(CVector2<int>(w, h));
		mipmaps.append(newMipmapV);
		prevH = h;
		prevW = w;
		w /= 2;
		h /= 2;
		prevBitmap = mipmaps.last().data();
	}
}
