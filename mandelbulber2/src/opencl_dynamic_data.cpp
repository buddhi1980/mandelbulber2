/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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

#include "opencl_dynamic_data.hpp"

#include "lights.hpp"
#include "material.h"
#include "render_worker.hpp"

// custom includes
#ifdef USE_OPENCL
#include "opencl/material_cl.h"
#include "opencl/input_data_structures.h"
#endif

#ifdef USE_OPENCL
cOpenClDynamicData::cOpenClDynamicData()
{
	totalDataOffset = 0; // counter for actual address in structure
	materialsOffset = 0;
	AOVectorsOffset = 0;
	lightsOffset = 0;
	materialsOffsetAddress = 0;
	AOVectorsOffsetAddress = 0;
	lightsOffsetAddress = 0;
}

cOpenClDynamicData::~cOpenClDynamicData()
{
}

int cOpenClDynamicData::PutDummyToAlign(int dataLength, int alignmentSize, QByteArray *array)
{
	int missingBytes = dataLength % alignmentSize;
	if (missingBytes > 0)
	{
		char *dummyData = new char[missingBytes];
		memset(dummyData, 0, missingBytes);
		array->append(dummyData, missingBytes);
		return missingBytes;
	}
	else
	{
		return 0;
	}
}

void cOpenClDynamicData::ReserveHeader()
{
	/* main header:
	 * cl_int materialsOffset
	 * cl_int AOVectorsOffset
	 * cl_int lightsOffset
	 */

	// reserve bytes for array offset
	materialsOffsetAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(&materialsOffset), sizeof(materialsOffset));
	totalDataOffset += sizeof(materialsOffset);

	AOVectorsOffsetAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(&AOVectorsOffset), sizeof(AOVectorsOffset));
	totalDataOffset += sizeof(AOVectorsOffset);

	lightsOffsetAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(&lightsOffset), sizeof(lightsOffset));
	totalDataOffset += sizeof(lightsOffset);
}

void cOpenClDynamicData::FillHeader()
{
	data.replace(materialsOffsetAddress, sizeof(materialsOffset),
		reinterpret_cast<char *>(&materialsOffset), sizeof(materialsOffset));

	data.replace(AOVectorsOffsetAddress, sizeof(AOVectorsOffset),
		reinterpret_cast<char *>(&AOVectorsOffset), sizeof(AOVectorsOffset));

	data.replace(lightsOffsetAddress, sizeof(lightsOffset), reinterpret_cast<char *>(&lightsOffset),
		sizeof(lightsOffset));
}

void cOpenClDynamicData::Clear()
{
	data.clear();
	totalDataOffset = 0;
}

QByteArray &cOpenClDynamicData::GetData(void)
{
	return data;
}

void cOpenClDynamicData::BuildMaterialsData(const QMap<int, cMaterial> &materials)
{
	/* material dynamic data structure

	header:
	cl_int numberOfMaterials

	materials offsets:
	cl_int material[0] offset
	cl_int material[1] offset
	...
	cl_int material[numberOfMaterials] offset

	---- material 0 ---
	+0	cl_int materialClOffset (offset for material data)
	+4	cl_int paletteItemsOffset
	+8	cl_int palette size (bytes)
	+12	cl_int paletteLength (number of color palette items)

	+16	sMaterialCl material

		palette items:
			cl_float3 color[0]
			cl_float3 color[1]
			...
			cl_float3 color[paletteLength]
	-------------------

	*/

	totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);
	materialsOffset = totalDataOffset;

	cl_int numberOfMaterials = materials.size();

	// numberOfMaterials
	data.append(reinterpret_cast<char *>(&numberOfMaterials), sizeof(numberOfMaterials));
	int headerSize = sizeof(numberOfMaterials);
	totalDataOffset += headerSize;

	// reserve bytes for material offsets
	cl_int *materialOffsets = new cl_int[numberOfMaterials];
	int materialOffsetsSize = sizeof(cl_int) * numberOfMaterials;
	memset(materialOffsets, 0, materialOffsetsSize);

	int materialOffsetsAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(materialOffsets), materialOffsetsSize);
	totalDataOffset += materialOffsetsSize;

	// add dummy bytes for alignment to 16
	totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);

	int materialIndex = 0;

	foreach (const cMaterial &material, materials)
	{
		materialOffsets[materialIndex] = totalDataOffset;

		sMaterialCl materialCl = clCopySMaterialCl(material);
		cColorPalette palette = material.palette;
		int paletteSize = palette.GetSize();
		cl_float4 *paletteCl = new cl_float4[paletteSize];
		for (int i = 0; i < paletteSize; i++)
		{
			paletteCl[i] = toClFloat4(CVector4(palette.GetColor(i).R / 256.0,
				palette.GetColor(i).G / 256.0, palette.GetColor(i).B / 256.0, 0.0));
		}

		cl_int materialClOffset = 0;
		cl_int paletteItemsOffset = 0;
		cl_int paletteSizeBytes = sizeof(cl_float4) * paletteSize;
		cl_int paletteLength = paletteSize;

		// reserve bytes for cl_int materialClOffset
		int materialClOffsetAddress = totalDataOffset;
		data.append(reinterpret_cast<char *>(&materialClOffset), sizeof(materialClOffset));
		totalDataOffset += sizeof(materialClOffset);

		// reserve bytes cl_int paletteItemsOffset
		int paletteItemsOffsetAddress = totalDataOffset;
		data.append(reinterpret_cast<char *>(&paletteItemsOffset), sizeof(paletteItemsOffset));
		totalDataOffset += sizeof(paletteItemsOffset);

		// cl_int palette size (bytes)
		data.append(reinterpret_cast<char *>(&paletteSizeBytes), sizeof(paletteSizeBytes));
		totalDataOffset += sizeof(paletteSizeBytes);

		// cl_int paletteLength (number of color palette items)
		data.append(reinterpret_cast<char *>(&paletteLength), sizeof(paletteLength));
		totalDataOffset += sizeof(paletteLength);

		// add dummy bytes for alignment to 16
		totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);

		// sMaterialCl material
		materialClOffset = totalDataOffset;
		data.append(reinterpret_cast<char *>(&materialCl), sizeof(materialCl));
		totalDataOffset += sizeof(materialCl);

		// fill materialClOffset value
		data.replace(materialClOffsetAddress, sizeof(materialClOffset),
			reinterpret_cast<char *>(&materialClOffset), sizeof(materialClOffset));

		// add dummy bytes for alignment to 16
		totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);

		// palette data
		paletteItemsOffset = totalDataOffset;
		data.append(reinterpret_cast<char *>(paletteCl), paletteSizeBytes);
		totalDataOffset += paletteSizeBytes;

		// fill paletteItemsOffset value
		data.replace(paletteItemsOffsetAddress, sizeof(paletteItemsOffset),
			reinterpret_cast<char *>(&paletteItemsOffset), sizeof(paletteItemsOffset));

		materialIndex++;
		delete[] paletteCl;
	}

	// fill materials offsets:
	data.replace(materialOffsetsAddress, materialOffsetsSize,
		reinterpret_cast<char *>(materialOffsets), materialOffsetsSize);

	delete[] materialOffsets;
}

void cOpenClDynamicData::BuildAOVectorsData(const sVectorsAround *AOVectors, cl_int vectorsCount)
{
	/* use __attribute__((aligned(16))) in kernel code for array
	 *
	 * header:
	 * cl_int vectorsCount
	 * cl_int arrayOffset;
	 *
	 * array (aligned to 16):
	 * 	sVectorsAroundCl item1
	 * 	sVectorsAroundCl item2
	 * 	...
	 *	sVectorsAroundCl itemN
	 */

	totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);
	AOVectorsOffset = totalDataOffset;

	data.append(reinterpret_cast<char *>(&vectorsCount), sizeof(vectorsCount));
	totalDataOffset += sizeof(vectorsCount);

	// reserve bytes for array offset
	cl_int arrayOffset = 0;
	int arrayOffsetAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(&arrayOffset), sizeof(arrayOffset));
	totalDataOffset += sizeof(arrayOffset);

	// copy AO vectors aligned to 16
	for (int i = 0; i < vectorsCount; i++)
	{
		totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);
		if (i == 0) arrayOffset = totalDataOffset;

		sVectorsAroundCl vector;
		vector.v = toClFloat3(AOVectors[i].v);
		vector.color = toClFloat3(sRGB(AOVectors[i].R, AOVectors[i].G, AOVectors[i].B));
		vector.alpha = AOVectors[i].alpha;
		vector.beta = AOVectors[i].beta;

		data.append(reinterpret_cast<char *>(&vector), sizeof(vector));
		totalDataOffset += sizeof(vector);
	}

	// replace arrayOffset:
	data.replace(arrayOffsetAddress, sizeof(arrayOffset), reinterpret_cast<char *>(&arrayOffset),
		sizeof(arrayOffset));
}

void cOpenClDynamicData::BuildLightsData(const cLights *lights)
{
	/* use __attribute__((aligned(16))) in kernel code for array
	 *
	 * header:
	 * cl_int numberOfLights
	 * cl_int arrayOffset;
	 *
	 * array (aligned to 16):
	 * 	sLightCl light1
	 * 	sLightCl light1
	 *  ...
	 * 	sLightCl lightN
	 */

	totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);
	lightsOffset = totalDataOffset;

	cl_int numberOfLights = lights->GetNumberOfLights();
	data.append(reinterpret_cast<char *>(&numberOfLights), sizeof(numberOfLights));
	totalDataOffset += sizeof(numberOfLights);

	// reserve bytes for array offset
	cl_int arrayOffset = 0;
	int arrayOffsetAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(&arrayOffset), sizeof(arrayOffset));
	totalDataOffset += sizeof(arrayOffset);

	// copy lights aligned to 16
	for (int i = 0; i < numberOfLights; i++)
	{
		// allign struct to 16
		totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);
		if (i == 0) arrayOffset = totalDataOffset;

		sLightCl lightCl;
		sLight *light = lights->GetLight(i);
		lightCl.position = toClFloat3(light->position);
		lightCl.colour = toClFloat3(light->colour);
		lightCl.intensity = light->intensity;
		lightCl.enabled = light->enabled;

		data.append(reinterpret_cast<char *>(&lightCl), sizeof(lightCl));
		totalDataOffset += sizeof(lightCl);
	}

	// replace arrayOffset:
	data.replace(arrayOffsetAddress, sizeof(arrayOffset), reinterpret_cast<char *>(&arrayOffset),
		sizeof(arrayOffset));
}

#endif // USE_OPENCL
