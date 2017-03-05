/*
 * trace_behind.h
 *
 *  Created on: 5 mar 2017
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_TRACE_BEHIND_H_
#define MANDELBULBER2_SRC_TRACE_BEHIND_H_

double traceBehindFractal(cParameterContainer *params, cFractalContainer *fractals, double maxDist,
	CVector3 viewVector, double startingDepth, double resolution);

#endif /* MANDELBULBER2_SRC_TRACE_BEHIND_H_ */
