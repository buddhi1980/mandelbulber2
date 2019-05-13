/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * calculation of shadow from main light source
 */

#if defined(SHADOWS) || defined(VOLUMETRIC_LIGHTS)
float3 MainShadow(__constant sClInConstants *consts, sRenderData *renderData,
	sShaderInputDataCl *input, sClCalcParams *calcParam)
{
	float3 shadow = (float3){1.0f, 1.0f, 1.0f};

	// starting point
	float3 point2;

	float factor = input->delta / consts->params.resolution;
	if (!consts->params.penetratingLights) factor = consts->params.viewDistanceMax;
	float dist;

	float DEFactor = consts->params.DEFactor;
	if (consts->params.iterFogEnabled || consts->params.volumetricLightEnabled[0]) DEFactor = 1.0f;

	// float start = input->delta;
	float start = input->distThresh;
	if (consts->params.interiorMode) start = input->distThresh * DEFactor;

	float opacity;
	float shadowTemp = 1.0f;
	float iterFogSum = 0.0f;

	float softRange = tan(consts->params.shadowConeAngle / 180.0f * M_PI_F);
	float maxSoft = 0.0f;

	const bool bSoft = !consts->params.iterFogEnabled && !consts->params.common.iterThreshMode
										 && !consts->params.interiorMode && softRange > 0.0f
										 && !(consts->params.monteCarloSoftShadows && consts->params.DOFMonteCarlo);

	float3 shadowVect = input->lightVect;

#ifdef MC_SOFT_SHADOWS
	float3 randomVector;
	randomVector.x = Random(10000, &input->randomSeed) / 5000.0f - 1.0f;
	randomVector.y = Random(10000, &input->randomSeed) / 5000.0f - 1.0f;
	randomVector.z = Random(10000, &input->randomSeed) / 5000.0f - 1.0f;
	float randomSphereRadius = pow(Random(10000, &input->randomSeed) / 10000.0f, 1.0f / 3.0f);
	float3 randomSphere = randomVector * (softRange * randomSphereRadius / length(randomVector));
	shadowVect += randomSphere;
#endif // MC_SOFT_SHADOWS

	int count = 0;
	float step = 0.0f;
	for (float i = start; i < factor; i += step)
	{
		point2 = input->point + shadowVect * i;

		float dist_thresh;
		if (consts->params.iterFogEnabled || consts->params.volumetricLightEnabled[0])
		{
			dist_thresh = CalcDistThresh(point2, consts);
		}
		else
			dist_thresh = input->distThresh;

		calcParam->distThresh = dist_thresh;
		calcParam->detailSize = dist_thresh;
		formulaOut outF;

		outF = CalculateDistance(consts, point2, calcParam, renderData);
		dist = outF.distance;

		bool limitsAcheved = false;
#ifdef LIMITS_ENABLED
		limitsAcheved = any(isless(point2, consts->params.limitMin))
										|| any(isgreater(point2, consts->params.limitMax));
#endif // LIMITS_ENABLED

		if (bSoft && !limitsAcheved)
		{
			float angle = (dist - dist_thresh) / i;
			if (angle < 0.0f) angle = 0.0f;
			if (dist < dist_thresh) angle = 0;
			float softShadow = 1.0f - angle / softRange;
			if (consts->params.penetratingLights) softShadow *= (factor - i) / factor;
			if (softShadow < 0.0f) softShadow = 0.0f;
			if (softShadow > maxSoft) maxSoft = softShadow;
		}

#ifdef ITER_FOG
		opacity =
			IterOpacity(dist * DEFactor, outF.iters, consts->params.N, consts->params.iterFogOpacityTrim,
				consts->params.iterFogOpacityTrimHigh, consts->params.iterFogOpacity);
		opacity *= (factor - i) / factor;
		opacity = min(opacity, 1.0f);
		iterFogSum = opacity + (1.0f - opacity) * iterFogSum;
#else
		opacity = 0.0f;
#endif

		shadowTemp = 1.0f - iterFogSum;

		if (dist < dist_thresh || shadowTemp <= 0.0f)
		{
			shadowTemp -= (factor - i) / factor;
			if (!consts->params.penetratingLights) shadowTemp = 0.0f;
			if (shadowTemp < 0.0f) shadowTemp = 0.0f;
			break;
		}

		step = dist * DEFactor;
		step = max(step, 1e-6f);

		count++;
		if (count > MAX_RAYMARCHING) break;
	}
	if (!bSoft)
	{
		shadow.s0 = shadowTemp;
		shadow.s1 = shadowTemp;
		shadow.s2 = shadowTemp;
	}
	else
	{
		shadow.s0 = 1.0f - maxSoft;
		shadow.s1 = 1.0f - maxSoft;
		shadow.s2 = 1.0f - maxSoft;
	}
	return shadow;
}
#endif
