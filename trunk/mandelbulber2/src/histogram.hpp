/*
 * histogram.hpp
 *
 *  Created on: Mar 28, 2015
 *      Author: krzysztof
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
