/*
 * four_fractals.cpp
 *
 *  Created on: Apr 26, 2014
 *      Author: krzysztof
 */

#include "four_fractals.hpp"

cFourFractals::cFourFractals()
{
	fourFractals = NULL;
	DEType = fractal::deltaDE;
}

cFourFractals::~cFourFractals()
{
	if(fourFractals)
	{
		for(int i=0; i<4; i++)
		{
			delete fourFractals[i];
		}
	}
	delete[] fourFractals;
}

cFourFractals::cFourFractals(parameters::container *par)
{
	fourFractals = new cFractal*[4];
	for(int i = 0; i < 4; i++)
	{
		fourFractals[i] = new cFractal(&par[i]);
	}

	//temporary assing of DE type
	fractal::enumFractalFormula formula = fourFractals[0]->formula;
	for (int i = 0; i < fractalList.size(); i++)
	{
		if(fractalList[i].internalID == formula)
		{
			DEType = fractalList[i].DEType;
		}
	}

}
