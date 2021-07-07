/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * chromatic aberration post effect
 */

float3 Hsv2rgb(float hue, float sat, float val)
{
	float3 rgb;
	float h = hue / 60.0f;
	int i = (int)h;
	float f = h - i;
	float p = val * (1.0f - sat);
	float q = val * (1.0f - (sat * f));
	float t = val * (1.0f - (sat * (1.0f - f)));
	switch (i)
	{
		case 0: rgb = (float3){val, t, p}; break;
		case 1: rgb = (float3){q, val, p}; break;
		case 2: rgb = (float3){p, val, t}; break;
		case 3: rgb = (float3){p, q, val}; break;
		case 4: rgb = (float3){t, p, val}; break;
		case 5: rgb = (float3){val, p, q}; break;
	}

	return rgb;
}

//------------------ MAIN RENDER FUNCTION --------------------
kernel void PostFilter(
	__global float4 *inputImage, __global float4 *out, sParamsChromaticAberrationCl p)
{
	const unsigned int i = get_global_id(0);
	const int2 scr = (int2){i % p.width, i / p.width};

	float2 scr_f = convert_float2(scr);
	float aspectRatio = (float)p.height / p.width;
	scr_f.x = (scr_f.x / p.width) - 0.5f;
	scr_f.y = ((scr_f.y / p.height) - 0.5f) * aspectRatio;

	const float blurSize = p.blurRadius * p.width * 0.002f * length(scr_f);
	float radialBlurSizeSp = p.aberrationIntensity * length(scr_f) * p.width * 0.007f;
	float radialBlurSizeScr = p.aberrationIntensity * p.width * 0.0003f;
	const int intBlurSize = (int)((blurSize) + radialBlurSizeSp);

	float3 weight = 0.0f;

	int yStart = max(0, scr.y - intBlurSize);
	int yEnd = min(p.height, scr.y + intBlurSize);

	float4 newPixel = 0.0f;

	for (int yy = yStart; yy <= yEnd; yy++)
	{
		int xStart = max(0, scr.x - intBlurSize);
		int xEnd = min(p.width, scr.x + intBlurSize);
		for (int xx = xStart; xx <= xEnd; xx++)
		{
			float2 d = (float2){scr.x - xx, scr.y - yy};
			float2 radialVector = normalize(scr_f);
			float radialBlurSize =
				radialBlurSizeSp
				* clamp((blurSize - fabs(radialVector.x * d.y - d.x * radialVector.y)), 0.0f, 1.0f);

			float radius = length(d);

			// anti-aliased circle
			float3 fWeight = clamp(radialBlurSize - radius, 0.0f, 1.0f);

			float colorSelector = 0.1f
														* (length(scr_f) - length(scr_f + d / p.width)
															 + 0.002f * p.aberrationIntensity * length(scr_f))
														* p.width;

			float reverser = (p.reverse) ? -1.0f : 1.0f;

			float3 colorWeight;
			colorWeight.s0 =
				clamp(1.0f - fabs(colorSelector - radialBlurSizeSp * 0.1f * reverser) / radialBlurSizeScr,
					0.0f, 1.0f);
			colorWeight.s1 = clamp(1.0f - fabs(colorSelector + 0.0f) / radialBlurSizeScr, 0.0f, 1.0f);
			colorWeight.s2 =
				clamp(1.0f - fabs(colorSelector + radialBlurSizeSp * 0.1f * reverser) / radialBlurSizeScr,
					0.0f, 1.0f);

			fWeight *= colorWeight;

			if (length(fWeight) > 0.0f)
			{
				weight += fWeight;

				int inBuffIndex = xx + yy * p.width;
				float4 oldPixel = clamp(inputImage[inBuffIndex], 0.0f, 100.0f);

				oldPixel.s0 *= fWeight.s0;
				oldPixel.s1 *= fWeight.s1;
				oldPixel.s2 *= fWeight.s2;

				newPixel += oldPixel;
			}
		}
	}

	if (weight.s0 > 0.0f) newPixel.s0 /= weight.s0;
	if (weight.s1 > 0.0f) newPixel.s1 /= weight.s1;
	if (weight.s2 > 0.0f) newPixel.s2 /= weight.s2;

	out[i] = newPixel;
}
