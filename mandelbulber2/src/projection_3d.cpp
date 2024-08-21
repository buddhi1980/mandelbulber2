/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-22 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * 3d projection functions - CalculateViewVector() and InvProjection3D()
 *
 * perspective based view vector calculation
 */

#include "projection_3d.hpp"

#include "common_math.h"

CVector3 CalculateViewVector(CVector2<double> normalizedPoint, double fov,
	params::enumPerspectiveType perspType, const CRotationMatrix &mRot)
{
	CVector3 viewVector;

	switch (perspType)
	{
		case params::perspFishEye:
		case params::perspFishEyeCut:
		{
			CVector2<double> v = normalizedPoint;
			double r = v.Length();
			if (r == 0.0)
			{
				viewVector.x = 0.0;
				viewVector.z = 0.0;
				viewVector.y = 1.0;
			}
			else
			{
				viewVector.x = v.x / r * sin(r * fov);
				viewVector.z = v.y / r * sin(r * fov);
				viewVector.y = cos(r * fov);
			}
			viewVector.Normalize();
			break;
		}
		case params::perspEquirectangular:
		{
			CVector2<double> v = normalizedPoint * 0.5;
			viewVector.x = sin(fov * v.x) * cos(fov * v.y);
			viewVector.z = sin(fov * v.y);
			viewVector.y = cos(fov * v.x) * cos(fov * v.y);
			viewVector.Normalize();
			break;
		}
		case params::perspThreePoint:
		{
			viewVector.x = normalizedPoint.x * fov;
			viewVector.y = 1.0;
			viewVector.z = normalizedPoint.y * fov;
			viewVector.Normalize();
			break;
		}
	}
	viewVector = mRot.RotateVector(viewVector);

	return viewVector;
}

CVector3 InvProjection3D(const CVector3 &point, const CVector3 &camera,
	const CRotationMatrix &mRotInv, params::enumPerspectiveType perspectiveType, double fov,
	double imgWidth, double imgHeight)
{
	CVector3 screenPoint;
	CVector3 baseZ(0.0, 1.0, 0.0);

	double aspectRatio = imgWidth / imgHeight;
	if (perspectiveType == params::perspEquirectangular) aspectRatio = 2.0;

	CVector3 viewVector = point - camera;
	viewVector = mRotInv.RotateVector(viewVector);

	double x, y, z;
	if (perspectiveType == params::perspFishEye)
	{
		z = viewVector.Length();
		if (viewVector.y < 0) z = -z;
		viewVector.Normalize();
		double r = sqrt(viewVector.x * viewVector.x + viewVector.z * viewVector.z);

		double r2 = asin(r) * 2.0;

		// TODO: to check if multiplying by 2.0 is correct
		if (perspectiveType == params::perspEquirectangular) r2 *= 2.0;

		x = (viewVector.x / fov) * r2 / r / 2.0;
		y = (viewVector.z / fov) * r2 / r / 2.0;
	}
	else
	{
		double length = viewVector.Length();

		viewVector.x /= viewVector.y;
		viewVector.z /= viewVector.y;

		x = viewVector.x / fov;
		y = viewVector.z / fov;
		z = length; // * sign(viewVector.y);

		if (viewVector.y < 0) z = -z;
	}
	screenPoint.x = (x / aspectRatio + 0.5) * imgWidth;
	screenPoint.y = (-y + 0.5) * imgHeight;
	screenPoint.z = z;

	return screenPoint;
}

double CalcFOV(double fovDeegres, params::enumPerspectiveType perspType)
{
	double fov = 0.0;
	switch (perspType)
	{
		case params::perspThreePoint:
		{
			fov = 2.0 * tan(fovDeegres / 360.0 * M_PI);
			break;
		}
		case params::perspFishEye:
		case params::perspEquirectangular:
		case params::perspFishEyeCut:
		{
			fov = fovDeegres / 180.0 * M_PI;
			break;
		}
	}
	return fov;
}
