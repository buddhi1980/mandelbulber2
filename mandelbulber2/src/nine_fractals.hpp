/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-22 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cNineFractals - container for 9 elements of hybrid fractal
 * Beware: On change maybe needs porting changes to opencl/fractal_sequence_cl.h
 */

#ifndef MANDELBULBER2_SRC_NINE_FRACTALS_HPP_
#define MANDELBULBER2_SRC_NINE_FRACTALS_HPP_

#include <memory>
#include <vector>

#include "algebra.hpp"
#include "fractal_enums.h"

#include "formula/definition/all_fractal_list.hpp"

// custom includes
#ifdef USE_OPENCL
#include "opencl/fractal_sequence_cl.h"
#endif

// forward declarations
class cParameterContainer;
class cFractalContainer;
struct sFractal;
class cAbstractFractal;

class cNineFractals
{
public:
	cNineFractals(std::shared_ptr<const cFractalContainer> fractalPar,
		std::shared_ptr<const cParameterContainer> generalPar);
	sFractal *GetFractal(int index) const { return fractals[index].get(); }
	int GetSequence(const int i) const;
	bool IsHybrid() const { return isHybrid; }
	fractal::enumDEType GetDEType(int formulaIndex) const;
	fractal::enumDEFunctionType GetDEFunctionType(int formulaIndex) const;
	inline double GetWeight(int formulaIndex) const { return formulaWeight[formulaIndex]; }
	inline int GetMaxFractalIndex() const { return maxFractalIndex; }
	inline bool IsAddCConstant(int formulaIndex) const { return addCConstant[formulaIndex]; }
	inline bool IsCheckForBailout(int formulaIndex) const { return checkForBailout[formulaIndex]; }
	inline bool UseOptimizedDE() const { return useOptimizedDE; }
	QString GetDETypeString() const;
	inline double GetBailout(int formulaIndex) const { return bailout[formulaIndex]; }
	inline bool IsJuliaEnabled(int formulaIndex) const { return juliaEnabled[formulaIndex]; }
	inline CVector3 GetJuliaConstant(int formulaIndex) const { return juliaConstant[formulaIndex]; }
	inline CVector3 GetConstantMultiplier(int formulaIndex) const
	{
		return constantMultiplier[formulaIndex];
	}
	inline double GetInitialWAxis(int formulaIndex) const { return initialWAxis[formulaIndex]; }
	inline bool UseAdditionalBailoutCond(int formulaIndex) const
	{
		return useAdditionalBailoutCond[formulaIndex];
	};
	inline cAbstractFractal *GetFractalFormulaFunction(int formulaIndex) const
	{
		return fractalFormulaFunctions[formulaIndex];
	}
	inline fractal::enumDEAnalyticFunction GetDEAnalyticFunction(int formulaIndex) const
	{
		return DEAnalyticFunction[formulaIndex];
	}
	inline fractal::enumColoringFunction GetColoringFunction(int formulaIndex) const
	{
		return coloringFunction[formulaIndex];
	}
	inline int GetFormulaMaxiter(int formulaIndex) const { return formulaMaxiter[formulaIndex]; }

	static int GetIndexOnFractalList(fractal::enumFractalFormula formula);

#ifdef USE_OPENCL
	void CopyToOpenclData(sClFractalSequence *sequence) const;
#endif

private:
	std::vector<std::unique_ptr<sFractal>> fractals;
	bool forceDeltaDE;
	bool forceAnalyticDE;
	bool isHybrid;
	bool isBoolean;
	fractal::enumDEFunctionType optimizedDEType;
	bool useOptimizedDE;
	int maxFractalIndex;
	int maxN;
	std::vector<int> hybridSequence;
	int hybridSequenceLength;

	std::vector<double> formulaWeight;
	std::vector<fractal::enumDEFunctionType> DEFunctionType;
	std::vector<fractal::enumDEType> DEType;
	std::vector<fractal::enumDEAnalyticFunction> DEAnalyticFunction;
	std::vector<fractal::enumColoringFunction> coloringFunction;
	std::vector<int> counts;
	std::vector<int> formulaStartIteration;
	std::vector<int> formulaStopIteration;
	std::vector<bool> addCConstant;
	std::vector<bool> checkForBailout;
	std::vector<double> bailout;
	std::vector<bool> juliaEnabled;
	std::vector<CVector3> juliaConstant;
	std::vector<CVector3> constantMultiplier;
	std::vector<double> initialWAxis;
	std::vector<bool> useAdditionalBailoutCond;
	std::vector<int> formulaMaxiter;
	std::vector<cAbstractFractal *> fractalFormulaFunctions;

	void CreateSequence(std::shared_ptr<const cParameterContainer> generalPar);
};

#endif /* MANDELBULBER2_SRC_NINE_FRACTALS_HPP_ */
