/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * cRenderWorker::MainShadow method - calculates shadow from main light source
 */

#include "calculate_distance.hpp"
#include "fractparams.hpp"
#include "render_data.hpp"
#include "render_worker.hpp"

sRGBAfloat cRenderWorker::MainShadow(const sShaderInputData &input) const
{
	sRGBAfloat shadow(1.0, 1.0, 1.0, 1.0);

	// starting point
	CVector3 point2;

	double factor = input.delta / params->resolution;
	if (!params->penetratingLights) factor = params->viewDistanceMax;
	double dist;

	double DEFactor = params->DEFactor;
	if (params->iterFogEnabled || params->volumetricLightEnabled[0]) DEFactor = 1.0;

	// double start = input.delta;
	double start = input.distThresh;
	if (params->interiorMode) start = input.distThresh * DEFactor;

	double opacity;
	double shadowTemp = 1.0;
	double iterFogSum = 0.0f;

	double softRange = tan(params->shadowConeAngle / 180.0 * M_PI);
	double maxSoft = 0.0;

	const bool bSoft = !params->iterFogEnabled && !params->common.iterThreshMode
										 && !params->interiorMode && softRange > 0.0 && !params->monteCarloSoftShadows;

	CVector3 shadowVect = input.lightVect;
	if (params->DOFMonteCarlo && params->monteCarloSoftShadows)
	{
		CVector3 randomVector;
		randomVector.x = Random(10000) / 5000.0 - 1.0;
		randomVector.y = Random(10000) / 5000.0 - 1.0;
		randomVector.z = Random(10000) / 5000.0 - 1.0;
		double randomSphereRadius = pow(Random(10000) / 10000.0, 1.0 / 3.0);
		CVector3 randomSphere = randomVector * (softRange * randomSphereRadius / randomVector.Length());
		shadowVect += randomSphere;
	}

	for (double i = start; i < factor; i += dist * DEFactor)
	{
		point2 = input.point + shadowVect * i;

		double dist_thresh;
		if (params->iterFogEnabled || params->volumetricLightEnabled[0])
		{
			dist_thresh = CalcDistThresh(point2);
		}
		else
			dist_thresh = input.distThresh;

		sDistanceOut distanceOut;
		sDistanceIn distanceIn(point2, dist_thresh, false);
		dist = CalculateDistance(*params, *fractal, distanceIn, &distanceOut, data);
		data->statistics.totalNumberOfIterations += distanceOut.totalIters;

		bool limitsReached = false;
		if (params->limitsEnabled)
		{
			if (point2.x < params->limitMin.x || point2.x > params->limitMax.x
					|| point2.y < params->limitMin.y || point2.y > params->limitMax.y
					|| point2.z < params->limitMin.z || point2.z > params->limitMax.z)
			{
				limitsReached = true;
			}
		}

		if (bSoft && !limitsReached)
		{
			double angle = (dist - dist_thresh) / i;
			if (angle < 0) angle = 0;
			if (dist < dist_thresh) angle = 0;
			double softShadow = 1.0 - angle / softRange;
			if (params->penetratingLights) softShadow *= (factor - i) / factor;
			if (softShadow < 0) softShadow = 0;
			if (softShadow > maxSoft) maxSoft = softShadow;
		}

		if (params->iterFogEnabled)
		{
			opacity = IterOpacity(dist * DEFactor, distanceOut.iters, params->N,
				params->iterFogOpacityTrim, params->iterFogOpacityTrimHigh, params->iterFogOpacity);
			opacity *= (factor - i) / factor;
			opacity = qMin(opacity, 1.0);
			iterFogSum = opacity + (1.0 - opacity) * iterFogSum;
		}
		else
		{
			opacity = 0.0;
		}
		shadowTemp = 1.0 - iterFogSum;

		if (dist < dist_thresh || shadowTemp < 0.0)
		{
			shadowTemp -= (factor - i) / factor;
			if (!params->penetratingLights) shadowTemp = 0.0;
			if (shadowTemp < 0.0) shadowTemp = 0.0;
			break;
		}
	}
	if (!bSoft)
	{
		shadow.R = shadowTemp;
		shadow.G = shadowTemp;
		shadow.B = shadowTemp;
	}
	else
	{
		shadow.R = 1.0 - maxSoft;
		shadow.G = 1.0 - maxSoft;
		shadow.B = 1.0 - maxSoft;
	}
	return shadow;
}
