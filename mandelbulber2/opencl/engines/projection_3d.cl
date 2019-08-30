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
 * view vector projection based on perspective type
 * see also src/projection_3d.[hpp|cpp]
 */

inline float3 CalculateViewVector(float2 normalizedPoint, float fov)
{
	float3 viewVector;

#if defined(PERSP_FISH_EYE) || defined(PERSP_FISH_EYE_CUT)
	float2 v = normalizedPoint * M_PI_F;
	float r = length(v);
	if (r == 0.0f)
	{
		viewVector = 0.0f;
	}
	else
	{
		viewVector.x = v.x / r * sin(r * fov);
		viewVector.z = v.y / r * sin(r * fov);
		viewVector.y = cos(r * fov);
	}
	viewVector = normalize(viewVector);
#endif

#ifdef PERSP_EQUIRECTANGULAR
	float2 v = normalizedPoint * M_PI_F;
	viewVector.x = sin(fov * v.x) * cos(fov * v.y);
	viewVector.z = sin(fov * v.y);
	viewVector.y = cos(fov * v.x) * cos(fov * v.y);
	viewVector = normalize(viewVector);
#endif

#ifdef PERSP_THREE_POINT
	viewVector.x = normalizedPoint.x * fov;
	viewVector.y = 1.0f;
	viewVector.z = normalizedPoint.y * fov;
	viewVector = normalize(viewVector);
#endif
	return viewVector;
}
