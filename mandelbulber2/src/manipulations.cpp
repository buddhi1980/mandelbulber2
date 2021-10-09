/*
 * manipulations.cpp
 *
 *  Created on: 9 paź 2021
 *      Author: krzysztof
 */

#include "manipulations.h"

#include <memory>
#include "camera_target.hpp"
#include "parameters.hpp"
#include "camera_movement_modes.h"
#include "fractal_container.hpp"
#include "write_log.hpp"
#include "interface.hpp"
#include "projection_3d.hpp"
#include "rendered_image_widget.hpp"
#include "cimage.hpp"
#include "light.h"
#include "trace_behind.h"

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
	par = _params;
	parFractal = _fractalParams;
}

void cManipulations::AssingImage(std::shared_ptr<cImage> _image)
{
	image = _image;
}

void cManipulations::AssignRenderedImageWidget(RenderedImage *widget)
{
	renderedImageWidget = widget;
}

void cManipulations::MoveCamera(QString buttonName)
{
	using namespace cameraMovementEnums;

	WriteLog("cInterface::MoveCamera(QString buttonName): button: " + buttonName, 2);

	CVector3 camera = par->Get<CVector3>("camera");
	CVector3 target = par->Get<CVector3>("target");
	CVector3 topVector = par->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);

	bool legacyCoordinateSystem = par->Get<bool>("legacy_coordinate_system");
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
		enumCameraMovementStepMode(par->Get<int>("camera_absolute_distance_mode"));
	enumCameraMovementMode movementMode =
		enumCameraMovementMode(par->Get<int>("camera_movement_mode"));

	// movement step
	double step;
	if (stepMode == absolute)
	{
		step = par->Get<double>("camera_movement_step");
	}
	else
	{
		double relativeStep = par->Get<double>("camera_movement_step");

		CVector3 point;
		if (movementMode == moveTarget)
			point = target;
		else
			point = camera;

		double distance = cInterface::GetDistanceForPoint(point, par, parFractal);

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
	par->Set("camera", camera);
	par->Set("target", target);

	// recalculation of camera-target
	cCameraTarget::enumRotationMode rollMode =
		cCameraTarget::enumRotationMode(par->Get<int>("camera_straight_rotation"));
	if (movementMode == moveCamera)
		cameraTarget.SetCamera(camera, rollMode);
	else if (movementMode == moveTarget)
		cameraTarget.SetTarget(target, rollMode);
	else if (movementMode == fixedDistance)
		cameraTarget.SetCameraTargetTop(camera, target, topVector);

	topVector = cameraTarget.GetTopVector();
	par->Set("camera_top", topVector);
	CVector3 rotation = cameraTarget.GetRotation();
	par->Set("camera_rotation", rotation * (180.0 / M_PI));
	double dist = cameraTarget.GetDistance();
	par->Set("camera_distance_to_target", dist);
}

void cManipulations::RotateCamera(QString buttonName)
{
	using namespace cameraMovementEnums;

	WriteLog("cInterface::RotateCamera(QString buttonName): button: " + buttonName, 2);

	CVector3 camera = par->Get<CVector3>("camera");
	CVector3 target = par->Get<CVector3>("target");
	CVector3 topVector = par->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);
	double distance = cameraTarget.GetDistance();

	enumCameraRotationMode rotationMode =
		enumCameraRotationMode(par->Get<int>("camera_rotation_mode"));
	cCameraTarget::enumRotationMode rollMode =
		cCameraTarget::enumRotationMode(par->Get<int>("camera_straight_rotation"));

	bool legacyCoordinateSystem = par->Get<bool>("legacy_coordinate_system");
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
	double rotationStep = par->Get<double>("camera_rotation_step") * (M_PI / 180.0);
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

	par->Set("camera", camera);
	par->Set("target", target);
	par->Set("camera_top", topVector);
	CVector3 rotation = cameraTarget.GetRotation();
	par->Set("camera_rotation", rotation * (180.0 / M_PI));
	double dist = cameraTarget.GetDistance();
	par->Set("camera_distance_to_target", dist);
}

void cManipulations::CameraOrTargetEdited(QWidget *navigationWidget)
{
	WriteLog("cInterface::CameraOrTargetEdited(void)", 2);

	// get data from interface before synchronization
	CVector3 camera = par->Get<CVector3>("camera");
	CVector3 target = par->Get<CVector3>("target");
	CVector3 topVector = par->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);

	SynchronizeInterfaceWindow(navigationWidget, par, qInterface::read);

	camera = par->Get<CVector3>("camera");
	target = par->Get<CVector3>("target");

	// recalculation of camera-target
	cCameraTarget::enumRotationMode rollMode =
		cCameraTarget::enumRotationMode(par->Get<int>("camera_straight_rotation"));
	cameraTarget.SetCamera(camera, rollMode);
	cameraTarget.SetTarget(target, rollMode);

	topVector = cameraTarget.GetTopVector();
	par->Set("camera_top", topVector);
	CVector3 rotation = cameraTarget.GetRotation();
	par->Set("camera_rotation", rotation * (180.0 / M_PI));
	double dist = cameraTarget.GetDistance();
	par->Set("camera_distance_to_target", dist);

	SynchronizeInterfaceWindow(navigationWidget, par, qInterface::write);
}

void cManipulations::RotationEdited(QWidget *navigationWidget)
{
	using namespace cameraMovementEnums;

	WriteLog("cInterface::RotationEdited(void)", 2);
	// get data from interface before synchronization
	SynchronizeInterfaceWindow(navigationWidget, par, qInterface::read);
	CVector3 camera = par->Get<CVector3>("camera");
	CVector3 target = par->Get<CVector3>("target");
	CVector3 topVector = par->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);
	double distance = cameraTarget.GetDistance();
	CVector3 rotation = par->Get<CVector3>("camera_rotation") * (M_PI / 180.0);

	enumCameraRotationMode rotationMode =
		enumCameraRotationMode(par->Get<int>("camera_rotation_mode"));

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
	par->Set("camera", camera);
	par->Set("target", target);
	par->Set("camera_top", cameraTarget.GetTopVector());
	SynchronizeInterfaceWindow(navigationWidget, par, qInterface::write);
}

void cManipulations::CameraDistanceEdited(QWidget *navigationWidget)
{
	using namespace cameraMovementEnums;

	WriteLog("cInterface::CameraDistanceEdited()", 2);

	SynchronizeInterfaceWindow(navigationWidget, par, qInterface::read);
	CVector3 camera = par->Get<CVector3>("camera");
	CVector3 target = par->Get<CVector3>("target");
	CVector3 topVector = par->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);
	CVector3 forwardVector = cameraTarget.GetForwardVector();

	double distance = par->Get<double>("camera_distance_to_target");

	enumCameraMovementMode movementMode =
		enumCameraMovementMode(par->Get<int>("camera_movement_mode"));
	if (movementMode == moveTarget)
	{
		target = camera + forwardVector * distance;
	}
	else
	{
		camera = target - forwardVector * distance;
	}

	cameraTarget.SetCameraTargetTop(camera, target, topVector);
	par->Set("camera", camera);
	par->Set("target", target);
	par->Set("camera_top", cameraTarget.GetTopVector());
	SynchronizeInterfaceWindow(navigationWidget, par, qInterface::write);
}

void cManipulations::SetByMouse(QWidget *navigationWidget, CVector2<double> screenPoint,
	Qt::MouseButton button, const QList<QVariant> &mode)
{
	using namespace cameraMovementEnums;

	WriteLog(
		QString("MoveCameraByMouse(CVector2<double> screenPoint, Qt::MouseButton button): button: ")
			+ QString::number(int(button)),
		2);
	// get data from interface

	RenderedImage::enumClickMode clickMode = RenderedImage::enumClickMode(mode.at(0).toInt());

	SynchronizeInterfaceWindow(navigationWidget, par, qInterface::read);
	CVector3 camera = par->Get<CVector3>("camera");
	CVector3 target = par->Get<CVector3>("target");
	CVector3 topVector = par->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);

	enumCameraMovementStepMode stepMode =
		enumCameraMovementStepMode(par->Get<int>("camera_absolute_distance_mode"));
	enumCameraMovementMode movementMode =
		enumCameraMovementMode(par->Get<int>("camera_movement_mode"));
	params::enumPerspectiveType perspType =
		params::enumPerspectiveType(par->Get<int>("perspective_type"));
	cCameraTarget::enumRotationMode rollMode =
		cCameraTarget::enumRotationMode(par->Get<int>("camera_straight_rotation"));
	double movementStep = par->Get<double>("camera_movement_step");
	double fov = CalcFOV(par->Get<double>("fov"), perspType);
	bool legacyCoordinateSystem = par->Get<bool>("legacy_coordinate_system");
	double reverse = legacyCoordinateSystem ? -1.0 : 1.0;

	double sweetSpotHAngle = par->Get<double>("sweet_spot_horizontal_angle") / 180.0 * M_PI;
	double sweetSpotVAngle = par->Get<double>("sweet_spot_vertical_angle") / 180.0 * M_PI;

	CVector2<double> imagePoint;
	imagePoint = screenPoint / image->GetPreviewScale();

	int width = image->GetWidth();
	int height = image->GetHeight();

	if (imagePoint.x >= 0 && imagePoint.x < image->GetWidth() && imagePoint.y >= 0
			&& imagePoint.y < image->GetHeight())
	{
		double depth = image->GetPixelZBuffer(imagePoint.x, imagePoint.y);
		if (depth < 1e10 || true)
		{
			CVector3 viewVector;
			double aspectRatio = double(width) / height;

			if (perspType == params::perspEquirectangular) aspectRatio = 2.0;

			double wheelDistance = 1.0;
			if (clickMode == RenderedImage::clickMoveCamera)
			{

				if (mode.length() > 1) // if mouse wheel delta is available
				{
					int wheelDelta = mode.at(1).toInt();
					wheelDistance = 0.001 * fabs(wheelDelta);
				}
			}

			CVector3 angles = cameraTarget.GetRotation();
			CRotationMatrix mRot;
			mRot.SetRotation(angles);
			mRot.RotateZ(-sweetSpotHAngle);
			mRot.RotateX(sweetSpotVAngle);

			CVector2<double> normalizedPoint;
			normalizedPoint.x = (imagePoint.x / width - 0.5) * aspectRatio;
			normalizedPoint.y = (imagePoint.y / height - 0.5) * (-1.0) * reverse;

			normalizedPoint *= wheelDistance;

			viewVector = CalculateViewVector(normalizedPoint, fov, perspType, mRot);

			CVector3 point = camera + viewVector * depth;

			if (depth > 1000.0)
			{
				double estDistance = cInterface::GetDistanceForPoint(camera, par, parFractal);
				point = camera + viewVector * estDistance;
			}

			switch (clickMode)
			{
				case RenderedImage::clickMoveCamera:
				{
					double distance = (camera - point).Length();

					double moveDistance = (stepMode == absolute) ? movementStep : distance * movementStep;
					moveDistance *= wheelDistance;

					if (stepMode == relative)
					{
						if (moveDistance > depth * 0.99) moveDistance = depth * 0.99;
					}

					if (button == Qt::RightButton)
					{
						moveDistance *= -1.0;
					}

					switch (movementMode)
					{
						case moveTarget: target = point; break;

						case moveCamera: camera += viewVector * moveDistance; break;

						case fixedDistance:
							camera += viewVector * moveDistance;
							target = point;
							break;
					}

					// recalculation of camera-target
					if (movementMode == moveCamera)
						cameraTarget.SetCamera(camera, rollMode);
					else if (movementMode == moveTarget)
						cameraTarget.SetTarget(target, rollMode);
					else if (movementMode == fixedDistance)
						cameraTarget.SetCameraTargetTop(camera, target, topVector);

					if (rollMode == cCameraTarget::constantRoll)
					{
						cameraTarget.SetCameraTargetRotation(camera, target, angles.z);
					}

					par->Set("camera", camera);
					par->Set("target", target);

					topVector = cameraTarget.GetTopVector();
					par->Set("camera_top", topVector);
					CVector3 rotation = cameraTarget.GetRotation();
					par->Set("camera_rotation", rotation * (180.0 / M_PI));
					double dist = cameraTarget.GetDistance();
					par->Set("camera_distance_to_target", dist);

					SynchronizeInterfaceWindow(navigationWidget, par, qInterface::write);

					renderedImageWidget->setNewZ(depth - moveDistance);

					emit signalRender();

					break;
				}
				case RenderedImage::clickFogVisibility:
				{
					double fogDepth = depth;
					par->Set("basic_fog_visibility", fogDepth);
					emit signalWriteInterfaceBasicFog(par);
					emit signalRender();
					break;
				}
				case RenderedImage::clickDOFFocus:
				{
					emit signalDisablePeriodicRefresh();
					double DOF = depth;
					par->Set("DOF_focus", DOF);
					emit signalWriteInterfaceDOF(par);
					emit signalRefreshPostEffects();
					emit signalReEnablePeriodicRefresh();
					break;
				}
				case RenderedImage::clickPlaceLight:
				{
					int lightIndex = mode.at(1).toInt();
					double frontDist = par->Get<double>("aux_light_manual_placement_dist");
					bool placeBehind = par->Get<bool>("aux_light_place_behind");
					double distanceLimit = par->Get<double>("view_distance_max");
					bool relativePosition = par->Get<bool>(cLight::Name("relative_position", lightIndex));

					CVector3 pointCorrected;

					if (!placeBehind)
					{
						pointCorrected = point - viewVector * frontDist;
					}
					else
					{
						frontDist = traceBehindFractal(par, parFractal, frontDist, viewVector, depth,
													1.0 / image->GetHeight(), distanceLimit)
												* (-1.0);
						pointCorrected = point - viewVector * frontDist;
					}

					double estDistance = cInterface::GetDistanceForPoint(pointCorrected, par, parFractal);
					double intensity = estDistance * estDistance;

					if (relativePosition)
					{
						// without rotation
						CVector3 viewVectorTemp =
							CalculateViewVector(normalizedPoint, fov, perspType, CRotationMatrix());

						CVector3 point2 = viewVectorTemp * (depth - frontDist);
						pointCorrected = CVector3(point2.x, point2.z, point2.y);
					}

					par->Set(cLight::Name("position", lightIndex), pointCorrected);
					par->Set(cLight::Name("intensity", lightIndex), intensity);
					emit signalWriteInterfaceLights(par);
					emit signalRender();
					break;
				}
				case RenderedImage::clickGetJuliaConstant:
				{
					par->Set("julia_c", point);
					emit signalEnableJuliaMode();
					emit signalWriteInterfaceJulia(par);

					// StartRender();
					break;
				}
				case RenderedImage::clickPlacePrimitive:
				{
					QString parameterName = mode.at(3).toString() + "_position";
					par->Set(parameterName, point);
					emit signalWriteInterfacePrimitives(par);
					break;
				}
				case RenderedImage::clickDoNothing:
					// nothing
					break;
				case RenderedImage::clickFlightSpeedControl:
					// nothing
					break;
				case RenderedImage::clickPlaceRandomLightCenter:
				{
					double distanceCameraToCenter = CVector3(camera - point).Length();
					par->Set("random_lights_distribution_center", point);
					par->Set("random_lights_distribution_radius", 0.5 * distanceCameraToCenter);
					par->Set("random_lights_max_distance_from_fractal", 0.1 * distanceCameraToCenter);
					emit signalWriteInterfaceRandomLights(par);
					emit signalRender();
					break;
				}
				case RenderedImage::clickGetPoint:
				{
					emit signalDisablePeriodicRefresh();
					SynchronizeInterfaceWindow(navigationWidget, par, qInterface::read);
					CVector3 oldPoint = par->Get<CVector3>("meas_point");
					double distanceFromLast = (point - oldPoint).Length();
					double distanceFromCamera = (point - camera).Length();
					CVector3 midPoint = 0.5 * (point + oldPoint);
					par->Set("meas_point", point);
					par->Set("meas_midpoint", midPoint);
					par->Set("meas_distance_from_last", distanceFromLast);
					par->Set("meas_distance_from_camera", distanceFromCamera);
					emit signalWriteInterfaceMeasuremets(par);
					emit signalShowMeasuremetsDock();
					emit signalReEnablePeriodicRefresh();
					break;
				}
				case RenderedImage::clickWrapLimitsAroundObject:
				{
					double distanceCameraToCenter = CVector3(camera - point).Length();
					CVector3 distanceV111_100 = CVector3(1.0 * distanceCameraToCenter,
						1.0 * distanceCameraToCenter, 1.0 * distanceCameraToCenter);
					CVector3 limitMin = point - distanceV111_100;
					CVector3 limitMax = point + distanceV111_100;
					// try to find object close limits in the bounding box defined by point +- 100% distance
					// to view vector
					gMainInterface->SetBoundingBoxAsLimits(limitMin, limitMax, par, parFractal);
					break;
				}
			}
		}
	}
}
