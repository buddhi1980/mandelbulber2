/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-23 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * dynamic data for textures loaded to OpenCL kernel
 */

#include <QSet>

#include "common_math.h"
#include "system.hpp"

// custom includes
#ifdef USE_OPENCL
#include "opencl_textures_data.h"
#include "material.h"
#include "render_data.hpp"
#include "write_log.hpp"
#include "lights.hpp"
#include "light.h"

cOpenClTexturesData::cOpenClTexturesData(int _numberOfTextures)
		: cOpenClAbstractDynamicData(_numberOfTextures)
{
	numberOfTextures = _numberOfTextures;

	useColorTexture = false;
	useDiffussionTexture = false;
	useLuminosityTexture = false;
	useNormalMapTexture = false;
	useNormalMapTextureFromBumpmap = false;
	useDisplacementMap = false;
	useReflectanceTexture = false;
	useTransparencyTexture = false;
	useTransparencyAlphaTexture = false;
	useRoughnessTexture = false;

	useLightTexture = false;

	usePlanarMapping = false;
	useCylindicalMapping = false;
	useSphericalMapping = false;
	useCubicMapping = false;
}

cOpenClTexturesData::~cOpenClTexturesData()
{
	// nothing to destroy
}

int cOpenClTexturesData::CheckNumberOfTextures(
	const sTextures &textures, const std::map<int, cMaterial> &materials, const cLights &lights)
{
	int texturesCounter = 0;
	QSet<QString> listOfTextures;

	CountTexture(&textures.envmapTexture, false, &listOfTextures, &texturesCounter);

	for (auto const &materialPair : materials) // for each material from materials
	{
		cMaterial const &material = materialPair.second;
		CountTexture(&material.colorTexture, false, &listOfTextures, &texturesCounter);
		CountTexture(&material.diffusionTexture, false, &listOfTextures, &texturesCounter);
		CountTexture(&material.displacementTexture, true, &listOfTextures,
			&texturesCounter); // will be stored as 16bit grey texture
		CountTexture(&material.luminosityTexture, false, &listOfTextures, &texturesCounter);
		CountTexture(&material.normalMapTexture, false, &listOfTextures, &texturesCounter);
		CountTexture(&material.reflectanceTexture, false, &listOfTextures, &texturesCounter);
		CountTexture(&material.transparencyTexture, false, &listOfTextures, &texturesCounter);
		CountTexture(&material.transparencyAlphaTexture, false, &listOfTextures, &texturesCounter);
		CountTexture(&material.roughnessTexture, false, &listOfTextures, &texturesCounter);
	}

	for (int i = 0; i < lights.GetNumberOfLights(); i++)
	{
		cLight const *light = lights.GetLight(i);
		CountTexture(&light->colorTexture, false, &listOfTextures, &texturesCounter);
	}

	return texturesCounter;
}

bool cOpenClTexturesData::CountTexture(
	const cTexture *texture, bool grey16bit, QSet<QString> *listOfTextures, int *counter)
{
	bool added = false;
	if (texture->IsLoaded())
	{
		QString filename = texture->GetFileName();
		if (grey16bit)
			filename += "grey16bit"; // it is needed to separate 16bit texture if the same file is used
															 // for standard textures

		if (!listOfTextures->contains(filename))
		{
			(*counter)++;
			listOfTextures->insert(filename);
			added = true;
		}
	}
	return added;
}

void cOpenClTexturesData::BuildAllTexturesData(const sTextures &textures,
	const std::map<int, cMaterial> &materials, const cLights &lights,
	QMap<QString, int> *textureIndexes)
{
	WriteLog("OpenCL - BuildAllTexturesData()", 2);

	int textureIndex = -1;
	QSet<QString> listOfTextures;
	textureIndexes->clear();

	if (CountTexture(&textures.envmapTexture, false, &listOfTextures, &textureIndex))
	{
		BuildTextureData(&textures.envmapTexture, textureIndex, false);
		textureIndexes->insert(textures.envmapTexture.GetFileName(), textureIndex);
	}

	useColorTexture = false;
	useDiffussionTexture = false;
	useLuminosityTexture = false;
	useNormalMapTexture = false;
	useNormalMapTextureFromBumpmap = false;
	useDisplacementMap = false;
	useReflectanceTexture = false;
	useTransparencyTexture = false;
	useTransparencyAlphaTexture = false;
	useRoughnessTexture = false;

	useLightTexture = false;

	usePlanarMapping = false;
	useCylindicalMapping = false;
	useSphericalMapping = false;
	useCubicMapping = false;

	for (auto const &materialPair : materials) // for each material from materials
	{
		cMaterial const &material = materialPair.second;
		if (material.colorTexture.IsLoaded()) useColorTexture = true;
		if (CountTexture(&material.colorTexture, false, &listOfTextures, &textureIndex))
		{
			BuildTextureData(&material.colorTexture, textureIndex, false);
			textureIndexes->insert(material.colorTexture.GetFileName(), textureIndex);
		}

		if (material.diffusionTexture.IsLoaded()) useDiffussionTexture = true;
		if (CountTexture(&material.diffusionTexture, false, &listOfTextures, &textureIndex))
		{
			BuildTextureData(&material.diffusionTexture, textureIndex, false);
			textureIndexes->insert(material.diffusionTexture.GetFileName(), textureIndex);
		}

		if (material.displacementTexture.IsLoaded()) useDisplacementMap = true;
		if (CountTexture(&material.displacementTexture, true, &listOfTextures, &textureIndex))
		{
			// will be stored as 16bit grey texture
			BuildTextureData(&material.displacementTexture, textureIndex, true);
			textureIndexes->insert(
				material.displacementTexture.GetFileName() + "grey16bit", textureIndex);
		}

		if (material.luminosityTexture.IsLoaded()) useLuminosityTexture = true;
		if (CountTexture(&material.luminosityTexture, false, &listOfTextures, &textureIndex))
		{
			BuildTextureData(&material.luminosityTexture, textureIndex, false);
			textureIndexes->insert(material.luminosityTexture.GetFileName(), textureIndex);
		}

		if (material.normalMapTexture.IsLoaded()) useNormalMapTexture = true;
		if (material.normalMapTextureFromBumpmap) useNormalMapTextureFromBumpmap = true;

		if (CountTexture(&material.normalMapTexture, false, &listOfTextures, &textureIndex))
		{
			BuildTextureData(&material.normalMapTexture, textureIndex, false);
			textureIndexes->insert(material.normalMapTexture.GetFileName(), textureIndex);
		}

		if (material.reflectanceTexture.IsLoaded()) useReflectanceTexture = true;
		if (CountTexture(&material.reflectanceTexture, false, &listOfTextures, &textureIndex))
		{
			BuildTextureData(&material.reflectanceTexture, textureIndex, false);
			textureIndexes->insert(material.reflectanceTexture.GetFileName(), textureIndex);
		}

		if (material.transparencyTexture.IsLoaded()) useTransparencyTexture = true;
		if (CountTexture(&material.transparencyTexture, false, &listOfTextures, &textureIndex))
		{
			BuildTextureData(&material.transparencyTexture, textureIndex, false);
			textureIndexes->insert(material.transparencyTexture.GetFileName(), textureIndex);
		}

		if (material.transparencyAlphaTexture.IsLoaded()) useTransparencyAlphaTexture = true;
		if (CountTexture(&material.transparencyAlphaTexture, false, &listOfTextures, &textureIndex))
		{
			BuildTextureData(&material.transparencyAlphaTexture, textureIndex, false);
			textureIndexes->insert(material.transparencyAlphaTexture.GetFileName(), textureIndex);
		}

		if (material.roughnessTexture.IsLoaded()) useRoughnessTexture = true;
		if (CountTexture(&material.roughnessTexture, false, &listOfTextures, &textureIndex))
		{
			BuildTextureData(&material.roughnessTexture, textureIndex, false);
			textureIndexes->insert(material.roughnessTexture.GetFileName(), textureIndex);
		}

		switch (material.textureMappingType)
		{
			case texture::mappingPlanar: usePlanarMapping = true; break;
			case texture::mappingCylindrical: useCylindicalMapping = true; break;
			case texture::mappingSpherical: useSphericalMapping = true; break;
			case texture::mappingCubic: useCubicMapping = true; break;
		}
	}

	for (int i = 0; i < lights.GetNumberOfLights(); i++)
	{
		cLight const *light = lights.GetLight(i);
		if (light->colorTexture.IsLoaded()) useLightTexture = true;
		if (CountTexture(&light->colorTexture, false, &listOfTextures, &textureIndex))
		{
			BuildTextureData(&light->colorTexture, textureIndex, false);
			textureIndexes->insert(light->colorTexture.GetFileName(), textureIndex);
		}
	}

	if (textureIndex == -1) // nothing in the buffer -> add some dummy data
	{
		cl_float4 dummy;
		data.append(reinterpret_cast<char *>(&dummy), sizeof(dummy));
		totalDataOffset += sizeof(dummy);
	}

	WriteLog("OpenCL - BuildAllTexturesData() finished", 3);
}

void cOpenClTexturesData::BuildTextureData(
	const cTexture *texture, int textureIndex, bool grey16bit)
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

	WriteLogString("OpenCL - BuildTextureData()", texture->GetFileName(), 3);

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

	arrayOffset = totalDataOffset;

	const std::vector<sRGBA8> hdrBitmap = texture->GetHDRBitmap();

	if (!grey16bit)
	{
		data.append(
			reinterpret_cast<const char *>(hdrBitmap.data()), sizeof(cl_uchar4) * numberOfPixels);
	}
	else
	{
		for (size_t i = 0; i < numberOfPixels; i++)
		{
			// greyscale 16bit texture is coded using first two bytes
			int x = i % textureWidth;
			int y = i / textureWidth;
			sRGBFloat pixel = texture->FastPixel(x, y);
			int brightness = clamp(int(pixel.R * 65535.0), 0, 65535);
			cl_uchar4 clpixel = {{cl_uchar(brightness / 256), cl_uchar(brightness % 256), 0, 0}};
			data.append(reinterpret_cast<char *>(&clpixel), sizeof(clpixel));
		}
	}

	totalDataOffset += sizeof(cl_uchar4) * numberOfPixels;

	// replace arrayOffset:
	data.replace(arrayOffsetAddress, sizeof(arrayOffset), reinterpret_cast<char *>(&arrayOffset),
		sizeof(arrayOffset));
}

QString cOpenClTexturesData::GetDefinesCollector() const
{
	QString definesCollector;
	if (useColorTexture) definesCollector += " -DUSE_COLOR_TEXTURE";
	if (useDiffussionTexture) definesCollector += " -DUSE_DIFFUSION_TEXTURE";
	if (useLuminosityTexture) definesCollector += " -DUSE_LUMINOSITY_TEXTURE";
	if (useNormalMapTexture) definesCollector += " -DUSE_NORMAL_MAP_TEXTURE";
	if (useNormalMapTextureFromBumpmap) definesCollector += " -DUSE_NORMAL_MAP_BUMP_TEXTURE";
	if (useDisplacementMap) definesCollector += " -DUSE_DISPLACEMENT_TEXTURE";
	if (useReflectanceTexture) definesCollector += " -DUSE_REFLECTANCE_TEXTURE";
	if (useTransparencyTexture) definesCollector += " -DUSE_TRANSPARENCY_TEXTURE";
	if (useTransparencyAlphaTexture) definesCollector += " -DUSE_TRANSPARENCY_ALPHA_TEXTURE";
	if (useRoughnessTexture) definesCollector += " -DUSE_ROUGHNESS_TEXTURE";
	if (useLightTexture) definesCollector += " -DUSE_LIGHT_TEXTURE";
	if (usePlanarMapping) definesCollector += " -DUSE_PLANAR_MAPPING";
	if (useCylindicalMapping) definesCollector += " -DUSE_CYLINDRICAL_MAPPING";
	if (useSphericalMapping) definesCollector += " -DUSE_SPHERICAL_MAPPING";
	if (useCubicMapping) definesCollector += " -DUSE_CUBIC_MAPPING";
	return definesCollector;
}

#endif // USE_OPENCL
