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
 * bicubic interpolation used for texture mapping
 */

#ifdef USE_TEXTURES

inline float3 cubicInterpolate(float3 p[4], float x)
{
	return p[1]
				 + 0.5f * x
						 * (p[2] - p[0]
								 + x
										 * (2.0f * p[0] - 5.0f * p[1] + 4.0f * p[2] - p[3]
												 + x * (3.0f * (p[1] - p[2]) + p[3] - p[0])));
}

#ifdef USE_DISPLACEMENT_TEXTURE
inline float cubicInterpolateGrey(float p[4], float x)
{
	return p[1]
				 + 0.5f * x
						 * (p[2] - p[0]
								 + x
										 * (2.0f * p[0] - 5.0f * p[1] + 4.0f * p[2] - p[3]
												 + x * (3.0f * (p[1] - p[2]) + p[3] - p[0])));
}
#endif

float3 bicubicInterpolate(float3 p[4][4], float x, float y)
{
	float3 yy[4];
	yy[0] = cubicInterpolate(p[0], y);
	yy[1] = cubicInterpolate(p[1], y);
	yy[2] = cubicInterpolate(p[2], y);
	yy[3] = cubicInterpolate(p[3], y);
	return cubicInterpolate(yy, x);
}

#ifdef USE_DISPLACEMENT_TEXTURE
float bicubicInterpolateGrey(float p[4][4], float x, float y)
{
	float yy[4];
	yy[0] = cubicInterpolateGrey(p[0], y);
	yy[1] = cubicInterpolateGrey(p[1], y);
	yy[2] = cubicInterpolateGrey(p[2], y);
	yy[3] = cubicInterpolateGrey(p[3], y);
	return cubicInterpolateGrey(yy, x);
}
#endif

float3 BicubicInterpolation(float x, float y, __global uchar4 *texture, int w, int h)
{
	if (x > 0.0f)
		x = fmod(x, 1.0f);
	else
		x = 1.0f + fmod(x, 1.0f);

	if (y > 0.0f)
		y = fmod(y, 1.0f);
	else
		y = 1.0f + fmod(y, 1.0f);

	float textureCordX = x * w;
	float textureCordY = y * h;

	int ix = (int)textureCordX;
	int iy = (int)textureCordY;
	float rx = textureCordX - ix;
	float ry = textureCordY - iy;

	float3 color[4][4];

	for (int yy = 0; yy < 4; yy++)
	{
		for (int xx = 0; xx < 4; xx++)
		{
			int texturePointAddress =
				TexturePixelAddressInt((int2){ix, iy}, (int2){w, h}, (int2){xx - 1, yy - 1});
			uchar4 pixel = texture[texturePointAddress];

			color[xx][yy] = (float3){pixel.s0 / 256.0f, pixel.s1 / 256.0f, pixel.s2 / 256.0f};
		}
	}

	float3 dColor = bicubicInterpolate(color, rx, ry);
	dColor = clamp(dColor, (float3){0.0f, 0.0f, 0.0f}, (float3){1.0f, 1.0f, 1.0f});

	return dColor;
}

#ifdef USE_DISPLACEMENT_TEXTURE
float BicubicInterpolationGrey16(float x, float y, __global uchar4 *texture, int w, int h)
{
	if (x > 0.0f)
		x = fmod(x, 1.0f);
	else
		x = 1.0f + fmod(x, 1.0f);

	if (y > 0.0f)
		y = fmod(y, 1.0f);
	else
		y = 1.0f + fmod(y, 1.0f);

	float textureCordX = x * w;
	float textureCordY = y * h;

	int ix = (int)textureCordX;
	int iy = (int)textureCordY;
	float rx = textureCordX - ix;
	float ry = textureCordY - iy;

	float grey[4][4];

	for (int yy = 0; yy < 4; yy++)
	{
		for (int xx = 0; xx < 4; xx++)
		{
			int texturePointAddress =
				TexturePixelAddressInt((int2){ix, iy}, (int2){w, h}, (int2){xx - 1, yy - 1});
			uchar4 pixel = texture[texturePointAddress];

			grey[xx][yy] = (pixel.s0 * 256 + pixel.s1) / 65536.0f;
		}
	}

	float dGrey = bicubicInterpolateGrey(grey, rx, ry);
	dGrey = clamp(dGrey, 0.0f, 1.0f);

	return dGrey;
}
#endif

#endif
