/*
 * fractal_coloring.hpp
 *
 *  Created on: 18 kwi 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_FRACTAL_COLORING_HPP_
#define MANDELBULBER2_SRC_FRACTAL_COLORING_HPP_

struct sFractalColoring
{
	enum enumFractalColoringAlgorithm
	{
		fractalColoringNone = -1,
		fractalColoringStandard = 0,
		fractalColoringZDotPoint = 1,
		fractalColoringSphere = 2,
		fractalColoringCross = 3,
		fractalColoringLine = 4
	};

	sFractalColoring()
	{
		sphereRadius = 0.0;
		coloringAlgorithm = fractalColoringNone;
	}

	CVector3 lineDirection;
	double sphereRadius;
	enumFractalColoringAlgorithm coloringAlgorithm;
};

#endif /* MANDELBULBER2_SRC_FRACTAL_COLORING_HPP_ */
