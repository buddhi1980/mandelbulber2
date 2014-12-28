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
	table = ui->tableWidget_flightAnimation;
}

void cFlightAnimation::slotRecordFilght()
{
	if(gAnimFrames)
	{
		RecordFlight(gAnimFrames);
	}
	else
	{
		qCritical() << "gAnimFrames not allocated";
	}
}

void cFlightAnimation::RecordFlight(cAnimationFrames *_frames)
{
	//TODO displaying of frames as thumbnails or table
	//TODO saving of all frames into csv file

	frames = _frames;
	frames->Clear();

	PrepareTable();



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
	double linearSpeed = gPar->Get<double>("flight_speed");
	double rotationSpeed = 0.1;
	double inertia = gPar->Get<double>("flight_inertia");
	int index = 0;

	while(!interface->stopRequest)
	{
		CVector2<double> mousePosition = interface->renderedImage->GetLastMousePositionScaled();

		//speed
		double distanceToSurface = mainInterface->GetDistanceForPoint(cameraPosition, gPar, gParFractal);
		linearSpeed = distanceToSurface * 0.05;

		//integrator for position
		cameraAcceleration = (cameraTarget.GetForwardVector() * linearSpeed - cameraSpeed)/(inertia + 1.0);
		cameraSpeed += cameraAcceleration;
		cameraPosition += cameraSpeed;

		//rotation
		cameraAngularAcceleration.x = (mousePosition.x * rotationSpeed - cameraAngularSpeed.x) / (inertia + 1.0);
		cameraAngularAcceleration.y = (mousePosition.y * rotationSpeed - cameraAngularSpeed.y) / (inertia + 1.0);
		cameraAngularSpeed += cameraAngularAcceleration;

		//TODO other modes of rotation
		CVector3 forwardVector = cameraTarget.GetForwardVector();
		forwardVector = forwardVector.RotateAroundVectorByAngle(cameraTarget.GetTopVector(), -cameraAngularSpeed.x);
		forwardVector = forwardVector.RotateAroundVectorByAngle(cameraTarget.GetRightVector(), -cameraAngularSpeed.y);
		top = cameraTarget.GetTopVector();
		top = top.RotateAroundVectorByAngle(cameraTarget.GetRightVector(), -cameraAngularSpeed.y);

		//update position and rotation
		target = cameraPosition + forwardVector * cameraSpeed.Length();
		cameraTarget.SetCameraTargetTop(cameraPosition, target, top);

		//update parameters
		gPar->Set("camera", cameraPosition);
		gPar->Set("target", target);
		gPar->Set("camera_top", top);
		gPar->Set("camera_rotation", cameraTarget.GetRotation()*180.0 / M_PI);
		gPar->Set("camera_distance_to_target", cameraTarget.GetDistance());
		interface->SynchronizeInterfaceWindow(ui->dockWidget_navigation, gPar, cInterface::write);
		renderJob->ChangeCameraTargetPosition(cameraTarget);

		//add new frame to container
		frames->AddFrame(*gPar, *gParFractal);

		//add column to table
		int newColumn = AddColumn(frames->GetFrame(frames->GetNumberOfFrames() - 1));

		//render frame
		renderJob->Execute();

		//create thumbnail
		QImage qimage((const uchar*)interface->mainImage->ConvertTo8bit(), interface->mainImage->GetWidth(), interface->mainImage->GetHeight(), interface->mainImage->GetWidth()*sizeof(sRGB8), QImage::Format_RGB888);
		QPixmap pixmap;
		pixmap.convertFromImage(qimage);
		QIcon icon(pixmap.scaled(QSize(100, 70), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
		table->setItem(0, newColumn, new QTableWidgetItem(icon, QString()));

		//TODO now is temporarysaving of images
		QString filename = systemData.dataDirectory + "images/image" + QString("%1").arg(index, 5, 10, QChar('0')) + QString(".jpg");
		SaveJPEGQt(filename, interface->mainImage->ConvertTo8bit(), interface->mainImage->GetWidth(), interface->mainImage->GetHeight(), 90);
		index++;
	}

	delete renderJob;
}

void cFlightAnimation::PrepareTable()
{
	table->clear();
	CreateRowsInTable();
}

void cFlightAnimation::CreateRowsInTable()
{
	QList<cAnimationFrames::sParameterDescription> parList = frames->GetListOfUsedParameters();
	tableRowNames.clear();
	table->setRowCount(0);
	table->setColumnCount(0);
	table->setIconSize(QSize(100, 70));

	table->insertRow(0);
	table->setVerticalHeaderItem(0, new QTableWidgetItem(tr("preview")));
	table->setRowHeight(0, 70);
	tableRowNames.append(tr("preview"));

	parameterRows.clear();
	for (int i = 0; i < parList.size(); ++i)
	{
		int row = AddVariableToTable(parList[i]);
		parameterRows.append(row);
	}
}

int cFlightAnimation::AddVariableToTable(const cAnimationFrames::sParameterDescription &parameterDescription)
{
	using namespace parameterContainer;
	enumVarType type = parameterDescription.varType;
	int row = table->rowCount();
	if (type == typeVector3)
	{
		QString varName;
		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_x";
		tableRowNames.append(varName);
		table->insertRow(row);
		table->setVerticalHeaderItem(row, new QTableWidgetItem(varName));
		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_y";
		tableRowNames.append(varName);
		table->insertRow(row + 1);
		table->setVerticalHeaderItem(row + 1, new QTableWidgetItem(varName));
		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_z";
		tableRowNames.append(varName);
		table->insertRow(row + 2);
		table->setVerticalHeaderItem(row + 2, new QTableWidgetItem(varName));
	}
	else
	{
		QString varName = parameterDescription.containerName + "_" + parameterDescription.parameterName;
		tableRowNames.append(varName);
		table->insertRow(table->rowCount());
		table->setVerticalHeaderItem(table->rowCount() - 1, new QTableWidgetItem(varName));
	}
	//TODO other parameter types
	return row;
}

int cFlightAnimation::AddColumn(const cParameterContainer &params)
{
	int newColumn = table->columnCount();
	table->insertColumn(newColumn);

	QList<cAnimationFrames::sParameterDescription> parList = frames->GetListOfUsedParameters();

	using namespace parameterContainer;
	for(int i=0; i<parList.size(); ++i)
	{
		QString parameterName = parList[i].containerName + "_" + parList[i].parameterName;
		enumVarType type = parList[i].varType;
		int row = parameterRows[i];

		if (type == typeVector3)
		{
			CVector3 val = params.Get<CVector3>(parameterName);
			table->setItem(row, newColumn, new QTableWidgetItem(QString::number(val.x, 'g', 16)));
			table->setItem(row + 1, newColumn, new QTableWidgetItem(QString::number(val.y, 'g', 16)));
			table->setItem(row + 2, newColumn, new QTableWidgetItem(QString::number(val.z, 'g', 16)));
		}
		else
		{
			QString val = params.Get<QString>(parameterName);
			table->setItem(row, newColumn, new QTableWidgetItem(val));
		}
		//TODO other parameter types
	}

	return newColumn;
}
