/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Normal vector calculation for opencl
 */

float3 NormalVector(__constant sClInConstants *consts, sRenderData *renderData, float3 point,
	float mainDistance, float distThresh, float invertMode, sClCalcParams *calcParam)
{
#ifndef SLOW_SHADING
	float delta = distThresh * consts->params.smoothness;
#ifdef INTERIOR_MODE
	delta = calcParam->distThresh * 0.2f * consts->params.smoothness;
#endif

	calcParam->distThresh = distThresh;
	calcParam->detailSize = distThresh;
	calcParam->normalCalculationMode = true;

	float3 deltas[6];
	deltas[0] = (float3){delta, 0.0f, 0.0f};
	deltas[1] = (float3){-delta, 0.0f, 0.0f};
	deltas[2] = (float3){0.0f, delta, 0.0f};
	deltas[3] = (float3){0.0f, -delta, 0.0f};
	deltas[4] = (float3){0.0f, 0.0f, delta};
	deltas[5] = (float3){0.0f, 0.0f, -delta};

	float s[6];
	for (int i = 0; i < 6; i++)
	{
		s[i] = CalculateDistance(consts, point + deltas[i], calcParam, renderData).distance;
	}

	float3 normal = (float3){s[0] - s[1], s[2] - s[3], s[4] - s[5]};
	normal = normalize(normal);
	calcParam->normalCalculationMode = false;

	if (invertMode) normal *= -1.0f;

	return normal;
#else

	// calculating normal vector based on average value of binary central difference

	float3 point2;
	float3 point3;

	float delta = calcParam->detailSize * consts->params.smoothness;
#ifdef INTERIOR_MODE
	delta = calcParam->distThresh * 0.2f * consts->params.smoothness;
#endif

	formulaOut out;
	out.z = (float4)(0.0f, 0.0f, 0.0f, 0.0f);
	out.iters = 0;
	out.distance = 0.0f;
	out.colorIndex = 0.0f;
	out.orbitTrapR = 0.0f;
	out.maxiter = false;
	out.objectId = 0;

	float3 normal = 0.0f;
	for (point2.x = -1.0f; point2.x <= 1.0f; point2.x += 0.2f) //+0.2
	{
		for (point2.y = -1.0f; point2.y <= 1.0f; point2.y += 0.2f)
		{
			for (point2.z = -1.0f; point2.z <= 1.0f; point2.z += 0.2f)
			{
				point3 = point + point2 * delta;

				out = Fractal(consts, point3, calcParam, calcModeNormal, NULL, -1);
				float pseudoDistance = 1.0f + consts->params.N - out.iters;
				normal += point2 * pseudoDistance;
			}
		}
	}
	normal = normalize(normal);

	if (invertMode) normal *= -1.0f;

	return normal;
#endif
}
