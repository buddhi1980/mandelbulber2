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

#ifndef FOUR_FRACTALS_HPP_
#define FOUR_FRACTALS_HPP_

#include "fractal.h"
#include "parameters.hpp"
#include "fractal_container.hpp"
#include "fractal_list.hpp"
#include <QtCore>

class cFourFractals
{
public:
	cFourFractals();
	~cFourFractals();
	cFourFractals(const cFractalContainer *fractalPar, const cParameterContainer *generalPar);
	cFractal* GetFractal(int index) const {return fourFractals[index];}
	cFractal **fourFractals;
	int GetSequence(int i) const;
	bool IsHybrid() const {return isHybrid;}
	fractal::enumDEType GetDEType(int formulaIndex) const;
	double GetWeight(int formulaIndex) const {return formulaWeight[formulaIndex];}

private:
	fractal::enumDEType DEType[NUMBER_OF_FRACTALS];
	QVector<int> hybridSequence;
	void CreateSequence(const cParameterContainer *generalPar);
	int maxN;
	bool isHybrid;
	double formulaWeight[NUMBER_OF_FRACTALS];
};


#endif /* FOUR_FRACTALS_HPP_ */
