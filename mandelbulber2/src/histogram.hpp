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

#ifndef MANDELBULBER2_SRC_HISTOGRAM_HPP_
#define MANDELBULBER2_SRC_HISTOGRAM_HPP_

#include <QtCore>
#include <algorithm>

class cHistogram
{
public:
	cHistogram();
	cHistogram(int size);
	cHistogram(const cHistogram &source);
	cHistogram &operator=(const cHistogram &source);
	~cHistogram();
	void Resize(int size);
	void Clear();

	inline void Add(int index)
	{
		if (index >= 0 && index < histSize)
		{
			data[index]++;
			count++;
			sum += index;
		}
		else if (index >= histSize)
		{
			data[histSize]++;
			count++;
			sum += histSize;
		}
	}

	long GetHist(int index) const;
	long long GetCount() const { return count; }
	long long GetSum() const { return sum; }
	int GetSize() const { return histSize; }

private:
	void Alloc(int size);
	void Copy(const cHistogram &source);

	long *data;
	long long count;
	long long sum;
	int histSize;
};

Q_DECLARE_METATYPE(cHistogram)

#endif /* MANDELBULBER2_SRC_HISTOGRAM_HPP_ */
