/*
 * calculation_mode.h
 *
 *  Created on: 29 paź 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_CALCULATION_MODE_H_
#define MANDELBULBER2_SRC_CALCULATION_MODE_H_

namespace fractal
{
enum enumCalculationMode
{
	calcModeNormal = 0,
	calcModeColouring = 1,
	calcModeFake_AO = 2,
	calcModeDeltaDE1 = 3,
	calcModeDeltaDE2 = 4,
	calcModeOrbitTrap = 5
};
};

#endif /* MANDELBULBER2_SRC_CALCULATION_MODE_H_ */
