/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2021 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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

#ifndef MANDELBULBER2_OPENCL_LIGHT_CL_H_
#define MANDELBULBER2_OPENCL_LIGHT_CL_H_

#ifndef OPENCL_KERNEL_CODE
#include "opencl_algebra.h"
#endif /* OPENCL_KERNEL_CODE */

typedef enum
{
	lightDirectional = 0,
	lightPoint = 1,
	lightConical = 2,
	lightProjection = 3,
	lightBeam = 4
} enumLightTypeCl;

typedef enum
{
	lightDecay1R = 0,
	lightDecay1R2 = 1,
	lightDecay1R3 = 2,
} enumLightDecayFunctionCl;

typedef struct
{
	// booleans
	cl_int enabled;
	cl_int castShadows;
	cl_int penetrating;
	cl_int relativePosition;
	cl_int repeatTexture;
	cl_int volumetric;

	// enums
	enumLightTypeCl type;
	enumLightDecayFunctionCl decayFunction;

	cl_int colorTextureIndex;

	cl_float coneAngle;
	cl_float coneSoftAngle;
	cl_float intensity;
	cl_float visibility;
	cl_float volumetricVisibility;
	cl_float size;
	cl_float softShadowCone;
	cl_float contourSharpness;

	cl_float coneRatio;
	cl_float coneSoftRatio;

	cl_float projectionHorizontalRatio;
	cl_float projectionVerticalRatio;

	cl_float3 color;

	cl_float3 position;
	cl_float3 rotation;
	cl_float3 target;
	cl_float3 lightDirection;
	cl_float3 lightTopVector;
	cl_float3 lightRightVector;

	matrix33 rotMatrix;

} sLightCl;

#endif /* MANDELBULBER2_OPENCL_LIGHT_CL_H_ */
