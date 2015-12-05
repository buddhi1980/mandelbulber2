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

#include "nine_fractals.hpp"

cNineFractals::cNineFractals()
{
	fractals = new cFractal*[NUMBER_OF_FRACTALS];
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		fractals[i] = NULL;
		DEType[i] = fractal::deltaDE;
	}
	maxN = 0;
	isHybrid = false;
	maxFractalIndex = 0;
}

cNineFractals::~cNineFractals()
{
	if (fractals)
	{
		for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
		{
			if (fractals[i])
			{
				delete fractals[i];
			}
		}
		delete[] fractals;
	}
}

cNineFractals::cNineFractals(const cFractalContainer *par, const cParameterContainer *generalPar)
{
	fractals = new cFractal*[NUMBER_OF_FRACTALS];
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		fractals[i] = new cFractal(&par->at(i));
		fractals[i]->formula = (fractal::enumFractalFormula) generalPar->Get<int>("formula", i + 1);
		formulaWeight[i] = generalPar->Get<double>("formula_weight", i + 1);
		formulaStartIteriation[i] = generalPar->Get<int>("formula_start_iteration", i + 1);
		formulaStopIteration[i] = generalPar->Get<int>("formula_stop_iteration", i + 1);
	}

	maxN = generalPar->Get<int>("N");
	maxFractalIndex = 0;
	CreateSequence(generalPar);

	if (generalPar->Get<bool>("hybrid_fractal_enable"))
	{
		DEType[0] = fractal::deltaDE;
	}
	else
	{
		for (int f = 0; f < NUMBER_OF_FRACTALS; f++)
		{
			fractal::enumFractalFormula formula = fractals[f]->formula;
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

void cNineFractals::CreateSequence(const cParameterContainer *generalPar)
{
	hybridSequence.clear();

	bool hybridFractalEnabled = generalPar->Get<bool>("hybrid_fractal_enable");
	if (hybridFractalEnabled) isHybrid = true;
	else isHybrid = false;

	int repeatFrom = generalPar->Get<int>("repeat_from");

	int fractalNo = 0;
	int counter = 0;

	int counts[NUMBER_OF_FRACTALS];
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		counts[i] = generalPar->Get<int>("formula_iterations", i + 1);
	}

	for (int i = 0; i < maxN * 5; i++)
	{
		if (hybridFractalEnabled)
		{
			counter++;

			int repeatCount = 0;
			while ((fractals[fractalNo]->formula == fractal::none || i < formulaStartIteriation[fractalNo]
					|| i > formulaStopIteration[fractalNo]) && repeatCount < NUMBER_OF_FRACTALS)
			{
				fractalNo++;
				if (fractalNo >= NUMBER_OF_FRACTALS) fractalNo = repeatFrom - 1;
				repeatCount++;
			}
			hybridSequence.append(fractalNo);
			if(fractalNo > maxFractalIndex) maxFractalIndex = fractalNo;

			if (counter >= counts[fractalNo])
			{
				counter = 0;
				fractalNo++;
				if(fractalNo >= NUMBER_OF_FRACTALS) fractalNo = repeatFrom - 1;
			}
		}
		else
		{
			hybridSequence.append(0);
		}
	}
}

int cNineFractals::GetSequence(int i) const
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

fractal::enumDEType cNineFractals::GetDEType(int formulaIndex) const
{
	fractal::enumDEType type = fractal::deltaDE;
	if (formulaIndex == -1)
	{
		type = DEType[0];
	}
	else
	{
		type = DEType[formulaIndex];
	}
	return type;
}
