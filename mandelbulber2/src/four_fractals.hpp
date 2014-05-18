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
#include "fractal_list.hpp"

class cFourFractals
{
public:
	cFourFractals();
	~cFourFractals();
	cFourFractals(const parameters::container *par);
	cFractal* GetFractal(int index) const {return fourFractals[index];}
	cFractal **fourFractals;
	fractal::enumDEType DEType;
};


#endif /* FOUR_FRACTALS_HPP_ */
