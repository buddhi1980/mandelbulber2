/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * distance calculation function for opencl
 */

#ifndef MANDELBULBER2_OPENCL_ENGINES_CALCULATE_DISTANCE_CL_
#define MANDELBULBER2_OPENCL_ENGINES_CALCULATE_DISTANCE_CL_

// calculation of distance where ray-marching stops
float CalcDistThresh(float3 point, __constant sClInConstants *consts)
{
	float distThresh;
	if (consts->params.constantDEThreshold)
		distThresh = consts->params.DEThresh;
	else
		distThresh = length(consts->params.camera - point) * consts->params.resolution
								 * consts->params.fov / consts->params.detailLevel;

#if defined(PERSP_FISH_EYE) || defined(PERSP_FISH_EYE_CUT) || defined(PERSP_EQUIRECTANGULAR)
	distThresh *= M_PI;
#endif

	distThresh = max(distThresh, 1e-6f);

	return distThresh;
}

// calculation of "voxel" size
float CalcDelta(float3 point, __constant sClInConstants *consts)
{
	float delta;
	delta = length(consts->params.camera - point) * consts->params.resolution * consts->params.fov;

#if defined(PERSP_FISH_EYE) || defined(PERSP_FISH_EYE_CUT) || defined(PERSP_EQUIRECTANGULAR)
	delta *= M_PI;
#endif

	delta = max(delta, 1e-6f);
	return delta;
}

formulaOut CalculateDistance(
	__constant sClInConstants *consts, float3 point, sClCalcParams *calcParam)
{
	formulaOut out;
	out.z = 0.0f;
	out.iters = 0;
	out.distance = 0.0f;
	out.colorIndex = 0.0f;
	out.maxiter = false;

	float limitBoxDist = 0.0f;

#ifdef LIMITS_ENABLED
	float3 boxDistance = max(point - consts->params.limitMax, -(point - consts->params.limitMin));
	limitBoxDist = max(max(boxDistance.x, boxDistance.y), boxDistance.z);

	if (limitBoxDist > calcParam->detailSize)
	{
		out.maxiter = false;
		out.distance = limitBoxDist;
		out.iters = 0;
		return out;
	}
#endif

#ifdef ANALYTIC_DE
	out = Fractal(consts, point, calcParam, calcModeNormal);
	bool maxiter = out.maxiter;

	if (maxiter) out.distance = 0.0f;

	if (out.iters < consts->params.minN && out.distance < calcParam->detailSize)
		out.distance = calcParam->detailSize;

#ifdef INTERIOR_MODE
	if (!calcParam->normalCalculationMode)
	{
		if (out.distance < 0.5f * calcParam->detailSize || maxiter)
		{
			out.distance = calcParam->detailSize;
			out.maxiter = false;
		}
	}
	else
	{
		if (out.distance < 0.9f * calcParam->detailSize)
		{
			out.distance = calcParam->detailSize - out.distance;
			out.maxiter = false;
		}
	}
#endif

	if (consts->params.common.iterThreshMode && !calcParam->normalCalculationMode && !maxiter)
	{
		if (out.distance < calcParam->detailSize)
		{
			out.distance = calcParam->detailSize * 1.01f;
		}
	}

	if (isinf(out.distance)) out.distance = 0.0f;
	if (out.distance < 0.0f) out.distance = 0.0f;
	if (out.distance > 1.0f) out.distance = 1.0f;

#elif DELTA_DE
	// float delta = length(point)*consts->fractal.deltaDEStep;
	float delta = 1.0e-5;
	float3 dr = 0.0;

	out = Fractal(consts, point, calcParam, calcModeDeltaDE1);
	calcParam->deltaDEMaxN = out.iters - 1;
	bool maxiter = out.maxiter;

	float r = length(out.z);
	float r11 =
		length(Fractal(consts, point + (float3){delta, 0.0f, 0.0f}, calcParam, calcModeDeltaDE2).z);
	float r12 =
		length(Fractal(consts, point + (float3){-delta, 0.0f, 0.0f}, calcParam, calcModeDeltaDE2).z);
	dr.x = min(fabs(r11 - r), fabs(r12 - r)) / delta;
	float r21 =
		length(Fractal(consts, point + (float3){0.0f, delta, 0.0f}, calcParam, calcModeDeltaDE2).z);
	float r22 =
		length(Fractal(consts, point + (float3){0.0f, -delta, 0.0f}, calcParam, calcModeDeltaDE2).z);
	dr.y = min(fabs(r21 - r), fabs(r22 - r)) / delta;
	float r31 =
		length(Fractal(consts, point + (float3){0.0f, 0.0f, delta}, calcParam, calcModeDeltaDE2).z);
	float r32 =
		length(Fractal(consts, point + (float3){0.0f, 0.0f, -delta}, calcParam, calcModeDeltaDE2).z);
	dr.z = min(fabs(r31 - r), fabs(r32 - r)) / delta;
	float d = length(dr);

	if (isinf(r) || isinf(d) || d == 0.0f)
	{
		out.distance = 0.0f;
	}
	else
	{
#ifdef DELTA_LINEAR_DE
		out.distance = 0.5f * r / d;
#elif DELTA_LOG_DE
		out.distance = 0.5f * r * native_log(r) / d;
#elif DELTA_PSEUDO_KLEINIAN_DE
		out.distance = .... // TODO to be written later
#endif
	}

	if (maxiter) out.distance = 0.0f;

	if (out.iters < consts->params.minN && out.distance < calcParam->detailSize)
		out.distance = calcParam->detailSize;

#ifdef INTERIOR_MODE
	if (!calcParam->normalCalculationMode)
	{
		if (out.distance < 0.5f * calcParam->detailSize || maxiter)
		{
			out.distance = calcParam->detailSize;
			out.maxiter = false;
		}
	}
	else
	{
		if (out.distance < 0.9f * calcParam->detailSize)
		{
			out.distance = calcParam->detailSize - out.distance;
			out.maxiter = false;
		}
	}
#endif

	if (consts->params.common.iterThreshMode && !calcParam->normalCalculationMode && !maxiter)
	{
		if (out.distance < calcParam->detailSize)
		{
			out.distance = calcParam->detailSize * 1.01f;
		}
	}

	if (out.distance < 0.0f) out.distance = 0.0f;
	if (out.distance > 1.0f) out.distance = 1.0f;

#endif // DELTA_DE

#ifdef LIMITS_ENABLED
	if (limitBoxDist < calcParam->detailSize)
	{
		out.distance = max(out.distance, limitBoxDist);
	}
#endif

	float distFromCamera = length(point - consts->params.camera);
	float distanceLimitMin = consts->params.viewDistanceMin - distFromCamera;
	out.distance = max(out.distance, distanceLimitMin);

	if (distanceLimitMin > calcParam->detailSize)
	{
		out.maxiter = false;
		out.iters = 0;
	}

	return out;
}

#endif // MANDELBULBER2_OPENCL_ENGINES_CALCULATE_DISTANCE_CL_
