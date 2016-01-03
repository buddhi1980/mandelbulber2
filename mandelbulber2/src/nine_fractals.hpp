/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cFourFractals - container for 4 elements of hybrid fractal
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#ifndef NINE_FRACTALS_HPP_
#define NINE_FRACTALS_HPP_

#include "fractal.h"
#include "parameters.hpp"
#include "fractal_container.hpp"
#include "fractal_list.hpp"
#include <QtCore>

class cNineFractals
{
public:
	cNineFractals(const cFractalContainer *fractalPar, const cParameterContainer *generalPar);
	~cNineFractals();
	cFractal* GetFractal(int index) const {return fractals[index];}
	cFractal **fractals;
	int GetSequence(int i) const;
	bool IsHybrid() const {return isHybrid;}
	fractal::enumDEType GetDEType(int formulaIndex) const;
	fractal::enumDEFunctionType GetDEFunctionType(int formulaIndex) const;
	inline double GetWeight(int formulaIndex) const {return formulaWeight[formulaIndex];}
	inline int GetMaxFractalIndex() const {return maxFractalIndex;}
	inline bool IsDontAddCContant(int formulaIndex) const {return dontAddCConstant[formulaIndex];}
	inline bool IsCheckForBailout(int formulaIndex) const {return checkForBailout[formulaIndex];}
	inline bool UseOptimizedDE() const {return useOptimizedDE;}
	QString GetDETypeString() const;
	inline double GetBailout(int formulaIndex) const {return bailout[formulaIndex];};

private:
	bool forceDeltaDE;
	bool isHybrid;
	fractal::enumDEFunctionType optimizedDEType;
	bool useOptimizedDE;
	int maxFractalIndex;
	int maxN;
	QVector<int> hybridSequence;

	double formulaWeight[NUMBER_OF_FRACTALS];
	fractal::enumDEFunctionType DEFunctionType[NUMBER_OF_FRACTALS];
	fractal::enumDEType DEType[NUMBER_OF_FRACTALS];
	int counts[NUMBER_OF_FRACTALS];
	int formulaStartIteriation[NUMBER_OF_FRACTALS];
	int formulaStopIteration[NUMBER_OF_FRACTALS];
	bool dontAddCConstant[NUMBER_OF_FRACTALS];
	bool checkForBailout[NUMBER_OF_FRACTALS];
	double bailout[NUMBER_OF_FRACTALS];

	void CreateSequence(const cParameterContainer *generalPar);
	int GetIndexOnFractalList(fractal::enumFractalFormula formula);
};


#endif /* NINE_FRACTALS_HPP_ */
