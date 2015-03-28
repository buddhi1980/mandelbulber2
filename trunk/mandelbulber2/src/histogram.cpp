/*
 * histogram.cpp
 *
 *  Created on: Mar 28, 2015
 *      Author: krzysztof
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
