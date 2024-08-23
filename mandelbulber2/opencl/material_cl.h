/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * material struct for opencl
 */

#ifndef MANDELBULBER2_OPENCL_MATERIAL_CL_H_
#define MANDELBULBER2_OPENCL_MATERIAL_CL_H_

#ifndef OPENCL_KERNEL_CODE
#include "fractal_coloring_cl.hpp"
#include "opencl_algebra.h"
#include "texture_enums_cl.h"

#include "src/include_header_wrapper.hpp"
#include "src/material.h"

#endif /* not OPENCL_KERNEL_CODE */

typedef struct
{
	cl_int id;
	cl_int textureFractalizeStartIteration;
	cl_int perlinNoiseIterations;

	cl_float shading;
	cl_float specular;
	cl_float specularWidth;
	cl_float specularMetallic;
	cl_float specularMetallicRoughness;
	cl_float specularMetallicWidth;
	cl_float reflectance;
	cl_float luminosity;
	cl_float luminosityEmissive;
	cl_float surfaceRoughness;
	cl_float transparencyIndexOfRefraction;
	cl_float transparencyOfInterior;
	cl_float transparencyOfSurface;
	cl_float paletteOffset;
	cl_float coloring_speed;
	cl_float colorTextureIntensity;
	cl_float diffusionTextureIntensity;
	cl_float luminosityTextureIntensity;
	cl_float reflectanceTextureIntensity;
	cl_float transparencyTextureIntensity;
	cl_float transparencyAlphaTextureIntensity;
	cl_float transparencyTextureIntensityVol;
	cl_float transparencyAlphaTextureIntensityVol;
	cl_float roughnessTextureIntensity;
	cl_float displacementTextureHeight;
	cl_float normalMapTextureHeight;
	cl_float iridescenceIntensity;
	cl_float iridescenceSubsurfaceThickness;
	cl_float textureFractalizeCubeSize;
	cl_float perlinNoiseValueOffset;
	cl_float perlinNoiseColorIntensity;
	cl_float perlinNoiseLuminosityIntensity;
	cl_float perlinNoiseDisplacementIntensity;
	cl_float perlinNoiseReflectanceIntensity;
	cl_float perlinNoiseTransparencyColorIntensity;
	cl_float perlinNoiseTransparencyAlphaIntensity;
	cl_float perlinNoiseTransparencyColorIntensityVol;
	cl_float perlinNoiseTransparencyAlphaIntensityVol;

	cl_float3 color;
	cl_float3 luminosityColor;
	cl_float3 transparencyInteriorColor;
	cl_float3 reflectionsColor;
	cl_float3 specularColor;
	cl_float3 transparencyColor;

	cl_float3 textureCenter;
	cl_float3 textureScale;
	cl_float3 perlinNoisePeriod;
	cl_float3 perlinNoisePositionOffset;

	matrix33 rotMatrixTexture;
	matrix33 rotMatrixPerlinNoise;

	enumTextureMappingCl textureMappingType;
	cl_int fresnelReflectance;
	cl_int useColorsFromPalette;
	cl_int specularPlasticEnable;
	cl_int metallic;
	cl_int roughSurface;

	cl_int perlinNoiseEnable;
	cl_int perlinNoiseAbs;
	cl_int perlinNoiseColorEnable;
	cl_int perlinNoiseColorInvert;
	cl_int perlinNoiseLuminosityEnable;
	cl_int perlinNoiseLuminosityInvert;
	cl_int perlinNoiseDisplacementEnable;
	cl_int perlinNoiseDisplacementInvert;
	cl_int perlinNoiseReflectanceEnable;
	cl_int perlinNoiseReflectanceInvert;
	cl_int perlinNoiseTransparencyColorEnable;
	cl_int perlinNoiseTransparencyColorInvert;
	cl_int perlinNoiseTransparencyAlphaEnable;
	cl_int perlinNoiseTransparencyAlphaInvert;

	cl_int useColorTexture;
	cl_int useDiffusionTexture;
	cl_int useLuminosityTexture;
	cl_int useDisplacementTexture;
	cl_int useNormalMapTexture;
	cl_int normalMapTextureFromBumpmap;
	cl_int normalMapTextureInvertGreen;
	cl_int useReflectanceTexture;
	cl_int useTransparencyTexture;
	cl_int useTransparencyAlphaTexture;
	cl_int useRoughnessTexture;
	cl_int iridescenceEnabled;
	cl_int textureFractalize;

	cl_int insideColoringEnable;
	cl_int subsurfaceScattering;

	cl_int colorTextureIndex;
	cl_int diffusionTextureIndex;
	cl_int luminosityTextureIndex;
	cl_int displacementTextureIndex;
	cl_int normalMapTextureIndex;
	cl_int reflectanceTextureIndex;
	cl_int transparencyTextureIndex;
	cl_int transparencyAlphaTextureIndex;
	cl_int roughnessTextureIndex;

	cl_int surfaceGradientEnable;
	cl_int specularGradientEnable;
	cl_int diffuseGradientEnable;
	cl_int luminosityGradientEnable;
	cl_int roughnessGradientEnable;
	cl_int reflectanceGradientEnable;
	cl_int transparencyGradientEnable;

	sFractalColoringCl fractalColoring;
} sMaterialCl;

#ifndef OPENCL_KERNEL_CODE
sMaterialCl clCopySMaterialCl(const cMaterial &source)
{
	sMaterialCl target;

	target.id = source.id;
	target.textureFractalizeStartIteration = source.textureFractalizeStartIteration;
	target.perlinNoiseIterations = source.perlinNoiseIterations;

	target.shading = source.shading;
	target.specular = source.specular;
	target.specularWidth = source.specularWidth;
	target.specularMetallic = source.specularMetallic;
	target.specularMetallicRoughness = source.specularMetallicRoughness;
	target.specularMetallicWidth = source.specularMetallicWidth;
	target.reflectance = source.reflectance;
	target.luminosity = source.luminosity;
	target.luminosityEmissive = source.luminosityEmissive;
	target.surfaceRoughness = source.surfaceRoughness;
	target.transparencyIndexOfRefraction = source.transparencyIndexOfRefraction;
	target.transparencyOfInterior = source.transparencyOfInterior;
	target.transparencyOfSurface = source.transparencyOfSurface;
	target.paletteOffset = source.paletteOffset;
	target.coloring_speed = source.coloring_speed;
	target.colorTextureIntensity = source.colorTextureIntensity;
	target.diffusionTextureIntensity = source.diffusionTextureIntensity;
	target.luminosityTextureIntensity = source.luminosityTextureIntensity;
	target.reflectanceTextureIntensity = source.reflectanceTextureIntensity;
	target.transparencyTextureIntensity = source.transparencyTextureIntensity;
	target.transparencyAlphaTextureIntensity = source.transparencyAlphaTextureIntensity;
	target.transparencyTextureIntensityVol = source.transparencyTextureIntensityVol;
	target.transparencyAlphaTextureIntensityVol = source.transparencyAlphaTextureIntensityVol;
	target.roughnessTextureIntensity = source.roughnessTextureIntensity;
	target.displacementTextureHeight = source.displacementTextureHeight;
	target.normalMapTextureHeight = source.normalMapTextureHeight;
	target.iridescenceIntensity = source.iridescenceIntensity;
	target.iridescenceSubsurfaceThickness = source.iridescenceSubsurfaceThickness;
	target.textureFractalizeCubeSize = source.textureFractalizeCubeSize;
	target.perlinNoiseValueOffset = source.perlinNoiseValueOffset;
	target.perlinNoiseColorIntensity = source.perlinNoiseColorIntensity;
	target.perlinNoiseLuminosityIntensity = source.perlinNoiseLuminosityIntensity;
	target.perlinNoiseDisplacementIntensity = source.perlinNoiseDisplacementIntensity;
	target.perlinNoiseReflectanceIntensity = source.perlinNoiseReflectanceIntensity;
	target.perlinNoiseTransparencyColorIntensity = source.perlinNoiseTransparencyColorIntensity;
	target.perlinNoiseTransparencyAlphaIntensity = source.perlinNoiseTransparencyAlphaIntensity;
	target.perlinNoiseTransparencyColorIntensityVol = source.perlinNoiseTransparencyColorIntensityVol;
	target.perlinNoiseTransparencyAlphaIntensityVol = source.perlinNoiseTransparencyAlphaIntensityVol;

	target.color = toClFloat3(source.color);
	target.luminosityColor = toClFloat3(source.luminosityColor);
	target.transparencyInteriorColor = toClFloat3(source.transparencyInteriorColor);
	target.reflectionsColor = toClFloat3(source.reflectionsColor);
	target.specularColor = toClFloat3(source.specularColor);
	target.transparencyColor = toClFloat3(source.transparencyColor);

	target.textureCenter = toClFloat3(source.textureCenter);
	target.textureScale = toClFloat3(source.textureScale);
	target.perlinNoisePeriod = toClFloat3(source.perlinNoisePeriod);
	target.perlinNoisePositionOffset = toClFloat3(source.perlinNoisePositionOffset);

	target.rotMatrixTexture = toClMatrix33(source.rotMatrixTexture);
	target.rotMatrixPerlinNoise = toClMatrix33(source.rotMatrixPerlinNoise);

	target.textureMappingType = static_cast<enumTextureMappingCl>(source.textureMappingType);
	target.fresnelReflectance = source.fresnelReflectance;
	target.useColorsFromPalette = source.useColorsFromPalette;
	target.specularPlasticEnable = source.specularPlasticEnable;
	target.metallic = source.metallic;
	target.roughSurface = source.roughSurface;

	target.perlinNoiseEnable = source.perlinNoiseEnable;
	target.perlinNoiseAbs = source.perlinNoiseAbs;
	target.perlinNoiseColorEnable = source.perlinNoiseColorEnable;
	target.perlinNoiseColorInvert = source.perlinNoiseColorInvert;
	target.perlinNoiseLuminosityEnable = source.perlinNoiseLuminosityEnable;
	target.perlinNoiseLuminosityInvert = source.perlinNoiseLuminosityInvert;
	target.perlinNoiseDisplacementEnable = source.perlinNoiseDisplacementEnable;
	target.perlinNoiseDisplacementInvert = source.perlinNoiseDisplacementInvert;
	target.perlinNoiseReflectanceEnable = source.perlinNoiseReflectanceEnable;
	target.perlinNoiseReflectanceInvert = source.perlinNoiseReflectanceInvert;
	target.perlinNoiseTransparencyColorEnable = source.perlinNoiseTransparencyColorEnable;
	target.perlinNoiseTransparencyColorInvert = source.perlinNoiseTransparencyColorInvert;
	target.perlinNoiseTransparencyAlphaEnable = source.perlinNoiseTransparencyAlphaEnable;
	target.perlinNoiseTransparencyAlphaInvert = source.perlinNoiseTransparencyAlphaInvert;

	target.useColorTexture = source.useColorTexture;
	target.useDiffusionTexture = source.useDiffusionTexture;
	target.useLuminosityTexture = source.useLuminosityTexture;
	target.useDisplacementTexture = source.useDisplacementTexture;
	target.useNormalMapTexture = source.useNormalMapTexture;
	target.normalMapTextureFromBumpmap = source.normalMapTextureFromBumpmap;
	target.normalMapTextureInvertGreen = source.normalMapTextureInvertGreen;
	target.useReflectanceTexture = source.useReflectanceTexture;
	target.useTransparencyTexture = source.useTransparencyTexture;
	target.useTransparencyAlphaTexture = source.useTransparencyAlphaTexture;
	target.useRoughnessTexture = source.useRoughnessTexture;
	target.iridescenceEnabled = source.iridescenceEnabled;
	target.textureFractalize = source.textureFractalize;

	target.insideColoringEnable = source.insideColoringEnable;
	target.subsurfaceScattering = source.subsurfaceScattering;

	target.surfaceGradientEnable = source.surfaceGradientEnable;
	target.specularGradientEnable = source.specularGradientEnable;
	target.diffuseGradientEnable = source.diffuseGradientEnable;
	target.luminosityGradientEnable = source.luminosityGradientEnable;
	target.roughnessGradientEnable = source.roughnessGradientEnable;
	target.reflectanceGradientEnable = source.reflectanceGradientEnable;
	target.transparencyGradientEnable = source.transparencyGradientEnable;

	// these are initialized in cOpenClDynamicData::BuildMaterialsData()
	target.colorTextureIndex = 0;
	target.diffusionTextureIndex = 0;
	target.luminosityTextureIndex = 0;
	target.displacementTextureIndex = 0;
	target.normalMapTextureIndex = 0;
	target.reflectanceTextureIndex = 0;
	target.transparencyTextureIndex = 0;
	target.roughnessTextureIndex = 0;

	target.fractalColoring = clCopySFractalColoringCl(source.fractalColoring);

	return target;
}

#endif

#endif /* MANDELBULBER2_OPENCL_MATERIAL_CL_H_ */
