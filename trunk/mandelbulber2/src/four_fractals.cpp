/*
 * four_fractals.cpp
 *
 *  Created on: Apr 26, 2014
 *      Author: krzysztof
 */

#include "four_fractals.hpp"

cFourFractals::cFourFractals()
{
	for (int i=0; i<4; i++)
	{
		memset(&fourFractals[i], 0, sizeof(cFractal));
	}
}


