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
 * OpenCL structures for hybrid fractal sequences
 * derived from src/hybrid_fractal_sequences.h
 */

#ifndef MANDELBULBER2_OPENCL_HYBRID_SEQUENCE_CL_H_
#define MANDELBULBER2_OPENCL_HYBRID_SEQUENCE_CL_H_

#ifndef OPENCL_KERNEL_CODE
#include "fractal_cl.h"
#include "fractal_sequence_cl.h"
#endif

// OpenCL version of cHybridFractalSequences::sFractalData
// Note: fractalFormulaObject pointer is not transferred to OpenCL
typedef struct
{
	cl_float formulaWeight;
	cl_int formulaIterations;
	cl_int formulaStartIteration;
	cl_int formulaStopIteration;
	cl_int addCConstant;
	cl_int checkForBailout;
	cl_float bailout;
	cl_int useAdditionalBailoutCond;
	sFractalCl fractalParameters;
} sHybridFractalDataCl;

// OpenCL version of cHybridFractalSequences::sSequence
// The variable-length arrays (seqence and fractData) are stored separately in dynamic data
typedef struct
{
	cl_int length;
	cl_int numberOfFractalsInTheSequence;
	cl_int internalObjectId;

	enumDEFunctionTypeCl DEFunctionType;
	enumDETypeCl DEType;
	enumDEAnalyticFunctionCl DEAnalyticFunction;
	enumColoringFunctionCl coloringFunction;

	cl_int isHybrid;
	cl_int juliaEnabled;
	cl_float3 juliaConstant;
	cl_float3 constantMultiplier;
	cl_float initialWAxis;
	cl_int formulaMaxiter;

	cl_int sequenceArrayOffset;   // offset to sequence array (cl_int[]) in dynamic data
	cl_int fractDataArrayOffset;  // offset to fractData array (sHybridFractalDataCl[]) in dynamic data
	cl_int formulaBaseIndex;      // global index of first formula in consts->fractal[] for this sequence
} sHybridSequenceCl;

#endif /* MANDELBULBER2_OPENCL_HYBRID_SEQUENCE_CL_H_ */
