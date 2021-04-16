/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * cLights class - container for light source definitions
 */

#include "lights.hpp"

#include "calculate_distance.hpp"
#include "common_math.h"
#include "fractal_container.hpp"
#include "fractparams.hpp"
#include "global_data.hpp"
#include "light.h"
#include "nine_fractals.hpp"
#include "parameters.hpp"
#include "random.hpp"
#include "system_data.hpp"
#include "write_log.hpp"

cLights::cLights() : QObject()
{
	numberOfLights = 0;
	lightsReady = false;
	isAnyLight = false;
}

cLights::cLights(const std::shared_ptr<cParameterContainer> _params,
	const std::shared_ptr<cFractalContainer> _fractal, bool loadTextures, bool quiet,
	bool useNetRender)
		: QObject()
{
	numberOfLights = 0;
	lightsReady = false;
	isAnyLight = false;
	dummyLight = cLight();
	Set(_params, _fractal, loadTextures, quiet, useNetRender);
}

cLights::~cLights()
{
	// nothing to delete
}

void cLights::Set(const std::shared_ptr<cParameterContainer> _params,
	const std::shared_ptr<cFractalContainer> _fractal, bool loadTextures, bool quiet,
	bool useNetRender)
{
	WriteLog("Preparation of lights started", 2);
	// move parameters from containers to structures
	std::unique_ptr<const sParamRender> params(new sParamRender(_params));
	std::unique_ptr<const cNineFractals> fractals(new cNineFractals(_fractal, _params));

	lights.clear();
	numberOfLights = 0;
	isAnyLight = false;

	QList<QString> listOfParameters = _params->GetListOfParameters();
	for (auto &parameterName : listOfParameters)
	{
		const int lengthOfPrefix = 5;
		if (parameterName.leftRef(lengthOfPrefix) == "light")
		{
			int positionOfDash = parameterName.indexOf('_');
			int lightIndex =
				parameterName.midRef(lengthOfPrefix, positionOfDash - lengthOfPrefix).toInt();
			if (parameterName.midRef(positionOfDash + 1) == "is_defined")
			{
				lights.push_back(cLight(lightIndex, _params, loadTextures, quiet, useNetRender));
				isAnyLight = true;
				numberOfLights++;
			}
		}
	}

	int numberOfRandomLights = _params->Get<int>("random_lights_number");
	bool randomLightsEnabled = _params->Get<bool>("random_lights_group");

	// auto generated random lights
	if (randomLightsEnabled && numberOfRandomLights > 0)
	{
		numberOfLights += numberOfRandomLights;

		int randomSeed = _params->Get<int>("random_lights_random_seed");
		double maxDistanceFromFractal = _params->Get<double>("random_lights_max_distance_from_fractal");
		CVector3 randomLightsCenter = _params->Get<CVector3>("random_lights_distribution_center");
		double distributionRadius = _params->Get<double>("random_lights_distribution_radius");
		bool allLightsInOneColor = _params->Get<bool>("random_lights_one_color_enable");
		sRGBFloat commonColor = toRGBFloat(_params->Get<sRGB>("random_lights_color"));
		float lightsIntensity = _params->Get<double>("random_lights_intensity");
		float lightsSize = _params->Get<double>("random_lights_size");
		float lightsVisibility = _params->Get<double>("random_lights_visibility");
		float lightsSoftShadowCone = _params->Get<double>("random_lights_soft_shadow_cone");
		bool lightsPenetrating = _params->Get<bool>("random_lights_penetrating");
		bool lightsCastShadows = _params->Get<bool>("random_lights_cast_shadows");

		sDistanceOut distanceOut;
		cRandom random;
		random.Initialize(randomSeed);

		for (int i = 0; i < numberOfRandomLights; i++)
		{
			int trialNumber = 0;
			double radiusMultiplier = 1.0;
			double distance = 0;
			CVector3 position;

			// try random positioning of light, until distance to surface satisfies
			while (distance <= 0 || distance >= maxDistanceFromFractal * radiusMultiplier)
			{
				CVector3 rv;
				rv.x = random.DoubleRandom(-1.0, 1.0);
				rv.y = random.DoubleRandom(-1.0, 1.0);
				rv.z = random.DoubleRandom(-1.0, 1.0);
				position = randomLightsCenter + rv * distributionRadius * radiusMultiplier;

				sDistanceIn distanceIn(position, 0.0, false);
				distance = CalculateDistance(*params, *fractals, distanceIn, &distanceOut);

				trialNumber++;
				if (trialNumber % 100 == 0) radiusMultiplier *= 1.01;
				if (trialNumber > 100000) break;
			}

			sRGBFloat colour;
			if (allLightsInOneColor)
			{
				colour = commonColor;
			}
			else
			{
				colour = sRGBFloat(float(random.Random(20000, 100000, 1)),
					float(random.Random(20000, 100000, 1)), float(random.Random(20000, 100000, 1)));
				double convertColorRatio = 1.0 / dMax(colour.R, colour.G, colour.B);
				colour.R *= convertColorRatio;
				colour.G *= convertColorRatio;
				colour.B *= convertColorRatio;
			}

			double distanceLimited = max(0.1 * maxDistanceFromFractal, distance);
			double intensity = lightsIntensity * distanceLimited * distanceLimited;

			cLight newLight;

			newLight.position = position;
			newLight.color = colour;
			newLight.intensity = float(intensity);
			newLight.enabled = true;
			newLight.type = cLight::lightPoint;
			newLight.size = lightsSize;
			newLight.visibility = lightsVisibility;
			newLight.softShadowCone = lightsSoftShadowCone;
			newLight.penetrating = lightsPenetrating;
			newLight.castShadows = lightsCastShadows;
			newLight.decayFunction = cLight::lightDecay1R2;

			lights.push_back(newLight);

			isAnyLight = true;

			emit updateProgressAndStatus(QObject::tr("Positioning random lights"),
				QObject::tr("Positioned light %1 of %2")
					.arg(QString::number(i + 1), QString::number(numberOfRandomLights)),
				((i + 1.0) / numberOfRandomLights));
			// qDebug() << QString("Light no. %1: pos: %2, distance=%3").arg(QString::number(i),
			// position.Debug(), QString::number(distance));

			if (systemData.globalStopRequest) break;
		}
	}

	lightsReady = true;

	WriteLog("Preparation of lights finished", 2);
}

const cLight *cLights::GetLight(const int index) const
{
	if (lightsReady)
	{
		if (index < numberOfLights)
		{
			return &(lights[index]);
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
	return const_cast<cLight *>(&dummyLight);
}

QList<int> cLights::GetListOfLights(std::shared_ptr<cParameterContainer> params)
{
	QList<QString> listOfParameters = params->GetListOfParameters();

	static QList<int> listOfFoundLights;
	listOfFoundLights.clear();

	for (auto &parameterName : listOfParameters)
	{
		if (parameterName.left(5) == "light")
		{
			int positionOfDash = parameterName.indexOf('_');
			int lightIndex = parameterName.midRef(5, positionOfDash - 5).toInt();
			if (listOfFoundLights.indexOf(lightIndex) < 0)
			{
				listOfFoundLights.append(lightIndex);
			}
		}
	}
	return listOfFoundLights;
}
