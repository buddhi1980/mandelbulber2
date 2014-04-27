/*
 * four_fractals.hpp
 *
 *  Created on: Apr 26, 2014
 *      Author: krzysztof
 */

#ifndef FOUR_FRACTALS_HPP_
#define FOUR_FRACTALS_HPP_

#include "fractal.h"
#include "parameters.hpp"

class cFourFractals
{
public:
	cFourFractals();
	~cFourFractals();
	cFourFractals(parameters::container *par);
	cFractal* GetFractal(int index) {return fourFractals[index];}
	cFractal **fourFractals;
};


#endif /* FOUR_FRACTALS_HPP_ */
