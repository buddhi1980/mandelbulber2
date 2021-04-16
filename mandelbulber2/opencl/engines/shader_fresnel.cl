/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2021 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * calculate Fresnel equations for reflectance and refraction
 */

#ifdef USE_REFRACTION
float3 RefractVector(const float3 normal, const float3 incident, float n1, float n2)
{
	const float n = n1 / n2;
	const float cosI = -dot(normal, incident);
	const float sinT2 = n * n * (1.0f - cosI * cosI);
	if (sinT2 > 1.0f) return (float3){0.0f, 0.0f, 0.0f}; // total internal reflection
	const float cosT = sqrt(1.0f - sinT2);
	return incident * n + normal * (n * cosI - cosT);
}
#endif

#if defined(USE_REFRACTION) || defined(USE_REFLECTANCE) || defined(USE_ENV_MAPPING)
float Reflectance(const float3 normal, const float3 incident, float n1, float n2)
{
	const float n = n1 / n2;
	const float cosI = -dot(normal, incident);
	const float sinT2 = n * n * (1.0f - cosI * cosI);
	if (sinT2 > 1.0f) return 1.0f; // total internal reflection
	const float cosT = sqrt(1.0f - sinT2);
	const float r0rth = (n1 * cosI - n2 * cosT) / (n1 * cosI + n2 * cosT);
	const float rPar = (n2 * cosI - n1 * cosT) / (n2 * cosI + n1 * cosT);
	return (r0rth * r0rth + rPar * rPar) / 2.0f;
}
#endif