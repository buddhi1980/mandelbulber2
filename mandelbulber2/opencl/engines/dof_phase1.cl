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
kernel void DOFPhase1(
	__global float *zBuffer, __global float4 *inImage, __global float4 *out, sParamsDOF params)
{
	const int scrX = get_global_id(0);
	const int scrY = get_global_id(1);
	const int cl_offsetX = get_global_offset(0);
	const int cl_offsetY = get_global_offset(1);
	const int outBuffIndex = (scrX - cl_offsetX) + (scrY - cl_offsetY) * get_global_size(0);
	const int inBuffIndex = scrX + scrY * params.width;

	const int2 scr = (int2){scrX, scrY};

	float totalWeight = 0.0f;
	float4 tempPixel = 0.0f;

	float z = zBuffer[inBuffIndex];
	if (z > 1e-8f)
	{
		float blur1 = (z - params.neutral) / z * params.deep;
		float blur = fabs(blur1);
		if (blur > params.maxRadius) blur = params.maxRadius;
		int size = (int)blur;
		int xStart = max(scr.x - size, 0);
		int xStop = min(scr.x + size, params.width - 1);
		int yStart = max(scr.y - size, 0);
		int yStop = min(scr.y + size, params.height - 1);

		int2 point;
		for (point.y = yStart; point.y <= yStop; point.y++)
		{
			for (point.x = xStart; point.x <= xStop; point.x++)
			{
				int2 intDelta = scr - point;
				float2 delta = convert_float2(intDelta);
				float r = length(delta);
				float weight = blur - r;
				weight = clamp(weight, 0.0f, 1.0f);

				int inBuffIndex2 = point.x + point.y * params.width;
				float z2 = zBuffer[inBuffIndex2];
				float blur2 = (z2 - params.neutral) / z2 * params.deep;
				if (blur1 > blur2)
				{
					if (blur1 * blur2 < 0.0f)
					{
						weight = 0.0f;
					}
					else
					{
						float weight2 = 0.0f;
						if (blur1 > 0.0f)
							weight2 = 1.1f - blur1 / blur2;
						else
							weight2 = 1.1f - blur2 / blur1;
						if (weight2 < 0.0f) weight2 = 0.0f;
						weight *= weight2 * 10.0f;
					}
				}

				totalWeight += weight;
				if (weight > 0.0f)
				{
					float4 pix = inImage[inBuffIndex2];
					tempPixel += pix * weight;
				}
			}
		}
	}

	float4 newPixel;
	if (totalWeight > 0.0f)
	{
		newPixel = tempPixel / totalWeight;
	}
	else
	{
		newPixel = inImage[inBuffIndex];
	}

	out[outBuffIndex] = newPixel;
}
