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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen
 */

#include "animation_flight.hpp"
#include "global_data.hpp"
#include "render_job.hpp"
#include "system.hpp"
#include "error_message.hpp"
#include "progress_text.hpp"
#include <QFileDialog>
#include <QMessageBox>
#include "thumbnail_widget.h"
#include <QInputDialog>
#include "undo.h"

cFlightAnimation::cFlightAnimation(cInterface *_interface, cAnimationFrames *_frames, QObject *parent) : QObject(parent), mainInterface(_interface), frames(_frames)
{
	ui = mainInterface->mainWindow->ui;
	QApplication::connect(ui->pushButton_record_flight, SIGNAL(clicked()), this, SLOT(slotRecordFlight()));
	QApplication::connect(ui->pushButton_continue_recording, SIGNAL(clicked()), this, SLOT(slotContinueRecording()));
	QApplication::connect(ui->pushButton_render_flight, SIGNAL(clicked()), this, SLOT(slotRenderFlight()));
	QApplication::connect(ui->pushButton_delete_all_images, SIGNAL(clicked()), this, SLOT(slotDeleteAllImages()));
	QApplication::connect(ui->pushButton_show_animation, SIGNAL(clicked()), this, SLOT(slotShowAnimation()));
	QApplication::connect(ui->pushButton_flight_refresh_table, SIGNAL(clicked()), this, SLOT(slotRefreshTable()));
	QApplication::connect(ui->pushButton_flight_to_keyframe_export, SIGNAL(clicked()), this, SLOT(slotExportFlightToKeyframes()));


	QApplication::connect(ui->button_selectAnimFlightImageDir, SIGNAL(clicked()), this, SLOT(slotSelectAnimFlightImageDir()));
	QApplication::connect(mainInterface->renderedImage, SIGNAL(flightStrafe(CVector2<double>)), this, SLOT(slotFlightStrafe(CVector2<double>)));
	QApplication::connect(mainInterface->renderedImage, SIGNAL(flightYawAndPitch(CVector2<double>)), this, SLOT(slotFlightYawAndPitch(CVector2<double>)));
	QApplication::connect(mainInterface->renderedImage, SIGNAL(flightSpeedIncrease()), this, SLOT(slotIncreaseSpeed()));
	QApplication::connect(mainInterface->renderedImage, SIGNAL(flightSpeedDecrease()), this, SLOT(slotDecreaseSpeed()));
	QApplication::connect(mainInterface->renderedImage, SIGNAL(flightRotation(double)), this, SLOT(slotFlightRotation(double)));
	QApplication::connect(mainInterface->renderedImage, SIGNAL(flightPause()), this, SLOT(slotRecordPause()));
	QApplication::connect(ui->tableWidget_flightAnimation, SIGNAL(cellChanged(int, int)), this, SLOT(slotTableCellChanged(int, int)));

	QApplication::connect(ui->spinboxInt_flight_first_to_render, SIGNAL(valueChanged(int)), this, SLOT(slotMovedSliderFirstFrame(int)));
	QApplication::connect(ui->spinboxInt_flight_last_to_render, SIGNAL(valueChanged(int)), this, SLOT(slotMovedSliderLastFrame(int)));

	table = ui->tableWidget_flightAnimation;
	linearSpeedSp = 0.0;
	rotationDirection = 0;
	recordPause = false;
}

void cFlightAnimation::slotRecordFlight()
{
	if(frames)
	{
		RecordFlight(false);
	}
	else
	{
		qCritical() << "gAnimFrames not allocated";
	}
}

void cFlightAnimation::slotContinueRecording()
{
	if(frames)
	{
		RecordFlight(true);
	}
	else
	{
		qCritical() << "gAnimFrames not allocated";
	}
}

void cFlightAnimation::slotRenderFlight()
{
	if(frames)
	{
		if(frames->GetNumberOfFrames() > 0)
		{
			RenderFlight();
		}
		else
		{
			cErrorMessage::showMessage(QObject::tr("No frames to render"), cErrorMessage::errorMessage, ui->centralwidget);
		}

	}
	else
	{
		qCritical() << "gAnimFrames not allocated";
	}
}

void cFlightAnimation::RecordFlight(bool continueRecording)
{
	//get latest values of all parameters
	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	gUndo.Store(gPar, gParFractal, frames, NULL);

	if(!continueRecording)
	{
		//confirmation dialog before start
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(
			ui->centralwidget,
			QObject::tr("Are you sure to start recording of new animation?"),
			QObject::tr("This will delete all images in the image folder.\nProceed?"),
			QMessageBox::Yes|QMessageBox::No);

		if (reply == QMessageBox::Yes)
		{
			DeleteAllFilesFromDirectory(gPar->Get<QString>("anim_flight_dir"), "frame_?????.*");
		}
		else
		{
			return;
		}
	}
	else
	{
		if (frames->GetNumberOfFrames() == 0)
		{
			cErrorMessage::showMessage(QObject::tr("No frames recorded before. Unable to continue."), cErrorMessage::errorMessage);
			return;
		}
	}

	//check if main image is not used by other rendering process
	if(mainInterface->mainImage->IsUsed())
	{
		cErrorMessage::showMessage(QObject::tr("Rendering engine is busy. Stop unfinished rendering before starting new one"), cErrorMessage::errorMessage);
		return;
	}

	ProgressStatusText(QObject::tr("Recordning flight path"), tr("waiting 3 seconds"), 0.0, ui->statusbar, mainInterface->progressBar);
	gApplication->processEvents();
	Wait(3000);

	if (!continueRecording)
	{
		frames->Clear();

		bool addSpeeds = gPar->Get<bool>("flight_add_speeds");

		//add default parameters for animation
		if (frames->GetListOfUsedParameters().size() == 0)
		{
			gAnimFrames->AddAnimatedParameter("camera", gPar->GetAsOneParameter("camera"));
			gAnimFrames->AddAnimatedParameter("target", gPar->GetAsOneParameter("target"));
			gAnimFrames->AddAnimatedParameter("camera_top", gPar->GetAsOneParameter("camera_top"));
			if (addSpeeds)
			{
				{
					gAnimFrames->AddAnimatedParameter("flight_movement_speed_vector", gPar->GetAsOneParameter("flight_movement_speed_vector"));
					gAnimFrames->AddAnimatedParameter("flight_rotation_speed_vector", gPar->GetAsOneParameter("flight_rotation_speed_vector"));
				}
			}
		}

		PrepareTable();
	}

	//setup cursor mode for renderedImage widget
	QList<QVariant> clickMode;
	clickMode.append((int)RenderedImage::clickFlightSpeedControl);
	mainInterface->renderedImage->setClickMode(clickMode);

	//setup of rendering engine
	cRenderJob *renderJob = new cRenderJob(gPar, gParFractal, mainInterface->mainImage, &mainInterface->stopRequest, mainInterface->mainWindow, mainInterface->renderedImage);

	renderJob->Init(cRenderJob::flightAnimRecord);
	mainInterface->stopRequest = false;

	//vector for speed and rotation control
	CVector3 cameraSpeed;
	CVector3 cameraAcceleration;
	CVector3 cameraAccelerationRotation;
	CVector3 cameraAngularSpeed;
	CVector3 cameraAngularAcceleration;
	CVector3 cameraRotation;

	int index = 0;
	if(continueRecording)
	{
		frames->GetFrameAndConsolidate(frames->GetNumberOfFrames() - 1, gPar, gParFractal);
		cameraSpeed = gPar->Get<CVector3>("flight_movement_speed_vector");
		cameraAngularSpeed = gPar->Get<CVector3>("flight_rotation_speed_vector");
		index = frames->GetNumberOfFrames() - 1;
	}

	CVector3 cameraPosition = gPar->Get<CVector3>("camera");
	CVector3 target = gPar->Get<CVector3>("target");
	CVector3 top = gPar->Get<CVector3>("camera_top");

	cCameraTarget cameraTarget(cameraPosition, target, top);

	double maxRenderTime = gPar->Get<double>("flight_sec_per_frame");;
	renderJob->SetMaxRenderTime(maxRenderTime);

	linearSpeedSp = gPar->Get<double>("flight_speed");
	enumSpeedMode speedMode = (enumSpeedMode)gPar->Get<double>("flight_speed_control");
	double rotationSpeedSp =  gPar->Get<double>("flight_rotation_speed")/100.0;
	double rollSpeedSp =  gPar->Get<double>("flight_roll_speed")/100.0;
	double inertia = gPar->Get<double>("flight_inertia");

	QString framesDir = gPar->Get<QString>("anim_flight_dir");

	recordPause = false;

	mainInterface->progressBarAnimation->show();
	while(!mainInterface->stopRequest)
	{
		ProgressStatusText(QObject::tr("Recording flight animation"), tr("Recording flight animation. Frame: ") + QString::number(index), 0.0, ui->statusbar, mainInterface->progressBarAnimation);

		bool wasPaused = false;
		while(recordPause)
		{
			wasPaused = true;
			ProgressStatusText(QObject::tr("Recording flight animation"), tr("Paused. Frame: ") + QString::number(index), 0.0, ui->statusbar, mainInterface->progressBarAnimation);
			gApplication->processEvents();
			if(mainInterface->stopRequest) break;
		}

		if(wasPaused)
		{
			//parameter refresh after pause
			mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
			renderJob->UpdateParameters(gPar, gParFractal);
			rotationSpeedSp =  gPar->Get<double>("flight_rotation_speed")/100.0;
			rollSpeedSp =  gPar->Get<double>("flight_roll_speed")/100.0;
			inertia = gPar->Get<double>("flight_inertia");
			double maxRenderTime = gPar->Get<double>("flight_sec_per_frame");;
			renderJob->SetMaxRenderTime(maxRenderTime);

			if(mainInterface->stopRequest) break;
		}

		//speed
		double linearSpeed;
		double distanceToSurface = gMainInterface->GetDistanceForPoint(cameraPosition, gPar, gParFractal);
		if(speedMode == speedRelative)
		{
			linearSpeed = distanceToSurface * linearSpeedSp;
		}
		else
		{
			linearSpeed = linearSpeedSp;
		}

		//integrator for position
		if(strafe.Length() == 0)
		{
			cameraAcceleration = (cameraTarget.GetForwardVector() * linearSpeed - cameraSpeed)/(inertia + 1.0);
		}
		else
		{
			CVector3 direction;
			if(strafe.x != 0)
			{
				direction += cameraTarget.GetRightVector() * strafe.x;
			}
			if(strafe.y != 0)
			{
				direction += cameraTarget.GetTopVector() * strafe.y;
			}
			cameraAcceleration = (direction * linearSpeed - cameraSpeed)/(inertia + 1.0);
		}
		cameraSpeed += cameraAcceleration;
		cameraPosition += cameraSpeed;

		//rotation
		cameraAngularAcceleration.x = (yawAndPitch.x * rotationSpeedSp - cameraAngularSpeed.x) / (inertia + 1.0);
		cameraAngularAcceleration.y = (yawAndPitch.y * rotationSpeedSp - cameraAngularSpeed.y) / (inertia + 1.0);
		cameraAngularAcceleration.z = (rotationDirection * rollSpeedSp - cameraAngularSpeed.z) / (inertia + 1.0);
		cameraAngularSpeed += cameraAngularAcceleration;

		CVector3 forwardVector = cameraTarget.GetForwardVector();
		forwardVector = forwardVector.RotateAroundVectorByAngle(cameraTarget.GetTopVector(), -cameraAngularSpeed.x);
		forwardVector = forwardVector.RotateAroundVectorByAngle(cameraTarget.GetRightVector(), -cameraAngularSpeed.y);

		top = cameraTarget.GetTopVector();
		top = top.RotateAroundVectorByAngle(cameraTarget.GetRightVector(), -cameraAngularSpeed.y);
		top = top.RotateAroundVectorByAngle(cameraTarget.GetForwardVector(), -cameraAngularSpeed.z);

		//update position and rotation
		target = cameraPosition + forwardVector * cameraSpeed.Length();
		cameraTarget.SetCameraTargetTop(cameraPosition, target, top);

		//update parameters
		gPar->Set("camera", cameraPosition);
		gPar->Set("target", target);
		gPar->Set("camera_top", top);
		gPar->Set("camera_rotation", cameraTarget.GetRotation()*180.0 / M_PI);
		gPar->Set("camera_distance_to_target", cameraTarget.GetDistance());
		gPar->Set("flight_movement_speed_vector", cameraSpeed);
		gPar->Set("flight_rotation_speed_vector", cameraAngularSpeed);
		gPar->Set("frame_no", index);

		mainInterface->SynchronizeInterfaceWindow(ui->dockWidget_navigation, gPar, cInterface::write);
		renderJob->ChangeCameraTargetPosition(cameraTarget);

		//add new frame to container
		frames->AddFrame(*gPar, *gParFractal);

		//add column to table
		int newColumn = AddColumn(frames->GetFrame(frames->GetNumberOfFrames() - 1));

		//update HUD
		RenderedImage::sFlightData flightData;
		flightData.frame = frames->GetNumberOfFrames();
		flightData.camera = cameraPosition;
		flightData.speed = cameraSpeed.Length();
		flightData.speedSp = linearSpeed;
		flightData.distance = distanceToSurface;
		flightData.rotation = cameraTarget.GetRotation();
		flightData.speedVector = cameraSpeed;
		flightData.forwardVector = forwardVector;
		flightData.topVector = top;

		mainInterface->renderedImage->SetFlightData(flightData);

		//render frame
		bool result = renderJob->Execute();
		if(!result) break;

		//create thumbnail
		if(ui->checkBox_flight_show_thumbnails->isChecked())
		{
			UpdateThumbnailFromImage(newColumn);
		}

		QString filename = GetFlightFilename(index);
		SaveImage(filename, (enumImageFileType)gPar->Get<int>("flight_animation_image_type"), gMainInterface->mainImage);
		index++;
	}

	//retrieve original click mode
	QList<QVariant> item = ui->comboBox_mouse_click_function->itemData(ui->comboBox_mouse_click_function->currentIndex()).toList();
	gMainInterface->renderedImage->setClickMode(item);

	UpdateLimitsForFrameRange();
	ui->spinboxInt_flight_last_to_render->setValue(frames->GetNumberOfFrames());

	delete renderJob;
}

void cFlightAnimation::UpdateThumbnailFromImage(int index)
{
	table->blockSignals(true);
	QImage qimage((const uchar*)mainInterface->mainImage->ConvertTo8bit(), mainInterface->mainImage->GetWidth(), mainInterface->mainImage->GetHeight(), mainInterface->mainImage->GetWidth()*sizeof(sRGB8), QImage::Format_RGB888);
	QPixmap pixmap;
	pixmap.convertFromImage(qimage);
	QIcon icon(pixmap.scaled(QSize(100, 70), Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
	table->setItem(0, index, new QTableWidgetItem(icon, QString()));
	table->blockSignals(false);
}

void cFlightAnimation::PrepareTable()
{
	//manual delete of all cellWidgets
	//FIXME deleting of cell widgets doesn't work properly. QTableWidgets don't free memory when clear() or removeCellWidget is used
	//It calls destructors for cell widgets only when QTable widget is destroyed.
	//even if cThumbnailWidget destructors are called, there is still some copy of widget inside the table.

//	for(int i = 1; i < table->columnCount(); i++)
//	{
//		table->removeCellWidget(0, i);
//	}
//	for(int i=0; i<thumbnailWidgets.size(); i++)
//	{
//		qDebug() << thumbnailWidgets[i];
//		delete thumbnailWidgets[i];
//	}
// thumbnailWidgets.clear();

	table->setRowCount(0);
	table->setColumnCount(0);
	table->clear();
	tableRowNames.clear();
	table->verticalHeader()->setDefaultSectionSize(gPar->Get<int>("ui_font_size") + 6);
	CreateRowsInTable();
}

void cFlightAnimation::CreateRowsInTable()
{
	QList<cAnimationFrames::sParameterDescription> parList = frames->GetListOfUsedParameters();
	table->setIconSize(QSize(100, 70));
	table->insertRow(0);
	table->setVerticalHeaderItem(0, new QTableWidgetItem(tr("preview")));
	table->setRowHeight(0, 70);
	tableRowNames.append(tr("preview"));

	rowParameter.clear();
	rowParameter.append(-1);

	parameterRows.clear();
	for (int i = 0; i < parList.size(); ++i)
	{
		int row = AddVariableToTable(parList[i], i);
		parameterRows.append(row);
	}
}

int cFlightAnimation::AddVariableToTable(const cAnimationFrames::sParameterDescription &parameterDescription, int index)
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
		rowParameter.append(index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_y";
		tableRowNames.append(varName);
		table->insertRow(row + 1);
		table->setVerticalHeaderItem(row + 1, new QTableWidgetItem(varName));
		rowParameter.append(index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_z";
		tableRowNames.append(varName);
		table->insertRow(row + 2);
		table->setVerticalHeaderItem(row + 2, new QTableWidgetItem(varName));
		rowParameter.append(index);
	}
	else if (type == typeRgb)
	{
		QString varName;
		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_R";
		tableRowNames.append(varName);
		table->insertRow(row);
		table->setVerticalHeaderItem(row, new QTableWidgetItem(varName));
		rowParameter.append(index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_G";
		tableRowNames.append(varName);
		table->insertRow(row + 1);
		table->setVerticalHeaderItem(row + 1, new QTableWidgetItem(varName));
		rowParameter.append(index);

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_B";
		tableRowNames.append(varName);
		table->insertRow(row + 2);
		table->setVerticalHeaderItem(row + 2, new QTableWidgetItem(varName));
		rowParameter.append(index);
	}
	else
	{
		QString varName = parameterDescription.containerName + "_" + parameterDescription.parameterName;
		tableRowNames.append(varName);
		table->insertRow(table->rowCount());
		table->setVerticalHeaderItem(table->rowCount() - 1, new QTableWidgetItem(varName));
		rowParameter.append(index);
	}
	return row;
}

int cFlightAnimation::AddColumn(const cAnimationFrames::sAnimationFrame &frame)
{
	table->blockSignals(true);
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
			CVector3 val = frame.parameters.Get<CVector3>(parameterName);
			table->setItem(row, newColumn, new QTableWidgetItem(QString("%L1").arg(val.x, 0, 'g', 16)));
			table->setItem(row + 1, newColumn, new QTableWidgetItem(QString("%L1").arg(val.y, 0, 'g', 16)));
			table->setItem(row + 2, newColumn, new QTableWidgetItem(QString("%L1").arg(val.z, 0, 'g', 16)));
		}
		else if (type == typeRgb)
		{
			sRGB val = frame.parameters.Get<sRGB>(parameterName);
			table->setItem(row, newColumn, new QTableWidgetItem(QString::number(val.R)));
			table->setItem(row + 1, newColumn, new QTableWidgetItem(QString::number(val.G)));
			table->setItem(row + 2, newColumn, new QTableWidgetItem(QString::number(val.B)));
		}
		else
		{
			QString val = frame.parameters.Get<QString>(parameterName);
			table->setItem(row, newColumn, new QTableWidgetItem(val));
		}
	}
	table->blockSignals(false);
	return newColumn;
}

void cFlightAnimation::RenderFlight()
{
	if(mainInterface->mainImage->IsUsed())
	{
		cErrorMessage::showMessage(QObject::tr("Rendering engine is busy. Stop unfinished rendering before starting new one"), cErrorMessage::errorMessage);
		return;
	}

	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	gUndo.Store(gPar, gParFractal, frames, NULL);

	cRenderJob *renderJob = new cRenderJob(gPar, gParFractal, mainInterface->mainImage, &mainInterface->stopRequest, mainInterface->mainWindow, mainInterface->renderedImage);

	renderJob->Init(cRenderJob::flightAnim);
	mainInterface->stopRequest = false;

	QString framesDir = gPar->Get<QString>("anim_flight_dir");

	mainInterface->progressBarAnimation->show();
	cProgressText progressText;
	progressText.ResetTimer();

	int startFrame = gPar->Get<int>("flight_first_to_render");
	int endFrame = gPar->Get<int>("flight_last_to_render");

	// Check if frames have already been rendered
	for(int index = 0; index < frames->GetNumberOfFrames(); ++index)
	{
		QString filename = GetFlightFilename(index);
		cAnimationFrames::sAnimationFrame frame = frames->GetFrame(index);
		frame.alreadyRendered = QFile(filename).exists() || index < startFrame || index >= endFrame;
		frames->ModifyFrame(index, frame);
	}

	int unrenderedTotal = frames->GetUnrenderedTotal();

	if(frames->GetNumberOfFrames() > 0 && unrenderedTotal == 0){
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(
			ui->centralwidget,
			QObject::tr("Truncate Image Folder"),
			QObject::tr("The animation has already been rendered completely.\n Do you want to purge the output folder?\n")
			+ QObject::tr("This will delete all images in the image folder.\nProceed?"),
			QMessageBox::Yes|QMessageBox::No);

		if (reply == QMessageBox::Yes)
		{
			DeleteAllFilesFromDirectory(gPar->Get<QString>("anim_flight_dir"), "frame_?????.*");
			return RenderFlight();
		}
		else
		{
			return;
		}
	}

	for(int index = 0; index < frames->GetNumberOfFrames(); ++index)
	{
		double percentDoneFrame = (frames->GetUnrenderedTillIndex(index) * 1.0) / unrenderedTotal;
		QString progressTxt = progressText.getText(percentDoneFrame);

		ProgressStatusText(QObject::tr("Animation start"), QObject::tr("Frame %1 of %2").arg((index + 1)).arg(frames->GetNumberOfFrames()) + " " + progressTxt, percentDoneFrame,
				ui->statusbar, mainInterface->progressBarAnimation);

		// Skip already rendered frames
		if (frames->GetFrame(index).alreadyRendered)
		{
			//int firstMissing = index;
			while (index < frames->GetNumberOfFrames() && frames->GetFrame(index).alreadyRendered)
			{
				index++;
			}
			index--;
			//qDebug() << QObject::tr("Skip already rendered frame(s) %1 - %2").arg(firstMissing).arg(index);
			continue;
		}

		if (mainInterface->stopRequest) break;
		frames->GetFrameAndConsolidate(index, gPar, gParFractal);
		mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);

		//show distance in statistics table
		double distance = mainInterface->GetDistanceForPoint(gPar->Get<CVector3>("camera"), gPar, gParFractal);
		ui->tableWidget_statistics->item(4, 0)->setText(QString("%L1").arg(distance));

		if(gNetRender->IsServer())
		{
			gNetRender->WaitForAllClientsReady(2.0);
		}

		gPar->Set("frame_no", index);

		renderJob->UpdateParameters(gPar, gParFractal);
		int result = renderJob->Execute();
		if (!result) break;

		QString filename = GetFlightFilename(index);
		SaveImage(filename, (enumImageFileType)gPar->Get<int>("flight_animation_image_type"), gMainInterface->mainImage);
	}
	ProgressStatusText(QObject::tr("Animation finished"), progressText.getText(1.0), 1.0, ui->statusbar, mainInterface->progressBarAnimation);
}

void cFlightAnimation::RefreshTable()
{
	mainInterface->progressBarAnimation->show();
	PrepareTable();
	gApplication->processEvents();

	int noOfFrames = frames->GetNumberOfFrames();

	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	cParameterContainer tempPar = *gPar;
	cFractalContainer tempFract = *gParFractal;

	for(int i=0; i < noOfFrames; i++)
	{
		int newColumn = AddColumn(frames->GetFrame(i));

		if(ui->checkBox_flight_show_thumbnails->isChecked())
		{
			cThumbnailWidget *thumbWidget = new cThumbnailWidget(100, 70, NULL, table);
			thumbWidget->UseOneCPUCore(true);
			frames->GetFrameAndConsolidate(i, &tempPar, &tempFract);
			thumbWidget->AssignParameters(tempPar, tempFract);
			table->setCellWidget(0, newColumn, thumbWidget);
		}
		if(i % 100 == 0)
		{
			ProgressStatusText(QObject::tr("Refreshing animation"), tr("Refreshing animation frames"), (double)i / noOfFrames, ui->statusbar, mainInterface->progressBarAnimation);
			gApplication->processEvents();
		}
	}

	UpdateLimitsForFrameRange();

	mainInterface->progressBarAnimation->hide();
}

QString cFlightAnimation::GetParameterName(int rowNumber)
{
	int parameterNumber = rowParameter[rowNumber];

	QString fullParameterName;
	QList<cAnimationFrames::sParameterDescription> list = frames->GetListOfUsedParameters();
	if(parameterNumber >= 0)
	{
		fullParameterName = list[parameterNumber].containerName + "_" + list[parameterNumber].parameterName;
	}
	else
	{
		qCritical() << "cFlightAnimation::GetParameterNumber(int rowNumber): row not found";
	}
	return fullParameterName;
}

void cFlightAnimation::RenderFrame(int index)
{
	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	frames->GetFrameAndConsolidate(index, gPar, gParFractal);
	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);

	mainInterface->StartRender();
}

void cFlightAnimation::DeleteFramesFrom(int index)
{
	gUndo.Store(gPar, gParFractal, frames, NULL);
	for(int i = frames->GetNumberOfFrames() - 1; i >= index; i--) table->removeColumn(index);
	frames->DeleteFrames(index, frames->GetNumberOfFrames() - 1);
	UpdateLimitsForFrameRange();
}

void cFlightAnimation::DeleteFramesTo(int index)
{
	gUndo.Store(gPar, gParFractal, frames, NULL);
	for(int i = 0; i <= index; i++) table->removeColumn(0);
	frames->DeleteFrames(0, index);
	UpdateLimitsForFrameRange();
}

void cFlightAnimation::slotFlightStrafe(CVector2<double> _strafe)
{
	strafe = _strafe;
}

void cFlightAnimation::slotFlightYawAndPitch(CVector2<double> _yawAndPitch)
{
	yawAndPitch = _yawAndPitch;
}

void cFlightAnimation::slotIncreaseSpeed()
{
	mainInterface->SynchronizeInterfaceWindow(ui->scrollAreaWidgetContents_flightAnimationParameters, gPar, cInterface::read);
	linearSpeedSp = gPar->Get<double>("flight_speed") * 1.1;
	gPar->Set("flight_speed", linearSpeedSp);
	mainInterface->SynchronizeInterfaceWindow(ui->scrollAreaWidgetContents_flightAnimationParameters, gPar, cInterface::write);
}

void cFlightAnimation::slotDecreaseSpeed()
{
	mainInterface->SynchronizeInterfaceWindow(ui->scrollAreaWidgetContents_flightAnimationParameters, gPar, cInterface::read);
	linearSpeedSp = gPar->Get<double>("flight_speed") * 0.9;
	gPar->Set("flight_speed", linearSpeedSp);
	mainInterface->SynchronizeInterfaceWindow(ui->scrollAreaWidgetContents_flightAnimationParameters, gPar, cInterface::write);
}

void cFlightAnimation::slotFlightRotation(double direction)
{
	rotationDirection = direction;
}

void cFlightAnimation::slotSelectAnimFlightImageDir()
{
	QFileDialog* dialog = new QFileDialog();
	dialog->setFileMode(QFileDialog::DirectoryOnly);
	dialog->setNameFilter(QObject::tr("Animation Image Folder"));
	dialog->setDirectory(gPar->Get<QString>("anim_flight_dir"));
	dialog->setAcceptMode(QFileDialog::AcceptOpen);
	dialog->setWindowTitle(QObject::tr("Choose Animation Image Folder"));
	dialog->setOption(QFileDialog::ShowDirsOnly);
	QStringList filenames;

	if(dialog->exec())
	{
		filenames = dialog->selectedFiles();
		QString filename = filenames.first() + "/";
		ui->text_anim_flight_dir->setText(filename);
		gPar->Set("anim_flight_dir", filename);
	}
}

void cFlightAnimation::slotTableCellChanged(int row, int column)
{
	if(row > 0)
	{
		table->blockSignals(true);
		QTableWidgetItem *cell = table->item(row, column);
		QString cellText = cell->text();

		cAnimationFrames::sAnimationFrame frame = frames->GetFrame(column);

		QString parameterName = GetParameterName(row);
		int parameterFirstRow = parameterRows[rowParameter[row]];
		int vectIndex = row - parameterFirstRow;

		using namespace parameterContainer;
		enumVarType type = frame.parameters.GetVarType(parameterName);

		if(type == typeVector3)
		{
			CVector3 vect = frame.parameters.Get<CVector3>(parameterName);
			if(vectIndex == 0) vect.x = systemData.locale.toDouble(cellText);
			if(vectIndex == 1) vect.y = systemData.locale.toDouble(cellText);
			if(vectIndex == 2) vect.z = systemData.locale.toDouble(cellText);
			frame.parameters.Set(parameterName, vect);
		}
		else if(type == typeRgb)
		{
			sRGB col = frame.parameters.Get<sRGB>(parameterName);
			if(vectIndex == 0) col.R = cellText.toInt();
			if(vectIndex == 1) col.G = cellText.toInt();
			if(vectIndex == 2) col.B = cellText.toInt();
			frame.parameters.Set(parameterName, col);
		}
		else
		{
			frame.parameters.Set(parameterName, cellText);
		}

		frames->ModifyFrame(column, frame);

		//update thumbnail
		if (ui->checkBox_flight_show_thumbnails->isChecked())
		{
			cParameterContainer tempPar = *gPar;
			cFractalContainer tempFract = *gParFractal;
			frames->GetFrameAndConsolidate(column, &tempPar, &tempFract);
			cThumbnailWidget *thumbWidget = (cThumbnailWidget*) table->cellWidget(0, column);

			if (!thumbWidget)
			{
				cThumbnailWidget *thumbWidget = new cThumbnailWidget(100, 70, NULL, table);
				thumbWidget->UseOneCPUCore(true);
				thumbWidget->AssignParameters(tempPar, tempFract);
				table->setCellWidget(0, column, thumbWidget);
			}
			else
			{
				thumbWidget->AssignParameters(tempPar, tempFract);
			}
		}

		table->blockSignals(false);
	}
}

void cFlightAnimation::slotDeleteAllImages()
{
	mainInterface->SynchronizeInterfaceWindow(ui->scrollAreaWidgetContents_flightAnimationParameters, gPar, cInterface::read);

	QMessageBox::StandardButton reply;
	reply = QMessageBox::question(
		ui->centralwidget,
		QObject::tr("Truncate Image Folder"),
		QObject::tr("This will delete all images in the image folder.\nProceed?"),
		QMessageBox::Yes|QMessageBox::No);

	if (reply == QMessageBox::Yes)
	{
		DeleteAllFilesFromDirectory(gPar->Get<QString>("anim_flight_dir"), "frame_?????.*");
	}
}

void cFlightAnimation::slotShowAnimation()
{
	WriteLog("Prepare PlayerWidget class");
	mainInterface->SynchronizeInterfaceWindow(ui->scrollAreaWidgetContents_keyframeAnimationParameters, gPar, cInterface::read);
	mainInterface->imageSequencePlayer = new PlayerWidget();
	mainInterface->imageSequencePlayer->SetFilePath(gPar->Get<QString>("anim_flight_dir"));
	mainInterface->imageSequencePlayer->show();
}

void cFlightAnimation::slotRecordPause()
{
	recordPause = !recordPause;
	//qDebug() << recordPause;
}

void cFlightAnimation::InterpolateForward(int row, int column)
{
	gUndo.Store(gPar, gParFractal, frames, NULL);

	QTableWidgetItem *cell = ui->tableWidget_flightAnimation->item(row, column);
	QString cellText = cell->text();

	QString parameterName = GetParameterName(row);

	cAnimationFrames::sAnimationFrame frame = frames->GetFrame(column);

	using namespace parameterContainer;
	enumVarType type = frame.parameters.GetVarType(parameterName);

	bool valueIsInteger = false;
	bool valueIsDouble = false;
	bool valueIsText = false;
	int valueInteger = 0;
	double valueDouble = 0.0;
	QString valueText;

	bool ok;
	int lastFrame = QInputDialog::getInt(mainInterface->mainWindow, "Parameter interpolation", "Enter last frame number",
			column + 1, column + 2, frames->GetNumberOfFrames(), 1, &ok);
	if(!ok) return;

	int numberOfFrames = (lastFrame - column  - 1);

	switch(type)
	{
		case typeBool:
		case typeInt:
		case typeRgb:
		{
			valueIsInteger = true;
			valueInteger = cellText.toInt();
			//qDebug() << valueInteger;
			break;
		}
		case typeDouble:
		case typeVector3:
		{
			valueIsDouble = true;
			valueDouble = systemData.locale.toDouble(cellText);
			//qDebug() << valueDouble;
			break;
		}
		default:
		{
			valueIsText = true;
			valueText = cellText;
			break;
		}
	}

	int finalInteger = 0;
	double finalDouble = 0.0;
	double integerStep = 0.0;
	double doubleStep = 0.0;

	if(valueIsInteger)
	{
		finalInteger = QInputDialog::getInt(mainInterface->mainWindow, "Parameter interpolation", "Enter value for last frame",
				valueInteger, 0, 2147483647, 1, &ok);
		integerStep = (double)(finalInteger - valueInteger) / numberOfFrames;
	}
	else if(valueIsDouble)
	{
		finalDouble = systemData.locale.toDouble(QInputDialog::getText(mainInterface->mainWindow, "Parameter interpolation", "Enter value for last frame", QLineEdit::Normal,
				QString("%L1").arg(valueDouble, 0, 'g', 16), &ok));
		doubleStep = (finalDouble - valueDouble) / numberOfFrames;
	}

	if(!ok) return;

	for(int i = column; i < lastFrame; i++)
	{
		QString newCellText;
		if(valueIsInteger)
		{
			int newValue = integerStep * i + valueInteger;
			newCellText = QString::number(newValue);
		}
		else if(valueIsDouble)
		{
			double newValue = doubleStep * (i - column) + valueDouble;
			newCellText = QString("%L1").arg(newValue, 0, 'g', 16);
		}
		else if(valueIsText)
		{
			newCellText = valueText;
		}
		QTableWidgetItem *newCell = ui->tableWidget_flightAnimation->item(row, i);
		newCell->setText(newCellText);
	}
}

void cFlightAnimation::slotRefreshTable()
{
	RefreshTable();
}

QString cFlightAnimation::GetFlightFilename(int index)
{
	QString filename = gPar->Get<QString>("anim_flight_dir") + "frame_" + QString("%1").arg(index, 5, 10, QChar('0'));
	switch((enumImageFileType)gPar->Get<double>("flight_animation_image_type"))
	{
		case IMAGE_FILE_TYPE_JPG:
			filename += QString(".jpg");
		break;
		case IMAGE_FILE_TYPE_PNG:
			filename += QString(".png");
		break;
		case IMAGE_FILE_TYPE_EXR:
			filename += QString(".exr");
		break;
	}
	return filename;
}

void cFlightAnimation::slotExportFlightToKeyframes()
{
	mainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::read);
	gUndo.Store(gPar, gParFractal, gAnimFrames, gKeyframes);

	if(gKeyframes->GetFrames().size() > 0)
	{
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(
			ui->centralwidget,
			QObject::tr("Export flight to keyframes"),
			QObject::tr("There are already captured keyframes present.\nDiscard current keyframes?"),
			QMessageBox::Yes|QMessageBox::No);

		if (reply == QMessageBox::No) return;
	}

	gKeyframes->ClearAll();
	gKeyframes->ClearMorphCache();
	gKeyframes->SetListOfParametersAndClear(gAnimFrames->GetListOfParameters());

	int step = gPar->Get<int>("frames_per_keyframe");

	for(int i=0; i < frames->GetNumberOfFrames(); i += step)
	{
		gKeyframes->AddFrame(frames->GetFrame(i));
	}

	ui->tabWidgetFlightKeyframe->setCurrentIndex(1);
	ui->pushButton_refresh_keyframe_table->animateClick();
}

void cFlightAnimation::UpdateLimitsForFrameRange(void)
{
	int noOfFrames = frames->GetNumberOfFrames();

	ui->spinboxInt_flight_first_to_render->setMaximum(noOfFrames);
	ui->sliderInt_flight_first_to_render->setMaximum(noOfFrames);

	ui->spinboxInt_flight_last_to_render->setMaximum(noOfFrames);
	ui->sliderInt_flight_last_to_render->setMaximum(noOfFrames);
}

void cFlightAnimation::slotMovedSliderFirstFrame(int value)
{
	if(value > ui->spinboxInt_flight_last_to_render->value())
		ui->spinboxInt_flight_last_to_render->setValue(value);
}
void cFlightAnimation::slotMovedSliderLastFrame(int value)
{
	if(value < ui->spinboxInt_flight_first_to_render->value())
		ui->spinboxInt_flight_first_to_render->setValue(value);
}

