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
 * material struct for opencl
 */

#ifndef MANDELBULBER2_OPENCL_MATERIAL_CL_H_
#define MANDELBULBER2_OPENCL_MATERIAL_CL_H_

#ifndef OPENCL_KERNEL_CODE
#include "fractal_coloring_cl.hpp"
#include "opencl_algebra.h"

#include "src/material.h"
#endif /* OPENCL_KERNEL_CODE */

typedef struct
{
	cl_int id;

	cl_float shading;
	cl_float specular;
	cl_float specularWidth;
	cl_float reflectance;
	cl_float luminosity;
	cl_float transparencyIndexOfRefraction;
	cl_float transparencyOfInterior;
	cl_float transparencyOfSurface;
	cl_float paletteOffset;
	cl_float coloring_speed;
	cl_float colorTextureIntensity;
	cl_float diffusionTextureIntensity;
	cl_float luminosityTextureIntensity;
	cl_float displacementTextureHeight;
	cl_float normalMapTextureHeight;
	cl_float iridescenceIntensity;
	cl_float iridescenceSubsurfaceThickness;

	cl_float3 color;
	cl_float3 luminosityColor;
	cl_float3 transparencyInteriorColor;
	cl_float3 specularColor;

	// cColorPalette palette;

	// cTexture colorTexture;
	// cTexture diffusionTexture;
	// cTexture luminosityTexture;
	// cTexture displacementTexture;
	// cTexture normalMapTexture;

	cl_float3 textureCenter;
	cl_float3 textureRotation;
	cl_float3 textureScale;

	matrix33 rotMatrix;

	// texture::enumTextureMapping textureMappingType;
	cl_int fresnelReflectance;
	cl_int useColorsFromPalette;
	cl_int metallic;

	cl_int useColorTexture;
	cl_int useDiffusionTexture;
	cl_int useLuminosityTexture;
	cl_int useDisplacementTexture;
	cl_int useNormalMapTexture;
	cl_int normalMapTextureFromBumpmap;
	cl_int iridescenceEnabled;

	sFractalColoringCl fractalColoring;
} sMaterialCl;

#ifndef OPENCL_KERNEL_CODE
sMaterialCl clCopySMaterialCl(const cMaterial &source)
{
	sMaterialCl target;

	target.id = source.id;

	target.shading = source.shading;
	target.specular = source.specular;
	target.specularWidth = source.specularWidth;
	target.reflectance = source.reflectance;
	target.luminosity = source.luminosity;
	target.transparencyIndexOfRefraction = source.transparencyIndexOfRefraction;
	target.transparencyOfInterior = source.transparencyOfInterior;
	target.transparencyOfSurface = source.transparencyOfSurface;
	target.paletteOffset = source.paletteOffset;
	target.coloring_speed = source.coloring_speed;
	target.colorTextureIntensity = source.colorTextureIntensity;
	target.diffusionTextureIntensity = source.diffusionTextureIntensity;
	target.luminosityTextureIntensity = source.luminosityTextureIntensity;
	target.displacementTextureHeight = source.displacementTextureHeight;
	target.normalMapTextureHeight = source.normalMapTextureHeight;
	target.iridescenceIntensity = source.iridescenceIntensity;
	target.iridescenceSubsurfaceThickness = source.iridescenceSubsurfaceThickness;

	target.color = toClFloat3(source.color);
	target.luminosityColor = toClFloat3(source.luminosityColor);
	target.transparencyInteriorColor = toClFloat3(source.transparencyInteriorColor);
	target.specularColor = toClFloat3(source.specularColor);

	target.textureCenter = toClFloat3(source.textureCenter);
	target.textureRotation = toClFloat3(source.textureRotation);
	target.textureScale = toClFloat3(source.textureScale);

	target.fresnelReflectance = source.fresnelReflectance;
	target.useColorsFromPalette = source.useColorsFromPalette;
	target.metallic = source.metallic;

	target.useColorTexture = source.useColorTexture;
	target.useDiffusionTexture = source.useDiffusionTexture;
	target.useLuminosityTexture = source.useLuminosityTexture;
	target.useDisplacementTexture = source.useDisplacementTexture;
	target.useNormalMapTexture = source.useNormalMapTexture;
	target.normalMapTextureFromBumpmap = source.normalMapTextureFromBumpmap;
	target.iridescenceEnabled = source.iridescenceEnabled;

	target.fractalColoring = clCopySFractalColoringCl(source.fractalColoring);

	return target;
}

#endif

#endif /* MANDELBULBER2_OPENCL_MATERIAL_CL_H_ */
