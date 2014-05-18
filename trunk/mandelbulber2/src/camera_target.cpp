/*
 * camera_target.cpp
 *
 *  Created on: May 11, 2014
 *      Author: krzysztof
 */

#include "camera_target.hpp"

cCameraTarget::cCameraTarget(CVector3 _camera, CVector3 _target, CVector3 _rotation)
{
	SetCameraTarget(_camera, _target, _rotation);
}

void cCameraTarget::SetCameraTarget(CVector3 _camera, CVector3 _target, CVector3 _rotation)
{
	camera = _camera;
	target = _target;

	viewVector = (target - camera);
	viewVector.Normalize();
	distance = (target - camera).Length();
	yaw = viewVector.GetAlpha();
	pitch = viewVector.GetBeta();
	roll = _rotation.z;
}

void cCameraTarget::SetCamera(CVector3 _camera)
{
	camera = _camera;
	SetCameraTarget(camera, target, CVector3(yaw, pitch, roll));
}

void cCameraTarget::SetTarget(CVector3 _target)
{
	target = _target;
	SetCameraTarget(camera, target, CVector3(yaw, pitch, roll));
}
