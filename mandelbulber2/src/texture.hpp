/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#ifndef MANDELBULBER2_SRC_TEXTURE_HPP_
#define MANDELBULBER2_SRC_TEXTURE_HPP_

#include <vector>

#include <QByteArray>
#include <QCache>
#include <QList>
#include <QString>
#include <QVector>

#include "algebra.hpp"
#include "color_structures.hpp"

class cTexture
{
public:
	enum enumUseMipmaps
	{
		doNotUseMipmaps,
		useMipmaps
	};

	cTexture(QString filename, enumUseMipmaps mode, int frameNo, bool beQuiet, bool useNetRender);
	cTexture();
	cTexture(const cTexture &tex);
	cTexture &operator=(const cTexture &tex);
	cTexture &operator=(cTexture &&tex);
	cTexture(cTexture &&other);

	~cTexture() = default;
	int Height() const { return height; }
	int Width() const { return width; }
	sRGBFloat Pixel(float x, float y, float pixelSize = 0.0) const;
	sRGBFloat Pixel(CVector2<float> point, float pixelSize = 0.0) const;
	inline sRGBFloat FastPixel(int x, int y) const { return bitmapFloat[x + y * width]; }
	bool IsLoaded() const { return loaded; }
	QString GetFileName() const { return originalFileName; }
	void FromQByteArray(QByteArray *buffer, enumUseMipmaps mode);
	CVector3 NormalMapFromBumpMap(CVector2<float> point, float bump, float pixelSize = 0.0) const;
	CVector3 NormalMap(
		CVector2<float> point, float bump, bool invertGreen, float pixelSize = 0.0) const;
	size_t GetMemorySize() const;
	const std::vector<sRGBA8> &GetHDRBitmap() const { return bitmapHDR; }

private:
	sRGBFloat LinearInterpolation(float x, float y) const;
	static sRGBFloat BicubicInterpolation(float x, float y, const sRGBFloat *_bitmap, int w, int h);
	sRGBFloat MipMap(float x, float y, float pixelSize) const;
	void CreateMipMaps();
	static int WrapInt(int a, int size) { return (a + size) % size; }
	void ComputeHDRBItmap();
	std::vector<sRGBFloat> bitmapFloat;
	std::vector<sRGBA8> bitmapHDR;
	int width;
	int height;
	bool loaded;
	QString originalFileName;
	QList<QVector<sRGBFloat>> mipmaps;
	QList<CVector2<int>> mipmapSizes;

	static const int defaultSize = 5;
};

#endif /* MANDELBULBER2_SRC_TEXTURE_HPP_ */
