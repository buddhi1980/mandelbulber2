/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2020 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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

// Code based on https://github.com/Reputeless/PerlinNoise/blob/master/PerlinNoise.hpp
//----------------------------------------------------------------------------------------
//
//	siv::PerlinNoise
//	Perlin noise library for modern C++
//
//	Copyright (C) 2013-2020 Ryo Suzuki <reputeless@gmail.com>
//
//	Permission is hereby granted, free of charge, to any person obtaining a copy
//	of this software and associated documentation files(the "Software"), to deal
//	in the Software without restriction, including without limitation the rights
//	to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
//	copies of the Software, and to permit persons to whom the Software is
//	furnished to do so, subject to the following conditions :
//
//	The above copyright notice and this permission notice shall be included in
//	all copies or substantial portions of the Software.
//
//	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
//	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//	THE SOFTWARE.
//
//----------------------------------------------------------------------------------------

#ifdef CLOUDS

float PrelinFade(float t)
{
	return t * t * t * (t * (t * 6.0f - 15.0f) + 10.0f);
}

float PerlinLerp(float t, float a, float b)
{
	return a + t * (b - a);
}

float PerlinGrad(uchar hash, float x, float y, float z)
{
	const uchar h = hash & 15;
	const float u = h < 8 ? x : y;
	const float v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

float PerlinWeight(int octaves)
{
	float amp = 1.0f;
	float value = 0.0f;

	for (int i = 0; i < octaves; ++i)
	{
		value += amp;
		amp /= 2.0f;
	}

	return value;
}

///////////////////////////////////////
//
//	Noise [-1, 1]
//

float PerlinNoise3D(float x, float y, float z, __global uchar *p)
{
	int X = ((int)floor(x)) & 255;
	int Y = ((int)floor(y)) & 255;
	int Z = ((int)floor(z)) & 255;

	x -= floor(x);
	y -= floor(y);
	z -= floor(z);

	float u = PrelinFade(x);
	float v = PrelinFade(y);
	float w = PrelinFade(z);

	int A = p[X] + Y;
	int AA = p[A] + Z;
	int AB = p[A + 1] + Z;
	int B = p[X + 1] + Y;
	int BA = p[B] + Z;
	int BB = p[B + 1] + Z;

	return PerlinLerp(w,
		PerlinLerp(v, PerlinLerp(u, PerlinGrad(p[AA], x, y, z), PerlinGrad(p[BA], x - 1.0f, y, z)),
			PerlinLerp(u, PerlinGrad(p[AB], x, y - 1.0f, z), PerlinGrad(p[BB], x - 1.0f, y - 1.0f, z))),
		PerlinLerp(v,
			PerlinLerp(
				u, PerlinGrad(p[AA + 1], x, y, z - 1.0f), PerlinGrad(p[BA + 1], x - 1.0f, y, z - 1.0f)),
			PerlinLerp(u, PerlinGrad(p[AB + 1], x, y - 1.0f, z - 1.0f),
				PerlinGrad(p[BB + 1], x - 1.0f, y - 1.0f, z - 1.0f))));
}

///////////////////////////////////////
//
//	Accumulated octave noise
//	* Return value can be outside the range [-1, 1]
//

float AccumulatedOctavePerlinNoise3D(float x, float y, float z, int octaves, __global uchar *p)
{
	float result = 0.0f;
	float amp = 1.0f;

	for (int i = 0; i < octaves; ++i)
	{
		result += PerlinNoise3D(x, y, z, p) * amp;
		x *= 2.0f;
		y *= 2.0f;
		z *= 2.0f;
		amp /= 2.0f;
	}

	return result; // unnormalized
}

///////////////////////////////////////
//
//	Normalized octave noise [-1, 1]
//

float NormalizedOctavePerlinNoise3D(float x, float y, float z, int octaves, __global uchar *p)
{
	return AccumulatedOctavePerlinNoise3D(x, y, z, octaves, p) / PerlinWeight(octaves);
}

///////////////////////////////////////
//
//	Normalized octave noise [0, 1]
//

float NormalizedOctavePerlinNoise3D_0_1(float x, float y, float z, int octaves, __global uchar *p)
{
	return NormalizedOctavePerlinNoise3D(x, y, z, octaves, p) * 0.5f + 0.5f;
}

#endif // CLOUDS
