/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include "common_math.h"
#include "error_message.hpp"
#include "files.h"
#include "qimage.h"
#include "resource_http_provider.hpp"
#include "netrender.hpp"

// constructor
cTexture::cTexture(
	QString filename, enumUseMipmaps mode, int frameNo, bool beQuiet, bool useNetRender)
{
	bitmap = nullptr;

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
	bitmap = LoadPNG(filename, width, height);

	// if not, try to use Qt image loader
	if (!bitmap)
	{
		WriteLogString("Loading texture - loading using QImage", filename, 3);
		QImage qImage;
		qImage.load(filename);
		qImage = qImage.convertToFormat(QImage::Format_RGB888);
		if (!qImage.isNull())
		{
			width = qImage.width();
			height = qImage.height();
			bitmap = new sRGBA16[width * height];
			for (int y = 0; y < height; y++)
			{
				sRGB8 *line = reinterpret_cast<sRGB8 *>(qImage.scanLine(y));
				for (int x = 0; x < width; x++)
				{
					const sRGBA16 pixel(static_cast<unsigned short>(line[x].R) * 256,
						static_cast<unsigned short>(line[x].G) * 256,
						static_cast<unsigned short>(line[x].B) * 256, 65535);
					bitmap[x + y * width] = pixel;
				}
			}
		}
	}

	if (bitmap)
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
		bitmap = new sRGBA16[100 * 100];
		memset(bitmap, 255, sizeof(sRGBA16) * 100 * 100);
	}

	WriteLogString("Loading texture - finished", filename, 3);
}

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

void cTexture::FromQByteArray(QByteArray *buffer, enumUseMipmaps mode)
{
	if (bitmap)
	{
		delete[] bitmap;
	}

	bitmap = nullptr;
	QImage qImage(*buffer);
	qImage.loadFromData(*buffer);
	qImage = qImage.convertToFormat(QImage::Format_RGB888);

	if (!qImage.isNull())
	{
		width = qImage.width();
		height = qImage.height();
		bitmap = new sRGBA16[width * height];
		for (int y = 0; y < height; y++)
		{
			sRGB8 *line = reinterpret_cast<sRGB8 *>(qImage.scanLine(y));
			for (int x = 0; x < width; x++)
			{
				const sRGBA16 pixel(static_cast<unsigned short>(line[x].R) * 256,
					static_cast<unsigned short>(line[x].G) * 256,
					static_cast<unsigned short>(line[x].B) * 256, 65535);
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
		bitmap = new sRGBA16[100 * 100];
		memset(bitmap, 255, sizeof(sRGBA16) * 100 * 100);
	}
}

cTexture::cTexture()
{
	width = 100;
	height = 100;
	loaded = false;
	bitmap = new sRGBA16[100 * 100];
	memset(bitmap, 255, sizeof(sRGBA16) * 100 * 100);
}

// destructor
cTexture::~cTexture()
{
	if (bitmap)
	{
		delete[] bitmap;
		bitmap = nullptr;
	}
}

// read pixel
sRGBFloat cTexture::Pixel(double x, double y, double pixelSize) const
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

sRGBFloat cTexture::Pixel(CVector2<double> point, double pixelSize) const
{
	if (point.x > 0)
		point.x = fmod(point.x, 1.0);
	else
		point.x = 1.0 + fmod(point.x, 1.0);

	if (point.y > 0)
		point.y = fmod(point.y, 1.0);
	else
		point.y = 1.0 + fmod(point.y, 1.0);

	point.x *= double(width);
	point.y *= double(height);
	return MipMap(point.x, point.y, pixelSize);
}

sRGBA16 cTexture::LinearInterpolation(double x, double y) const
{
	sRGBA16 color;
	const int ix = int(x);
	const int iy = int(y);
	const double rx = x - int(x);
	const double ry = y - int(y);
	const sRGBA16 k1 = bitmap[iy * width + ix];
	const sRGBA16 k2 = bitmap[iy * width + ix + 1];
	const sRGBA16 k3 = bitmap[(iy + 1) * width + ix];
	const sRGBA16 k4 = bitmap[(iy + 1) * width + ix + 1];
	color.R = static_cast<unsigned short>(k1.R * (1.0 - rx) * (1.0 - ry) + k2.R * rx * (1.0 - ry)
																				+ k3.R * (1.0 - rx) * ry + k4.R * (rx * ry));
	color.G = static_cast<unsigned short>(k1.G * (1.0 - rx) * (1.0 - ry) + k2.G * rx * (1.0 - ry)
																				+ k3.G * (1.0 - rx) * ry + k4.G * (rx * ry));
	color.B = static_cast<unsigned short>(k1.B * (1.0 - rx) * (1.0 - ry) + k2.B * rx * (1.0 - ry)
																				+ k3.B * (1.0 - rx) * ry + k4.B * (rx * ry));
	return color;
}

sRGBFloat cTexture::BicubicInterpolation(double x, double y, const sRGBA16 *bitmap, int w, int h)
{
	const int ix = int(x);
	const int iy = int(y);
	const double rx = x - ix;
	const double ry = y - iy;

	double R[4][4], G[4][4], B[4][4];

	for (int yy = 0; yy < 4; yy++)
	{
		for (int xx = 0; xx < 4; xx++)
		{
			int ixx = ix + xx - 1;
			int iyy = iy + yy - 1;
			ixx = (ixx + w) % w;
			iyy = (iyy + h) % h;
			const int address2 = ixx + iyy * w;
			const sRGBA16 pixel = bitmap[address2];
			R[xx][yy] = pixel.R;
			G[xx][yy] = pixel.G;
			B[xx][yy] = pixel.B;
		}
	}

	double dR = bicubicInterpolate(R, rx, ry);
	double dG = bicubicInterpolate(G, rx, ry);
	double dB = bicubicInterpolate(B, rx, ry);
	if (dR < 0) dR = 0;
	if (dR > 65535) dR = 65535;
	if (dG < 0) dG = 0;
	if (dG > 65535) dG = 65535;
	if (dB < 0) dB = 0;
	if (dB > 65535) dB = 65535;

	return sRGBFloat(float(dR / 65536.0), float(dG / 65536.0), float(dB / 65536.0));
}

sRGBA16 cTexture::FastPixel(int x, int y) const
{
	return bitmap[x + y * width];
}

CVector3 cTexture::NormalMapFromBumpMap(CVector2<double> point, double bump, double pixelSize) const
{
	const int intX = int(point.x);
	const int intY = int(point.y);
	point.x = point.x - intX;
	point.y = point.y - intY;
	if (point.x < 0.0) point.x += 1.0;
	if (point.y < 0.0) point.y += 1.0;

	double m[3][3];
	for (int y = 0; y <= 2; y++)
	{
		for (int x = 0; x <= 2; x++)
		{
			m[x][y] = MipMap(point.x * width + x - 1.0, point.y * height + y - 1.0, pixelSize).R;
		}
	}
	CVector3 normal;
	normal.x = bump * (m[2][2] - m[0][2] + 2 * (m[2][1] - m[0][1]) + m[2][0] - m[0][0]);
	normal.y = bump * (m[0][0] - m[0][2] + 2 * (m[1][0] - m[1][2]) + m[2][0] - m[2][2]);
	normal.z = 1.0;
	normal.Normalize();
	return normal;
}

CVector3 cTexture::NormalMap(
	CVector2<double> point, double bump, bool invertGreen, double pixelSize) const
{
	const int intX = int(point.x);
	const int intY = int(point.y);
	point.x = point.x - intX;
	point.y = point.y - intY;
	if (point.x < 0.0) point.x += 1.0;
	if (point.y < 0.0) point.y += 1.0;
	const sRGBFloat normalPixel = MipMap(point.x * width, point.y * height, pixelSize);
	CVector3 normal(normalPixel.R * 2.0 - 1.0, normalPixel.G * 2.0 - 1.0, normalPixel.B);
	normal.x *= -bump;
	normal.y *= -bump;
	if (invertGreen) normal.y *= -1.0;
	normal.Normalize();

	return normal;
}

sRGBFloat cTexture::MipMap(double x, double y, double pixelSize) const
{
	pixelSize /= double(max(width, height));
	if (mipmaps.size() > 0 && pixelSize > 0)
	{
		if (pixelSize < 1e-20) pixelSize = 1e-20;
		double dMipLayer = -log(pixelSize) / log(2.0);
		if (dMipLayer < 0) dMipLayer = 0;
		if (dMipLayer + 1 >= mipmaps.size() - 1) dMipLayer = mipmaps.size() - 1;

		const int layerBig = int(dMipLayer);
		const int layerSmall = int(dMipLayer + 1);
		const double sizeMultipleBig = pow(2.0, double(layerBig));
		const double sizeMultipleSmall = pow(2.0, double(layerSmall));
		const double trans = dMipLayer - layerBig;
		const double transN = 1.0 - trans;

		const sRGBA16 *bigBitmap, *smallBitmap;
		CVector2<int> bigBitmapSize, smallBitmapSize;
		if (layerBig >= 0 && layerBig <= mipmaps.length() && layerSmall >= 0
				&& layerSmall <= mipmaps.length())
		{
			if (layerBig == 0)
			{
				bigBitmap = bitmap;
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
		return BicubicInterpolation(x, y, bitmap, width, height);
	}
}

void cTexture::CreateMipMaps()
{
	int prevW = width;
	int prevH = height;
	int w = width / 2;
	int h = height / 2;
	sRGBA16 *prevBitmap = bitmap;
	while (w > 0 && h > 0)
	{
		QVector<sRGBA16> newMipmapV(w * h);
		sRGBA16 *newMipmap = newMipmapV.data();

		for (int y = 0; y < h; y++)
		{
			for (int x = 0; x < w; x++)
			{
				sRGBA16 newPixel;
				const sRGBA16 p1 = prevBitmap[WrapInt(x * 2, prevW) + WrapInt(y * 2, prevH) * prevW];
				const sRGBA16 p2 = prevBitmap[WrapInt(x * 2 + 1, prevW) + WrapInt(y * 2, prevH) * prevW];
				const sRGBA16 p3 = prevBitmap[WrapInt(x * 2, prevW) + WrapInt(y * 2 + 1, prevH) * prevW];
				const sRGBA16 p4 =
					prevBitmap[WrapInt(x * 2 + 1, prevW) + WrapInt(y * 2 + 1, prevH) * prevW];
				newPixel.R = static_cast<unsigned short>((int(p1.R) + p2.R + p3.R + p4.R) / 4);
				newPixel.G = static_cast<unsigned short>((int(p1.G) + p2.G + p3.G + p4.G) / 4);
				newPixel.B = static_cast<unsigned short>((int(p1.B) + p2.B + p3.B + p4.B) / 4);
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
