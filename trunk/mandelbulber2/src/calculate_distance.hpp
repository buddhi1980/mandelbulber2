/*
 * calculate_distance.hpp
 *
 *  Created on: May 1, 2014
 *      Author: krzysztof
 */

#ifndef CALCULATE_DISTANCE_HPP_
#define CALCULATE_DISTANCE_HPP_

#include "compute_fractal.hpp"
#include "fractparams.hpp"
#include "four_fractals.hpp"

struct sDistanceIn
{
	CVector3 point;
	double detailSize;
	sDistanceIn(CVector3 _point, double _detailSize) : point(_point), detailSize(_detailSize) {};
};

struct sDistanceOut
{
	double distance;
	double colorIndex;
	int iters;
	fractal::enumObjectType object;
	bool maxiter;
};

double CalculateDistance(const cParamRender &params, const cFourFractals &four, const sDistanceIn &in, sDistanceOut *out);

#endif /* CALCULATE_DISTANCE_HPP_ */
