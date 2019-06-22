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
 * calculation light sources based on fractal orbit traps
 */

#ifdef FAKE_LIGHTS
float3 FakeLightsShader(__constant sClInConstants *consts, sShaderInputDataCl *input,
	sClCalcParams *calcParams, float3 surfaceColor, sClGradientsCollection *gradients,
	float3 *specularOut)
{
	float3 fakeLights;

	float delta = input->distThresh * consts->params.smoothness;

	formulaOut outF;

	outF = Fractal(consts, input->point, calcParams, calcModeOrbitTrap, NULL, -1);
	float rr = outF.orbitTrapR;
	float r = 1.0f / (rr + 1e-20f);

	float fakeLight = consts->params.fakeLightsIntensity / r;

	float3 out;
	calcParams->distThresh = input->distThresh;
	calcParams->detailSize = input->delta;

	outF = Fractal(
		consts, input->point + (float3){delta, 0.0f, 0.0f}, calcParams, calcModeOrbitTrap, NULL, -1);
	float rx = 1.0f / (outF.orbitTrapR + 1e-30f);

	outF = Fractal(
		consts, input->point + (float3){0.0f, delta, 0.0f}, calcParams, calcModeOrbitTrap, NULL, -1);
	float ry = 1.0f / (outF.orbitTrapR + 1e-30f);

	outF = Fractal(
		consts, input->point + (float3){0.0f, 0.0f, delta}, calcParams, calcModeOrbitTrap, NULL, -1);
	float rz = 1.0f / (outF.orbitTrapR + 1e-30f);

	float3 fakeLightNormal;
	fakeLightNormal.x = r - rx;
	fakeLightNormal.y = r - ry;
	fakeLightNormal.z = r - rz;

	fakeLightNormal = normalize(fakeLightNormal);

	float fakeLight2 = fakeLight * dot(input->normal, fakeLightNormal);
	if (fakeLight2 < 0.0f) fakeLight2 = 0.0f;

	fakeLights = fakeLight2 * consts->params.fakeLightsColor;

	float3 fakeSpec =
		SpecularHighlightCombined(input, calcParams, fakeLightNormal, surfaceColor, gradients);
	fakeSpec = fakeSpec * consts->params.fakeLightsColor / r;

	fakeSpec = 0.0f; // TODO to check why in CPU code it's zero
	*specularOut = fakeSpec;
	return fakeLights;
}
#endif // FAKE_LIGTS
