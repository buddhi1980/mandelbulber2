/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * data structure common for main parameters and fractal parameters
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

#ifndef COMMONPARAMS_HPP_
#define COMMONPARAMS_HPP_

#include "algebra.hpp"

struct sFractalFoldings
{
	double boxLimit; //parameters of TGlad's folding
	double boxValue;
	double sphericalOuter;
	double sphericalInner;
	bool boxEnable;
	bool sphericalEnable;
};

struct sCommonParams
{
	CVector3 fakeLightsOrbitTrap;
	CVector3 juliaC;

	CVector3 constantMultiplier;
	double bailout;

	int fakeLightsMaxIter;
	int fakeLightsMinIter;

	bool juliaMode;

  CVector3 fractalPosition;
  CVector3 fractalRotation;
  CVector3 repeat;
	CRotationMatrix mRotFractalRotation;

	sFractalFoldings foldings;
};



#endif /* COMMONPARAMS_HPP_ */
