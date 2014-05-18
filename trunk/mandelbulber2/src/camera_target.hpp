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
	cCameraTarget(CVector3 _camera, CVector3 _target, CVector3 _rotation);
	void SetCameraTarget(CVector3 _camera, CVector3 _target, CVector3 _rotation);
	void SetCamera(CVector3 _camera);
	void SetTarget(CVector3 _target);
	void SetRotation(CVector3 _rot);
	CVector3 GetCamera() {return camera;}
	CVector3 GetTarget() {return target;};
	CVector3 GetRotation() {return CVector3(yaw, pitch, roll);}

private:
	CVector3 camera;
	CVector3 target;
	double yaw;
	double pitch;
	double roll;
	double distance;
	CVector3 viewVector;

	//probably should be implemented vector wich will indicate actual top direction!
};



#endif /* CAMERA_TARGET_HPP_ */
