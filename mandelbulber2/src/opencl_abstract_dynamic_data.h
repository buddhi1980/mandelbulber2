/*
 * opencl_abstract_dynamic_data.h
 *
 *  Created on: 10 sie 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPENCL_ABSTRACT_DYNAMIC_DATA_H_
#define MANDELBULBER2_SRC_OPENCL_ABSTRACT_DYNAMIC_DATA_H_

#include <QtCore>
#include "include_header_wrapper.hpp"

#ifdef USE_OPENCL
class OpenClAbstractDynamicData
{
public:
	OpenClAbstractDynamicData(int _numberOfItems);
	~OpenClAbstractDynamicData();

	void ReserveHeader();
	void FillHeader();
	void Clear();
	QByteArray &GetData();

protected:
	static int PutDummyToAlign(int dataLength, int alignmentSize, QByteArray *array);

	struct sItemOffset
	{
		sItemOffset(cl_int _itemOffset, int _itemOffsetAddress)
				: itemOffset(_itemOffset), itemOffsetAddress(_itemOffsetAddress)
		{
		}
		cl_int itemOffset;
		int itemOffsetAddress;
	};

	QByteArray data;
	cl_int totalDataOffset;
	const int numberOfItems;

	QList<sItemOffset> itemOffsets;
};
#endif // USE_OPENCL

#endif /* MANDELBULBER2_SRC_OPENCL_ABSTRACT_DYNAMIC_DATA_H_ */
