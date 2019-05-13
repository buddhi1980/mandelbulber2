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
 * calculation of fast ambient occlusion effect
 */

#ifdef AO_MODE_FAST
float3 FastAmbientOcclusion(__constant sClInConstants *consts, sRenderData *renderData,
	sShaderInputDataCl *input, sClCalcParams *calcParam)
{
	// reference Iñigo Quilez –iq/rgba:
	// http://www.iquilezles.org/www/material/nvscene2008/rwwtt.pdf
	float delta = input->distThresh;
	float aoTemp = 0;
	float quality = consts->params.ambientOcclusionQuality;
	float lastDist = 1e20f;
	for (int i = 1; i < quality * quality; i++)
	{
		float scan = i * i * delta;
		float3 pointTemp = input->point + input->normal * scan;

		calcParam->distThresh = input->distThresh;
		formulaOut outF;
		outF = CalculateDistance(consts, pointTemp, calcParam, renderData);
		float dist = outF.distance;

		if (dist > lastDist * 2.0f) dist = lastDist * 2.0f;
		lastDist = dist;
		aoTemp += 1.0f / pow(2.0f, i) * (scan - consts->params.ambientOcclusionFastTune * dist)
							/ input->distThresh;
	}
	float ao = 1.0f - 0.2f * aoTemp;
	if (ao < 0) ao = 0;
	float3 output = (float3){ao, ao, ao};
	return output;
}
#endif
