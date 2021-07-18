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
 * HDR blur post effect
 */

//------------------ MAIN RENDER FUNCTION --------------------
kernel void PostFilter(__global float4 *inputImage, __global float4 *out, sParamsHDRBlurCl p)
{
	const unsigned int i = get_global_id(0);
	const int2 scr = (int2){i % p.width, i / p.width};

	const float2 scr_f = convert_float2(scr);

	const float blurSize = p.radius * (p.width + p.height) * 0.001f;
	const float blurSize2 = blurSize * blurSize;
	const int intBlurSize = (int)(blurSize + 1);

	const float limiter = p.intensity;

	float weight = 0.0f;
	int yStart = max(0, scr.y - intBlurSize);
	int yEnd = min(p.height - 1, scr.y + intBlurSize);

	float4 newPixel = 0.0f;

	for (int yy = yStart; yy < yEnd; yy++)
	{
		int xStart = max(0, scr.x - intBlurSize);
		int xEnd = min(p.width - 1, scr.x + intBlurSize);
		for (int xx = xStart; xx < xEnd; xx++)
		{
			float dx = scr.x - xx;
			float dy = scr.y - yy;
			float r2 = dx * dx + dy * dy;
			if (r2 < blurSize2)
			{
				float value = 1.0f / (r2 / (0.2f * blurSize) + limiter);
				// if(dx == 0 && dy == 0) value = 10.0;
				weight += value;

				int inBuffIndex = xx + yy * p.width;
				float4 oldPixel = clamp(inputImage[inBuffIndex], 0.0f, 100.0f);

				newPixel += oldPixel * value;
			}
		}
	}

	if (weight > 0.0f)
	{
		newPixel /= weight;
	}

	out[i] = newPixel;
}
