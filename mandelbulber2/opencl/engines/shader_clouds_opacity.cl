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
 * calculates opacity of clouds based on Perlin noise
 */

#ifdef CLOUDS
float CloudOpacity(float3 point, float distance) const
{
	float h = 1.0f; // height factor
	CVector3 point2;

	if (params->cloudsPlainShape)
	{
		point2 = params->mRotCloudsRotation.RotateVector(point - params->cloudsCenter);
		h = clamp(2.0f - abs(3.0f / params->cloudsHeight * (point2.z)), 0.0f, 1.0f);
	}
	else
	{
		h = 1.0f;
		point2 = point;
	}

	if (params->cloudsDistanceMode)
	{
		h *= clamp(2.0f - abs(3.0f / params->cloudsDistanceLayer * (distance - params->cloudsDistance)),
			0.0f, 1.0f);
	}

	if (h > 0.0f)
	{
		float opacity = NormalizedOctavePerlinNoise3D_0_1(point2.x / params->cloudsPeriod,
			point2.y / params->cloudsPeriod, point2.z / params->cloudsPeriod, params->cloudsIterations);
		opacity =
			clamp(opacity - 1.0f + params->cloudsDensity * 2.0f, 0.0f, 1.0f) * params->cloudsOpacity;

		opacity = opacity * h;
		return opacity;
	}
	else
	{
		return 0.0f;
	}
}

#endif // CLOUDS
