/*
 * compute_fractal.hpp
 *
 *  Created on: Apr 27, 2014
 *      Author: krzysztof
 */

#ifndef COMPUTE_FRACTAL_HPP_
#define COMPUTE_FRACTAL_HPP_

#include "algebra.hpp"
#include "primitives.h"
#include "four_fractals.hpp"
#include "commonparams.hpp"

struct sFractalIn
{
	CVector3 point;
	int minN;
	int maxN;
	sCommonParams common;
	sFractalIn(CVector3 _point, int _minN, int _maxN, sCommonParams _common)
		: point(_point), minN(_minN), maxN(_maxN), common(_common) {};
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

template<fractal::enumCalculationMode Mode>
void Compute(const cFourFractals &four, const sFractalIn &in, sFractalOut *out);

#endif /* COMPUTE_FRACTAL_HPP_ */
