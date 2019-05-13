/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <QtCore>

#include "cimage.hpp"
cStereo::cStereo()
{
	swapped = false;
	stereoMode = stereoDisabled;
	imageBuffer = nullptr;
	imageBufferWidth = 0;
	imageBufferHeight = 0;
	forceEye = eyeNone;
}

cStereo::~cStereo()
{
	if (imageBuffer)
	{
		delete[] imageBuffer;
		imageBuffer = nullptr;
	}
}

void cStereo::SetMode(enumStereoMode mode)
{
	stereoMode = mode;
}

CVector3 cStereo::CalcEyePosition(
	CVector3 camera, CVector3 direction, CVector3 top, double distance, enumEye eye) const
{
	if (swapped) distance *= -1.0;
	CVector3 eyePosition;
	CVector3 sideVector = direction.Cross(top);
	sideVector.Normalize();
	if (eye == eyeLeft)
	{
		eyePosition = camera + sideVector * distance;
	}
	else
	{
		eyePosition = camera - sideVector * distance;
	}
	return eyePosition;
}

sRGBFloat cStereo::MixColorsRedCyan(sRGBFloat left, sRGBFloat right)
{
	sRGBFloat color(right.R, left.G, left.B);
	return color;
}

sRGB16 cStereo::MixColorsRedCyan16(sRGB16 left, sRGB16 right)
{
	sRGB16 color(right.R, left.G, left.B);
	return color;
}

CVector2<int> cStereo::ModifyImageResolution(CVector2<int> resolution) const
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

CVector2<double> cStereo::ModifyImagePoint(CVector2<double> imagePoint) const
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
			newImagePoint.y = -(fmod((-imagePoint.y + 0.5) * 2.0, 1.0) - 0.5);
			newImagePoint.x = imagePoint.x;
			break;
		case stereoRedCyan: newImagePoint = imagePoint; break;
	}
	return newImagePoint;
}

cStereo::enumEye cStereo::WhichEye(CVector2<double> imagePoint) const
{
	enumEye eye = eyeLeft;
	switch (stereoMode)
	{
		case stereoDisabled: eye = eyeLeft; break;
		case stereoLeftRight: eye = imagePoint.x >= 0.0 ? eyeLeft : eyeRight; break;
		case stereoTopBottom: eye = imagePoint.y >= 0.0 ? eyeLeft : eyeRight; break;
		case stereoRedCyan: eye = eyeLeft; break;
	}
	return eye;
}

double cStereo::ModifyAspectRatio(double aspectRatio) const
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

int cStereo::GetNumberOfRepeats() const
{
	if (stereoMode == stereoRedCyan && forceEye == eyeNone)
	{
		return 2;
	}
	else
	{
		return 1;
	}
}

void cStereo::WhichEyeForAnaglyph(enumEye *eye, int repeat) const
{
	if (stereoMode == stereoRedCyan)
	{
		if (forceEye == eyeLeft)
			*eye = eyeLeft;
		else if (forceEye == eyeRight)
			*eye = eyeRight;
		else
			*eye = enumEye(repeat % 2);
	}
	// else do not modify eye selection
}

cRegion<int> cStereo::GetRegion(CVector2<int> imageResolution, enumEye eye) const
{
	cRegion<int> region;

	switch (stereoMode)
	{
		case stereoDisabled: region = cRegion<int>(0, 0, imageResolution.x, imageResolution.y); break;
		case stereoLeftRight:
			if (eye == eyeLeft)
				region = cRegion<int>(0, 0, imageResolution.x / 2, imageResolution.y);
			else
				region = cRegion<int>(imageResolution.x / 2, 0, imageResolution.x, imageResolution.y);
			break;

		case stereoTopBottom:
			if (eye == eyeLeft)
				region = cRegion<int>(0, 0, imageResolution.x, imageResolution.y / 2);
			else
				region = cRegion<int>(0, imageResolution.y / 2, imageResolution.x, imageResolution.y);
			break;
		case stereoRedCyan: region = cRegion<int>(0, 0, imageResolution.x, imageResolution.y); break;
	}
	return region;
}

void cStereo::ViewVectorCorrection(double correction, const CRotationMatrix &mRot,
	const CRotationMatrix &mRotInv, enumEye eye, params::enumPerspectiveType perspType,
	CVector3 *viewVector) const
{
	if (swapped) correction *= -1.0;
	CVector3 viewVectorTemp = *viewVector;

	if (perspType == params::perspThreePoint)
	{
		viewVectorTemp = mRotInv.RotateVector(viewVectorTemp);
		double rxz2 = viewVectorTemp.x * viewVectorTemp.x + viewVectorTemp.z * viewVectorTemp.z;
		CVector3 viewVectorCorrection;
		if (rxz2 < 1)
		{
			viewVectorCorrection.x = correction / 10.0 * sqrt(1.0 - rxz2);
		}

		if (eye == cStereo::eyeLeft)
		{
			viewVectorTemp -= viewVectorCorrection;
			viewVectorTemp.Normalize();
			viewVectorTemp = mRot.RotateVector(viewVectorTemp);
			*viewVector = viewVectorTemp;
		}
		else
		{
			viewVectorTemp += viewVectorCorrection;
			viewVectorTemp.Normalize();
			viewVectorTemp = mRot.RotateVector(viewVectorTemp);
			*viewVector = viewVectorTemp;
		}
	}
	else
	{
		viewVectorTemp = mRotInv.RotateVector(viewVectorTemp);
		if (eye == cStereo::eyeLeft)
		{
			viewVectorTemp =
				viewVectorTemp.RotateAroundVectorByAngle(CVector3(0.0, 0.0, 1.0), correction / 10.0 / M_PI);
			viewVectorTemp = mRot.RotateVector(viewVectorTemp);
			*viewVector = viewVectorTemp;
		}
		else
		{
			viewVectorTemp = viewVectorTemp.RotateAroundVectorByAngle(
				CVector3(0.0, 0.0, 1.0), -correction / 10.0 / M_PI);
			viewVectorTemp = mRot.RotateVector(viewVectorTemp);
			*viewVector = viewVectorTemp;
		}
	}
}

void cStereo::StoreImageInBuffer(cImage *image)
{
	int width = image->GetWidth();
	int height = image->GetHeight();
	unsigned int size = width * height;
	if (imageBuffer) delete[] imageBuffer;
	imageBuffer = new sRGB16[size];
	sRGB16 *image16Ptr = image->GetImage16Ptr();
	memcpy(imageBuffer, image16Ptr, size * sizeof(sRGB16));
	imageBufferWidth = width;
	imageBufferHeight = height;
}

void cStereo::MixImages(cImage *image) const
{
	int width = image->GetWidth();
	int height = image->GetHeight();
	if (width == imageBufferWidth && height == imageBufferHeight)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				sRGB16 pixel = image->GetPixelImage16(x, y);
				sRGB16 pixel2 = imageBuffer[x + y * width];
				sRGB16 newPixel = MixColorsRedCyan16(pixel2, pixel);
				image->PutPixelImage16(x, y, newPixel);
			}
		}
	}
	else
	{
		qCritical() << "cStereo::MixImages(cImage *image): image buffer size is different than cImage";
	}
}

void cStereo::ForceEye(enumEye eye)
{
	forceEye = eye;
}
