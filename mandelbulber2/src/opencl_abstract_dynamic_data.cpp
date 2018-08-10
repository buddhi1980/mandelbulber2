/*
 * opencl_abstract_dynamic_data.cpp
 *
 *  Created on: 10 sie 2018
 *      Author: krzysztof
 */

#include "opencl_abstract_dynamic_data.h"

OpenClAbstractDynamicData::OpenClAbstractDynamicData(int _numberOfItems)
		: numberOfItems(_numberOfItems)
{
	totalDataOffset = 0;
}

OpenClAbstractDynamicData::~OpenClAbstractDynamicData()
{
	// nothing
}

void OpenClAbstractDynamicData::Clear()
{
	data.clear();
	itemOffsets.clear();
	totalDataOffset = 0;
}

QByteArray &OpenClAbstractDynamicData::GetData()
{
	return data;
}

int OpenClAbstractDynamicData::PutDummyToAlign(int dataLength, int alignmentSize, QByteArray *array)
{
	int missingBytes = alignmentSize - dataLength % alignmentSize;
	if (missingBytes > 0 && missingBytes != alignmentSize)
	{
		char *dummyData = new char[missingBytes];
		memset(dummyData, 0, missingBytes);
		array->append(dummyData, missingBytes);
		delete[] dummyData;
		return missingBytes;
	}
	else
	{
		return 0;
	}
}

void OpenClAbstractDynamicData::ReserveHeader()
{
	for (int i = 0; i < numberOfItems; i++)
	{
		sItemOffset offset(0, totalDataOffset);
		data.append(reinterpret_cast<char *>(&offset.itemOffset), sizeof(offset.itemOffset));
		totalDataOffset += sizeof(offset.itemOffset);
		itemOffsets.append(offset);
	}
}

void OpenClAbstractDynamicData::FillHeader()
{
	for (int i = 0; i < numberOfItems; i++)
	{
		data.replace(itemOffsets.at(i).itemOffsetAddress, sizeof(itemOffsets.at(i).itemOffset),
			reinterpret_cast<char *>(&itemOffsets[i].itemOffset), sizeof(itemOffsets.at(i).itemOffset));
	}
}
