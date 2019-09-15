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
 * cRenderWorker::FakeLights method - calculates lights based on fractal orbit traps
 */
#include "compute_fractal.hpp"
#include "fractparams.hpp"
#include "render_worker.hpp"

sRGBAfloat cRenderWorker::FakeLights(
	const sShaderInputData &input, sRGBAfloat surfaceColor, sRGBAfloat *fakeSpec) const
{
	sRGBAfloat fakeLights;

	double delta = input.distThresh * params->smoothness;

	sFractalIn fractIn(input.point, params->minN, params->N, params->common, -1, false);
	sFractalOut fractOut;
	Compute<fractal::calcModeOrbitTrap>(*fractal, fractIn, &fractOut);
	double rr = fractOut.orbitTrapR;
	double r = 1.0 / (rr + 1e-30);

	double fakeLight = params->fakeLightsIntensity / r;

	CVector3 deltaX(delta, 0.0, 0.0);
	CVector3 deltaY(0.0, delta, 0.0);
	CVector3 deltaZ(0.0, 0.0, delta);

	fractIn.point = input.point + deltaX;
	Compute<fractal::calcModeOrbitTrap>(*fractal, fractIn, &fractOut);
	double rx = 1.0 / (fractOut.orbitTrapR + 1e-30);

	fractIn.point = input.point + deltaY;
	Compute<fractal::calcModeOrbitTrap>(*fractal, fractIn, &fractOut);
	double ry = 1.0 / (fractOut.orbitTrapR + 1e-30);

	fractIn.point = input.point + deltaZ;
	Compute<fractal::calcModeOrbitTrap>(*fractal, fractIn, &fractOut);
	double rz = 1.0 / (fractOut.orbitTrapR + 1e-30);

	CVector3 fakeLightNormal;
	fakeLightNormal.x = r - rx;
	fakeLightNormal.y = r - ry;
	fakeLightNormal.z = r - rz;

	if (qFuzzyIsNull(fakeLightNormal.x) && qFuzzyIsNull(fakeLightNormal.y)
			&& qFuzzyIsNull(fakeLightNormal.z))
	{
		fakeLightNormal.x = 0.0;
	}
	else
	{
		fakeLightNormal.Normalize();
	}
	float fakeLight2 = fakeLight * input.normal.Dot(fakeLightNormal);
	if (fakeLight2 < 0) fakeLight2 = 0;

	fakeLights.R = fakeLight2 * params->fakeLightsColor.R;
	fakeLights.G = fakeLight2 * params->fakeLightsColor.G;
	fakeLights.B = fakeLight2 * params->fakeLightsColor.B;

	sRGBAfloat fakeSpecular =
		SpecularHighlightCombined(input, fakeLightNormal, surfaceColor, sRGBFloat(1.0, 1.0, 1.0));

	fakeSpec->R = fakeSpecular.R / r * params->fakeLightsColor.R;
	fakeSpec->G = fakeSpecular.G / r * params->fakeLightsColor.G;
	fakeSpec->B = fakeSpecular.B / r * params->fakeLightsColor.B;

	*fakeSpec = sRGBAfloat();
	return fakeLights;
}
