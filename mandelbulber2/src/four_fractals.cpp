/*
 * four_fractals.cpp
 *
 *  Created on: Apr 26, 2014
 *      Author: krzysztof
 */

#include "four_fractals.hpp"

cFourFractals::cFourFractals()
{
	fourFractals = new cFractal*[4];
	for(int i=0; i<4; i++)
		fourFractals[i] = NULL;
	DEType = fractal::deltaDE;
	maxN = 0;
	isHybrid = false;
}

cFourFractals::~cFourFractals()
{
	if (fourFractals)
	{
		for (int i = 0; i < 4; i++)
		{
			if(fourFractals[i])
			{
				delete fourFractals[i];
			}
		}
		//delete[] fourFractals;
	}
}

cFourFractals::cFourFractals(const cParameterContainer *par, const cParameterContainer *generalPar)
{
	fourFractals = new cFractal*[4];
	for (int i = 0; i < 4; i++)
	{
		fourFractals[i] = new cFractal(&par[i]);
		fourFractals[i]->formula = (fractal::enumFractalFormula) par[i].Get<int>("formula", i + 1);
	}

	maxN = generalPar->Get<int>("N");
	CreateSequence(par, generalPar);

	if (generalPar->Get<bool>("hybrid_fractal_enable"))
	{
		DEType = fractal::deltaDE;
	}
	else
	{
		fractal::enumFractalFormula formula = fourFractals[0]->formula;
		for (int i = 0; i < fractalList.size(); i++)
		{
			if (fractalList[i].internalID == formula)
			{
				DEType = fractalList[i].DEType;
			}
		}
	}
}

void cFourFractals::CreateSequence(const cParameterContainer *par, const cParameterContainer *generalPar)
{
	hybridSequence.clear();

	bool hybridFractalEnabled = generalPar->Get<bool>("hybrid_fractal_enable");
	if(hybridFractalEnabled) isHybrid = true;

	int fractalNo = 0;
	int counter = 0;

	int counts[4];
	for (int i = 0; i < 4; i++)
	{
		counts[i] = par[i].Get<int>("formula_iterations", i + 1);
	}

	for(int i = 0; i < maxN * 5; i++)
	{
		if(hybridFractalEnabled)
		{
			counter ++;

			int repeatCount = 0;
			while(fourFractals[fractalNo]->formula == fractal::none && repeatCount < 4)
			{
				fractalNo++;
				fractalNo = fractalNo % 4;
				repeatCount++;
			}
			hybridSequence.append(fractalNo);

			if(counter >= counts[fractalNo])
			{
				counter = 0;
				fractalNo++;
				fractalNo = fractalNo % 4;
			}
		}
		else
		{
			hybridSequence.append(0);
		}
	}
}

int cFourFractals::GetSequence(int i) const
{
	if (i < hybridSequence.size())
	{
		return hybridSequence[i];
	}
	else
	{
		return 0;
	}
}
