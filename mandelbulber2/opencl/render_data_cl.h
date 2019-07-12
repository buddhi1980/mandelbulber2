/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * structure of sRenderData, which is used as a container for all rendering relevant data
 */

#ifndef MANDELBULBER2_OPENCL_RENDER_DATA_CL_H_
#define MANDELBULBER2_OPENCL_RENDER_DATA_CL_H_

typedef struct
{
#if !defined(MESH_EXPORT) || 1
	float3 lightVector;
	float3 viewVectorNotRotated;
#endif

#ifdef CHROMATIC_ABERRATION
	float hue;
#endif

#if defined(FULL_ENGINE) || defined(MESH_EXPORT)
	__global sMaterialCl **materials;
	__global float4 **palettes;

#ifdef USE_SURFACE_GRADIENT
	int *paletteSurfaceOffsets;
	int *paletteSurfaceLengths;
#endif
#ifdef USE_SPECULAR_GRADIENT
	int *paletteSpecularOffsets;
	int *paletteSpecularLengths;
#endif
#ifdef USE_DIFFUSE_GRADIENT
	int *paletteDiffuseOffsets;
	int *paletteDiffuseLengths;
#endif
#ifdef USE_LUMINOSITY_GRADIENT
	int *paletteLuminosityOffsets;
	int *paletteLuminosityLengths;
#endif
#ifdef USE_ROUGHNESS_GRADIENT
	int *paletteRoughnessOffsets;
	int *paletteRoughnessLengths;
#endif
#ifdef USE_REFLECTANCE_GRADIENT
	int *paletteReflectanceOffsets;
	int *paletteReflectanceLengths;
#endif
#ifdef USE_TRANSPARENCY_GRADIENT
	int *paletteTransparencyOffsets;
	int *paletteTransparencyLengths;
#endif
#else
	__global sMaterialCl *material;
	__global float4 *palette;

#ifdef USE_SURFACE_GRADIENT
	int paletteSurfaceOffset;
	int paletteSurfaceLength;
#endif
#ifdef USE_SPECULAR_GRADIENT
	int paletteSpecularOffset;
	int paletteSpecularLength;
#endif
#ifdef USE_DIFFUSE_GRADIENT
	int paletteDiffuseOffset;
	int paletteDiffuseLength;
#endif
#ifdef USE_LUMINOSITY_GRADIENT
	int paletteLuminosityOffset;
	int paletteLuminosityLength;
#endif
#ifdef USE_ROUGHNESS_GRADIENT
	int paletteRoughnessOffset;
	int paletteRoughnessLength;
#endif
#ifdef USE_REFLECTANCE_GRADIENT
	int paletteReflectanceOffset;
	int paletteReflectanceLength;
#endif
#ifdef USE_TRANSPARENCY_GRADIENT
	int paletteTransparencyOffset;
	int paletteTransparencyLength;
#endif

#endif

#ifdef USE_TEXTURES
	__global uchar4 **textures;
	int2 *textureSizes;
#endif

#if !defined(MESH_EXPORT) || 1
	__global sVectorsAroundCl *AOVectors;
	__global sLightCl *lights;
#endif
	__global sPrimitiveCl *primitives;
	__global sPrimitiveGlobalPositionCl *primitivesGlobalPosition;

#if !defined(MESH_EXPORT) || 1
	__global sObjectDataCl *objectsData;

	int reflectionsMax;
	int numberOfLights;
	int AOVectorsCount;
#endif

	int numberOfPrimitives;

#if !defined(MESH_EXPORT) || 1
	matrix33 mRot;
	matrix33 mRotInv;
#endif
} sRenderData;

#endif /* MANDELBULBER2_OPENCL_RENDER_DATA_CL_H_ */
