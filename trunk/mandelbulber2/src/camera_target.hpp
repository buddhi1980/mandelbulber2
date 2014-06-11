/*
 * camera_target.hpp
 *
 *  Created on: May 10, 2014
 *      Author: krzysztof
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
	CVector3 camera;
	CVector3 target;

	double yaw;
	double pitch;
	double roll;
	double distance;
	CVector3 forwardVector;
	CVector3 topVector;
	CVector3 rightVector;

	//probably should be implemented vector wich will indicate actual top direction!
};



#endif /* CAMERA_TARGET_HPP_ */
