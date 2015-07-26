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

#include "histogram.hpp"

cHistogram::cHistogram()
{
	histSize = 0;
	count = 0;
	sum = 0;
	data = NULL;
}

cHistogram::cHistogram(int size)
{
	data = NULL;
	Alloc(size);
}

cHistogram::~cHistogram()
{
	if(data) delete[] data;
	data = NULL;
}

cHistogram::cHistogram(const cHistogram &source)
{
	data = NULL;
	Copy(source);
}

cHistogram& cHistogram::operator=(const cHistogram &source)
{
	if(this != &source)
	{
		Copy(source);
	}
	return *this;
}

void cHistogram::Resize(int size)
{
	if(data) delete[] data;
	data = NULL;
	Alloc(size);
}

void cHistogram::Alloc(int size)
{
	data = new long[size + 1];
	histSize = size;
	count = 0;
	sum = 0;
	for(int i = 0; i <= histSize; i++)
	{
		data[i] = 0;
	}
}

long cHistogram::GetHist(int index) const
{
	if(index >= 0 && index < histSize)
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

	for(int i=0; i < histSize; i++)
	{
		data[i] = source.data[i];
	}
}

void cHistogram::Clear()
{
	for(int i = 0; i <= histSize; i++)
	{
		data[i] = 0;
	}
}
