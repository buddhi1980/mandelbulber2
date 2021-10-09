/*
 * manipulations.cpp
 *
 *  Created on: 9 paź 2021
 *      Author: krzysztof
 */

#include "manipulations.h"

#include "camera_target.hpp"
#include "parameters.hpp"
#include "camera_movement_modes.h"
#include "fractal_container.hpp"
#include "write_log.hpp"
#include "interface.hpp"

cManipulations::cManipulations(QObject *parent) : QObject(parent)
{
	// TODO Auto-generated constructor stub
}

cManipulations::~cManipulations()
{
	// TODO Auto-generated destructor stub
}

void cManipulations::AssignParameterContainers(
	std::shared_ptr<cParameterContainer> _params, std::shared_ptr<cFractalContainer> _fractalParams)
{
	params = _params;
	fractalParams = _fractalParams;
}

void cManipulations::MoveCamera(QString buttonName)
{
	using namespace cameraMovementEnums;

	WriteLog("cInterface::MoveCamera(QString buttonName): button: " + buttonName, 2);

	CVector3 camera = params->Get<CVector3>("camera");
	CVector3 target = params->Get<CVector3>("target");
	CVector3 topVector = params->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);

	bool legacyCoordinateSystem = params->Get<bool>("legacy_coordinate_system");
	double reverse = legacyCoordinateSystem ? -1.0 : 1.0;

	// get direction vector
	CVector3 direction;
	if (buttonName == "bu_move_left")
		direction = cameraTarget.GetRightVector() * (-1.0);
	else if (buttonName == "bu_move_right")
		direction = cameraTarget.GetRightVector() * (1.0);
	else if (buttonName == "bu_move_up")
		direction = cameraTarget.GetTopVector() * (1.0) * reverse;
	else if (buttonName == "bu_move_down")
		direction = cameraTarget.GetTopVector() * (-1.0) * reverse;
	else if (buttonName == "bu_move_forward")
		direction = cameraTarget.GetForwardVector() * (1.0);
	else if (buttonName == "bu_move_backward")
		direction = cameraTarget.GetForwardVector() * (-1.0);

	enumCameraMovementStepMode stepMode =
		enumCameraMovementStepMode(params->Get<int>("camera_absolute_distance_mode"));
	enumCameraMovementMode movementMode =
		enumCameraMovementMode(params->Get<int>("camera_movement_mode"));

	// movement step
	double step;
	if (stepMode == absolute)
	{
		step = params->Get<double>("camera_movement_step");
	}
	else
	{
		double relativeStep = params->Get<double>("camera_movement_step");

		CVector3 point;
		if (movementMode == moveTarget)
			point = target;
		else
			point = camera;

		double distance = cInterface::GetDistanceForPoint(point, params, fractalParams);

		step = relativeStep * distance;
	}

	// movement
	if (movementMode == moveCamera)
		camera += direction * step;
	else if (movementMode == moveTarget)
		target += direction * step;
	else if (movementMode == fixedDistance)
	{
		camera += direction * step;
		target += direction * step;
	}

	// put data to interface
	params->Set("camera", camera);
	params->Set("target", target);

	// recalculation of camera-target
	cCameraTarget::enumRotationMode rollMode =
		cCameraTarget::enumRotationMode(params->Get<int>("camera_straight_rotation"));
	if (movementMode == moveCamera)
		cameraTarget.SetCamera(camera, rollMode);
	else if (movementMode == moveTarget)
		cameraTarget.SetTarget(target, rollMode);
	else if (movementMode == fixedDistance)
		cameraTarget.SetCameraTargetTop(camera, target, topVector);

	topVector = cameraTarget.GetTopVector();
	params->Set("camera_top", topVector);
	CVector3 rotation = cameraTarget.GetRotation();
	params->Set("camera_rotation", rotation * (180.0 / M_PI));
	double dist = cameraTarget.GetDistance();
	params->Set("camera_distance_to_target", dist);
}

void cManipulations::RotateCamera(QString buttonName)
{
	using namespace cameraMovementEnums;

	WriteLog("cInterface::RotateCamera(QString buttonName): button: " + buttonName, 2);

	CVector3 camera = params->Get<CVector3>("camera");
	CVector3 target = params->Get<CVector3>("target");
	CVector3 topVector = params->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);
	double distance = cameraTarget.GetDistance();

	enumCameraRotationMode rotationMode =
		enumCameraRotationMode(params->Get<int>("camera_rotation_mode"));
	cCameraTarget::enumRotationMode rollMode =
		cCameraTarget::enumRotationMode(params->Get<int>("camera_straight_rotation"));

	bool legacyCoordinateSystem = params->Get<bool>("legacy_coordinate_system");
	double reverse = legacyCoordinateSystem ? -1.0 : 1.0;

	CVector3 rotationAxis;
	if (rollMode == cCameraTarget::constantRoll)
	{
		if (buttonName == "bu_rotate_left")
			rotationAxis = CVector3(0.0, 0.0, 1.0);
		else if (buttonName == "bu_rotate_right")
			rotationAxis = CVector3(0.0, 0.0, -1.0);
		else if (buttonName == "bu_rotate_up")
		{
			rotationAxis = CVector3(1.0 * reverse, 0.0, 0.0);
			rotationAxis = rotationAxis.RotateAroundVectorByAngle(
				CVector3(0.0, 0.0, 1.0), cameraTarget.GetRotation().x);
		}
		else if (buttonName == "bu_rotate_down")
		{
			rotationAxis = CVector3(-1.0 * reverse, 0.0, 0.0);
			rotationAxis = rotationAxis.RotateAroundVectorByAngle(
				CVector3(0.0, 0.0, 1.0), cameraTarget.GetRotation().x);
		}
		else if (buttonName == "bu_rotate_roll_left")
			rotationAxis = cameraTarget.GetForwardVector() * (-1.0) * reverse;
		else if (buttonName == "bu_rotate_roll_right")
			rotationAxis = cameraTarget.GetForwardVector() * (1.0) * reverse;
	}
	else
	{
		if (buttonName == "bu_rotate_left")
			rotationAxis = cameraTarget.GetTopVector() * (1.0);
		else if (buttonName == "bu_rotate_right")
			rotationAxis = cameraTarget.GetTopVector() * (-1.0);
		else if (buttonName == "bu_rotate_up")
			rotationAxis = cameraTarget.GetRightVector() * (1.0) * reverse;
		else if (buttonName == "bu_rotate_down")
			rotationAxis = cameraTarget.GetRightVector() * (-1.0) * reverse;
		else if (buttonName == "bu_rotate_roll_left")
			rotationAxis = cameraTarget.GetForwardVector() * (-1.0) * reverse;
		else if (buttonName == "bu_rotate_roll_right")
			rotationAxis = cameraTarget.GetForwardVector() * (1.0) * reverse;
	}

	if (rotationMode == rotateAroundTarget) rotationAxis *= -1.0;

	// rotation of vectors
	CVector3 forwardVector = cameraTarget.GetForwardVector();
	double rotationStep = params->Get<double>("camera_rotation_step") * (M_PI / 180.0);
	forwardVector = forwardVector.RotateAroundVectorByAngle(rotationAxis, rotationStep);
	topVector = topVector.RotateAroundVectorByAngle(rotationAxis, rotationStep);

	if (rotationMode == rotateCamera)
	{
		target = camera + forwardVector * distance;
	}
	else
	{
		camera = target - forwardVector * distance;
	}

	// recalculation of camera-target
	cameraTarget.SetCameraTargetTop(camera, target, topVector);

	params->Set("camera", camera);
	params->Set("target", target);
	params->Set("camera_top", topVector);
	CVector3 rotation = cameraTarget.GetRotation();
	params->Set("camera_rotation", rotation * (180.0 / M_PI));
	double dist = cameraTarget.GetDistance();
	params->Set("camera_distance_to_target", dist);
}

void cManipulations::CameraOrTargetEdited(QWidget *navigationWidget)
{
	WriteLog("cInterface::CameraOrTargetEdited(void)", 2);

	// get data from interface before synchronization
	CVector3 camera = params->Get<CVector3>("camera");
	CVector3 target = params->Get<CVector3>("target");
	CVector3 topVector = params->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);

	SynchronizeInterfaceWindow(navigationWidget, params, qInterface::read);

	camera = params->Get<CVector3>("camera");
	target = params->Get<CVector3>("target");

	// recalculation of camera-target
	cCameraTarget::enumRotationMode rollMode =
		cCameraTarget::enumRotationMode(params->Get<int>("camera_straight_rotation"));
	cameraTarget.SetCamera(camera, rollMode);
	cameraTarget.SetTarget(target, rollMode);

	topVector = cameraTarget.GetTopVector();
	params->Set("camera_top", topVector);
	CVector3 rotation = cameraTarget.GetRotation();
	params->Set("camera_rotation", rotation * (180.0 / M_PI));
	double dist = cameraTarget.GetDistance();
	params->Set("camera_distance_to_target", dist);

	SynchronizeInterfaceWindow(navigationWidget, params, qInterface::write);
}

void cManipulations::RotationEdited(QWidget *navigationWidget)
{
	using namespace cameraMovementEnums;

	WriteLog("cInterface::RotationEdited(void)", 2);
	// get data from interface before synchronization
	SynchronizeInterfaceWindow(navigationWidget, params, qInterface::read);
	CVector3 camera = params->Get<CVector3>("camera");
	CVector3 target = params->Get<CVector3>("target");
	CVector3 topVector = params->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);
	double distance = cameraTarget.GetDistance();
	CVector3 rotation = params->Get<CVector3>("camera_rotation") * (M_PI / 180.0);

	enumCameraRotationMode rotationMode =
		enumCameraRotationMode(params->Get<int>("camera_rotation_mode"));

	CVector3 forwardVector(0.0, 1.0, 0.0);
	forwardVector = forwardVector.RotateAroundVectorByAngle(CVector3(0.0, 1.0, 0.0), rotation.z);
	forwardVector = forwardVector.RotateAroundVectorByAngle(CVector3(1.0, 0.0, 0.0), rotation.y);
	forwardVector = forwardVector.RotateAroundVectorByAngle(CVector3(0.0, 0.0, 1.0), rotation.x);

	if (rotationMode == rotateCamera)
	{
		target = camera + forwardVector * distance;
	}
	else
	{
		camera = target - forwardVector * distance;
	}
	cameraTarget.SetCameraTargetRotation(camera, target, rotation.z);
	params->Set("camera", camera);
	params->Set("target", target);
	params->Set("camera_top", cameraTarget.GetTopVector());
	SynchronizeInterfaceWindow(navigationWidget, params, qInterface::write);
}

void cManipulations::CameraDistanceEdited(QWidget *navigationWidget)
{
	using namespace cameraMovementEnums;

	WriteLog("cInterface::CameraDistanceEdited()", 2);

	SynchronizeInterfaceWindow(navigationWidget, params, qInterface::read);
	CVector3 camera = params->Get<CVector3>("camera");
	CVector3 target = params->Get<CVector3>("target");
	CVector3 topVector = params->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);
	CVector3 forwardVector = cameraTarget.GetForwardVector();

	double distance = params->Get<double>("camera_distance_to_target");

	enumCameraMovementMode movementMode =
		enumCameraMovementMode(params->Get<int>("camera_movement_mode"));
	if (movementMode == moveTarget)
	{
		target = camera + forwardVector * distance;
	}
	else
	{
		camera = target - forwardVector * distance;
	}

	cameraTarget.SetCameraTargetTop(camera, target, topVector);
	params->Set("camera", camera);
	params->Set("target", target);
	params->Set("camera_top", cameraTarget.GetTopVector());
	SynchronizeInterfaceWindow(navigationWidget, params, qInterface::write);
}
