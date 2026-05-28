/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2025 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * OpenCL representation of node data for objects tree rendering
 * Corresponds to CPU-side cObjectsTree::sNodeDataForRendering
 */

#ifndef MANDELBULBER2_OPENCL_NODE_DATA_CL_H_
#define MANDELBULBER2_OPENCL_NODE_DATA_CL_H_

#ifndef OPENCL_KERNEL_CODE
#include "opencl_algebra.h"
#endif

typedef enum
{
	nodeTypeFractal = 1,
	nodeTypePrimitive = 2,
	nodeTypeHybrid = 10,
	nodeTypeBooleanAdd = 11,
	nodeTypeBooleanMul = 12,
	nodeTypeBooleanSub = 13,
} enumNodeTypeCl;

// 4x4 homogeneous transformation matrix (stored as 4 rows of float4)
typedef struct
{
	cl_float4 r1;
	cl_float4 r2;
	cl_float4 r3;
	cl_float4 r4;
} matrix44;

#ifndef OPENCL_KERNEL_CODE
inline matrix44 toClMatrix44(const CMatrix44 &source)
{
	matrix44 m;
	m.r1 = {{cl_float(source.m11), cl_float(source.m12), cl_float(source.m13),
		cl_float(source.m14)}};
	m.r2 = {{cl_float(source.m21), cl_float(source.m22), cl_float(source.m23),
		cl_float(source.m24)}};
	m.r3 = {{cl_float(source.m31), cl_float(source.m32), cl_float(source.m33),
		cl_float(source.m34)}};
	m.r4 = {{cl_float(source.m41), cl_float(source.m42), cl_float(source.m43),
		cl_float(source.m44)}};
	return m;
}
#endif

// Node data for rendering - corresponds to CPU cObjectsTree::sNodeDataForRendering
typedef struct
{
	cl_int id;
	enumNodeTypeCl type;
	cl_int parentId;
	cl_int userObjectId;
	cl_int internalObjectId;
	cl_int primitiveIdx;
	cl_int level;
	cl_int hybridSequenceIndex;
	cl_float3 repeat;
	cl_float scale;
	cl_float absScale;
	cl_int material;
	matrix33 rotationMatrix;
	matrix44 inverseTransformMatrix;
} sNodeDataForRenderingCl;

#endif /* MANDELBULBER2_OPENCL_NODE_DATA_CL_H_ */
