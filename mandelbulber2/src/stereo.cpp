/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-16 Krzysztof Marczak     §R-==%w["'~5]m%=L.=~5N
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
 * cStereo class - to render stereoscopic images
 */

#include "stereo.h"

cStereo::cStereo()
{
	swapped = false;
	stereoMode = stereoDisabled;
}

cStereo::~cStereo()
{
	// TODO Auto-generated destructor stub
}

void cStereo::SetMode(enumStereoMode mode)
{
	stereoMode = mode;
}

CVector3 cStereo::CalcEyePosition(
	CVector3 camera, CVector3 direction, CVector3 top, double distance, enumEye eye)
{
	CVector3 eyePosition;
	CVector3 sideVector = direction.Cross(top);
	sideVector.Normalize();
	if(eye == eyeLeft)
	{
		eyePosition = camera + sideVector * distance;
	}
	else
	{
		eyePosition = camera - sideVector * distance;
	}
	return eyePosition;
}

sRGBfloat cStereo::MixColorsRedCyan(sRGBfloat left, sRGBfloat right)
{
	sRGBfloat color(left.R, right.G, right.B);
	return color;
}

CVector2<int> cStereo::ModifyImageResolution(CVector2<int> resolution)
{
	CVector2<int> newResolution;
	switch (stereoMode)
	{
		case stereoDisabled: newResolution = resolution; break;
		case stereoLeftRight:
			newResolution.x = resolution.x * 2;
			newResolution.y = resolution.y;
			break;
		case stereoTopBottom:
			newResolution.x = resolution.x;
			newResolution.y = resolution.y * 2;
			break;
		case stereoRedCyan: newResolution = resolution; break;
	}
	return newResolution;
}

CVector2<double> cStereo::ModifyImagePoint(CVector2<double> imagePoint)
{
	CVector2<double> newImagePoint;
	switch (stereoMode)
	{
		case stereoDisabled: newImagePoint = imagePoint; break;
		case stereoLeftRight:
			newImagePoint.x = fmod((imagePoint.x + 0.5) * 2.0, 1.0) - 0.5;
			newImagePoint.y = imagePoint.y;
			break;
		case stereoTopBottom:
			newImagePoint.y = fmod((imagePoint.y + 0.5) * 2.0, 1.0) - 0.5;
			newImagePoint.x = imagePoint.x;
			break;
		case stereoRedCyan: newImagePoint = imagePoint; break;
	}
	return newImagePoint;
}

cStereo::enumEye cStereo::WhichEye(CVector2<double> imagePoint)
{
	enumEye eye;
	switch (stereoMode)
	{
		case stereoDisabled: eye = eyeLeft; break;
		case stereoLeftRight: eye = (imagePoint.x >= 0.0) ? eyeRight : eyeLeft; break;
		case stereoTopBottom: eye = (imagePoint.y >= 0.0) ? eyeRight : eyeLeft; break;
		case stereoRedCyan:  eye = eyeLeft; break;
	}
	return eye;
}

double cStereo::ModifyAspectRatio(double aspectRatio)
{
	double newAspectRatio = 1.0;

	switch (stereoMode)
	{
		case stereoDisabled: newAspectRatio = aspectRatio; break;
		case stereoLeftRight: newAspectRatio = aspectRatio * 0.5; break;
		case stereoTopBottom: newAspectRatio = aspectRatio * 2.0; break;
		case stereoRedCyan: newAspectRatio = aspectRatio; break;
	}
	return newAspectRatio;
}

int cStereo::GetNumberOfRepeats()
{
	if(stereoMode == stereoRedCyan)
	{
		return 2;
	}
	else
	{
		return 1;
	}
}

void cStereo::WhichEyeForAnaglyph(enumEye *eye, int repeat)
{
	if (stereoMode == stereoRedCyan)
	{
		*eye = (enumEye)(repeat % 2);
	}
	//else do not modify eye selection
}
