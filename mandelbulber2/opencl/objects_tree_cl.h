/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2024 Mandelbulber Team         §R-==%w["'~5]m%=L.=~5N
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
 * OpenCL structures for objects tree nodes
 */

#ifndef MANDELBULBER2_OPENCL_OBJECTS_TREE_CL_H_
#define MANDELBULBER2_OPENCL_OBJECTS_TREE_CL_H_

#ifndef OPENCL_KERNEL_CODE
#include "src/include_header_wrapper.hpp"
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

/* Node data for rendering, stored in dynamic buffer.
 * The inverseTransformMatrix is a row-major 4×4 matrix stored as 16 floats.
 * Rows are [m0..m3], [m4..m7], [m8..m11], [m12..m15].
 * TransformPoint(p) = (m0*x+m1*y+m2*z+m3, m4*x+m5*y+m6*z+m7, m8*x+m9*y+m10*z+m11)
 */
typedef struct
{
	cl_int type;						 /* enumNodeTypeCl */
	cl_int level;					 /* depth in the tree (0 = root) */
	cl_int internalObjectId; /* object index for coloring/material */
	cl_int primitiveIdx;		 /* index into primitives array (for primitive nodes) */

	cl_int hybridSequenceIndex; /* index into sequences array */
	cl_float absScale;					/* absolute scale factor of this node */
	cl_float3 repeat;						/* repeat modulation in world space (0 = disabled) */

	cl_float m[16]; /* row-major 4×4 inverse transform matrix */
} sClNodeDataForRendering;

#ifndef OPENCL_KERNEL_CODE
/* Helper to convert CMatrix44 to cl_float array */
#include "src/algebra.hpp"
inline void CMatrix44ToClFloatArray(const CMatrix44 &m, cl_float out[16])
{
	out[0] = cl_float(m.m11);
	out[1] = cl_float(m.m12);
	out[2] = cl_float(m.m13);
	out[3] = cl_float(m.m14);
	out[4] = cl_float(m.m21);
	out[5] = cl_float(m.m22);
	out[6] = cl_float(m.m23);
	out[7] = cl_float(m.m24);
	out[8] = cl_float(m.m31);
	out[9] = cl_float(m.m32);
	out[10] = cl_float(m.m33);
	out[11] = cl_float(m.m34);
	out[12] = 0.0f;
	out[13] = 0.0f;
	out[14] = 0.0f;
	out[15] = 1.0f;
}
#endif /* OPENCL_KERNEL_CODE */

#endif /* MANDELBULBER2_OPENCL_OBJECTS_TREE_CL_H_ */
