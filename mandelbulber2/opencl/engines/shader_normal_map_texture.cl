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
 * modification of surface normal vector by normal map texture
 */

#ifdef USE_TEXTURES
#ifdef USE_NORMAL_MAP_TEXTURE
float3 NormalMapShader(sShaderInputDataCl *input, sRenderData *renderData,
	__global sObjectDataCl *objectData, int textureIndex)
{
	if (textureIndex >= 0)
	{
		float3 textureVectorX = 0.0f;
		float3 textureVectorY = 0.0f;
		float2 texturePoint = TextureMapping(
			input->point, input->normal, objectData, input->material, &textureVectorX, &textureVectorY);

		texturePoint += (float2){0.5f, 0.5f};

		float3 n = input->normal;
		// tangent vectors:
		float3 t = normalize(cross(n, textureVectorX));
		float3 b = normalize(cross(n, textureVectorY));
		matrix33 tbn;
		tbn.m1 = b;
		tbn.m2 = t;
		tbn.m3 = n;
		tbn = TransposeMatrix(tbn);

		int2 textureSize = renderData->textureSizes[textureIndex];
		__global uchar4 *texture = renderData->textures[textureIndex];

		float3 texNormal;

		if (!input->material->normalMapTextureFromBumpmap)
		{
			float3 pixel =
				BicubicInterpolation(texturePoint.x, texturePoint.y, texture, textureSize.x, textureSize.y);

			texNormal = (float3){pixel.s0 * 2.0f - 1.0f, pixel.s1 * 2.0f - 1.0f, pixel.s2};
			texNormal.x *= -input->material->normalMapTextureHeight;
			texNormal.y *= -input->material->normalMapTextureHeight;
			if (input->material->normalMapTextureInvertGreen) texNormal.y *= -1.0f;

			texNormal = normalize(texNormal);
		}
#ifdef USE_NORMAL_MAP_BUMP_TEXTURE
		else
		{
			float m[3][3];
			for (int y = 0; y <= 2; y++)
			{
				for (int x = 0; x <= 2; x++)
				{
					int texturePointAddress =
						TexturePixelAddress(texturePoint, textureSize, (int2){x - 1, y - 1});
					uchar4 pixel = texture[texturePointAddress];
					m[x][y] = pixel.s0 / 256.0f;
				}
			}
			texNormal.x = input->material->normalMapTextureHeight
										* (m[2][2] - m[0][2] + 2.0f * (m[2][1] - m[0][1]) + m[2][0] - m[0][0]);
			texNormal.y = input->material->normalMapTextureHeight
										* (m[0][0] - m[0][2] + 2.0f * (m[1][0] - m[1][2]) + m[2][0] - m[2][2]);
			texNormal.z = 1.0f;
			texNormal = normalize(texNormal);
		}
#endif

		float3 normal = Matrix33MulFloat3(tbn, texNormal);
		return normalize(normal);
	}
	else
	{
		return input->normal;
	}
}
#endif // USE_NORMAL_MAP_TEXTURE
#endif // USE_TEXTURES
