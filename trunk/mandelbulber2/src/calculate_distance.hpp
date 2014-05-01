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
};

struct sDistanceOut
{
	double distance;
	double colorIndex;
	int iters;
	fractal::enumObjectType object;
	bool maxiter;
};

#endif /* CALCULATE_DISTANCE_HPP_ */
