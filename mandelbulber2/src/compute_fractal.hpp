/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-16 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Compute - function fractal computation
 */

#ifndef MANDELBULBER2_SRC_COMPUTE_FRACTAL_HPP_
#define MANDELBULBER2_SRC_COMPUTE_FRACTAL_HPP_

#include "commonparams.hpp"
#include "fractal_coloring.hpp"
#include "calculation_mode.h"

// forward declarations
class cNineFractals;

struct sFractalIn
{
	CVector3 point;
	int minN;
	int maxN;
	sCommonParams common;
	int forcedFormulaIndex;
	sFractalColoring fractalColoring;
	sFractalIn(CVector3 _point, int _minN, int _maxN, const sCommonParams &_common,
		int _forcedFormulaIndex, const sFractalColoring &_fractalColoring = sFractalColoring())
			: point(_point),
				minN(_minN),
				maxN(_maxN),
				common(_common),
				forcedFormulaIndex(_forcedFormulaIndex),
				fractalColoring(_fractalColoring)
	{
	}
};

struct sFractalOut
{
	CVector3 z;
	double distance;
	double colorIndex;
	double fakeAO;
	double orbitTrapR;
	int iters;
	bool maxiter;
};

template <fractal::enumCalculationMode Mode>
void Compute(const cNineFractals &fractals, const sFractalIn &in, sFractalOut *out);

#endif /* MANDELBULBER2_SRC_COMPUTE_FRACTAL_HPP_ */
