/*
 * opencl_textures_data.cpp
 *
 *  Created on: 10 sie 2018
 *      Author: krzysztof
 */

#ifdef USE_OPENCL

#include "opencl_textures_data.h"

#include "material.h"
#include "render_data.hpp"

cOpenClTexturesData::cOpenClTexturesData(int _numberOfTextures)
		: cOpenClAbstractDynamicData(_numberOfTextures)
{
	numberOfTextures = _numberOfTextures;
}

cOpenClTexturesData::~cOpenClTexturesData()
{
	// nothing to destroy
}

int cOpenClTexturesData::CheckNumberOfTextures(
	const sTextures &textures, const QMap<int, cMaterial> &materials)
{
	int numberOfTextures;
	QSet<QString> listOfTextures;

	CountTexture(&textures.backgroundTexture, &listOfTextures, &numberOfTextures);
	CountTexture(&textures.envmapTexture, &listOfTextures, &numberOfTextures);
	CountTexture(&textures.lightmapTexture, &listOfTextures, &numberOfTextures);

	for (const cMaterial &material : materials) // for each material from materials
	{
		CountTexture(&material.colorTexture, &listOfTextures, &numberOfTextures);
		CountTexture(&material.diffusionTexture, &listOfTextures, &numberOfTextures);
		CountTexture(&material.displacementTexture, &listOfTextures, &numberOfTextures);
		CountTexture(&material.luminosityTexture, &listOfTextures, &numberOfTextures);
		CountTexture(&material.normalMapTexture, &listOfTextures, &numberOfTextures);
	}

	return numberOfTextures;
}

bool cOpenClTexturesData::CountTexture(
	const cTexture *texture, QSet<QString> *listOfTextures, int *counter)
{
	bool added = false;
	if (texture->IsLoaded())
	{
		if (!listOfTextures->contains(texture->GetFileName()))
		{
			(*counter)++;
			listOfTextures->insert(texture->GetFileName());
			added = true;
		}
	}
	return added;
}

void cOpenClTexturesData::BuildAllTexturesData(
	const sTextures &textures, const QMap<int, cMaterial> &materials)
{
	int textureIndex = -1;
	QSet<QString> listOfTextures;

	if (CountTexture(&textures.backgroundTexture, &listOfTextures, &textureIndex))
		BuildTextureData(&textures.backgroundTexture, textureIndex);

	if (CountTexture(&textures.envmapTexture, &listOfTextures, &textureIndex))
		BuildTextureData(&textures.envmapTexture, textureIndex);

	if (CountTexture(&textures.lightmapTexture, &listOfTextures, &textureIndex))
		BuildTextureData(&textures.lightmapTexture, textureIndex);

	for (const cMaterial &material : materials) // for each material from materials
	{
		if (CountTexture(&material.colorTexture, &listOfTextures, &textureIndex))
			BuildTextureData(&material.colorTexture, textureIndex);

		if (CountTexture(&material.diffusionTexture, &listOfTextures, &textureIndex))
			BuildTextureData(&material.diffusionTexture, textureIndex);

		if (CountTexture(&material.displacementTexture, &listOfTextures, &textureIndex))
			BuildTextureData(&material.displacementTexture, textureIndex);

		if (CountTexture(&material.luminosityTexture, &listOfTextures, &textureIndex))
			BuildTextureData(&material.luminosityTexture, textureIndex);

		if (CountTexture(&material.normalMapTexture, &listOfTextures, &textureIndex))
			BuildTextureData(&material.normalMapTexture, textureIndex);
	}
}

void cOpenClTexturesData::BuildTextureData(const cTexture *texture, int textureIndex)
{
	// header:
	//+0 cl_int textureDataOffset
	//+4 cl_int width
	//+8 cl_int height

	// texture data:
	//+12 cl_char4 pixel[0]
	//+16 cl_char4 pixel[1]
	//    ...
	//    cl_char4 pixel[width*height]

	totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);
	itemOffsets[textureIndex].itemOffset = totalDataOffset;

	// reserve bytes for array offset
	cl_int arrayOffset = 0;
	int arrayOffsetAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(&arrayOffset), sizeof(arrayOffset));
	totalDataOffset += sizeof(arrayOffset);

	cl_int textureWidth = texture->Width();
	cl_int textureHeight = texture->Height();

	data.append(reinterpret_cast<char *>(&textureWidth), sizeof(textureWidth));
	totalDataOffset += sizeof(textureWidth);

	data.append(reinterpret_cast<char *>(&textureHeight), sizeof(textureHeight));
	totalDataOffset += sizeof(textureHeight);

	size_t numberOfPixels = size_t(textureWidth) * size_t(textureHeight);
	for (size_t i = 0; i < numberOfPixels; i++)
	{
		if (i == 0) arrayOffset = totalDataOffset;

		int x = i % textureWidth;
		int y = i / textureWidth;

		sRGBA16 pixel = texture->FastPixel(x, y);
		cl_char4 clpixel = {cl_char(pixel.R / 256), cl_char(pixel.G / 256), cl_char(pixel.B / 256),
			cl_char(pixel.A / 256)};

		data.append(reinterpret_cast<char *>(&clpixel), sizeof(clpixel));
		totalDataOffset += sizeof(clpixel);
	}

	// replace arrayOffset:
	data.replace(arrayOffsetAddress, sizeof(arrayOffset), reinterpret_cast<char *>(&arrayOffset),
		sizeof(arrayOffset));
}

#endif // USE_OPENCL
