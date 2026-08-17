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
 * data structures for primitive objects for OpenCL kernels
 */

#ifndef MANDELBULBER2_OPENCL_PRIMITIVES_CL_H_
#define MANDELBULBER2_OPENCL_PRIMITIVES_CL_H_

#ifndef OPENCL_KERNEL_CODE
#include "object_data_cl.h"
#include "opencl_algebra.h"
#endif

typedef enum
{
	clPrimBooleanOperatorAND = 0,
	clPrimBooleanOperatorOR = 1,
	clPrimBooleanOperatorSUB = 2,
	clPrimBooleanOperatorRevSUB = 3
} enumClPrimitiveBooleanOperator;

// primitive specified parameters
typedef struct ALIGN16
{
	cl_int empty;
} sPrimitivePlaneCl;

typedef struct ALIGN16
{
	cl_float3 limitsMax;
	cl_float3 limitsMin;
	cl_float rounding;
	cl_int empty;
	cl_int limitsEnable;


} sPrimitiveBoxCl;

typedef struct ALIGN16
{
	cl_float3 limitsMax;
	cl_float3 limitsMin;
	cl_float radius;
	cl_int empty;
	cl_int limitsEnable;


} sPrimitiveSphereCl;

typedef struct ALIGN16
{
	cl_float3 limitsMax;
	cl_float3 limitsMin;
	cl_float relativeAmplitude;
	cl_float animSpeed;
	cl_float animProgressionSpeed;
	cl_float length;
	cl_float waveFromObjectsRelativeAmplitude;
	cl_int empty;
	cl_int limitsEnable;
	cl_int waveFromObjectsEnable;
	cl_int iterations;
	cl_int animFrame;

} sPrimitiveWaterCl;

typedef struct ALIGN16
{
	cl_float3 limitsMax;
	cl_float3 limitsMin;
	cl_float2 wallNormal;
	cl_float radius;
	cl_float height;
	cl_int empty;
	cl_int caps;
	cl_int limitsEnable;

} sPrimitiveConeCl;

typedef struct ALIGN16
{
	cl_float3 limitsMax;
	cl_float3 limitsMin;
	cl_float radius;
	cl_float height;
	cl_int empty;
	cl_int caps;
	cl_int limitsEnable;
} sPrimitiveCylinderCl;

typedef struct ALIGN16
{
	cl_float3 limitsMax;
	cl_float3 limitsMin;
	cl_float radius;
	cl_float radiusLPow;
	cl_float tubeRadius;
	cl_float tubeRadiusLPow;
	cl_int empty;
	cl_int limitsEnable;
} sPrimitiveTorusCl;

typedef struct ALIGN16
{
	cl_float radius;
} sPrimitiveCircleCl;

typedef struct ALIGN16
{
	cl_float height;
	cl_float width;
} sPrimitiveRectangleCl;

typedef struct ALIGN16
{
	cl_float3 normals;
	cl_float triangleHeight;
	cl_float height;
	cl_float prismAngle;
	cl_int empty;
} sPrimitivePrismCl;

typedef struct ALIGN16
{
	cl_float3 limitsMax;
	cl_float3 limitsMin;
	cl_int empty;
	cl_int limitsEnable;
} sPrimitiveEllipsoidCl;

typedef struct ALIGN16
{
	matrix33 mRotAllPrimitivesRotation;
	cl_float3 allPrimitivesPosition;
	cl_float3 allPrimitivesRotation;

	cl_int primitiveIndexForBasicFog;
	cl_int primitiveIndexForDistFog;
	cl_int primitiveIndexForIterFog;
	cl_int primitiveIndexForClouds;
} sPrimitiveGlobalDataCl;

// union - variable container for primitive data
typedef union ALIGN16
{
	sPrimitivePlaneCl plane;
	sPrimitiveBoxCl box;
	sPrimitiveSphereCl sphere;
	sPrimitiveWaterCl water;
	sPrimitiveConeCl cone;
	sPrimitiveCylinderCl cylinder;
	sPrimitiveTorusCl torus;
	sPrimitiveCircleCl circle;
	sPrimitiveRectangleCl rectangle;
	sPrimitivePrismCl prism;
	sPrimitiveEllipsoidCl ellipsoid;
} uPrimitiveCl;

// full definition of primitive
typedef struct ALIGN16
{
	sObjectDataCl object; // structure
	uPrimitiveCl data;		// union !!!
	enumClPrimitiveBooleanOperator booleanOperator;
} sPrimitiveCl;

#endif /* MANDELBULBER2_OPENCL_PRIMITIVES_CL_H_ */
