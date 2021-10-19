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
#include "qt/dock_navigation.h"
#include "qt/dock_effects.h"

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

void cManipulations::AssignWidgets(
	RenderedImage *_imageWidget, cDockNavigation *_navigationWidget, cDockEffects *_effectsWidget)
{
	renderedImageWidget = _imageWidget;
	navigationWidget = _navigationWidget;
	effectsWidget = _effectsWidget;
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

void cManipulations::CameraOrTargetEdited()
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

void cManipulations::RotationEdited()
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

void cManipulations::CameraDistanceEdited()
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

void cManipulations::MovementStepModeChanged(int mode)
{
	using namespace cameraMovementEnums;

	SynchronizeInterfaceWindow(navigationWidget, par, qInterface::read);
	enumCameraMovementStepMode stepMode = enumCameraMovementStepMode(mode);
	double distance = cInterface::GetDistanceForPoint(par->Get<CVector3>("camera"), par, parFractal);
	double oldStep = par->Get<double>("camera_movement_step");
	double newStep;
	if (stepMode == absolute)
	{
		newStep = oldStep * distance;
		if (distance > 1.0 && newStep > distance * 0.5) newStep = distance * 0.5;
	}
	else
	{
		newStep = oldStep / distance;
		if (distance > 1.0 && newStep > 0.5) newStep = 0.5;
	}
	par->Set("camera_movement_step", newStep);
	SynchronizeInterfaceWindow(navigationWidget, par, qInterface::write);
}

void cManipulations::SetByMouse(
	CVector2<double> screenPoint, Qt::MouseButton button, const QList<QVariant> &mode)
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

void cManipulations::MouseDragStart(
	CVector2<double> screenPoint, Qt::MouseButtons button, const QList<QVariant> &mode)
{
	mouseDragData = sMouseDragInitData();

	RenderedImage::enumClickMode clickMode = RenderedImage::enumClickMode(mode.at(0).toInt());

	int lightIndex = -1;
	if (clickMode == RenderedImage::clickPlaceLight)
	{
		lightIndex = mode.at(1).toInt();
		mouseDragData.lightDrag = true;

		mouseDragData.lightStartPosition = par->Get<CVector3>(cLight::Name("position", lightIndex));
	}
	else
	{
		mouseDragData.cameraDrag = true;
	}

	SynchronizeInterfaceWindow(navigationWidget, par, qInterface::read);
	CVector3 camera = par->Get<CVector3>("camera");
	CVector3 target = par->Get<CVector3>("target");
	CVector3 topVector = par->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);

	params::enumPerspectiveType perspType =
		params::enumPerspectiveType(par->Get<int>("perspective_type"));
	double sweetSpotHAngle = par->Get<double>("sweet_spot_horizontal_angle") / 180.0 * M_PI;
	double sweetSpotVAngle = par->Get<double>("sweet_spot_vertical_angle") / 180.0 * M_PI;
	double fov = CalcFOV(par->Get<double>("fov"), perspType);
	bool legacyCoordinateSystem = par->Get<bool>("legacy_coordinate_system");
	double reverse = legacyCoordinateSystem ? -1.0 : 1.0;

	CVector2<double> imagePoint;
	imagePoint = screenPoint / image->GetPreviewScale();

	int width = image->GetWidth();
	int height = image->GetHeight();

	if (imagePoint.x >= 0 && imagePoint.x < image->GetWidth() && imagePoint.y >= 0
			&& imagePoint.y < image->GetHeight())
	{
		double depth = image->GetPixelZBuffer(imagePoint.x, imagePoint.y);
		if (depth < 1e10 || clickMode == RenderedImage::clickPlaceLight || true)
		{
			CVector3 viewVector;
			double aspectRatio = double(width) / height;

			if (perspType == params::perspEquirectangular) aspectRatio = 2.0;

			CVector3 angles = cameraTarget.GetRotation();
			CRotationMatrix mRot;
			mRot.SetRotation(angles);
			mRot.RotateZ(-sweetSpotHAngle);
			mRot.RotateX(sweetSpotVAngle);

			CVector2<double> normalizedPoint;
			normalizedPoint.x = (imagePoint.x / width - 0.5) * aspectRatio;
			normalizedPoint.y = (imagePoint.y / height - 0.5) * (-1.0) * reverse;

			viewVector = CalculateViewVector(normalizedPoint, fov, perspType, mRot);

			CVector3 point = camera + viewVector * depth;

			if (depth > 1000.0)
			{
				double estDistance = cInterface::GetDistanceForPoint(camera, par, parFractal);
				point = camera + viewVector * estDistance;
				depth = estDistance;
			}

			mouseDragData.startCamera = camera;
			mouseDragData.startTarget = target;
			mouseDragData.startTopVector = topVector;
			mouseDragData.startIndicatedPoint = point;
			mouseDragData.button = button;
			mouseDragData.startScreenPoint = screenPoint;
			mouseDragData.startNormalizedPoint = normalizedPoint;
			mouseDragData.startZ = depth;
			mouseDragData.lastRefreshTime.restart();
			mouseDragData.lastStartRenderingTime = 0;
			mouseDragData.lightIndex = lightIndex;

			if (clickMode == RenderedImage::clickMoveCamera
					|| clickMode == RenderedImage::clickPlaceLight)
			{
				mouseDragData.draggingStarted = true;
			}
		}
	}
}

void cManipulations::MouseDragFinish()
{
	mouseDragData.draggingStarted = false;
}

void cManipulations::MouseDragCameraLeftButton(const sMouseDragTempData &dragTempData)
{
	CVector3 camera = mouseDragData.startCamera;
	cCameraTarget cameraTarget(camera, mouseDragData.startTarget, mouseDragData.startTopVector);

	CVector3 angles = cameraTarget.GetRotation();
	CRotationMatrix mRot;
	mRot.SetRotation(angles);
	mRot.RotateZ(-dragTempData.sweetSpotHAngle);
	mRot.RotateX(dragTempData.sweetSpotVAngle);

	CVector2<double> normalizedPoint;
	normalizedPoint.x =
		(dragTempData.imagePoint.x / dragTempData.width - 0.5) * dragTempData.aspectRatio;
	normalizedPoint.y =
		(dragTempData.imagePoint.y / dragTempData.height - 0.5) * (-1.0) * dragTempData.reverse;

	CVector3 viewVector =
		CalculateViewVector(normalizedPoint, dragTempData.fov, dragTempData.perspType, mRot);

	CVector3 point = camera + viewVector * mouseDragData.startZ;
	CVector3 deltaPoint = point - mouseDragData.startIndicatedPoint;
	CVector3 pointCamera = camera - point;
	pointCamera.Normalize();

	CVector3 relativeVector;
	relativeVector.z = pointCamera.Dot(deltaPoint);
	relativeVector.x = pointCamera.Cross(cameraTarget.GetTopVector()).Dot(deltaPoint);
	relativeVector.y = pointCamera.Cross(cameraTarget.GetRightVector()).Dot(deltaPoint);

	double ratio = (camera - mouseDragData.startTarget).Length() / (camera - point).Length();
	if (dragTempData.perspType == params::perspThreePoint)
	{
		ratio /= -pointCamera.Dot(cameraTarget.GetForwardVector());
	}
	relativeVector *= ratio;

	CVector3 newTarget = mouseDragData.startTarget;
	newTarget -= relativeVector.x * cameraTarget.GetRightVector();
	newTarget += relativeVector.y * cameraTarget.GetTopVector();
	newTarget += relativeVector.z * cameraTarget.GetForwardVector();

	cameraTarget.SetTarget(newTarget, dragTempData.rollMode);

	par->Set("camera", camera);
	par->Set("target", newTarget);
	CVector3 topVector = cameraTarget.GetTopVector();
	par->Set("camera_top", topVector);
	CVector3 rotation = cameraTarget.GetRotation();
	par->Set("camera_rotation", rotation * (180.0 / M_PI));
	double dist = cameraTarget.GetDistance();
	par->Set("camera_distance_to_target", dist);
}

void cManipulations::MouseDragCaneraRightButton(
	int dx, int dy, const sMouseDragTempData &dragTempData)
{
	cCameraTarget cameraTarget(
		mouseDragData.startCamera, mouseDragData.startTarget, mouseDragData.startTopVector);

	CVector3 shiftedCamera = mouseDragData.startCamera - mouseDragData.startIndicatedPoint;
	CVector3 shiftedTarget = mouseDragData.startTarget - mouseDragData.startIndicatedPoint;
	shiftedCamera = shiftedCamera.RotateAroundVectorByAngle(
		cameraTarget.GetTopVector(), (double)(dx) / image->GetPreviewWidth() * M_PI_2);
	shiftedTarget = shiftedTarget.RotateAroundVectorByAngle(
		cameraTarget.GetTopVector(), (double)(dx) / image->GetPreviewWidth() * M_PI_2);

	CVector3 newCamera = shiftedCamera + mouseDragData.startIndicatedPoint;
	CVector3 newTarget = shiftedTarget + mouseDragData.startIndicatedPoint;
	cameraTarget.SetCamera(newCamera, dragTempData.rollMode);
	cameraTarget.SetTarget(newTarget, dragTempData.rollMode);

	shiftedCamera = shiftedCamera.RotateAroundVectorByAngle(
		cameraTarget.GetRightVector(), (double)(dy) / image->GetPreviewHeight() * M_PI_2);
	shiftedTarget = shiftedTarget.RotateAroundVectorByAngle(
		cameraTarget.GetRightVector(), (double)(dy) / image->GetPreviewHeight() * M_PI_2);

	newCamera = shiftedCamera + mouseDragData.startIndicatedPoint;
	newTarget = shiftedTarget + mouseDragData.startIndicatedPoint;

	cameraTarget.SetCamera(newCamera, dragTempData.rollMode);
	cameraTarget.SetTarget(newTarget, dragTempData.rollMode);

	par->Set("camera", cameraTarget.GetCamera());
	par->Set("target", cameraTarget.GetTarget());
	CVector3 topVector = cameraTarget.GetTopVector();
	par->Set("camera_top", topVector);
	CVector3 rotation = cameraTarget.GetRotation();
	par->Set("camera_rotation", rotation * (180.0 / M_PI));
	double dist = cameraTarget.GetDistance();
	par->Set("camera_distance_to_target", dist);
}

void cManipulations::MouseDragCameraMiddleButton(int dx)
{
	double angle = -(double)(dx) / image->GetPreviewHeight() * M_PI_2;
	cCameraTarget cameraTarget(
		mouseDragData.startCamera, mouseDragData.startTarget, mouseDragData.startTopVector);
	CVector3 newTopVector =
		mouseDragData.startTopVector.RotateAroundVectorByAngle(cameraTarget.GetForwardVector(), angle);
	cameraTarget.SetCameraTargetTop(
		mouseDragData.startCamera, mouseDragData.startTarget, newTopVector);
	par->Set("camera_top", newTopVector);
	CVector3 rotation = cameraTarget.GetRotation();
	par->Set("camera_rotation", rotation * (180.0 / M_PI));
}

void cManipulations::MouseDragLeftRightButtons(const sMouseDragTempData &dragTempData)
{
	CVector3 camera = mouseDragData.startCamera;
	cCameraTarget cameraTarget(camera, mouseDragData.startTarget, mouseDragData.startTopVector);

	CVector3 angles = cameraTarget.GetRotation();
	CRotationMatrix mRot;
	mRot.SetRotation(angles);
	mRot.RotateZ(-dragTempData.sweetSpotHAngle);
	mRot.RotateX(dragTempData.sweetSpotVAngle);

	CVector2<double> normalizedPoint;
	normalizedPoint.x =
		(dragTempData.imagePoint.x / dragTempData.width - 0.5) * dragTempData.aspectRatio;
	normalizedPoint.y =
		(dragTempData.imagePoint.y / dragTempData.height - 0.5) * (-1.0) * dragTempData.reverse;

	CVector3 viewVector =
		CalculateViewVector(normalizedPoint, dragTempData.fov, dragTempData.perspType, mRot);

	CVector3 point = camera + viewVector * mouseDragData.startZ;
	CVector3 deltaPoint = point - mouseDragData.startIndicatedPoint;
	CVector3 newTarget = mouseDragData.startTarget + deltaPoint;
	CVector3 newCamera = mouseDragData.startCamera + deltaPoint;

	cameraTarget.SetCameraTargetTop(newCamera, newTarget, cameraTarget.GetTopVector());

	par->Set("camera", newCamera);
	par->Set("target", newTarget);
	CVector3 topVector = cameraTarget.GetTopVector();
	par->Set("camera_top", topVector);
	CVector3 rotation = cameraTarget.GetRotation();
	par->Set("camera_rotation", rotation * (180.0 / M_PI));
	double dist = cameraTarget.GetDistance();
	par->Set("camera_distance_to_target", dist);
}

void cManipulations::LightDragLeftButton(const sMouseDragTempData &dragTempData, int dx, int dy)
{
	bool relativePosition =
		par->Get<bool>(cLight::Name("relative_position", mouseDragData.lightIndex));

	cCameraTarget cameraTarget(
		mouseDragData.startCamera, mouseDragData.startTarget, mouseDragData.startTopVector);

	CVector3 lightPosition = mouseDragData.lightStartPosition;

	if (relativePosition)
	{
		CVector3 deltaPositionRotated = cameraTarget.GetForwardVector() * lightPosition.z
																		+ cameraTarget.GetTopVector() * lightPosition.y
																		+ cameraTarget.GetRightVector() * lightPosition.x;
		lightPosition = mouseDragData.startCamera + deltaPositionRotated;
	}

	CRotationMatrix mRotInv;
	CVector3 rotation = cameraTarget.GetRotation();
	mRotInv.RotateY(-rotation.z);
	mRotInv.RotateX(-rotation.y);
	mRotInv.RotateZ(-rotation.x);

	CVector3 lightScreenPosition = InvProjection3D(lightPosition, mouseDragData.startCamera, mRotInv,
		dragTempData.perspType, dragTempData.fov, image->GetPreviewWidth(), image->GetPreviewHeight());
	CVector3 newLightScreenPosition = lightScreenPosition + CVector3(dx, dy, 0.0);

	CRotationMatrix mRot;
	mRot.SetRotation(rotation);

	CVector2<double> normalizedPoint;
	normalizedPoint.x =
		(newLightScreenPosition.x / image->GetPreviewWidth() - 0.5) * dragTempData.aspectRatio;
	normalizedPoint.y =
		(newLightScreenPosition.y / image->GetPreviewHeight() - 0.5) * (-1.0) * dragTempData.reverse;

	CVector3 viewVector =
		CalculateViewVector(normalizedPoint, dragTempData.fov, dragTempData.perspType, mRot);

	CVector3 newLightPosition;
	if (relativePosition)
	{
		// without rotation
		CVector3 viewVectorTemp = CalculateViewVector(
			normalizedPoint, dragTempData.fov, dragTempData.perspType, CRotationMatrix());
		CVector3 point2 = viewVectorTemp * lightScreenPosition.z;
		newLightPosition = CVector3(point2.x, point2.z, point2.y);
	}
	else
	{
		newLightPosition = mouseDragData.startCamera + viewVector * lightScreenPosition.z;
	}

	par->Set(cLight::Name("position", mouseDragData.lightIndex), newLightPosition);
}

void cManipulations::MouseDragDelta(int dx, int dy)
{
	if (mouseDragData.draggingStarted)
	{
		if (GetNumberOfStartedRenders() > 1) emit signalStop();

		if (mouseDragData.lastRefreshTime.elapsed()
					> par->Get<double>("auto_refresh_period") * 1000 + mouseDragData.lastStartRenderingTime
				&& GetNumberOfStartedRenders() < 2)
		{
			sMouseDragTempData dragTempData;

			mouseDragData.lastRefreshTime.restart();
			dragTempData.perspType = params::enumPerspectiveType(par->Get<int>("perspective_type"));
			dragTempData.sweetSpotHAngle = par->Get<double>("sweet_spot_horizontal_angle") / 180.0 * M_PI;
			dragTempData.sweetSpotVAngle = par->Get<double>("sweet_spot_vertical_angle") / 180.0 * M_PI;
			dragTempData.legacyCoordinateSystem = par->Get<bool>("legacy_coordinate_system");
			dragTempData.reverse = dragTempData.legacyCoordinateSystem ? -1.0 : 1.0;
			dragTempData.fov = CalcFOV(par->Get<double>("fov"), dragTempData.perspType);
			dragTempData.rollMode =
				cCameraTarget::enumRotationMode(par->Get<int>("camera_straight_rotation"));

			dragTempData.newScreenPoint = CVector2<double>(
				mouseDragData.startScreenPoint.x - dx, mouseDragData.startScreenPoint.y - dy);
			dragTempData.imagePoint = dragTempData.newScreenPoint / image->GetPreviewScale();

			dragTempData.width = image->GetWidth();
			dragTempData.height = image->GetHeight();
			dragTempData.aspectRatio = double(dragTempData.width) / dragTempData.height;
			if (dragTempData.perspType == params::perspEquirectangular) dragTempData.aspectRatio = 2.0;

			if (mouseDragData.cameraDrag)
			{
				switch (mouseDragData.button)
				{
					case Qt::LeftButton:
					{
						MouseDragCameraLeftButton(dragTempData);
						break;
					}
					case Qt::RightButton:
					{
						MouseDragCaneraRightButton(dx, dy, dragTempData);
						break;
					}
					case Qt::MiddleButton:
					{
						MouseDragCameraMiddleButton(dx);
						break;
					}

					case (Qt::LeftButton | Qt::RightButton):
					{
						MouseDragLeftRightButtons(dragTempData);
						break;
					}

					default: break;
				}

				QElapsedTimer timerStartRender;
				timerStartRender.start();
				SynchronizeInterfaceWindow(navigationWidget, par, qInterface::write);
				emit signalRender();
				mouseDragData.lastStartRenderingTime = timerStartRender.elapsed();
			}
			else if (mouseDragData.lightDrag)
			{
				switch (mouseDragData.button)
				{
					case Qt::LeftButton:
					{
						LightDragLeftButton(dragTempData, dx, dy);
						break;
					}
					default:
					{
						break;
					}
				}
				QElapsedTimer timerStartRender;
				timerStartRender.start();
				SynchronizeInterfaceWindow(navigationWidget, par, qInterface::write);
				renderedImageWidget->update();
				mouseDragData.lastStartRenderingTime = timerStartRender.elapsed();
			}
		}
	}
}

void cManipulations::MoveLightByWheel(double deltaWheel)
{
	double deltaLog = exp(deltaWheel * 0.0001);

	CVector3 lightPosition =
		par->Get<CVector3>(cLight::Name("position", renderedImageWidget->GetCurrentLightIndex()));

	bool relativePosition =
		par->Get<bool>(cLight::Name("relative_position", renderedImageWidget->GetCurrentLightIndex()));

	CVector3 newLightPosition;

	if (relativePosition)
	{
		newLightPosition = lightPosition * deltaLog;
	}
	else
	{
		CVector3 cameraPosition = par->Get<CVector3>("camera");
		CVector3 lightVector = lightPosition - cameraPosition;
		CVector3 newLightVector = lightVector * deltaLog;
		newLightPosition = cameraPosition + newLightVector;
	}

	par->Set(cLight::Name("position", renderedImageWidget->GetCurrentLightIndex()), newLightPosition);

	if (effectsWidget)
	{
		SynchronizeInterfaceWindow(effectsWidget, par, qInterface::write);
	}
	renderedImageWidget->update();
}
