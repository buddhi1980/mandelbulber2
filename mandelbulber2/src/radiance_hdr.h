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

#ifndef MANDELBULBER2_SRC_RADIANCE_HDR_H_
#define MANDELBULBER2_SRC_RADIANCE_HDR_H_

#include <functional>
#include <vector>

#include <QMap>
#include <QString>

#include "color_structures.hpp"

class QPixmap;

class cRadianceHDR
{
public:
	cRadianceHDR();
	~cRadianceHDR();

	bool Init(const QString filename, int *width, int *height);
	void Load(std::vector<sRGBFloat> *fBitmap);
	void LoadToQPixmap(QPixmap *pixmap);

	/**
	 * Save a scene linear float image as a classic Radiance RGBE file
	 * (FORMAT=32-bit_rle_rgbe). Negative values are clamped to zero since
	 * the format cannot represent them.
	 * @param filename target path
	 * @param width image width in pixels
	 * @param height image height in pixels
	 * @param rgb scene linear float pixels, size must be width*height
	 * @param meta optional render meta data, written as '#' comment lines
	 * @param progress optional callback with fraction [0..1] of written rows
	 * @return true on success
	 */
	static bool SaveRGBE(const QString &filename, uint64_t width, uint64_t height,
		const std::vector<sRGBFloat> &rgb, const QMap<QString, QString> &meta = {},
		const std::function<void(double)> &progress = nullptr);

private:
	QString actualFileName;
	size_t imageWidth = 0;
	size_t imageHeight = 0;
};

#endif /* MANDELBULBER2_SRC_RADIANCE_HDR_H_ */
