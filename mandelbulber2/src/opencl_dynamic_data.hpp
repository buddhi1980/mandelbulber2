/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * opencl dynamic data for transferring to the worker device
 */

#ifndef MANDELBULBER2_SRC_OPENCL_DYNAMIC_DATA_HPP_
#define MANDELBULBER2_SRC_OPENCL_DYNAMIC_DATA_HPP_

#include <QtCore>

#include "include_header_wrapper.hpp"

class cMaterial;
struct sVectorsAround;
class cLights;
class cPrimitives;
class cObjectData;

#ifdef USE_OPENCL
class cOpenClDynamicData
{
public:
	cOpenClDynamicData();
	~cOpenClDynamicData();

	void Clear();
	static int PutDummyToAlign(int dataLength, int alignmentSize, QByteArray *array);
	int BuildMaterialsData(const QMap<int, cMaterial> &materials); // returns array size
	void BuildAOVectorsData(const sVectorsAround *AOVectors, int verctorsCount);
	void BuildLightsData(const cLights *lights);
	void BuildPrimitivesData(const cPrimitives *primitives);
	void BuildObjectsData(const QVector<cObjectData> *objectData);
	void ReserveHeader();
	void FillHeader();
	QByteArray &GetData();

private:
	QByteArray data;
	cl_int totalDataOffset;

	cl_int materialsOffset;
	int materialsOffsetAddress;

	cl_int AOVectorsOffset;
	int AOVectorsOffsetAddress;

	cl_int lightsOffset;
	int lightsOffsetAddress;

	cl_int primitivesOffset;
	int primitivesOffsetAddress;

	cl_int objectsOffset;
	int objectsOffsetAddress;
};

#endif // USE_OPENCL

#endif /* MANDELBULBER2_SRC_OPENCL_DYNAMIC_DATA_HPP_ */
