/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-16 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Histogram class
 */

#include "histogram.hpp"

cHistogram::cHistogram()
{
	histSize = 0;
	count = 0;
	sum = 0;
	data = nullptr;
}

cHistogram::cHistogram(int size)
{
	data = nullptr;
	Alloc(size);
}

cHistogram::~cHistogram()
{
	if (data) delete[] data;
	data = nullptr;
}

cHistogram::cHistogram(const cHistogram &source)
{
	data = nullptr;
	Copy(source);
}

cHistogram &cHistogram::operator=(const cHistogram &source)
{
	if (this != &source)
	{
		Copy(source);
	}
	return *this;
}

void cHistogram::Resize(int size)
{
	if (data) delete[] data;
	data = nullptr;
	Alloc(size);
}

void cHistogram::Alloc(int size)
{
	data = new long[size + 1];
	histSize = size;
	count = 0;
	sum = 0;
	for (int i = 0; i <= histSize; i++)
	{
		data[i] = 0;
	}
}

long cHistogram::GetHist(int index) const
{
	if (index >= 0 && index <= histSize)
	{
		return data[index];
	}
	else
	{
		return 0;
	}
}

void cHistogram::Copy(const cHistogram &source)
{
	Resize(source.GetSize());

	count = source.count;
	sum = source.sum;

	for (int i = 0; i <= histSize; i++)
	{
		data[i] = source.data[i];
	}
}

void cHistogram::Clear()
{
	for (int i = 0; i <= histSize; i++)
	{
		data[i] = 0;
	}
	count = 0;
	sum = 0;
}
