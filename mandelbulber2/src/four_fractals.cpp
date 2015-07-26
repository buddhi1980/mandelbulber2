/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cFourFractals - container for 4 elements of hybrid fractal
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "four_fractals.hpp"

cFourFractals::cFourFractals()
{
	fourFractals = new cFractal*[NUMBER_OF_FRACTALS];
	for(int i=0; i<NUMBER_OF_FRACTALS; i++)
	{
		fourFractals[i] = NULL;
		DEType[i] = fractal::deltaDE;
	}
	maxN = 0;
	isHybrid = false;
}

cFourFractals::~cFourFractals()
{
	if (fourFractals)
	{
		for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
		{
			if(fourFractals[i])
			{
				delete fourFractals[i];
			}
		}
		delete[] fourFractals;
	}
}

cFourFractals::cFourFractals(const cFractalContainer *par, const cParameterContainer *generalPar)
{
	fourFractals = new cFractal*[NUMBER_OF_FRACTALS];
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		fourFractals[i] = new cFractal(&par->at(i));
		fourFractals[i]->formula = (fractal::enumFractalFormula) generalPar->Get<int>("formula", i + 1);
		formulaWeight[i] = generalPar->Get<double>("formula_weight", i + 1);
	}

	maxN = generalPar->Get<int>("N");
	CreateSequence(generalPar);

	if (generalPar->Get<bool>("hybrid_fractal_enable"))
	{
		DEType[0] = fractal::deltaDE;
	}
	else
	{
		for(int f = 0; f < NUMBER_OF_FRACTALS; f++)
		{
			fractal::enumFractalFormula formula = fourFractals[f]->formula;
			for (int i = 0; i < fractalList.size(); i++)
			{
				if (fractalList[i].internalID == formula)
				{
					DEType[f] = fractalList[i].DEType;
				}
			}
		}
	}
}

void cFourFractals::CreateSequence(const cParameterContainer *generalPar)
{
	hybridSequence.clear();

	bool hybridFractalEnabled = generalPar->Get<bool>("hybrid_fractal_enable");
	if(hybridFractalEnabled) isHybrid = true;
	else isHybrid = false;

	int fractalNo = 0;
	int counter = 0;

	int counts[NUMBER_OF_FRACTALS];
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		counts[i] = generalPar->Get<int>("formula_iterations", i + 1);
	}

	for(int i = 0; i < maxN * 5; i++)
	{
		if(hybridFractalEnabled)
		{
			counter ++;

			int repeatCount = 0;
			while(fourFractals[fractalNo]->formula == fractal::none && repeatCount < NUMBER_OF_FRACTALS)
			{
				fractalNo++;
				fractalNo = fractalNo % NUMBER_OF_FRACTALS;
				repeatCount++;
			}
			hybridSequence.append(fractalNo);

			if(counter >= counts[fractalNo])
			{
				counter = 0;
				fractalNo++;
				fractalNo = fractalNo % NUMBER_OF_FRACTALS;
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

fractal::enumDEType cFourFractals::GetDEType(int formulaIndex) const
{
	fractal::enumDEType type = fractal::deltaDE;
	if(formulaIndex == -1)
	{
		type = DEType[0];
	}
	else
	{
		type = DEType[formulaIndex];
	}
	return type;
}
