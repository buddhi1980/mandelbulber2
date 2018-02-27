/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cCameraTarget class - manipulation of camera and target coordinates and rotation
 */

#ifndef MANDELBULBER2_SRC_CAMERA_TARGET_HPP_
#define MANDELBULBER2_SRC_CAMERA_TARGET_HPP_

#include "algebra.hpp"

class cCameraTarget
{
public:
	enum enumRotationMode
	{
		constantRoll,
		constantTop
	};

	cCameraTarget(CVector3 _camera, CVector3 _target, CVector3 _top);
	void SetCameraTargetRotation(CVector3 _camera, CVector3 _target, double roll);
	void SetCameraTargetTop(CVector3 _camera, CVector3 _target, CVector3 _top);
	void SetCamera(CVector3 _camera, enumRotationMode mode);
	void SetTarget(CVector3 _target, enumRotationMode mode);
	CVector3 GetCamera() const { return camera; }
	CVector3 GetTarget() const { return target; }
	CVector3 GetRotation() const { return CVector3(yaw, pitch, roll); }
	CVector3 GetForwardVector() const { return forwardVector; }
	CVector3 GetTopVector() const { return topVector; }
	CVector3 GetRightVector() const { return rightVector; }
	double GetDistance() const { return distance; }
	static double CorrectAngle(double angle) { return fmod(angle + 3 * M_PI, 2 * M_PI) - M_PI; }

private:
	CVector3 camera;
	CVector3 target;

	double yaw;
	double pitch;
	double roll;
	double distance;
	CVector3 forwardVector;
	CVector3 topVector;
	CVector3 rightVector;

	// probably should be implemented vector which will indicate actual top direction!
};

#endif /* MANDELBULBER2_SRC_CAMERA_TARGET_HPP_ */
