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
	// TODO Auto-generated constructor stub
}

cOpenClTexturesData::~cOpenClTexturesData()
{
	// TODO Auto-generated destructor stub
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

void cOpenClTexturesData::CountTexture(
	const cTexture *texture, QSet<QString> *listOfTextures, int *counter)
{
	if (texture->IsLoaded())
	{
		if (!listOfTextures->contains(texture->GetFileName()))
		{
			(*counter)++;
			listOfTextures->insert(texture->GetFileName());
		}
	}
}

#endif // USE_OPENCL
