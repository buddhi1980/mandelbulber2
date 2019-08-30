/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2019 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * Stereoscopic renderimg
 */

float3 StereoCalcEyePosition(
	float3 camera, float3 direction, float3 top, float distance, int eye, bool swapped)
{
	if (swapped) distance *= -1.0f;
	float3 eyePosition;
	float3 sideVector = normalize(cross(direction, top));
	if (eye == 0)
	{
		eyePosition = camera + sideVector * distance;
	}
	else
	{
		eyePosition = camera - sideVector * distance;
	}
	return eyePosition;
}

void StereoViewVectorCorrection(float correction, const matrix33 *mRot, const matrix33 *mRotInv,
	int eye, bool swapped, float3 *viewVector)
{
	if (swapped) correction *= -1.0f;
	float3 viewVectorTemp = *viewVector;

#ifdef PERSP_THREE_POINT

	viewVectorTemp = Matrix33MulFloat3(*mRotInv, viewVectorTemp);
	float rxz2 = viewVectorTemp.x * viewVectorTemp.x + viewVectorTemp.z * viewVectorTemp.z;
	float3 viewVectorCorrection = 0.0f;
	if (rxz2 < 1.0f)
	{
		viewVectorCorrection.x = correction / 10.0f * sqrt(1.0f - rxz2);
	}

	if (eye == 0)
	{
		viewVectorTemp -= viewVectorCorrection;
		viewVectorTemp = normalize(viewVectorTemp);
		viewVectorTemp = Matrix33MulFloat3(*mRot, viewVectorTemp);
		*viewVector = viewVectorTemp;
	}
	else
	{
		viewVectorTemp += viewVectorCorrection;
		viewVectorTemp = normalize(viewVectorTemp);
		viewVectorTemp = Matrix33MulFloat3(*mRot, viewVectorTemp);
		*viewVector = viewVectorTemp;
	}

#else	// not PERSP_THREE_POINT

	viewVectorTemp = Matrix33MulFloat3(*mRotInv, viewVectorTemp);
	if (eye == 0)
	{
		viewVectorTemp = RotateAroundVectorByAngle(
			viewVectorTemp, (float3){0.0f, 0.0f, 1.0f}, correction / 10.0f / M_PI_F);
		viewVectorTemp = Matrix33MulFloat3(*mRot, viewVectorTemp);
		*viewVector = viewVectorTemp;
	}
	else
	{
		viewVectorTemp = RotateAroundVectorByAngle(
			viewVectorTemp, (float3){0.0f, 0.0f, 1.0f}, -correction / 10.0f / M_PI_F);
		viewVectorTemp = Matrix33MulFloat3(*mRot, viewVectorTemp);
		*viewVector = viewVectorTemp;
	}
#endif // PERSP_THREE_POINT
}

float2 StereoModifyImagePoint(float2 imagePoint)
{
	float2 newImagePoint = imagePoint;

#ifdef STEREO_LEFTRIGHT
	newImagePoint.x = fmod((imagePoint.x + 0.5f) * 2.0f, 1.0f) - 0.5f;
	newImagePoint.y = imagePoint.y;
#endif
#ifdef STEREO_TOPBOTTOM
	newImagePoint.y = -(fmod((-imagePoint.y + 0.5f) * 2.0f, 1.0f) - 0.5f);
	newImagePoint.x = imagePoint.x;
#endif

	return newImagePoint;
}

int StereoWhichEye(float2 imagePoint)
{
	int eye = 0;

#ifdef STEREO_LEFTRIGHT
	eye = imagePoint.x >= 0.0f ? 0 : 1;
#endif

#ifdef STEREO_TOPBOTTOM
	eye = imagePoint.y >= 0.0f ? 0 : 1;
#endif

	return eye;
}

float StereoModifyAspectRatio(float aspectRatio)
{
	float newAspectRatio = aspectRatio;

#ifdef STEREO_LEFTRIGHT
	newAspectRatio = aspectRatio * 0.5f;
#endif

#ifdef STEREO_TOPBOTTOM
	newAspectRatio = aspectRatio * 2.0f;
#endif

	return newAspectRatio;
}
