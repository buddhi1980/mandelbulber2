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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * container for 9 elements of hybrid fractal
 * file was manually derived from src/nine_fractal.*
 */

#ifndef MANDELBULBER2_OPENCL_FRACTAL_SEQUENCE_CL_H_
#define MANDELBULBER2_OPENCL_FRACTAL_SEQUENCE_CL_H_

#define OPENCL_FRACTAL_SEQUENCE_LENGTH 250

#ifndef OPENCL_KERNEL_CODE
#include "src/fractal_enums.h"
#endif

typedef enum { analyticDEType, deltaDEType } enumDETypeCl;

typedef enum {
	linearDEFunction = 1,
	logarithmicDEFunction = 2,
} enumDEFunctionTypeCl;

typedef enum {
	clAnalyticFunctionNone = 0,
	clAnalyticFunctionLinear = 1,
	clAnalyticFunctionLogarithmic = 2,
	clAnalyticFunctionIFS = 3,
	clAnalyticFunctionPseudoKleinian = 4,
	clAnalyticFunctionJosKleinian = 5
} enumDEAnalyticFunctionCl;

typedef enum {
	clColoringFunctionUndefined = -1,
	clColoringFunctionDefault = 0,
	clColoringFunctionABox = 1,
	clColoringFunctionIFS = 2,
	clColoringFunctionAmazingSurf = 3,
	clColoringFunctionABox2 = 4,
	clColoringFunctionDonut = 5,
} enumColoringFunctionCl;

typedef struct
{
	cl_int isHybrid;
	cl_int hybridSequence[OPENCL_FRACTAL_SEQUENCE_LENGTH];
	cl_float formulaWeight[NUMBER_OF_FRACTALS];
	enumDEFunctionTypeCl DEFunctionType[NUMBER_OF_FRACTALS];
	enumDETypeCl DEType[NUMBER_OF_FRACTALS];
	cl_int counts[NUMBER_OF_FRACTALS];
	cl_int formulaStartIteration[NUMBER_OF_FRACTALS];
	cl_int formulaStopIteration[NUMBER_OF_FRACTALS];
	cl_int addCConstant[NUMBER_OF_FRACTALS];
	cl_int checkForBailout[NUMBER_OF_FRACTALS];
	cl_float bailout[NUMBER_OF_FRACTALS];
	cl_int juliaEnabled[NUMBER_OF_FRACTALS];
	cl_float4 juliaConstant[NUMBER_OF_FRACTALS];
	cl_float4 constantMultiplier[NUMBER_OF_FRACTALS];
	cl_float initialWAxis[NUMBER_OF_FRACTALS];
	cl_int useAdditionalBailoutCond[NUMBER_OF_FRACTALS];
	enumDEAnalyticFunctionCl DEAnalyticFunction[NUMBER_OF_FRACTALS];
	enumColoringFunctionCl coloringFunction[NUMBER_OF_FRACTALS];
} sClFractalSequence;

#endif /* MANDELBULBER2_OPENCL_FRACTAL_SEQUENCE_CL_H_ */
