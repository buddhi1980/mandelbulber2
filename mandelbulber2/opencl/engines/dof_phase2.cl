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
// One work-item per pixel. It applies the blur entries of its tile in their sorted order, so
// every pixel is blended in the same order in every render (no work-items write the same pixel).
kernel void DOFPhase2(__global sDOFEntryCl *entries, __global int *tileStart,
	__global int *tileList, __global float4 *out, sParamsDOF params)
{
	const int2 scr2 = (int2){get_global_id(0), get_global_id(1)};
	if (scr2.x >= params.width || scr2.y >= params.height) return;

	const int tilesX = (params.width + DOF_PHASE2_TILE_SIZE - 1) / DOF_PHASE2_TILE_SIZE;
	const int tile = scr2.x / DOF_PHASE2_TILE_SIZE + (scr2.y / DOF_PHASE2_TILE_SIZE) * tilesX;
	const int listStart = tileStart[tile];
	const int listEnd = tileStart[tile + 1];

	const uint address = scr2.x + scr2.y * params.width;
	float4 pixel = out[address];

	for (int n = listStart; n < listEnd; n++)
	{
		sDOFEntryCl entry = entries[tileList[n]];
		int2 scr = (int2){entry.x, entry.y};
		int2 dInt = scr - scr2;
		if (abs(dInt.x) <= entry.size && abs(dInt.y) <= entry.size)
		{
			float2 d = convert_float2(dInt);
			float r = length(d);
			float blur = entry.blur;
			if (blur > r)
			{
				float4 center = (float4){entry.colorR, entry.colorG, entry.colorB, entry.alpha};
				float op = clamp(blur - r, 0.0f, 1.0f);
				op /= entry.factor;
				op = min(1.0f, op);
				float opN = 1.0f - op;
				pixel = opN * pixel + op * center;
			}
		}
	}

	out[address] = pixel;
}
