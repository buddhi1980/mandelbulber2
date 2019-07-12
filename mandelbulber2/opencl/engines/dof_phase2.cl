/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * DOF effect function optimized for opencl
 */

//------------------ MAIN RENDER FUNCTION --------------------
kernel void DOFPhase2(__global sSortedZBufferCl *sortedZBuffer, __global float4 *inImage,
	__global float4 *out, sParamsDOF params)
{
	const int index = get_global_id(0);
	int sortBufferSize = params.width * params.height;

	int ii = sortedZBuffer[sortBufferSize - index - 1].i;
	int x = ii % params.width;
	int y = ii / params.width;
	int2 scr = (int2){x, y};
	float z = sortedZBuffer[sortBufferSize - index - 1].z;

	if (z > 1e-8f)
	{
		float blur = fabs(z - params.neutral) / z * params.deep + 1.0f;
		if (blur > params.maxRadius) blur = params.maxRadius;
		int size = (int)blur;
		float4 center = inImage[x + y * params.width];
		float factor = (3.14f * (blur * blur - blur) + 1.0f) / params.blurOpacity;

		int2 scr2;

		for (scr2.y = y - size; scr2.y <= y + size; scr2.y++)
		{
			for (scr2.x = x - size; scr2.x <= x + size; scr2.x++)
			{
				if (scr2.x >= 0 && scr2.x < params.width && scr2.y >= 0 && scr2.y < params.height)
				{
					float2 d = convert_float2(scr - scr2);
					float r = length(d);
					if (blur > r)
					{
						float op = clamp(blur - r, 0.0f, 1.0f);
						op /= factor;
						op = min(1.0f, op);
						float opN = 1.0f - op;

						uint address = scr2.x + scr2.y * params.width;
						float4 old = out[address];
						out[address] = opN * old + op * center;
					}
				}
			}
		}
	}
}
