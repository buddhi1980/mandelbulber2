/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen (jenzebas@gmail.com)
 *
 * data structures for color representation
 */

#ifndef MANDELBULBER2_SRC_COLOR_STRUCTURES_HPP_
#define MANDELBULBER2_SRC_COLOR_STRUCTURES_HPP_

#include <QtCore>

template <typename T>
struct tsRGB
{
	T R, G, B;
	tsRGB(T r, T g, T b) : R(r), G(g), B(b) {}

	tsRGB()
	{
		R = 0;
		G = 0;
		B = 0;
	}
};

template <typename T>
struct tsRGBA
{
	T R, G, B, A;
	tsRGBA(T r, T g, T b, T a) : R(r), G(g), B(b), A(a) {}

	tsRGBA()
	{
		R = 0;
		G = 0;
		B = 0;
		A = 0;
	}

	tsRGBA(tsRGB<T> rgb) : R(rgb.R), G(rgb.G), B(rgb.B), A(0) {}
};

// explicitly define constructor for rgba with regards to max value of Opacity
template <>
inline tsRGBA<quint8>::tsRGBA()
{
	R = 0;
	G = 0;
	B = 0;
	A = static_cast<quint8>(255);
}
template <>
inline tsRGBA<quint16>::tsRGBA()
{
	R = 0;
	G = 0;
	B = 0;
	A = static_cast<quint16>(65535);
}
template <>
inline tsRGBA<float>::tsRGBA()
{
	R = 0.0;
	G = 0.0;
	B = 0.0;
	A = 1.0;
}

typedef tsRGB<quint8> sRGB8;
typedef tsRGB<quint16> sRGB16;
typedef tsRGB<float> sRGBfloat;
typedef tsRGB<qint32> sRGB;

typedef tsRGBA<quint8> sRGBA8;
typedef tsRGBA<quint16> sRGBA16;
typedef tsRGBA<float> sRGBAfloat;

#endif /* MANDELBULBER2_SRC_COLOR_STRUCTURES_HPP_ */
