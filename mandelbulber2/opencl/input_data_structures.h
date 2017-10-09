/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen (jenzebas@gmail.com)
 *
 * global structs for copying data to opencl and back
 */

#ifndef OPENCL_KERNEL_CODE
#include "fractal_cl.h"
#include "fractal_sequence_cl.h"
#include "fractparams_cl.hpp"
#endif

#ifndef INPUT_DATA_STRUCTURES
#define INPUT_DATA_STRUCTURES

typedef struct
{
	cl_float R;
	cl_float G;
	cl_float B;
	cl_float zBuffer;
	cl_ushort opacity;
	cl_ushort alpha;
	cl_uchar colR;
	cl_uchar colG;
	cl_uchar colB;
} sClPixel;

typedef struct
{
	sParamRenderCl params;
	sFractalCl fractal[NUMBER_OF_FRACTALS]; // temporary for testing
	sClFractalSequence sequence;
} sClInConstants;

typedef struct
{
	cl_int dummy[256];
} sClInBuff;

typedef struct
{
	cl_uint N;
	cl_uint deltaDEMaxN;
	cl_int randomSeed;
	cl_int iterThreshMode;
	cl_int normalCalculationMode;
	cl_float3 orbitTrap;
	cl_float distThresh;
	cl_float detailSize;
} sClCalcParams;

// ambient occlusion data
typedef struct
{
	cl_float3 v;
	cl_float3 color;
	cl_float alpha;
	cl_float beta;
} sVectorsAroundCl;

typedef struct
{
	cl_float3 position;
	cl_float3 colour;
	float intensity;
	cl_int enabled;
} sLightCl;

#endif // INPUT_DATA_STRUCTURES
