/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-16 Krzysztof Marczak     §R-==%w["'~5]m%=L.=~5N
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

// constructor
cTexture::cTexture(QString filename, enumUseMipmaps mode, bool beQuiet)
{
	bitmap = nullptr;

	// try to load image if it's PNG format (this one supports 16-bit depth images)
	bitmap = LoadPNG(filename, width, height);

	// if not try to use Qt image loader
	if (!bitmap)
	{
		QImage qimage;
		qimage.load(filename);
		qimage = qimage.convertToFormat(QImage::Format_RGB888);
		if (!qimage.isNull())
		{
			width = qimage.width();
			height = qimage.height();
			bitmap = new sRGBA16[width * height];
			for (int y = 0; y < height; y++)
			{
				sRGB8 *line = reinterpret_cast<sRGB8 *>(qimage.scanLine(y));
				for (int x = 0; x < width; x++)
				{
					sRGBA16 pixel(static_cast<unsigned short>(line[x].R) * 256, static_cast<unsigned short>(line[x].G) * 256,
						static_cast<unsigned short>(line[x].B) * 256, 65535);
					bitmap[x + y * width] = pixel;
				}
			}
		}
	}

	if (bitmap)
	{
		// width = qimage.width();
		// height = qimage.height();
		// bitmap = new sRGB8[width * height];

		// for (int y = 0; y < height; y++)
		//{
		//	memcpy(&bitmap[y * width], qimage.scanLine(y), sizeof(sRGB8) * width);
		//}

		// qDebug() << "cTexture::cTexture(QString filename, bool beQuiet): "
		// 				 << "(sRGB8*)(qimage.bits());:" << width * height * sizeof(sRGB8);
		loaded = true;
		originalFileName = filename;
		if (mode == useMipmaps)
		{
			CreateMipMaps();
		}
	}
	else
	{
		if (!beQuiet)
			cErrorMessage::showMessage(
				QObject::tr("Can't load texture!\n") + filename, cErrorMessage::errorMessage);
		width = 100;
		height = 100;
		loaded = false;
		bitmap = new sRGBA16[100 * 100];
		memset(bitmap, 255, sizeof(sRGBA16) * 100 * 100);
		// qDebug() << "cTexture::cTexture(QString filename, bool beQuiet): "
		// 				 << "new sRGB8[100 * 100];:" << width * height * sizeof(sRGB8);
	}
	invertGreen = false;
}

// copy constructor
cTexture::cTexture(const cTexture &tex)
{
	width = tex.width;
	height = tex.height;
	loaded = tex.loaded;
	originalFileName = tex.originalFileName;
	bitmap = new sRGBA16[width * height];
	// qDebug() << "cTexture::cTexture(const cTexture &tex): "
	// 				 << "new sRGB8[width * height]:" << width * height * sizeof(sRGB8);
	memcpy(bitmap, tex.bitmap, sizeof(sRGBA16) * width * height);
	mipmaps = tex.mipmaps;
	mipmapSizes = tex.mipmapSizes;
	invertGreen = tex.invertGreen;
}

cTexture &cTexture::operator=(const cTexture &tex)
{
	if (bitmap)
	{
		delete[] bitmap;
		bitmap = nullptr;
		// qDebug() << "cTexture& cTexture::operator=(const cTexture &tex): "
		// 				 << "delete[] bitmap;:" << width * height * sizeof(sRGB8);
	}
	width = tex.width;
	height = tex.height;
	loaded = tex.loaded;
	originalFileName = tex.originalFileName;
	bitmap = new sRGBA16[width * height];
	// qDebug() << "cTexture& cTexture::operator=(const cTexture &tex): "
	// 				 << "new sRGB8[width * height];:" << width * height * sizeof(sRGB8);
	memcpy(bitmap, tex.bitmap, sizeof(sRGBA16) * width * height);
	mipmaps = tex.mipmaps;
	mipmapSizes = tex.mipmapSizes;
	invertGreen = tex.invertGreen;

	return *this;
}

void cTexture::FromQByteArray(QByteArray *buffer, enumUseMipmaps mode)
{
	if (bitmap)
	{
		// qDebug() << "void cTexture::FromQByteArray(QByteArray buffer):"
		// 				 << "delete[] bitmap;:" << width * height * sizeof(sRGB8);
		delete[] bitmap;
	}

	bitmap = nullptr;
	QImage qimage(*buffer);
	qimage.loadFromData(*buffer);
	qimage = qimage.convertToFormat(QImage::Format_RGB888);

	if (!qimage.isNull())
	{
		width = qimage.width();
		height = qimage.height();
		bitmap = new sRGBA16[width * height];
		for (int y = 0; y < height; y++)
		{
			sRGB8 *line = reinterpret_cast<sRGB8 *>(qimage.scanLine(y));
			for (int x = 0; x < width; x++)
			{
				sRGBA16 pixel(static_cast<unsigned short>(line[x].R) * 256, static_cast<unsigned short>(line[x].G) * 256,
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
	// qDebug() << "cTexture::cTexture(void):"
	// 				 << "new sRGB8[100 * 100]" << width * height * sizeof(sRGB8);
	invertGreen = false;
}

// destructor
cTexture::~cTexture()
{
	if (bitmap)
	{
		delete[] bitmap;
		bitmap = nullptr;
		// qDebug() << "cTexture::~cTexture(void):"
		// 				 << "delete[] bitmap:" << width * height * sizeof(sRGB8);
	}
	else
	{
		qCritical() << "cTexture::~cTexture(void): bitmap was not allocated before";
	}
}

// read pixel
sRGBfloat cTexture::Pixel(double x, double y, double pixelSize) const
{
	if (x >= 0 && x < width && y >= 0 && y < height - 1.0)
	{
		return MipMap(x, y, pixelSize);
	}
	else
	{
		return sRGBfloat(0.0, 0.0, 0.0);
	}
}

sRGBfloat cTexture::Pixel(CVector2<double> point, double pixelSize) const
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
	int ix = int(x);
	int iy = int(y);
	double rx = x - int(x);
	double ry = y - int(y);
	sRGBA16 k1 = bitmap[iy * width + ix];
	sRGBA16 k2 = bitmap[iy * width + ix + 1];
	sRGBA16 k3 = bitmap[(iy + 1) * width + ix];
	sRGBA16 k4 = bitmap[(iy + 1) * width + ix + 1];
	color.R = static_cast<unsigned short>(k1.R * (1.0 - rx) * (1.0 - ry) + k2.R * rx * (1.0 - ry)
		+ k3.R * (1.0 - rx) * ry + k4.R * (rx * ry));
	color.G = static_cast<unsigned short>(k1.G * (1.0 - rx) * (1.0 - ry) + k2.G * rx * (1.0 - ry)
		+ k3.G * (1.0 - rx) * ry + k4.G * (rx * ry));
	color.B = static_cast<unsigned short>(k1.B * (1.0 - rx) * (1.0 - ry) + k2.B * rx * (1.0 - ry)
		+ k3.B * (1.0 - rx) * ry + k4.B * (rx * ry));
	return color;
}

sRGBfloat cTexture::BicubicInterpolation(
	double x, double y, const sRGBA16 *bitmap, int w, int h)
{
	int ix = int(x);
	int iy = int(y);
	double rx = x - ix;
	double ry = y - iy;

	double R[4][4], G[4][4], B[4][4];

	for (int yy = 0; yy < 4; yy++)
	{
		for (int xx = 0; xx < 4; xx++)
		{
			int ixx = ix + xx - 1;
			int iyy = iy + yy - 1;
			ixx = (ixx + w) % w;
			iyy = (iyy + h) % h;
			int address2 = ixx + iyy * w;
			sRGBA16 pixel = bitmap[address2];
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

	return sRGBfloat(float(dR / 65536.0), float(dG / 65536.0), float(dB / 65536.0));
}

sRGBA16 cTexture::FastPixel(int x, int y) const
{
	return bitmap[x + y * width];
}

CVector3 cTexture::NormalMapFromBumpMap(CVector2<double> point, double bump, double pixelSize) const
{
	int intX = int(point.x);
	int intY = int(point.y);
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

CVector3 cTexture::NormalMap(CVector2<double> point, double bump, double pixelSize) const
{
	int intX = int(point.x);
	int intY = int(point.y);
	point.x = point.x - intX;
	point.y = point.y - intY;
	if (point.x < 0.0) point.x += 1.0;
	if (point.y < 0.0) point.y += 1.0;
	sRGBfloat normalPixel = MipMap(point.x * width, point.y * height, pixelSize);
	CVector3 normal(normalPixel.R * 2.0 - 1.0, normalPixel.G * 2.0 - 1.0, normalPixel.B);
	normal.x *= -bump;
	normal.y *= -bump;
	if (invertGreen) normal.y *= -1.0;
	normal.Normalize();

	return normal;
}

sRGBfloat cTexture::MipMap(double x, double y, double pixelSize) const
{
	pixelSize /= double(max(width, height));
	if (mipmaps.size() > 0 && pixelSize > 0)
	{
		if (pixelSize < 1e-20) pixelSize = 1e-20;
		double dMipLayer = -log(pixelSize) / log(2.0);
		if (dMipLayer < 0) dMipLayer = 0;
		if (dMipLayer + 1 >= mipmaps.size() - 1) dMipLayer = mipmaps.size() - 1;

		int layerBig = int(dMipLayer);
		int layerSmall = int(dMipLayer + 1);
		double sizeMultipBig = pow(2.0, double(layerBig));
		double sizeMultipSmall = pow(2.0, double(layerSmall));
		double trans = dMipLayer - layerBig;
		double transN = 1.0 - trans;

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
			sRGBfloat pixelFromBig = BicubicInterpolation(
				x / sizeMultipBig, y / sizeMultipBig, bigBitmap, bigBitmapSize.x, bigBitmapSize.y);
			sRGBfloat pixelFromSmall = BicubicInterpolation(x / sizeMultipSmall, y / sizeMultipSmall,
				smallBitmap, smallBitmapSize.x, smallBitmapSize.y);

			sRGBfloat pixel;
			pixel.R = float(pixelFromSmall.R * trans + pixelFromBig.R * transN);
			pixel.G = float(pixelFromSmall.G * trans + pixelFromBig.G * transN);
			pixel.B = float(pixelFromSmall.B * trans + pixelFromBig.B * transN);
			return pixel;
		}
		else
		{
			return sRGBfloat();
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
				sRGBA16 p1 = prevBitmap[WrapInt(x * 2, prevW) + WrapInt(y * 2, prevH) * prevW];
				sRGBA16 p2 = prevBitmap[WrapInt(x * 2 + 1, prevW) + WrapInt(y * 2, prevH) * prevW];
				sRGBA16 p3 = prevBitmap[WrapInt(x * 2, prevW) + WrapInt(y * 2 + 1, prevH) * prevW];
				sRGBA16 p4 = prevBitmap[WrapInt(x * 2 + 1, prevW) + WrapInt(y * 2 + 1, prevH) * prevW];
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
