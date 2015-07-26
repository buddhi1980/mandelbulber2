/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * Histogram class
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#ifndef SRC_HISTOGRAM_HPP_
#define SRC_HISTOGRAM_HPP_

#include <QtCore>

class cHistogram
{
public:
	cHistogram();
	cHistogram(int size);
	cHistogram(const cHistogram &source);
	cHistogram& operator=(const cHistogram &source);
	~cHistogram();
	void Resize(int size);
	void Clear();

	inline void Add(int index)
	{
		if(index >= 0 && index < histSize)
		{
			data[index]++;
			count++;
			sum += index;
		}
		else if(index >= histSize)
		{
			data[histSize]++;
			count++;
			sum += index;
		}
	}

	long GetHist(int index) const;
	long long GetCount() const {return count;}
	long long GetSum() const {return sum;}
	int GetSize() const {return histSize;}

private:
	void Alloc(int size);
	void Free();
	void Copy(const cHistogram &source);

	long *data;
	long long count;
	long long sum;
	int histSize;
};

Q_DECLARE_METATYPE(cHistogram)

#endif /* SRC_HISTOGRAM_HPP_ */
