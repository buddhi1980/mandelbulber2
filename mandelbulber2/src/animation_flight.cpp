/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen (jenzebas@gmail.com)
 *
 * Functions for flight animation.
 *
 * cFlightAnimation contains all functionality for capturing, editing
 * and rendering of a flight path. The class holds a cAnimationFrames to store
 * the parameters of the frames and a table to display the frames in a widget.
 * The class keeps track of current velocity and acceleration and exposes
 * slots to change the camera acceleration.
 */

#include "animation_flight.hpp"

#include "ui_dock_animation.h"

#include "animation_frames.hpp"
#include "cimage.hpp"
#include "common_math.h"
#include "files.h"
#include "global_data.hpp"
#include "headless.h"
#include "initparameters.hpp"
#include "interface.hpp"
#include "netrender.hpp"
#include "opencl_engine_render_fractal.h"
#include "opencl_global.h"
#include "render_job.hpp"
#include "render_window.hpp"
#include "rendered_image_widget.hpp"
#include "rendering_configuration.hpp"
#include "undo.h"

#include "qt/dock_animation.h"
#include "qt/dock_navigation.h"
#include "qt/dock_statistics.h"
#include "qt/my_progress_bar.h"
#include "qt/my_table_widget_anim.hpp"
#include "qt/player_widget.hpp"
#include "qt/system_tray.hpp"
#include "qt/thumbnail_widget.h"

cFlightAnimation *gFlightAnimation = nullptr;

cFlightAnimation::cFlightAnimation(cInterface *_interface, cAnimationFrames *_frames,
	cImage *_image, QWidget *_imageWidget, cParameterContainer *_params, cFractalContainer *_fractal,
	QObject *parent)
		: QObject(parent), mainInterface(_interface), frames(_frames)
{
	if (mainInterface->mainWindow)
	{
		ui = mainInterface->mainWindow->GetWidgetDockAnimation()->GetUi();

		previewSize.setWidth(systemData.GetPreferredThumbnailSize());
		previewSize.setHeight(systemData.GetPreferredThumbnailSize() * 3 / 4);

		// connect flight control buttons
		connect(ui->pushButton_record_flight, SIGNAL(clicked()), this, SLOT(slotRecordFlight()));
		connect(
			ui->pushButton_continue_recording, SIGNAL(clicked()), this, SLOT(slotContinueRecording()));
		connect(ui->pushButton_render_flight, SIGNAL(clicked()), this, SLOT(slotRenderFlight()));
		connect(ui->pushButton_delete_all_images, SIGNAL(clicked()), this, SLOT(slotDeleteAllImages()));
		connect(ui->pushButton_show_animation, SIGNAL(clicked()), this, SLOT(slotShowAnimation()));
		connect(ui->pushButton_flight_refresh_table, SIGNAL(clicked()), this, SLOT(slotRefreshTable()));
		connect(ui->pushButton_flight_to_keyframe_export, SIGNAL(clicked()), this,
			SLOT(slotExportFlightToKeyframes()));
		connect(ui->button_selectAnimFlightImageDir, SIGNAL(clicked()), this,
			SLOT(slotSelectAnimFlightImageDir()));
		connect(ui->tableWidget_flightAnimation, SIGNAL(cellChanged(int, int)), this,
			SLOT(slotTableCellChanged(int, int)));
		connect(ui->spinboxInt_flight_first_to_render, SIGNAL(valueChanged(int)), this,
			SLOT(slotMovedSliderFirstFrame(int)));
		connect(ui->spinboxInt_flight_last_to_render, SIGNAL(valueChanged(int)), this,
			SLOT(slotMovedSliderLastFrame(int)));
		connect(ui->tableWidget_flightAnimation, SIGNAL(cellDoubleClicked(int, int)), this,
			SLOT(slotCellDoubleClicked(int, int)));
		connect(mainInterface->renderedImage, SIGNAL(ShiftModeChanged(bool)), this,
			SLOT(slotOrthogonalStrafe(bool)));

		// connect renderedImage signals
		connect(mainInterface->renderedImage, SIGNAL(StrafeChanged(CVector2<double>)), this,
			SLOT(slotFlightStrafe(CVector2<double>)));
		connect(mainInterface->renderedImage, SIGNAL(YawAndPitchChanged(CVector2<double>)), this,
			SLOT(slotFlightYawAndPitch(CVector2<double>)));
		connect(mainInterface->renderedImage, SIGNAL(SpeedChanged(double)), this,
			SLOT(slotFlightChangeSpeed(double)));
		connect(mainInterface->renderedImage, SIGNAL(SpeedSet(double)), this,
			SLOT(slotFlightSetSpeed(double)));
		connect(mainInterface->renderedImage, SIGNAL(RotationChanged(double)), this,
			SLOT(slotFlightRotation(double)));
		connect(mainInterface->renderedImage, SIGNAL(Pause()), this, SLOT(slotRecordPause()));

		// connect system tray
		connect(
			mainInterface->systemTray, SIGNAL(notifyRenderFlight()), this, SLOT(slotRenderFlight()));
		connect(this, SIGNAL(notifyRenderFlightRenderStatus(QString, QString)),
			mainInterface->systemTray, SLOT(showMessage(QString, QString)));

		// connect QuestionMessage signal
		connect(this, SIGNAL(QuestionMessage(const QString, const QString, QMessageBox::StandardButtons,
										QMessageBox::StandardButton *)),
			mainInterface->mainWindow, SLOT(slotQuestionMessage(const QString, const QString,
																	 QMessageBox::StandardButtons, QMessageBox::StandardButton *)));

		table = ui->tableWidget_flightAnimation;
	}
	else
	{
		ui = nullptr;
		table = nullptr;
	}

	connect(this, SIGNAL(showErrorMessage(QString, cErrorMessage::enumMessageType, QWidget *)),
		gErrorMessage, SLOT(slotShowMessage(QString, cErrorMessage::enumMessageType, QWidget *)));

	image = _image;
	imageWidget = _imageWidget;
	params = _params;
	fractalParams = _fractal;
	linearSpeedSp = 0.0;
	rotationDirection = 0;
	recordPause = false;
	orthogonalStrafe = false;
	negativeFlightSpeed = false;
}

void cFlightAnimation::slotRecordFlight()
{
	if (frames)
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
	if (frames)
	{
		RecordFlight(true);
	}
	else
	{
		qCritical() << "gAnimFrames not allocated";
	}
}

bool cFlightAnimation::slotRenderFlight()
{
	// get latest values of all parameters
	if (mainInterface->mainWindow)
	{
		mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	}

	if (frames)
	{
		if (frames->GetNumberOfFrames() == 0)
		{
			emit showErrorMessage(QObject::tr("No frames to render"), cErrorMessage::errorMessage,
				mainInterface->mainWindow->GetCentralWidget());
		}
		else if (!QDir(params->Get<QString>("anim_flight_dir")).exists())
		{
			emit showErrorMessage(
				QObject::tr("The folder %1 does not exist. Please specify a valid location.")
					.arg(params->Get<QString>("anim_flight_dir")),
				cErrorMessage::errorMessage, mainInterface->mainWindow->GetCentralWidget());
		}
		else
		{
			return RenderFlight(&gMainInterface->stopRequest);
		}
	}
	else
	{
		qCritical() << "gAnimFrames not allocated";
	}
	return false;
}

void cFlightAnimation::RecordFlight(bool continueRecording)
{
	mainInterface->DisablePeriodicRefresh();

	// get latest values of all parameters
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	gUndo.Store(params, fractalParams, frames, nullptr);

	if (!continueRecording)
	{
		const QMessageBox::StandardButton reply =
			QMessageBox::question(mainInterface->mainWindow->GetCentralWidget(),
				QObject::tr("Are you sure to start recording of new animation?"),
				QObject::tr("This will delete all images in the image folder.\nProceed?"),
				QMessageBox::Yes | QMessageBox::No);

		if (reply == QMessageBox::Yes)
		{
			cAnimationFrames::WipeFramesFromFolder(params->Get<QString>("anim_flight_dir"));
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
			cErrorMessage::showMessage(
				QObject::tr("No frames recorded before. Unable to continue."), cErrorMessage::errorMessage);
			return;
		}
	}

	// check if main image is not used by other rendering process
	if (image->IsUsed())
	{
		cErrorMessage::showMessage(
			QObject::tr("Rendering engine is busy. Stop unfinished rendering before starting new one"),
			cErrorMessage::errorMessage);
		return;
	}

	mainInterface->stopRequest = false;
	for (int i = 0; i < 30; i++)
	{
		if (mainInterface->stopRequest)
		{
			emit updateProgressHide();
			return;
		}
		emit updateProgressAndStatus(QObject::tr("Recording flight path"),
			tr("waiting %1 seconds").arg(QString::number(3.0 - 0.1 * i)), 0.0,
			cProgressText::progress_ANIMATION);
		gApplication->processEvents();
		Wait(100);
	}

	if (!continueRecording)
	{
		frames->Clear();

		const bool addSpeeds = params->Get<bool>("flight_add_speeds");

		// add default parameters for animation
		if (frames->GetListOfUsedParameters().size() == 0)
		{
			gAnimFrames->AddAnimatedParameter("camera", params->GetAsOneParameter("camera"), params);
			gAnimFrames->AddAnimatedParameter("target", params->GetAsOneParameter("target"), params);
			gAnimFrames->AddAnimatedParameter(
				"camera_top", params->GetAsOneParameter("camera_top"), params);
			if (addSpeeds)
			{
				{
					gAnimFrames->AddAnimatedParameter("flight_movement_speed_vector",
						params->GetAsOneParameter("flight_movement_speed_vector"), params);
					gAnimFrames->AddAnimatedParameter("flight_rotation_speed_vector",
						params->GetAsOneParameter("flight_rotation_speed_vector"), params);
				}
			}
		}

		PrepareTable();
	}

	// setup cursor mode for renderedImage widget
	QList<QVariant> clickMode;
	clickMode.append(int(RenderedImage::clickFlightSpeedControl));
	mainInterface->renderedImage->setClickMode(clickMode);

	// setup of rendering engine
	cRenderJob *renderJob = new cRenderJob(params, fractalParams, mainInterface->mainImage,
		&mainInterface->stopRequest, mainInterface->renderedImage);
	connect(renderJob, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
		this, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
	connect(
		renderJob, SIGNAL(updateStatistics(cStatistics)), this, SIGNAL(updateStatistics(cStatistics)));
	connect(renderJob, SIGNAL(updateImage()), mainInterface->renderedImage, SLOT(update()));

	cRenderingConfiguration config;
	config.DisableRefresh();
	config.SetMaxRenderTime(params->Get<double>("flight_sec_per_frame"));

	renderJob->Init(cRenderJob::flightAnimRecord, config);
	mainInterface->stopRequest = false;

	// vector for speed and rotation control
	CVector3 cameraSpeed;
	CVector3 cameraAcceleration;
	CVector3 cameraAngularSpeed;
	CVector3 cameraAngularAcceleration;

	int index = 0;
	if (continueRecording)
	{
		frames->GetFrameAndConsolidate(frames->GetNumberOfFrames() - 1, params, fractalParams);
		cameraSpeed = params->Get<CVector3>("flight_movement_speed_vector");
		cameraAngularSpeed = params->Get<CVector3>("flight_rotation_speed_vector");
		index = frames->GetNumberOfFrames() - 1;
	}

	CVector3 cameraPosition = params->Get<CVector3>("camera");
	CVector3 target = params->Get<CVector3>("target");
	CVector3 top = params->Get<CVector3>("camera_top");

	cCameraTarget cameraTarget(cameraPosition, target, top);

	linearSpeedSp = params->Get<double>("flight_speed");
	const enumSpeedMode speedMode = enumSpeedMode(params->Get<int>("flight_speed_control"));
	double rotationSpeedSp = params->Get<double>("flight_rotation_speed") / 100.0;
	double rollSpeedSp = params->Get<double>("flight_roll_speed") / 100.0;
	double inertia = params->Get<double>("flight_inertia");

	recordPause = false;

	mainInterface->mainWindow->GetWidgetDockNavigation()->LockAllFunctions();

	mainInterface->progressBarAnimation->show();

	while (!mainInterface->stopRequest)
	{
		emit updateProgressAndStatus(QObject::tr("Recording flight animation"),
			tr("Recording flight animation. Frame: ") + QString::number(index), 0.0,
			cProgressText::progress_ANIMATION);

		bool wasPaused = false;
		while (recordPause)
		{
			wasPaused = true;
			emit updateProgressAndStatus(QObject::tr("Recording flight animation"),
				tr("Paused. Frame: ") + QString::number(index), 0.0, cProgressText::progress_ANIMATION);
			gApplication->processEvents();
			if (mainInterface->stopRequest) break;
		}

		if (wasPaused)
		{
			// parameter refresh after pause
			mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
			renderJob->UpdateParameters(params, fractalParams);
			rotationSpeedSp = params->Get<double>("flight_rotation_speed") / 100.0;
			rollSpeedSp = params->Get<double>("flight_roll_speed") / 100.0;
			inertia = params->Get<double>("flight_inertia");

			config.SetMaxRenderTime(params->Get<double>("flight_sec_per_frame"));
			renderJob->UpdateConfig(config);

			if (mainInterface->stopRequest) break;
		}

		const double distanceToSurface =
			gMainInterface->GetDistanceForPoint(cameraPosition, params, fractalParams);

		// speed
		double linearSpeed = 0;
		switch (speedMode)
		{
			case speedRelative: linearSpeed = distanceToSurface * linearSpeedSp; break;
			case speedConstant: linearSpeed = linearSpeedSp; break;
		}

		// integrator for position
		if (strafe.Length() == 0)
		{
			double goForward = (negativeFlightSpeed) ? -1.0 : 1.0;
			cameraAcceleration =
				(cameraTarget.GetForwardVector() * goForward * linearSpeed - cameraSpeed) / (inertia + 1.0);
		}
		else
		{
			CVector3 direction;
			if (!orthogonalStrafe) direction = cameraTarget.GetForwardVector();

			if (strafe.x != 0)
			{
				direction += cameraTarget.GetRightVector() * strafe.x;
			}
			if (strafe.y != 0)
			{
				direction += cameraTarget.GetTopVector() * strafe.y;
			}
			cameraAcceleration = (direction * linearSpeed - cameraSpeed) / (inertia + 1.0);
		}
		cameraSpeed += cameraAcceleration;
		cameraPosition += cameraSpeed;

		// rotation
		cameraAngularAcceleration.x =
			(yawAndPitch.x * rotationSpeedSp - cameraAngularSpeed.x) / (inertia + 1.0);
		cameraAngularAcceleration.y =
			(yawAndPitch.y * rotationSpeedSp - cameraAngularSpeed.y) / (inertia + 1.0);
		cameraAngularAcceleration.z =
			(rotationDirection * rollSpeedSp - cameraAngularSpeed.z) / (inertia + 1.0);
		cameraAngularSpeed += cameraAngularAcceleration;

		CVector3 forwardVector = cameraTarget.GetForwardVector();
		forwardVector =
			forwardVector.RotateAroundVectorByAngle(cameraTarget.GetTopVector(), -cameraAngularSpeed.x);
		forwardVector =
			forwardVector.RotateAroundVectorByAngle(cameraTarget.GetRightVector(), -cameraAngularSpeed.y);

		top = cameraTarget.GetTopVector();
		top = top.RotateAroundVectorByAngle(cameraTarget.GetRightVector(), -cameraAngularSpeed.y);
		top = top.RotateAroundVectorByAngle(cameraTarget.GetForwardVector(), -cameraAngularSpeed.z);

		// update position and rotation
		target = cameraPosition + forwardVector * cameraSpeed.Length();
		cameraTarget.SetCameraTargetTop(cameraPosition, target, top);

		// update parameters
		params->Set("camera", cameraPosition);
		params->Set("target", target);
		params->Set("camera_top", top);
		params->Set("camera_rotation", cameraTarget.GetRotation() * 180.0 / M_PI);
		params->Set("camera_distance_to_target", cameraTarget.GetDistance());
		params->Set("flight_movement_speed_vector", cameraSpeed);
		params->Set("flight_rotation_speed_vector", cameraAngularSpeed);
		params->Set("frame_no", index);

		SynchronizeInterfaceWindow(
			mainInterface->mainWindow->GetWidgetDockNavigation(), params, qInterface::write);
		renderJob->ChangeCameraTargetPosition(cameraTarget);

		// add new frame to container
		frames->AddFrame(*params, *fractalParams);

		// add column to table
		const int newColumn = AddColumn(frames->GetFrame(frames->GetNumberOfFrames() - 1));

		// update HUD
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

		// render frame
		const bool result = renderJob->Execute();
		if (!result) break;

		// create thumbnail
		if (ui->checkBox_flight_show_thumbnails->isChecked())
		{
			UpdateThumbnailFromImage(newColumn);
		}

		const QString filename = GetFlightFilename(index);
		const ImageFileSave::enumImageFileType fileType =
			ImageFileSave::enumImageFileType(params->Get<int>("flight_animation_image_type"));
		SaveImage(filename, fileType, image, gMainInterface->mainWindow);

		gApplication->processEvents();

		index++;
	}

	if (!systemData.noGui && image->IsMainImage())
		mainInterface->mainWindow->GetWidgetDockNavigation()->UnlockAllFunctions();

	// retrieve original click mode
	const QList<QVariant> item =
		mainInterface->mainWindow->GetComboBoxMouseClickFunction()
			->itemData(mainInterface->mainWindow->GetComboBoxMouseClickFunction()->currentIndex())
			.toList();
	gMainInterface->renderedImage->setClickMode(item);

	UpdateLimitsForFrameRange();
	ui->spinboxInt_flight_last_to_render->setValue(frames->GetNumberOfFrames());

	delete renderJob;
}

void cFlightAnimation::UpdateThumbnailFromImage(int index) const
{
	table->blockSignals(true);
	const QImage qImage(static_cast<const uchar *>(image->ConvertTo8bit()), image->GetWidth(),
		image->GetHeight(), image->GetWidth() * sizeof(sRGB8), QImage::Format_RGB888);
	QPixmap pixmap;
	pixmap.convertFromImage(qImage);
	const QIcon icon(
		pixmap.scaled(previewSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
	table->setItem(0, index, new QTableWidgetItem(icon, QString()));
	table->blockSignals(false);
}

void cFlightAnimation::PrepareTable()
{
	table->setRowCount(0);
	table->setColumnCount(0);
	table->clear();
	tableRowNames.clear();
	QFontMetrics fm(mainInterface->mainWindow->font());
	table->verticalHeader()->setDefaultSectionSize(fm.height() + 6);
	CreateRowsInTable();
}

void cFlightAnimation::CreateRowsInTable()
{
	QList<cAnimationFrames::sParameterDescription> parList = frames->GetListOfUsedParameters();
	table->setIconSize(previewSize);
	table->insertRow(0);
	table->setVerticalHeaderItem(0, new QTableWidgetItem(tr("preview")));
	table->setRowHeight(0, previewSize.height());
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

int cFlightAnimation::AddVariableToTable(
	const cAnimationFrames::sParameterDescription &parameterDescription, int index)
{
	using namespace parameterContainer;
	const enumVarType type = parameterDescription.varType;
	const int row = table->rowCount();
	if (type == typeVector3)
	{
		QString varName =
			parameterDescription.containerName + "_" + parameterDescription.parameterName + "_x";
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
	else if (type == typeVector4)
	{
		QString varName =
			parameterDescription.containerName + "_" + parameterDescription.parameterName + "_x";
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

		varName = parameterDescription.containerName + "_" + parameterDescription.parameterName + "_w";
		tableRowNames.append(varName);
		table->insertRow(row + 3);
		table->setVerticalHeaderItem(row + 3, new QTableWidgetItem(varName));
		rowParameter.append(index);
	}
	else if (type == typeRgb)
	{
		QString varName =
			parameterDescription.containerName + "_" + parameterDescription.parameterName + "_R";
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
		const QString varName =
			parameterDescription.containerName + "_" + parameterDescription.parameterName;
		tableRowNames.append(varName);
		table->insertRow(table->rowCount());
		table->setVerticalHeaderItem(table->rowCount() - 1, new QTableWidgetItem(varName));
		rowParameter.append(index);
	}
	return row;
}

int cFlightAnimation::AddColumn(
	const cAnimationFrames::sAnimationFrame &frame, int indexOfExistingColumn)
{
	table->blockSignals(true);

	int newColumn;
	if (indexOfExistingColumn >= 0)
	{
		newColumn = indexOfExistingColumn;
	}
	else
	{
		newColumn = table->columnCount();
		table->insertColumn(newColumn);
	}

	table->setColumnWidth(newColumn, previewSize.width());

	QList<cAnimationFrames::sParameterDescription> parList = frames->GetListOfUsedParameters();

	using namespace parameterContainer;
	for (int i = 0; i < parList.size(); ++i)
	{
		const QString parameterName = parList[i].containerName + "_" + parList[i].parameterName;
		const enumVarType type = parList[i].varType;
		const int row = parameterRows[i];

		if (type == typeVector3)
		{
			const CVector3 val = frame.parameters.Get<CVector3>(parameterName);
			table->setItem(row, newColumn, new QTableWidgetItem(QString("%L1").arg(val.x, 0, 'g', 16)));
			table->setItem(
				row + 1, newColumn, new QTableWidgetItem(QString("%L1").arg(val.y, 0, 'g', 16)));
			table->setItem(
				row + 2, newColumn, new QTableWidgetItem(QString("%L1").arg(val.z, 0, 'g', 16)));
		}
		else if (type == typeVector4)
		{
			const CVector4 val = frame.parameters.Get<CVector4>(parameterName);
			table->setItem(row, newColumn, new QTableWidgetItem(QString("%L1").arg(val.x, 0, 'g', 16)));
			table->setItem(
				row + 1, newColumn, new QTableWidgetItem(QString("%L1").arg(val.y, 0, 'g', 16)));
			table->setItem(
				row + 2, newColumn, new QTableWidgetItem(QString("%L1").arg(val.z, 0, 'g', 16)));
			table->setItem(
				row + 3, newColumn, new QTableWidgetItem(QString("%L1").arg(val.w, 0, 'g', 16)));
		}
		else if (type == typeRgb)
		{
			const sRGB val = frame.parameters.Get<sRGB>(parameterName);
			table->setItem(row, newColumn, new QTableWidgetItem(QString::number(val.R)));
			table->setItem(row + 1, newColumn, new QTableWidgetItem(QString::number(val.G)));
			table->setItem(row + 2, newColumn, new QTableWidgetItem(QString::number(val.B)));
		}
		else
		{
			const QString val = frame.parameters.Get<QString>(parameterName);
			table->setItem(row, newColumn, new QTableWidgetItem(val));
		}
	}
	table->blockSignals(false);
	return newColumn;
}

bool cFlightAnimation::RenderFlight(bool *stopRequest)
{
	mainInterface->DisablePeriodicRefresh();

	if (image->IsUsed())
	{
		emit showErrorMessage(
			QObject::tr("Rendering engine is busy. Stop unfinished rendering before starting new one"),
			cErrorMessage::errorMessage);
		return false;
	}

	if (!systemData.noGui && image->IsMainImage())
	{
		mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
		gUndo.Store(params, fractalParams, frames, nullptr);
	}

	cRenderJob *renderJob = new cRenderJob(params, fractalParams, image, stopRequest, imageWidget);

	connect(renderJob, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
		this, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
	connect(
		renderJob, SIGNAL(updateStatistics(cStatistics)), this, SIGNAL(updateStatistics(cStatistics)));
	connect(renderJob, SIGNAL(updateImage()), mainInterface->renderedImage, SLOT(update()));

	cRenderingConfiguration config;
	config.EnableNetRender();

	if (systemData.noGui)
	{
		config.DisableRefresh();
		config.DisableProgressiveRender();
		config.EnableNetRender();
	}

	renderJob->Init(cRenderJob::flightAnim, config);
	*stopRequest = false;

	cProgressText progressText;
	progressText.ResetTimer();

	if (!systemData.noGui && image->IsMainImage())
		mainInterface->mainWindow->GetWidgetDockNavigation()->LockAllFunctions();

	try
	{
		const int startFrame = params->Get<int>("flight_first_to_render");
		int endFrame = params->Get<int>("flight_last_to_render");
		if (endFrame == 0) endFrame = frames->GetNumberOfFrames();

		// Check if frames have already been rendered
		for (int index = 0; index < frames->GetNumberOfFrames(); ++index)
		{
			const QString filename = GetFlightFilename(index);
			cAnimationFrames::sAnimationFrame frame = frames->GetFrame(index);
			frame.alreadyRendered = QFile(filename).exists() || index < startFrame || index >= endFrame;
			frames->ModifyFrame(index, frame);
		}

		const int unrenderedTotal = frames->GetUnrenderedTotal();

		if (frames->GetNumberOfFrames() > 0 && unrenderedTotal == 0)
		{
			bool deletePreviousRender;
			const QString questionTitle = QObject::tr("Truncate Image Folder");
			const QString questionText =
				QObject::tr(
					"The animation has already been rendered completely.\n Do you want to purge "
					"the output "
					"folder?\n")
				+ QObject::tr("This will delete all images in the image folder.\nProceed?");

			if (!systemData.noGui)
			{
				QMessageBox::StandardButton reply = QMessageBox::NoButton;
				emit QuestionMessage(
					questionTitle, questionText, QMessageBox::Yes | QMessageBox::No, &reply);
				while (reply == QMessageBox::NoButton)
				{
					gApplication->processEvents();
				}
				deletePreviousRender = (reply == QMessageBox::Yes);
			}
			else
			{
				// Exit if silent mode
				if (systemData.silent)
				{
					exit(0);
				}

				deletePreviousRender = cHeadless::ConfirmMessage(questionTitle + "\n" + questionText);
			}

			if (deletePreviousRender)
			{
				cAnimationFrames::WipeFramesFromFolder(params->Get<QString>("anim_flight_dir"));
				if (!systemData.noGui && image->IsMainImage())
					mainInterface->mainWindow->GetWidgetDockNavigation()->UnlockAllFunctions();

				return RenderFlight(stopRequest);
			}
			else
			{
				throw false;
			}
		}

		for (int index = 0; index < frames->GetNumberOfFrames(); ++index)
		{

			double percentDoneFrame;
			if (unrenderedTotal > 0)
				percentDoneFrame = (frames->GetUnrenderedTillIndex(index) * 1.0) / unrenderedTotal;
			else
				percentDoneFrame = 1.0;

			const QString progressTxt = progressText.getText(percentDoneFrame);

			// Skip already rendered frames
			if (frames->GetFrame(index).alreadyRendered)
			{
				// int firstMissing = index;
				while (index < frames->GetNumberOfFrames() && frames->GetFrame(index).alreadyRendered)
				{
					index++;
				}
				index--;
				// qDebug() << QObject::tr("Skip already rendered frame(s) %1 -
				// %2").arg(firstMissing).arg(index);
				continue;
			}

			emit updateProgressAndStatus(QObject::tr("Animation start"),
				QObject::tr("Frame %1 of %2").arg((index + 1)).arg(frames->GetNumberOfFrames()) + " "
					+ progressTxt,
				percentDoneFrame, cProgressText::progress_ANIMATION);

			if (*stopRequest) throw false;

			frames->GetFrameAndConsolidate(index, params, fractalParams);

			if (!systemData.noGui && image->IsMainImage())
			{
				mainInterface->SynchronizeInterface(params, fractalParams, qInterface::write);

				// show distance in statistics table
				const double distance = mainInterface->GetDistanceForPoint(
					params->Get<CVector3>("camera"), params, fractalParams);
				mainInterface->mainWindow->GetWidgetDockStatistics()->UpdateDistanceToFractal(distance);
			}

			if (gNetRender->IsServer())
			{
				gNetRender->WaitForAllClientsReady(10.0);
			}

			params->Set("frame_no", index);

			renderJob->UpdateParameters(params, fractalParams);
			const int result = renderJob->Execute();
			if (!result) throw false;

			const QString filename = GetFlightFilename(index);
			const ImageFileSave::enumImageFileType fileType =
				ImageFileSave::enumImageFileType(params->Get<int>("flight_animation_image_type"));
			SaveImage(filename, fileType, image, gMainInterface->mainWindow);
		}

		emit updateProgressAndStatus(QObject::tr("Animation finished"), progressText.getText(1.0), 1.0,
			cProgressText::progress_IMAGE);
		emit notifyRenderFlightRenderStatus(
			QObject::tr("Animation finished"), progressText.getText(1.0));
		emit updateProgressHide();
	}
	catch (bool ex)
	{
		QString resultStatus = QObject::tr("Rendering terminated");
		if (ex) resultStatus += " - " + QObject::tr("Error occured, see log output");
		emit updateProgressAndStatus(
			resultStatus, progressText.getText(1.0), cProgressText::progress_ANIMATION);
		emit updateProgressHide();
		delete renderJob;

		if (!systemData.noGui && image->IsMainImage())
			mainInterface->mainWindow->GetWidgetDockNavigation()->UnlockAllFunctions();

		return false;
	}

	if (!systemData.noGui && image->IsMainImage())
		mainInterface->mainWindow->GetWidgetDockNavigation()->UnlockAllFunctions();

	delete renderJob;
	return true;
}

void cFlightAnimation::RefreshTable()
{
	PrepareTable();
	gApplication->processEvents();

	const int noOfFrames = frames->GetNumberOfFrames();

	// it is needed to do it also here, because limits must be set just after loading of settings
	UpdateLimitsForFrameRange();

	SynchronizeInterfaceWindow(ui->tab_flight_animation, params, qInterface::read);
	cParameterContainer tempPar = *params;
	cFractalContainer tempFract = *fractalParams;

	table->setColumnCount(noOfFrames);

	for (int i = 0; i < noOfFrames; i++)
	{
		const int newColumn = AddColumn(frames->GetFrame(i), i);

		if (ui->checkBox_flight_show_thumbnails->isChecked())
		{
			cThumbnailWidget *thumbWidget =
				new cThumbnailWidget(previewSize.width(), previewSize.height(), 1, table);
			thumbWidget->UseOneCPUCore(true);
			frames->GetFrameAndConsolidate(i, &tempPar, &tempFract);
			thumbWidget->AssignParameters(tempPar, tempFract);
			table->setCellWidget(0, newColumn, thumbWidget);
		}
		if (i % 100 == 0)
		{
			emit updateProgressAndStatus(QObject::tr("Refreshing animation"),
				tr("Refreshing animation frames"), double(i) / noOfFrames,
				cProgressText::progress_ANIMATION);
			gApplication->processEvents();
		}
	}
	UpdateLimitsForFrameRange();
	emit updateProgressHide();
}

QString cFlightAnimation::GetParameterName(int rowNumber)
{
	const int parameterNumber = rowParameter[rowNumber];

	QString fullParameterName;
	QList<cAnimationFrames::sParameterDescription> list = frames->GetListOfUsedParameters();
	if (parameterNumber >= 0)
	{
		fullParameterName =
			list[parameterNumber].containerName + "_" + list[parameterNumber].parameterName;
	}
	else
	{
		qCritical() << "cFlightAnimation::GetParameterNumber(int rowNumber): row not found";
	}
	return fullParameterName;
}

void cFlightAnimation::RenderFrame(int index) const
{
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	frames->GetFrameAndConsolidate(index, params, fractalParams);
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::write);

	mainInterface->StartRender();
}

void cFlightAnimation::DeleteFramesFrom(int index) const
{
	gUndo.Store(params, fractalParams, frames, nullptr);
	for (int i = frames->GetNumberOfFrames() - 1; i >= index; i--)
		table->removeColumn(index);
	frames->DeleteFrames(index, frames->GetNumberOfFrames() - 1);
	UpdateLimitsForFrameRange();
}

void cFlightAnimation::DeleteFramesTo(int index) const
{
	gUndo.Store(params, fractalParams, frames, nullptr);
	for (int i = 0; i <= index; i++)
		table->removeColumn(0);
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

void cFlightAnimation::slotFlightChangeSpeed(double amount)
{
	SynchronizeInterfaceWindow(
		ui->scrollAreaWidgetContents_flightAnimationParameters, params, qInterface::read);
	linearSpeedSp = params->Get<double>("flight_speed") * amount;
	params->Set("flight_speed", linearSpeedSp);
	SynchronizeInterfaceWindow(
		ui->scrollAreaWidgetContents_flightAnimationParameters, params, qInterface::write);
}

void cFlightAnimation::slotFlightSetSpeed(double amount)
{
	SynchronizeInterfaceWindow(
		ui->scrollAreaWidgetContents_flightAnimationParameters, params, qInterface::read);
	negativeFlightSpeed = (amount < 0) ? true : false;
	linearSpeedSp = fabs(amount);
	params->Set("flight_speed", linearSpeedSp);
	SynchronizeInterfaceWindow(
		ui->scrollAreaWidgetContents_flightAnimationParameters, params, qInterface::write);
}

void cFlightAnimation::slotFlightRotation(double direction)
{
	rotationDirection = direction;
}

void cFlightAnimation::slotOrthogonalStrafe(bool _orthogonalStrafe)
{
	orthogonalStrafe = _orthogonalStrafe;
}

void cFlightAnimation::slotSelectAnimFlightImageDir() const
{
	QFileDialog *dialog = new QFileDialog();
	dialog->setFileMode(QFileDialog::DirectoryOnly);
	dialog->setNameFilter(QObject::tr("Animation Image Folder"));
	dialog->setDirectory(QDir::toNativeSeparators(params->Get<QString>("anim_flight_dir")));
	dialog->setAcceptMode(QFileDialog::AcceptOpen);
	dialog->setWindowTitle(QObject::tr("Choose Animation Image Folder"));
	dialog->setOption(QFileDialog::ShowDirsOnly);

	if (dialog->exec())
	{
		QStringList fileNames = dialog->selectedFiles();
		const QString filename = QDir::toNativeSeparators(fileNames.first() + QDir::separator());
		ui->text_anim_flight_dir->setText(filename);
		params->Set("anim_flight_dir", filename);
	}
}

void cFlightAnimation::slotTableCellChanged(int row, int column)
{
	if (row > 0)
	{
		table->blockSignals(true);
		QTableWidgetItem *cell = table->item(row, column);
		QString cellText = cell->text();

		cAnimationFrames::sAnimationFrame frame = frames->GetFrame(column);

		const QString parameterName = GetParameterName(row);
		const int parameterFirstRow = parameterRows[rowParameter[row]];
		const int vectIndex = row - parameterFirstRow;

		using namespace parameterContainer;
		const enumVarType type = frame.parameters.GetVarType(parameterName);

		if (type == typeVector3)
		{
			CVector3 vect = frame.parameters.Get<CVector3>(parameterName);
			if (vectIndex == 0) vect.x = systemData.locale.toDouble(cellText);
			if (vectIndex == 1) vect.y = systemData.locale.toDouble(cellText);
			if (vectIndex == 2) vect.z = systemData.locale.toDouble(cellText);
			frame.parameters.Set(parameterName, vect);
		}
		else if (type == typeVector4)
		{
			CVector4 vect = frame.parameters.Get<CVector4>(parameterName);
			if (vectIndex == 0) vect.x = systemData.locale.toDouble(cellText);
			if (vectIndex == 1) vect.y = systemData.locale.toDouble(cellText);
			if (vectIndex == 2) vect.z = systemData.locale.toDouble(cellText);
			if (vectIndex == 3) vect.w = systemData.locale.toDouble(cellText);
			frame.parameters.Set(parameterName, vect);
		}
		else if (type == typeRgb)
		{
			sRGB col = frame.parameters.Get<sRGB>(parameterName);
			if (vectIndex == 0) col.R = cellText.toInt();
			if (vectIndex == 1) col.G = cellText.toInt();
			if (vectIndex == 2) col.B = cellText.toInt();
			frame.parameters.Set(parameterName, col);
		}
		else
		{
			frame.parameters.Set(parameterName, cellText);
		}

		frames->ModifyFrame(column, frame);

		// update thumbnail
		if (ui->checkBox_flight_show_thumbnails->isChecked())
		{
			cParameterContainer tempPar = *params;
			cFractalContainer tempFract = *fractalParams;
			frames->GetFrameAndConsolidate(column, &tempPar, &tempFract);
			cThumbnailWidget *thumbWidget = static_cast<cThumbnailWidget *>(table->cellWidget(0, column));

			if (!thumbWidget)
			{
				thumbWidget = new cThumbnailWidget(previewSize.width(), previewSize.height(), 1, table);
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

void cFlightAnimation::slotDeleteAllImages() const
{
	SynchronizeInterfaceWindow(
		ui->scrollAreaWidgetContents_flightAnimationParameters, params, qInterface::read);

	const QMessageBox::StandardButton reply = QMessageBox::question(
		mainInterface->mainWindow->GetCentralWidget(), QObject::tr("Truncate Image Folder"),
		QObject::tr("This will delete all images in the image folder.\nProceed?"),
		QMessageBox::Yes | QMessageBox::No);

	if (reply == QMessageBox::Yes)
	{
		cAnimationFrames::WipeFramesFromFolder(params->Get<QString>("anim_flight_dir"));
	}
}

void cFlightAnimation::slotShowAnimation() const
{
	WriteLog("Prepare PlayerWidget class", 2);

	SynchronizeInterfaceWindow(
		ui->scrollAreaWidgetContents_keyframeAnimationParameters, params, qInterface::read);

	if (!mainInterface->imageSequencePlayer)
	{
		mainInterface->imageSequencePlayer = new PlayerWidget;
	}

	mainInterface->imageSequencePlayer->SetFilePath(params->Get<QString>("anim_flight_dir"));
	mainInterface->imageSequencePlayer->show();
	mainInterface->imageSequencePlayer->raise();
	mainInterface->imageSequencePlayer->activateWindow();
}

void cFlightAnimation::slotRecordPause()
{
	recordPause = !recordPause;
	// qDebug() << recordPause;
}

void cFlightAnimation::InterpolateForward(int row, int column)
{
	gUndo.Store(params, fractalParams, frames, nullptr);

	QTableWidgetItem *cell = ui->tableWidget_flightAnimation->item(row, column);
	QString cellText = cell->text();

	const QString parameterName = GetParameterName(row);

	cAnimationFrames::sAnimationFrame frame = frames->GetFrame(column);

	using namespace parameterContainer;
	const enumVarType type = frame.parameters.GetVarType(parameterName);

	bool valueIsInteger = false;
	bool valueIsDouble = false;
	bool valueIsText = false;
	int valueInteger = 0;
	double valueDouble = 0.0;
	QString valueText;

	bool ok;
	const int lastFrame = QInputDialog::getInt(mainInterface->mainWindow, "Parameter interpolation",
		"Enter last frame number", column + 1, column + 2, frames->GetNumberOfFrames(), 1, &ok);
	if (!ok) return;

	const int numberOfFrames = (lastFrame - column - 1);

	switch (type)
	{
		case typeBool:
		case typeInt:
		case typeRgb:
		{
			valueIsInteger = true;
			valueInteger = cellText.toInt();
			// qDebug() << valueInteger;
			break;
		}
		case typeDouble:
		case typeVector3:
		case typeVector4:
		{
			valueIsDouble = true;
			valueDouble = systemData.locale.toDouble(cellText);
			// qDebug() << valueDouble;
			break;
		}
		default:
		{
			valueIsText = true;
			valueText = cellText;
			break;
		}
	}

	double integerStep = 0.0;
	double doubleStep = 0.0;

	if (valueIsInteger)
	{
		const int finalInteger = QInputDialog::getInt(mainInterface->mainWindow,
			"Parameter interpolation", "Enter value for last frame", valueInteger, 0, 2147483647, 1, &ok);
		integerStep = double(finalInteger - valueInteger) / numberOfFrames;
	}
	else if (valueIsDouble)
	{
		const double finalDouble = systemData.locale.toDouble(QInputDialog::getText(
			mainInterface->mainWindow, "Parameter interpolation", "Enter value for last frame",
			QLineEdit::Normal, QString("%L1").arg(valueDouble, 0, 'g', 16), &ok));
		doubleStep = (finalDouble - valueDouble) / numberOfFrames;
	}

	if (!ok) return;

	for (int i = column; i < lastFrame; i++)
	{
		QString newCellText;
		if (valueIsInteger)
		{
			const int newValue = int(integerStep * (i - column) + valueInteger);
			newCellText = QString::number(newValue);
		}
		else if (valueIsDouble)
		{
			const double newValue = doubleStep * (i - column) + valueDouble;
			newCellText = QString("%L1").arg(newValue, 0, 'g', 16);
		}
		else if (valueIsText)
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

QString cFlightAnimation::GetFlightFilename(int index) const
{
	QString filename = params->Get<QString>("anim_flight_dir") + "frame_"
										 + QString("%1").arg(index, 7, 10, QChar('0'));
	filename += "." + ImageFileSave::ImageFileExtension(ImageFileSave::enumImageFileType(
											params->Get<int>("flight_animation_image_type")));
	return filename;
}

void cFlightAnimation::slotExportFlightToKeyframes() const
{
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	gUndo.Store(params, fractalParams, gAnimFrames, gKeyframes);

	if (gKeyframes->GetFrames().size() > 0)
	{
		const QMessageBox::StandardButton reply = QMessageBox::question(
			mainInterface->mainWindow->GetCentralWidget(), QObject::tr("Export flight to keyframes"),
			QObject::tr("There are already captured keyframes present.\nDiscard current keyframes?"),
			QMessageBox::Yes | QMessageBox::No);

		if (reply == QMessageBox::No) return;
	}

	gKeyframes->ClearAll();
	gKeyframes->ClearMorphCache();
	gKeyframes->SetListOfParametersAndClear(gAnimFrames->GetListOfParameters(), params);

	const int step = params->Get<int>("frames_per_keyframe");

	for (int i = 0; i < frames->GetNumberOfFrames(); i += step)
	{
		gKeyframes->AddFrame(frames->GetFrame(i));
	}

	ui->tabWidgetFlightKeyframe->setCurrentIndex(1);
	ui->pushButton_refresh_keyframe_table->animateClick();
}

void cFlightAnimation::UpdateLimitsForFrameRange() const
{
	const int noOfFrames = frames->GetNumberOfFrames();

	ui->spinboxInt_flight_first_to_render->setMaximum(noOfFrames);

	ui->spinboxInt_flight_last_to_render->setMaximum(noOfFrames);

	SynchronizeInterfaceWindow(ui->tab_flight_animation, gPar, qInterface::write);
}

void cFlightAnimation::slotMovedSliderFirstFrame(int value) const
{
	if (value > ui->spinboxInt_flight_last_to_render->value())
		ui->spinboxInt_flight_last_to_render->setValue(value);
}

void cFlightAnimation::slotMovedSliderLastFrame(int value) const
{
	if (value < ui->spinboxInt_flight_first_to_render->value())
		ui->spinboxInt_flight_first_to_render->setValue(value);
}

void cFlightAnimation::slotCellDoubleClicked(int row, int column) const
{
	if (row == 0)
	{
		RenderFrame(column);
	}
}
