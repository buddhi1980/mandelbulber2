/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * Functions for flight animation.
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

#include "animation_flight.hpp"
#include "global_data.hpp"
#include "render_job.hpp"
#include "system.hpp"
#include "files.h"

cFlightAnimation::cFlightAnimation(cInterface *_interface, QObject *parent) : QObject(parent), interface(_interface)
{
	ui = interface->mainWindow->ui;
	QApplication::connect(ui->pushButton_record_flight, SIGNAL(clicked()), this, SLOT(slotRecordFilght()));
	frames = NULL;
}

void cFlightAnimation::slotRecordFilght()
{
	qDebug() << "record flight animation";
	if(gAnimFrames)
	{
		RecordFlight(gAnimFrames);
	}
	else
	{
		qCritical() << "gAnimFrames not allocated";
	}
}

void cFlightAnimation::RecordFlight(cAnimationFrames *frames)
{
	//TODO displaying of frames as thumbnails or table
	//TODO saving of all frames into csv file

	frames->Clear();

	interface->SynchronizeInterface(gPar, gParFractal, cInterface::read);

	cRenderJob *renderJob = new cRenderJob(gPar, gParFractal, interface->mainImage, &interface->stopRequest, interface->renderedImage);
	renderJob->AssingStatusAndProgessBar(ui->statusbar, interface->progressBar);

	renderJob->Init(cRenderJob::flightAnimRecord);

	interface->stopRequest = false;

	CVector3 cameraSpeed;
	CVector3 cameraAcceleration;
	CVector3 cameraAccelerationRotation;
	CVector3 cameraAngularSpeed;
	CVector3 cameraAngularAcceleration;
	CVector3 cameraRotation;

	CVector3 cameraPosition = gPar->Get<CVector3>("camera");
	CVector3 target = gPar->Get<CVector3>("target");
	CVector3 top = gPar->Get<CVector3>("camera_top");

	cCameraTarget cameraTarget(cameraPosition, target, top);

	//TODO setting of max render time
	double maxRenderTime = 0.5;
	renderJob->SetMaxRenderTime(maxRenderTime);

	//TODO variable speed depending on distance to fractal surface
	double linearSpeed = 0.01;
	double rotationSpeed = 0.1;
	double inertia = 10.0;
	int index = 0;

	while(!interface->stopRequest)
	{
		CVector2<double> mousePosition = interface->renderedImage->GetLastMousePositionScaled();

		cameraAcceleration = (cameraTarget.GetForwardVector() * linearSpeed - cameraSpeed)/(inertia + 1.0);
		cameraSpeed += cameraAcceleration;
		cameraPosition += cameraSpeed;

		cameraAngularAcceleration.x = (mousePosition.x * rotationSpeed - cameraAngularSpeed.x) / (inertia + 1.0);
		cameraAngularAcceleration.y = (mousePosition.y * rotationSpeed - cameraAngularSpeed.y) / (inertia + 1.0);
		cameraAngularSpeed += cameraAngularAcceleration;

		//TODO other modes of rotation
		CVector3 forwardVector = cameraTarget.GetForwardVector();
		forwardVector = forwardVector.RotateAroundVectorByAngle(cameraTarget.GetTopVector(), -cameraAngularSpeed.x);
		forwardVector = forwardVector.RotateAroundVectorByAngle(cameraTarget.GetRightVector(), -cameraAngularSpeed.y);
		top = cameraTarget.GetTopVector();
		top = top.RotateAroundVectorByAngle(cameraTarget.GetRightVector(), -cameraAngularSpeed.y);

		target = cameraPosition + forwardVector * cameraSpeed.Length();
		cameraTarget.SetCameraTargetTop(cameraPosition, target, top);

		gPar->Set("camera", cameraPosition);
		gPar->Set("target", cameraPosition);
		gPar->Set("camera_top", top);
		gPar->Set("camera_rotation", cameraTarget.GetRotation()*180.0 / M_PI);
		gPar->Set("camera_distance_to_target", cameraTarget.GetDistance());
		interface->SynchronizeInterfaceWindow(ui->dockWidget_navigation, gPar, cInterface::write);

		renderJob->ChangeCameraTargetPosition(cameraTarget);

		frames->AddFrame(*gPar, *gParFractal);

		renderJob->Execute();

		//TODO now is temporarysaving of images
		QString filename = systemData.dataDirectory + "images/image" + QString("%1").arg(index, 5, 10, QChar('0')) + QString(".jpg");
		SaveJPEGQt(filename, interface->mainImage->ConvertTo8bit(), interface->mainImage->GetWidth(), interface->mainImage->GetHeight(), 90);
		index++;
	}

	delete renderJob;
}
