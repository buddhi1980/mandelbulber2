/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cCameraTarget class - manipulation of camera and target coordinates and rotation
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#ifndef CAMERA_TARGET_HPP_
#define CAMERA_TARGET_HPP_

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
	CVector3 GetCamera() {return camera;}
	CVector3 GetTarget() {return target;};
	CVector3 GetRotation() {return CVector3(yaw, pitch, roll);}
	CVector3 GetForwardVector() {return forwardVector;}
	CVector3 GetTopVector() {return topVector;}
	CVector3 GetRightVector() {return rightVector;}
	double GetDistance() {return distance;}

private:
	double CorrectAngle(double angle) {return fmod(angle + 3 * M_PI, 2 * M_PI) - M_PI;}

	CVector3 camera;
	CVector3 target;

	double yaw;
	double pitch;
	double roll;
	double distance;
	CVector3 forwardVector;
	CVector3 topVector;
	CVector3 rightVector;

	//probably should be implemented vector which will indicate actual top direction!
};



#endif /* CAMERA_TARGET_HPP_ */
