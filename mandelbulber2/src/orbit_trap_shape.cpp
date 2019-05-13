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
 * OrbitTrapShapeDistance - Function to calculate distance from different
 * shapes of orbit traps
 */

#include "orbit_trap_shape.hpp"

using std::max;
using std::min;

double OrbitTrapShapeDistance(CVector4 z4, const sCommonParams &par)
{
	double dist = 0.0;
	CVector3 z = z4.GetXYZ();
	CVector3 delta = z - par.fakeLightsOrbitTrap;

	switch (par.fakeLightsOrbitTrapShape)
	{
		case params::fakeLightsShapePoint:
		{
			dist = delta.Length();
			break;
		}
		case params::fakeLightsShapeLine:
		{
			delta = par.mRotFakeLightsRotation.RotateVector(delta);
			dist = sqrt(delta.y * delta.y + delta.z * delta.z);
			break;
		}
		case params::fakeLightsShapeCircle:
		{
			delta = par.mRotFakeLightsRotation.RotateVector(delta);
			double lengthYZ = sqrt(delta.y * delta.y + delta.z * delta.z) - par.fakeLightsOrbitTrapSize;
			dist = sqrt(lengthYZ * lengthYZ + delta.x * delta.x);
			break;
		}
		case params::fakeLightsShapeSquare:
		{
			delta = par.mRotFakeLightsRotation.RotateVector(delta);

			double dy = max(fabs(fabs(delta.y) - par.fakeLightsOrbitTrapSize),
				fabs(delta.z) - par.fakeLightsOrbitTrapSize);
			double dz = max(fabs(fabs(delta.z) - par.fakeLightsOrbitTrapSize),
				fabs(delta.y) - par.fakeLightsOrbitTrapSize);
			double deltaRect = min(dy, dz);
			dist = sqrt(deltaRect * deltaRect + delta.x * delta.x);
			break;
		}
		case params::fakeLightsShapeSphere:
		{
			delta = par.mRotFakeLightsRotation.RotateVector(delta);

			double lengthYZ = sqrt(delta.y * delta.y + delta.z * delta.z) - par.fakeLightsOrbitTrapSize;
			double distYZ = sqrt(lengthYZ * lengthYZ + delta.x * delta.x);

			double lengthXZ = sqrt(delta.x * delta.x + delta.z * delta.z) - par.fakeLightsOrbitTrapSize;
			double distXZ = sqrt(lengthXZ * lengthXZ + delta.y * delta.y);

			double lengthXY = sqrt(delta.x * delta.x + delta.y * delta.y) - par.fakeLightsOrbitTrapSize;
			double distXY = sqrt(lengthXY * lengthXY + delta.z * delta.z);

			dist = min(min(distYZ, distXZ), distXY);
			break;
		}
		case params::fakeLightsShapeCube:
		{
			delta = par.mRotFakeLightsRotation.RotateVector(delta);

			double size = par.fakeLightsOrbitTrapSize;

			double dy_x = max(fabs(fabs(delta.y) - size), fabs(delta.z) - size);
			double dz_x = max(fabs(fabs(delta.z) - size), fabs(delta.y) - size);
			double dx_z = max(fabs(fabs(delta.y) - size), fabs(delta.x) - size);
			double deltaRectYZ = min(dy_x, dz_x);
			double distYZ = CVector2<double>(deltaRectYZ, fabs(fabs(delta.x) - size)).Length();
			double distXZ = CVector2<double>(dx_z, fabs(fabs(z.z) - size)).Length();

			dist = min(distYZ, distXZ);
			break;
		}
	}

	return dist / par.fakeLightsThickness;
}
