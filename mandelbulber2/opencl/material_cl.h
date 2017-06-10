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

void BuildMaterialsData(QByteArray *materialsClData, const QMap<int, cMaterial> &materials)
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
		cl_int materalClOffset (offset for material data)
		cl_int paletteOffset (offset for palette data)
		sMaterialCl material
		cl_int palette size (bytes)
		cl_int paletteLength (number of color palette items)
		cl_int dummy
		cl_int dummy
			cl_float3 color[0]
			cl_float3 color[1]
			...
			cl_float3 color[paletteLength]
	-------------------

	*/
	cl_int numberOfMaterials = materials.size();

	// numberOfMaterials
	materialsClData->append(reinterpret_cast<char *>(&numberOfMaterials), sizeof(numberOfMaterials));

	int headerSize = sizeof(numberOfMaterials);

	cl_int *materialOffsets = new cl_int[numberOfMaterials];

	cl_int totalMaterialOffset = headerSize + sizeof(cl_int) * numberOfMaterials;
	int materialIndex = 0;

	QByteArray materialByteArrays;

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

		cl_int materalClOffset = sizeof(cl_int) * 2;
		cl_int paletteOffset = materalClOffset + sizeof(sMaterialCl);
		cl_int paletteSizeBytes = sizeof(cl_float4) * paletteSize;
		cl_int paletteLength = paletteSize;

		// cl_int materalClOffset
		materialByteArrays.append(reinterpret_cast<char *>(&materalClOffset), sizeof(materalClOffset));
		totalMaterialOffset += sizeof(materalClOffset);

		// cl_int paletteOffset
		materialByteArrays.append(reinterpret_cast<char *>(&paletteOffset), sizeof(paletteOffset));
		totalMaterialOffset += sizeof(paletteOffset);

		// sMaterialCl material
		materialByteArrays.append(reinterpret_cast<char *>(&materialCl), sizeof(materialCl));
		totalMaterialOffset += sizeof(materialCl);

		// cl_int palette size (bytes)
		materialByteArrays.append(
			reinterpret_cast<char *>(&paletteSizeBytes), sizeof(paletteSizeBytes));
		totalMaterialOffset += sizeof(paletteSizeBytes);

		// cl_int paletteLength (number of color palette items)
		materialByteArrays.append(reinterpret_cast<char *>(&paletteLength), sizeof(paletteLength));
		totalMaterialOffset += sizeof(paletteLength);

		// data alignment to 16 (temporary code)
		cl_int dummy = 0;
		materialByteArrays.append(reinterpret_cast<char *>(&dummy), sizeof(dummy));
		totalMaterialOffset += sizeof(dummy);
		materialByteArrays.append(reinterpret_cast<char *>(&dummy), sizeof(dummy));
		totalMaterialOffset += sizeof(dummy);

		qDebug() << totalMaterialOffset;

		// palette data
		materialByteArrays.append(reinterpret_cast<char *>(paletteCl), paletteSizeBytes);
		totalMaterialOffset += paletteSizeBytes;

		delete[] paletteCl;
	}

	// materials offsets:
	materialsClData->append(
		reinterpret_cast<char *>(materialOffsets), sizeof(cl_int) * numberOfMaterials);
	totalMaterialOffset += sizeof(cl_int) * numberOfMaterials;

	// materials
	materialsClData->append(materialByteArrays);

	delete[] materialOffsets;
}

#endif

#endif /* MANDELBULBER2_OPENCL_MATERIAL_CL_H_ */
