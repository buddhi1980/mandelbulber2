/*
 * opencl_dynamic_data.cpp
 *
 *  Created on: 17 cze 2017
 *      Author: krzysztof
 */

#ifdef USE_OPENCL

#include "opencl_dynamic_data.hpp"
#include "../src/material.h"
#include "../opencl/material_cl.h"

cOpenClDynamicData::cOpenClDynamicData()
{
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
	cl_int totalMaterialOffset = 0; // counter for actual address in structure

	cl_int numberOfMaterials = materials.size();

	// numberOfMaterials
	data.append(reinterpret_cast<char *>(&numberOfMaterials), sizeof(numberOfMaterials));
	int headerSize = sizeof(numberOfMaterials);
	totalMaterialOffset += headerSize;

	// reserve bytes for material offsets
	cl_int *materialOffsets = new cl_int[numberOfMaterials];
	int materialOffsetsSize = sizeof(cl_int) * numberOfMaterials;
	memset(materialOffsets, 0, materialOffsetsSize);

	int materialOffsetsAddress = totalMaterialOffset;
	data.append(reinterpret_cast<char *>(materialOffsets), materialOffsetsSize);
	totalMaterialOffset += materialOffsetsSize;

	// add dummy bytes for alignment to 16
	totalMaterialOffset += PutDummyToAlign(totalMaterialOffset, 16, &data);

	int materialIndex = 0;

	foreach (const cMaterial &material, materials)
	{
		materialOffsets[materialIndex] = totalMaterialOffset;

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
		int materialClOffsetAddress = totalMaterialOffset;
		data.append(reinterpret_cast<char *>(&materialClOffset), sizeof(materialClOffset));
		totalMaterialOffset += sizeof(materialClOffset);

		// reserve bytes cl_int paletteItemsOffset
		int paletteItemsOffsetAddress = totalMaterialOffset;
		data.append(reinterpret_cast<char *>(&paletteItemsOffset), sizeof(paletteItemsOffset));
		totalMaterialOffset += sizeof(paletteItemsOffset);

		// cl_int palette size (bytes)
		data.append(reinterpret_cast<char *>(&paletteSizeBytes), sizeof(paletteSizeBytes));
		totalMaterialOffset += sizeof(paletteSizeBytes);

		// cl_int paletteLength (number of color palette items)
		data.append(reinterpret_cast<char *>(&paletteLength), sizeof(paletteLength));
		totalMaterialOffset += sizeof(paletteLength);

		// add dummy bytes for alignment to 16
		totalMaterialOffset += PutDummyToAlign(totalMaterialOffset, 16, &data);

		// sMaterialCl material
		materialClOffset = totalMaterialOffset;
		data.append(reinterpret_cast<char *>(&materialCl), sizeof(materialCl));
		totalMaterialOffset += sizeof(materialCl);

		// fill materialClOffset value
		data.replace(materialClOffsetAddress, sizeof(materialClOffset),
			reinterpret_cast<char *>(&materialClOffset), sizeof(materialClOffset));

		// add dummy bytes for alignment to 16
		totalMaterialOffset += PutDummyToAlign(totalMaterialOffset, 16, &data);

		// palette data
		paletteItemsOffset = totalMaterialOffset;
		data.append(reinterpret_cast<char *>(paletteCl), paletteSizeBytes);
		totalMaterialOffset += paletteSizeBytes;

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

void cOpenClDynamicData::Clear()
{
	data.clear();
}

QByteArray &cOpenClDynamicData::GetData(void)
{
	return data;
}

#endif // USE_OPENCL
