/*
 * fractal_enums.h
 *
 *  Created on: 30 paź 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_FRACTAL_ENUMS_H_
#define MANDELBULBER2_SRC_FRACTAL_ENUMS_H_

#define NUMBER_OF_FRACTALS 9

namespace fractal
{
enum enumDEMethod
{
	preferedDEMethod = 0,
	forceDeltaDEMethod = 1,
};

enum enumDEFunctionType
{
	preferedDEfunction = 0,
	linearDEFunction = 1,
	logarithmicDEFunction = 2,
	pseudoKleinianDEFunction = 3,
	numberOfDEFunctions = 3, // used to prepare array
	withoutDEFunction = 99
};

enum enumCPixelAddition
{
	cpixelEnabledByDefault = 0,
	cpixelDisabledByDefault = 1,
	cpixelAlreadyHas = 2
};

enum enumDEType
{
	analyticDEType,
	deltaDEType
};
};

#endif /* MANDELBULBER2_SRC_FRACTAL_ENUMS_H_ */
