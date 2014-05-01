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

struct sFractalIn
{
	CVector3 point;
	CVector3 c;
	int minN;
	int maxN;
	sFractalIn(CVector3 _point, CVector3 _c, int _minN, int _maxN) : point(_point), c(_c), minN(_minN), maxN(_maxN) {};
};

struct sFractalOut
{
	CVector3 z;
	double distance;
	double colorIndex;
	int iters;
	bool maxiter;
};

template<fractal::enumCalculationMode Mode>
void Compute(const cFourFractals &four, const sFractalIn &in, sFractalOut *out);

#endif /* COMPUTE_FRACTAL_HPP_ */
