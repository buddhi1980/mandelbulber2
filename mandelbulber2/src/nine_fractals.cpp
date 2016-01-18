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

#include <algorithm>
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

	bool useDefaultBailout = generalPar->Get<bool>("use_default_bailout");
	double commonBailout = generalPar->Get<double>("bailout");
	isHybrid = generalPar->Get<bool>("hybrid_fractal_enable");
	bool isBoolean =  generalPar->Get<bool>("boolean_operators");
	double maxBailout = 0.0;

	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		fractals[i] = new cFractal(&par->at(i));
		fractals[i]->formula = (fractal::enumFractalFormula) generalPar->Get<int>("formula", i + 1);
		formulaWeight[i] = generalPar->Get<double>("formula_weight", i + 1);
		formulaStartIteriation[i] = generalPar->Get<int>("formula_start_iteration", i + 1);
		formulaStopIteration[i] = generalPar->Get<int>("formula_stop_iteration", i + 1);
		DEType[i] = fractal::deltaDEType;
		DEFunctionType[i] = fractal::logarithmicDEFunction;
		checkForBailout[i] = generalPar->Get<bool>("check_for_bailout", i + 1);

		//decide if use addition of C constant
		bool addc = false;
		if(fractalList[GetIndexOnFractalList(fractals[i]->formula)].cpixelAddition == fractal::cpixelAlreadyHas)
		{
			addc = false;
		}
		else
		{
			addc = !generalPar->Get<bool>("dont_add_c_constant", i + 1);
			if(fractalList[GetIndexOnFractalList(fractals[i]->formula)].cpixelAddition == fractal::cpixelDisabledByDefault)
				addc = !addc;
		}
		addCConstant[i] = addc;

		//defualt bailout or global one

		if(useDefaultBailout)
		{
			if(isHybrid)
				maxBailout = qMax(maxBailout, fractalList[GetIndexOnFractalList(fractals[i]->formula)].defaultBailout);
			else
				bailout[i] = fractalList[GetIndexOnFractalList(fractals[i]->formula)].defaultBailout;
		}
		else
		{
			bailout[i] = commonBailout;
		}

		//Julia parameters - local or global
		if (isBoolean)
		{
			juliaEnabled[i] = generalPar->Get<bool>("julia_mode", i + 1);
			juliaConstant[i] = generalPar->Get<CVector3>("julia_c", i + 1);
			constantMultiplier[i] = generalPar->Get<CVector3>("fractal_constant_factor", i + 1);
		}
		else
		{
			juliaEnabled[i] = generalPar->Get<bool>("julia_mode");
			juliaConstant[i] = generalPar->Get<CVector3>("julia_c");
			constantMultiplier[i] = generalPar->Get<CVector3>("fractal_constant_factor");
		}
	}

	//common bailout for all hybrid components
	if(isHybrid)
	{
		for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
		{
			bailout[i] = maxBailout;
		}
	}

	if((fractal::enumDEMethod)generalPar->Get<int>("delta_DE_method") == fractal::forceDeltaDEMethod)
		forceDeltaDE = true;
	else
		forceDeltaDE = false;

	optimizedDEType = fractal::withoutDEFunction;
	useOptimizedDE = false;

	maxN = generalPar->Get<int>("N");
	maxFractalIndex = 0;
	CreateSequence(generalPar);

	if (isHybrid || forceDeltaDE)
	{
		DEType[0] = fractal::deltaDEType;
		if((fractal::enumDEFunctionType)generalPar->Get<int>("delta_DE_function") == fractal::preferedDEfunction)
		{
			//finding preferred delta DE function
			int linearDECount = 0;
			int logarythmicDECount = 0;
			for (int f = 0; f < NUMBER_OF_FRACTALS; f++)
			{
				fractal::enumFractalFormula formula = fractals[f]->formula;
				int index = GetIndexOnFractalList(formula);

				//looking for the best DE function for DeltaDE mode

				fractal::enumDEFunctionType DEFunction = fractalList[index].DEFunctionType;
				if (DEFunction == fractal::logarithmicDEFunction)
				logarythmicDECount += counts[f];

				if (DEFunction == fractal::linearDEFunction)
				linearDECount += counts[f];

				//looking if it's possible to use analyticDEType
				if (!forceDeltaDE && fractalList[index].internalID != fractal::none)
				{
					if (optimizedDEType == fractal::withoutDEFunction)
					{
						optimizedDEType = DEFunction;
					}

					if ((optimizedDEType != DEFunction && DEFunction != fractal::withoutDEFunction)
							|| fractalList[index].DEType == fractal::deltaDEType)
					{
						optimizedDEType = fractal::preferedDEfunction;
					}

				}
			}
			if (linearDECount > logarythmicDECount)
			DEFunctionType[0] = fractal::linearDEFunction;
			else
			DEFunctionType[0] = fractal::logarithmicDEFunction;
		}
		else
		{
			DEFunctionType[0] = (fractal::enumDEFunctionType)generalPar->Get<int>("delta_DE_function");
		}

		//if it's possible to use analyticDEType then use optimized settings
		if(optimizedDEType == fractal::logarithmicDEFunction || optimizedDEType == fractal::linearDEFunction)
		{
			DEType[0] = fractal::analyticDEType;
			DEFunctionType[0] = optimizedDEType;
			useOptimizedDE = true;
		}
	}
	else
	{
		for (int f = 0; f < NUMBER_OF_FRACTALS; f++)
		{
			fractal::enumFractalFormula formula = fractals[f]->formula;
			int index = GetIndexOnFractalList(formula);

			DEType[f] = fractalList[index].DEType;
			DEFunctionType[f] = fractalList[index].DEFunctionType;
		}
	}
}

void cNineFractals::CreateSequence(const cParameterContainer *generalPar)
{
	hybridSequence.clear();

	int repeatFrom = generalPar->Get<int>("repeat_from");

	int fractalNo = 0;
	int counter = 0;


	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		counts[i] = generalPar->Get<int>("formula_iterations", i + 1);
	}

	for (int i = 0; i < maxN * 5; i++)
	{
		if (isHybrid)
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
			if(fractals[fractalNo]->formula != fractal::none && fractalNo > maxFractalIndex) maxFractalIndex = fractalNo;

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
	fractal::enumDEType type = fractal::deltaDEType;
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

fractal::enumDEFunctionType cNineFractals::GetDEFunctionType(int formulaIndex) const
{
	fractal::enumDEFunctionType type = fractal::logarithmicDEFunction;
	if (formulaIndex == -1)
	{
		type = DEFunctionType[0];
	}
	else
	{
		type = DEFunctionType[formulaIndex];
	}
	return type;
}

QString cNineFractals::GetDETypeString() const
{
	QString text;
	if(DEType[0] == fractal::analyticDEType)
	{
		text += "analytic";
	}
	else
	{
		text += "deltaDE";
	}

	if(DEFunctionType[0] == fractal::logarithmicDEFunction)
	{
		text += " logarithmic";
	}
	else
	{
		text += " linear";
	}
	return text;
}

int cNineFractals::GetIndexOnFractalList(fractal::enumFractalFormula formula)
{
	for (int i = 0; i < fractalList.size(); i++)
	{
		if (fractalList[i].internalID == formula)
		{
			return i;
		}
	}
	return 0;
}
