/*
 * material.h
 *
 *  Created on: 9 cze 2017
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_OPENCL_MATERIAL_CL_H_
#define MANDELBULBER2_OPENCL_MATERIAL_CL_H_

#ifndef OPENCL_KERNEL_CODE
#include "../opencl/opencl_algebra.h"
#include "../src/material.h"
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

	cl_int3 color;
	cl_int3 luminosityColor;
	cl_int3 transparencyInteriorColor;
	cl_int3 specularColor;

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

	cl_int useColorTexture;
	cl_int useDiffusionTexture;
	cl_int useLuminosityTexture;
	cl_int useDisplacementTexture;
	cl_int useNormalMapTexture;
	cl_int normalMapTextureFromBumpmap;
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

	target.color = toClInt3(source.color);
	target.luminosityColor = toClInt3(source.luminosityColor);
	target.transparencyInteriorColor = toClInt3(source.transparencyInteriorColor);
	target.specularColor = toClInt3(source.specularColor);

	target.textureCenter = toClFloat3(source.textureCenter);
	target.textureRotation = toClFloat3(source.textureRotation);
	target.textureScale = toClFloat3(source.textureScale);

	target.fresnelReflectance = source.fresnelReflectance;
	target.useColorsFromPalette = source.useColorsFromPalette;

	target.useColorTexture = source.useColorTexture;
	target.useDiffusionTexture = source.useDiffusionTexture;
	target.useLuminosityTexture = source.useLuminosityTexture;
	target.useDisplacementTexture = source.useDisplacementTexture;
	target.useNormalMapTexture = source.useNormalMapTexture;
	target.normalMapTextureFromBumpmap = source.normalMapTextureFromBumpmap;

	return target;
}

#endif

#endif /* MANDELBULBER2_OPENCL_MATERIAL_CL_H_ */
