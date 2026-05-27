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
 * Dynamic (variable-size) OpenCL fractal sequence and fractal entry structures.
 * These replace the fixed-size sClFractalSequence / sClInConstants::fractal[] arrays,
 * allowing an unlimited number of fractal objects in the scene.
 */

#ifndef MANDELBULBER2_OPENCL_FRACTAL_SEQUENCE_DYNAMIC_CL_H_
#define MANDELBULBER2_OPENCL_FRACTAL_SEQUENCE_DYNAMIC_CL_H_

#ifndef OPENCL_KERNEL_CODE
#include "src/include_header_wrapper.hpp"
#include "fractal_sequence_cl.h"
#endif

/* Per-sequence metadata stored in the dynamic buffer.
 *
 * Each sequence corresponds to a single leaf fractal object (or hybrid group)
 * in the objects tree.  The hybrid sequence look-up table is packed into a
 * separate flat int array; sequenceOffset is the first index in that array
 * for this sequence.  Similarly, the actual fractal parameters (sClFractalCl)
 * are packed into another flat array; fractalDataOffset is the first index
 * for this sequence.
 */
typedef struct
{
	cl_int isHybrid;
	cl_int juliaEnabled;
	cl_int formulaMaxiter;
	cl_int DEType;							/* enumDETypeCl */
	cl_int DEAnalyticFunction;	/* enumDEAnalyticFunctionCl */
	cl_int DEFunctionType;			/* enumDEFunctionTypeCl */
	cl_int coloringFunction;		/* enumColoringFunctionCl */

	cl_int numberOfFractalsInTheSequence; /* distinct fractal slots in this sequence */
	cl_int internalObjectId;							/* object id used for coloring */

	cl_int sequenceLength;	/* length of the hybrid iteration sequence */
	cl_int sequenceOffset;	/* index of first entry in the hybrid-seq flat int array */
	cl_int fractalDataOffset; /* index of first sClFractalEntryDynamic for this sequence */

	cl_float3 juliaConstant;
	cl_float3 constantMultiplier;
	cl_float initialWAxis;
	cl_int padding; /* alignment */
} sClSequenceDynamic;

/* Per-formula entry inside a sequence, stored in the fractal data flat array.
 * The fractal parameters (sClFractalCl) are NOT embedded here to allow proper
 * alignment and independent access; the fractalParameters field is intentionally
 * the LAST member so the compiler lays it out contiguously after the scalar fields.
 */
typedef struct
{
	cl_float formulaWeight;
	cl_int addCConstant;
	cl_int checkForBailout;
	cl_float bailout;
	cl_int useAdditionalBailoutCond;
	cl_int padding[3]; /* pad to 16-byte boundary before sFractalCl */
} sClFractalEntryMetaCl;

#endif /* MANDELBULBER2_OPENCL_FRACTAL_SEQUENCE_DYNAMIC_CL_H_ */
