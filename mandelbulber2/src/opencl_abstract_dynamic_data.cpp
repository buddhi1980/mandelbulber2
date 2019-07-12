/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * base class for data of dynamic size for OpenCL kernels
 */

#include "opencl_abstract_dynamic_data.h"

#ifdef USE_OPENCL
cOpenClAbstractDynamicData::cOpenClAbstractDynamicData(int _numberOfItems)
		: numberOfItems(_numberOfItems)
{
	totalDataOffset = 0;
}

cOpenClAbstractDynamicData::~cOpenClAbstractDynamicData()
{
	// nothing
}

void cOpenClAbstractDynamicData::Clear()
{
	data.clear();
	itemOffsets.clear();
	totalDataOffset = 0;
}

QByteArray &cOpenClAbstractDynamicData::GetData()
{
	return data;
}

int cOpenClAbstractDynamicData::PutDummyToAlign(
	int dataLength, int alignmentSize, QByteArray *array)
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

void cOpenClAbstractDynamicData::ReserveHeader()
{
	for (int i = 0; i < numberOfItems; i++)
	{
		sItemOffset offset(0, totalDataOffset);
		data.append(reinterpret_cast<char *>(&offset.itemOffset), sizeof(offset.itemOffset));
		totalDataOffset += sizeof(offset.itemOffset);
		itemOffsets.append(offset);
	}
}

void cOpenClAbstractDynamicData::FillHeader()
{
	for (int i = 0; i < numberOfItems; i++)
	{
		data.replace(itemOffsets.at(i).itemOffsetAddress, sizeof(itemOffsets.at(i).itemOffset),
			reinterpret_cast<char *>(&itemOffsets[i].itemOffset), sizeof(itemOffsets.at(i).itemOffset));
	}
}

#endif // USE_OPENCL
