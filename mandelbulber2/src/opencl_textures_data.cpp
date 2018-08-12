/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * TODO: description
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
	int numberOfTextures = 0;
	QSet<QString> listOfTextures;

	CountTexture(&textures.envmapTexture, &listOfTextures, &numberOfTextures);

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

	if (CountTexture(&textures.envmapTexture, &listOfTextures, &textureIndex))
		BuildTextureData(&textures.envmapTexture, textureIndex);

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

	if (textureIndex == -1) // nothing in the buffer -> add some dummy data
	{
		cl_float4 dummy;
		data.append(reinterpret_cast<char *>(&dummy), sizeof(dummy));
		totalDataOffset += sizeof(dummy);
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
