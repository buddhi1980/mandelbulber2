/*
 * camera_target.cpp
 *
 *  Created on: May 11, 2014
 *      Author: krzysztof
 */

#include "camera_target.hpp"
#include <QtCore>

cCameraTarget::cCameraTarget(CVector3 _camera, CVector3 _target, CVector3 _top)
{
	SetCameraTargetTop(_camera, _target, _top);
}

void cCameraTarget::SetCameraTargetRotation(CVector3 _camera, CVector3 _target, double _roll)
{
	camera = _camera;
	target = _target;

	forwardVector = (target - camera);
	forwardVector.Normalize();
	distance = (target - camera).Length();
	yaw = forwardVector.GetAlpha() - 0.5*M_PI;
	pitch = forwardVector.GetBeta();
	roll = _roll;

	//qDebug() << "yaw:" << yaw * 180.0/M_PI;
	//qDebug() << "pitch:" << pitch * 180.0/M_PI;
	//qDebug() << "roll:" << roll * 180.0/M_PI;

	//qDebug() << "viewVector" << forwardVector.x << forwardVector.y << forwardVector.z;
	topVector = CVector3(0.0 ,0.0, 1.0);
	topVector = topVector.RotateAroundVectorByAngle(CVector3(0.0, 1.0, 0.0), roll);
	//qDebug() << "top1" << topVector.x << topVector.y << topVector.z;
	topVector = topVector.RotateAroundVectorByAngle(CVector3(1.0, 0.0, 0.0), pitch);
	//qDebug() << "top2" << topVector.x << topVector.y << topVector.z;
	topVector = topVector.RotateAroundVectorByAngle(CVector3(0.0, 0.0, 1.0), yaw);
	//qDebug() << "top3" << topVector.x << topVector.y << topVector.z;

	rightVector = forwardVector.Cross(topVector);
}

void cCameraTarget::SetCameraTargetTop(CVector3 _camera, CVector3 _target, CVector3 _top)
{
	camera = _camera;
	target = _target;

	forwardVector = (target - camera);
	forwardVector.Normalize();
	distance = (target - camera).Length();
	yaw = forwardVector.GetAlpha() - 0.5*M_PI;
	pitch = forwardVector.GetBeta();

	CVector3 topVectorTemp = _top;
	topVectorTemp.Normalize();
	topVectorTemp = topVectorTemp.RotateAroundVectorByAngle(CVector3(0.0, 0.0, 1.0), -yaw);
	//qDebug() << "temp1" << topVectorTemp.x << topVectorTemp.y << topVectorTemp.z;
	topVectorTemp = topVectorTemp.RotateAroundVectorByAngle(CVector3(1.0, 0.0, 0.0), -pitch);
	//qDebug() << "temp2" << topVectorTemp.x << topVectorTemp.y << topVectorTemp.z;
	roll = -atan2(topVectorTemp.z, topVectorTemp.x) + 0.5 * M_PI;

	//qDebug() << "yaw:" << yaw * 180.0/M_PI;
	//qDebug() << "pitch:" << pitch * 180.0/M_PI;
	//qDebug() << "roll:" << roll * 180.0/M_PI;

	topVector = CVector3(0.0 ,0.0, 1.0);
	topVector = topVector.RotateAroundVectorByAngle(CVector3(0.0, 1.0, 0.0), roll);
	topVector = topVector.RotateAroundVectorByAngle(CVector3(1.0, 0.0, 0.0), pitch);
	topVector = topVector.RotateAroundVectorByAngle(CVector3(0.0, 0.0, 1.0), yaw);

	//qDebug() << "forward Vector" << forwardVector.x << forwardVector.y << forwardVector.z;
	//qDebug() << "top vector" << topVector.x << topVector.y << topVector.z;

	rightVector = forwardVector.Cross(topVector);
	//qDebug() << "right vector" << rightVector.x << rightVector.y << rightVector.z;
}

void cCameraTarget::SetCamera(CVector3 _camera, enumRotationMode mode)
{
	camera = _camera;
	if(mode == constantRoll)
	{
		SetCameraTargetRotation(camera, target, roll);
	}
	else
	{
		SetCameraTargetTop(camera, target, topVector);
	}
}

void cCameraTarget::SetTarget(CVector3 _target, enumRotationMode mode)
{
	target = _target;
	if(mode == constantRoll)
	{
		SetCameraTargetRotation(camera, target, roll);
	}
	else
	{
		SetCameraTargetTop(camera, target, topVector);
	}
}
