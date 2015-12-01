/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cLights class - container for light source definitions
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

#include "lights.hpp"
#include <stdlib.h>
#include "calculate_distance.hpp"
#include "system.hpp"
#include "common_math.h"
#include "random.hpp"
#include "global_data.hpp"

cLights::cLights() : QObject()
{
	lights = NULL;
	numberOfLights = 0;
	lightsReady = false;
}

cLights::cLights(const cParameterContainer *_params, const cFractalContainer *_fractal) : QObject()
{
	lights = NULL;
	numberOfLights = 0;
	lightsReady = false;

	Set(_params, _fractal);
}

cLights::~cLights()
{
	if (lights)
	{
		delete[] lights;
		lights = NULL;
	}
}

void cLights::Set(const cParameterContainer *_params, const cFractalContainer *_fractal)
{
	WriteLog("Preparation of lights started");
	//move parameters from containers to structures
	const cParamRender *params = new cParamRender(_params);
	const cFourFractals *fourFractals = new cFourFractals(_fractal, _params);

	numberOfLights = params->auxLightNumber + params->auxLightRandomNumber;

	if (lights) delete[] lights;
	lights = new sLight[numberOfLights];

	// custom user defined lights
	if(params->auxLightNumber > 0)
	{
		for (int i = 0; i < params->auxLightNumber; i++)
		{
			if (params->auxLightPreEnabled[i])
			{
				lights[i].position = params->auxLightPre[i];
				lights[i].intensity = params->auxLightPreIntensity[i];
				lights[i].colour = params->auxLightPreColour[i];
				lights[i].enabled = true;
			}
			else
			{
				lights[i].position = CVector3(0.0,0.0,0.0);
				lights[i].intensity = 0.0;
				lights[i].colour = sRGB(0,0,0);
				lights[i].enabled = false;
			}
		}
	}

	// auto generated random lights
	if (params->auxLightRandomEnabled && params->auxLightRandomNumber > 0)
	{
		sDistanceOut distanceOut;
		cRandom random;
		random.Initialize(params->auxLightRandomSeed);

		for (int i = 0; i < params->auxLightRandomNumber; i++)
		{
			int trialNumber = 0;
			double radiusMultiplier = 1.0;
			double distance = 0;
			CVector3 position;

			gApplication->processEvents();
			// try random positioning of light, until distance to surface suffies
			while(distance <= 0 || distance >= params->auxLightRandomMaxDistanceFromFractal * radiusMultiplier)
			{
				CVector3 rv;
				rv.x = random.DoubleRandom(-1.0, 1.0, 1.0e-12);
				rv.y = random.DoubleRandom(-1.0, 1.0, 1.0e-12);
				rv.z = random.DoubleRandom(-1.0, 1.0, 1.0e-12);
				position = params->auxLightRandomCenter + rv * params->auxLightRandomRadius * radiusMultiplier;

				sDistanceIn distanceIn(position, 0.0, false);
				distance = CalculateDistance(*params, *fourFractals, distanceIn, &distanceOut);

				trialNumber++;
				if (trialNumber % 100 == 0) radiusMultiplier *= 1.01;
				if(trialNumber > 100000) break;
			}

			sRGB colour(random.Random(20000, 100000, 1), random.Random(20000, 100000, 1), random.Random(20000, 100000, 1));
			double convertColorRatio = 65536.0 / dMax(colour.R, colour.G, colour.B);
			colour.R *= convertColorRatio;
			colour.G *= convertColorRatio;
			colour.B *= convertColorRatio;

			lights[i + params->auxLightNumber].position = position;
			lights[i + params->auxLightNumber].colour = colour;
			lights[i + params->auxLightNumber].intensity = distance * distance / params->auxLightRandomMaxDistanceFromFractal;
			lights[i + params->auxLightNumber].enabled = true;

			emit updateProgressAndStatus(
						QObject::tr("Positioning random lights"),
						QObject::tr("Positioned light %1 of %2").arg(QString::number(i + 1), QString::number(params->auxLightRandomNumber)),
						((i + 1.0) / params->auxLightRandomNumber));
			// qDebug() << QString("Light no. %1: pos: %2, distance=%3").arg(QString::number(i), position.Debug(), QString::number(distance));
		}
	}

	lightsReady = true;

	delete params;
	delete fourFractals;

	WriteLog("Preparation of lights finished");
}

cLights::sLight cLights::GetLight(int index) const
{
	sLight light;
	if (lightsReady)
	{
		if (index < numberOfLights)
		{
			light = lights[index];
		}
		else
		{
			qCritical() << "Tried to get light of wrong index. Index:" << index;
		}
	}
	else
	{
		qCritical() << "Lights not initialized";
	}
	return light;
}

void cLights::Copy(const cLights& _lights)
{
	numberOfLights = _lights.numberOfLights;
	lightsReady = _lights.lightsReady;
	if (lights) delete[] lights;
	lights = new sLight[numberOfLights];

	for (int i = 0; i < numberOfLights; i++)
	{
		lights[i] = _lights.lights[i];
	}
}
