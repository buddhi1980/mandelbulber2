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

#ifndef MANDELBULBER2_SRC_TEXTURE_HPP_
#define MANDELBULBER2_SRC_TEXTURE_HPP_

#include <qbytearray.h>
#include <qlist.h>
#include <qstring.h>

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

	cTexture(QString filename, enumUseMipmaps mode, bool beQuiet = false);
	cTexture();
	cTexture(const cTexture &tex);
	cTexture &operator=(const cTexture &tex);

	~cTexture(void);
	int Height(void) { return height; }
	int Width(void) { return width; }
	sRGBfloat Pixel(double x, double y, double pixelSize = 0.0);
	sRGBfloat Pixel(CVector2<double> point, double pixelSize = 0.0) const;
	sRGB8 FastPixel(int x, int y);
	bool IsLoaded(void) const { return loaded; }
	QString GetFileName() { return originalFileName; }
	void FromQByteArray(QByteArray *buffer, enumUseMipmaps mode);
	CVector3 NormalMapFromBumpMap(CVector2<double> point, double bump, double pixelSize = 0.0) const;
	CVector3 NormalMap(CVector2<double> point, double bump, double pixelSize = 0.0) const;
	void SetInvertGreen(bool invert) { invertGreen = invert; }

private:
	sRGB8 LinearInterpolation(double x, double y);
	sRGBfloat BicubicInterpolation(double x, double y, const sRGB8 *bitmap, int w, int h) const;
	sRGBfloat MipMap(double x, double y, double pixelSize) const;
	void CreateMipMaps();
	inline int WrapInt(int a, int size) { return (a + size) % size; }
	sRGB8 *bitmap;
	int width;
	int height;
	bool loaded;
	QString originalFileName;
	QList<QVector<sRGB8>> mipmaps;
	QList<CVector2<int>> mipmapSizes;
	bool invertGreen;
};

#endif /* MANDELBULBER2_SRC_TEXTURE_HPP_ */
