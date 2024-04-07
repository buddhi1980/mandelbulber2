/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * calculated ambient occlusion effect based on multiple rays and light map texture
 */

#ifdef AO_MODE_MULTIPLE_RAYS
float3 AmbientOcclusion(__constant sClInConstants *consts, sRenderData *renderData,
	sShaderInputDataCl *input, sClCalcParams *calcParam)
{
	float3 AO = 0.0f;

	float start_dist = input->distThresh;
	float end_dist = input->delta / consts->params.resolution;
	float intense;

#ifndef MONTE_CARLO
	for (int i = 0; i < renderData->AOVectorsCount; i++)
	{
#else
	int i = Random(renderData->AOVectorsCount - 1, &calcParam->randomSeed);
#endif
		sVectorsAroundCl v = renderData->AOVectors[i];

		float dist;

		float opacity = 0.0f;
		float shadowTemp = 1.0f;

		for (float r = start_dist; r < end_dist; r += dist * 2.0f)
		{
			float3 point2 = input->point + v.v * r;

			calcParam->distThresh = input->distThresh;
			formulaOut outF;
			outF = CalculateDistance(consts, point2, calcParam, renderData);
			dist = outF.distance;

#ifdef ITER_FOG
			opacity =
				IterOpacity(dist * 2.0f, outF.iters, consts->params.N, consts->params.iterFogOpacityTrim,
					consts->params.iterFogOpacityTrimHigh, consts->params.iterFogOpacity);
#endif

			// #if (defined(VOLUMETRIC_FOG) || defined(DIST_FOG_SHADOWS))
			//			if (consts->params.distanceFogShadows)
			//			{
			//				float distanceShifted;
			//				opacity += DistanceFogOpacity(dist * 2.0f, dist,
			// consts->params.volFogDistanceFromSurface, 					consts->params.volFogDistanceFactor,
			// consts->params.volFogDensity, &distanceShifted);
			//			}
			// #endif

			shadowTemp -= opacity * (end_dist - r) / end_dist;

			float dist_thresh;
			if (consts->params.iterFogEnabled)
			{
				dist_thresh = CalcDistThresh(point2, consts);
			}
			else
				dist_thresh = input->distThresh;

			if (dist < dist_thresh || shadowTemp < 0.0f)
			{
				shadowTemp -= (end_dist - r) / end_dist;
				if (shadowTemp < 0.0f) shadowTemp = 0.0f;
				break;
			}
		}

		intense = shadowTemp;

		AO += intense * v.color;

#ifndef MONTE_CARLO
	}
	AO /= renderData->AOVectorsCount;
#endif

	return AO;
}
#endif
