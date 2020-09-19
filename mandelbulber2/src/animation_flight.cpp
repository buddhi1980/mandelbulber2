/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <memory>

#include <QWidget>

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
#include "settings.hpp"
#include "system_data.hpp"
#include "system_directories.hpp"
#include "undo.h"
#include "wait.hpp"
#include "write_log.hpp"

#include "qt/dock_animation.h"
#include "qt/dock_navigation.h"
#include "qt/dock_statistics.h"
#include "qt/my_progress_bar.h"
#include "qt/my_table_widget_anim.hpp"
#include "qt/player_widget.hpp"
#include "qt/system_tray.hpp"
#include "qt/thumbnail_widget.h"

cFlightAnimation *gFlightAnimation = nullptr;

cFlightAnimation::cFlightAnimation(cInterface *_interface,
	std::shared_ptr<cAnimationFrames> _frames, std::shared_ptr<cImage> _image, QWidget *_imageWidget,
	std::shared_ptr<cParameterContainer> _params, std::shared_ptr<cFractalContainer> _fractal,
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
		connect(this,
			SIGNAL(QuestionMessage(
				const QString, const QString, QMessageBox::StandardButtons, QMessageBox::StandardButton *)),
			mainInterface->mainWindow,
			SLOT(slotQuestionMessage(const QString, const QString, QMessageBox::StandardButtons,
				QMessageBox::StandardButton *)));

		table = ui->tableWidget_flightAnimation;
	}
	else
	{
		ui = nullptr;
		table = nullptr;
	}

	// NetRender for animation
	// signals to NetRender
	connect(this, &cFlightAnimation::SendNetRenderSetup, gNetRender, &cNetRender::SendSetup);
	connect(this, &cFlightAnimation::NetRenderCurrentAnimation, gNetRender,
		&cNetRender::SetCurrentAnimation);
	connect(this, &cFlightAnimation::NetRenderConfirmRendered, gNetRender,
		&cNetRender::ConfirmRenderedFrame);
	connect(
		this, &cFlightAnimation::NetRenderAddFileToSender, gNetRender, &cNetRender::AddFileToSender);
	connect(
		this, &cFlightAnimation::NetRenderNotifyClientStatus, gNetRender, &cNetRender::NotifyStatus);

	// signals from NetRender
	connect(
		gNetRender, &cNetRender::FinishedFrame, this, &cFlightAnimation::slotNetRenderFinishedFrame);
	connect(this, &cFlightAnimation::NetRenderSendFramesToDoList, gNetRender,
		&cNetRender::SendFramesToDoList);
	connect(gNetRender, &cNetRender::UpdateFramesToDo, this,
		&cFlightAnimation::slotNetRenderUpdateFramesToDo);
	connect(
		this, &cFlightAnimation::NetRenderStopAllClients, gNetRender, &cNetRender::StopAllClients);
	connect(gNetRender, &cNetRender::animationStopRequest, this,
		&cFlightAnimation::slotAnimationStopRequest);

	connect(this, SIGNAL(showErrorMessage(QString, cErrorMessage::enumMessageType, QWidget *)),
		gErrorMessage, SLOT(slotShowMessage(QString, cErrorMessage::enumMessageType, QWidget *)));

	image = _image;
	imageWidget = dynamic_cast<RenderedImage *>(_imageWidget);
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
	gUndo->Store(params, fractalParams, frames, nullptr);

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
		if (mainInterface->stopRequest || systemData.globalStopRequest)
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
	std::unique_ptr<cRenderJob> renderJob(new cRenderJob(params, fractalParams,
		mainInterface->mainImage, &mainInterface->stopRequest, mainInterface->renderedImage));
	connect(renderJob.get(),
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
	connect(renderJob.get(), SIGNAL(updateStatistics(cStatistics)), this,
		SIGNAL(updateStatistics(cStatistics)));
	connect(renderJob.get(), SIGNAL(updateImage()), mainInterface->renderedImage, SLOT(update()));

	cRenderingConfiguration config;
	config.DisableRefresh();
	config.SetMaxRenderTime(params->Get<double>("flight_sec_per_frame"));

	renderJob->Init(cRenderJob::flightAnimRecord, config);
	mainInterface->stopRequest = false;

	image->SetFastPreview(true);

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
	imageWidget->SetEnableClickModes(false);

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
		if (qIsNull(strafe.Length()))
		{
			double goForward = (negativeFlightSpeed) ? -1.0 : 1.0;
			cameraAcceleration =
				(cameraTarget.GetForwardVector() * goForward * linearSpeed - cameraSpeed) / (inertia + 1.0);
		}
		else
		{
			CVector3 direction;
			if (!orthogonalStrafe) direction = cameraTarget.GetForwardVector();

			if (!qIsNull(strafe.x))
			{
				direction += cameraTarget.GetRightVector() * strafe.x;
			}
			if (!qIsNull(strafe.y))
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
		frames->AddFrame(params, fractalParams);

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

		const QString filename = GetFlightFilename(index, false);
		const ImageFileSave::enumImageFileType fileType =
			ImageFileSave::enumImageFileType(params->Get<int>("flight_animation_image_type"));
		SaveImage(filename, fileType, image, gMainInterface->mainWindow);

		gApplication->processEvents();

		index++;
	}

	if (!systemData.noGui && image->IsMainImage())
	{
		mainInterface->mainWindow->GetWidgetDockNavigation()->UnlockAllFunctions();
		imageWidget->SetEnableClickModes(true);
	}

	// retrieve original click mode
	const QList<QVariant> item =
		mainInterface->mainWindow->GetComboBoxMouseClickFunction()
			->itemData(mainInterface->mainWindow->GetComboBoxMouseClickFunction()->currentIndex())
			.toList();
	gMainInterface->renderedImage->setClickMode(item);

	image->SetFastPreview(false);

	UpdateLimitsForFrameRange();
	ui->spinboxInt_flight_last_to_render->setValue(frames->GetNumberOfFrames());
}

void cFlightAnimation::UpdateThumbnailFromImage(int index) const
{
	table->blockSignals(true);
	const QImage qImage(static_cast<const uchar *>(image->ConvertTo8bitChar()),
		int(image->GetWidth()), int(image->GetHeight()), int(image->GetWidth() * sizeof(sRGB8)),
		QImage::Format_RGB888);
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
			table->setItem(row, newColumn, new QTableWidgetItem(QString("%L1").arg(val.x, 0, 'g', 15)));
			table->setItem(
				row + 1, newColumn, new QTableWidgetItem(QString("%L1").arg(val.y, 0, 'g', 15)));
			table->setItem(
				row + 2, newColumn, new QTableWidgetItem(QString("%L1").arg(val.z, 0, 'g', 15)));
		}
		else if (type == typeVector4)
		{
			const CVector4 val = frame.parameters.Get<CVector4>(parameterName);
			table->setItem(row, newColumn, new QTableWidgetItem(QString("%L1").arg(val.x, 0, 'g', 15)));
			table->setItem(
				row + 1, newColumn, new QTableWidgetItem(QString("%L1").arg(val.y, 0, 'g', 15)));
			table->setItem(
				row + 2, newColumn, new QTableWidgetItem(QString("%L1").arg(val.z, 0, 'g', 15)));
			table->setItem(
				row + 3, newColumn, new QTableWidgetItem(QString("%L1").arg(val.w, 0, 'g', 15)));
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

std::shared_ptr<cRenderJob> cFlightAnimation::PrepareRenderJob(bool *stopRequest)
{
	std::shared_ptr<cRenderJob> renderJob(
		new cRenderJob(params, fractalParams, image, stopRequest, imageWidget));
	connect(renderJob.get(),
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
	connect(renderJob.get(), SIGNAL(updateStatistics(cStatistics)), this,
		SIGNAL(updateStatistics(cStatistics)));
	if (!systemData.noGui)
	{
		connect(renderJob.get(), SIGNAL(updateImage()), mainInterface->renderedImage, SLOT(update()));
		connect(renderJob.get(), SIGNAL(sendRenderedTilesList(QList<sRenderedTileData>)),
			mainInterface->renderedImage, SLOT(showRenderedTilesList(QList<sRenderedTileData>)));
	}
	return renderJob;
}

bool cFlightAnimation::InitFrameRanges(sFrameRanges *frameRanges)
{
	// range of frames to render
	frameRanges->startFrame = params->Get<int>("flight_first_to_render");
	frameRanges->endFrame = params->Get<int>("flight_last_to_render");
	frameRanges->totalFrames = frames->GetNumberOfFrames();
	if (frameRanges->totalFrames < 1) frameRanges->totalFrames = 1;

	if (frameRanges->endFrame == 0) frameRanges->endFrame = frameRanges->totalFrames;

	if (frameRanges->startFrame == frameRanges->endFrame)
	{
		emit showErrorMessage(
			QObject::tr(
				"There is no frame to render: first frame to render and last frame to render are equals."),
			cErrorMessage::warningMessage);
		return false;
	}
	return true;
}

void cFlightAnimation::InitFrameMarkers(const sFrameRanges &frameRanges)
{
	alreadyRenderedFrames.resize(frameRanges.totalFrames);
	alreadyRenderedFrames.fill(false);
	reservedFrames.clear();
	reservedFrames.resize(frameRanges.totalFrames);
	reservedFrames.fill(false);
}

void cFlightAnimation::CheckWhichFramesAreAlreadyRendered(const sFrameRanges &frameRanges)
{
	// Check if frames have already been rendered
	for (int index = 0; index < frames->GetNumberOfFrames(); ++index)
	{
		const QString filename = GetFlightFilename(index, false);
		alreadyRenderedFrames[index] =
			QFile(filename).exists() || index < frameRanges.startFrame || index >= frameRanges.endFrame;

		if (gNetRender->IsClient())
		{
			if (netRenderListOfFramesToRender.size() > 0)
			{
				if (index < netRenderListOfFramesToRender[0]) alreadyRenderedFrames[index] = true;
			}
		}
		reservedFrames[index] = alreadyRenderedFrames[index];
	}
}

bool cFlightAnimation::AllFramesAlreadyRendered(
	const sFrameRanges &frameRanges, bool *startRenderKeyframesAgain)
{
	bool result = true;

	if (!gNetRender->IsClient())
	{
		if (frames->GetNumberOfFrames() > 0 && frameRanges.unrenderedTotalBeforeRender == 0)
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
				{
					mainInterface->mainWindow->GetWidgetDockNavigation()->UnlockAllFunctions();
					imageWidget->SetEnableClickModes(true);
				}

				*startRenderKeyframesAgain = true;
			}
			else
			{
				result = false;
			}
		}
	}

	return result;
}

void cFlightAnimation::InitJobsForClients(const sFrameRanges &frameRanges)
{
	int numberOfFramesForNetRender;
	if (gNetRender->GetClientCount() == 0)
	{
		numberOfFramesForNetRender = 0;
	}
	else
	{
		numberOfFramesForNetRender =
			frameRanges.unrenderedTotalBeforeRender / gNetRender->GetClientCount() / 2 + 1;
		if (numberOfFramesForNetRender < minFramesForNetRender)
			numberOfFramesForNetRender = minFramesForNetRender;
	}
	if (numberOfFramesForNetRender > maxFramesForNetRender)
		numberOfFramesForNetRender = maxFramesForNetRender;

	qint32 renderId = rand();
	gNetRender->SetCurrentRenderId(renderId);
	gNetRender->SetAnimation(true);
	int frameIndex = 0;
	for (int i = 0; i < gNetRender->GetClientCount(); i++)
	{
		QList<int> startingFrames;
		for (int i = 0; i < numberOfFramesForNetRender; i++)
		{
			// looking for next unrendered frame
			bool notFound = false;
			while (alreadyRenderedFrames[frameIndex])
			{
				frameIndex++;
				if (frameIndex >= frameRanges.totalFrames)
				{
					notFound = true;
					break;
				}
			}
			if (notFound)
			{
				break;
			}
			else
			{
				startingFrames.append(frameIndex);
				reservedFrames[frameIndex] = true;
				frameIndex++;
			}
		}
		if (startingFrames.size() > 0)
		{
			emit SendNetRenderSetup(i, startingFrames);
		}
	}
	emit NetRenderCurrentAnimation(params, fractalParams, true);
}

void cFlightAnimation::UpadeProgressInformation(
	const sFrameRanges &frameRanges, cProgressText *progressText, int index)
{
	double percentDoneFrame;

	if (frameRanges.unrenderedTotalBeforeRender > 0)
		percentDoneFrame = (double(renderedFramesCount)) / frameRanges.unrenderedTotalBeforeRender;
	else
		percentDoneFrame = 1.0;

	const QString progressTxt = progressText->getText(percentDoneFrame);

	emit updateProgressAndStatus(QObject::tr("Animation start"),
		QObject::tr("Frame %1 of %2").arg((index + 1)).arg(frames->GetNumberOfFrames()) + " "
			+ progressTxt,
		percentDoneFrame, cProgressText::progress_ANIMATION);
}

void cFlightAnimation::UpdateCameraAndTarget()
{
	// recalculation of camera rotation and distance (just for display purposes)
	const CVector3 camera = params->Get<CVector3>("camera");
	const CVector3 target = params->Get<CVector3>("target");
	const CVector3 top = params->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, top);
	params->Set("camera_rotation", cameraTarget.GetRotation() * 180.0 / M_PI);
	params->Set("camera_distance_to_target", cameraTarget.GetDistance());

	if (!systemData.noGui && image->IsMainImage() && !gNetRender->IsClient())
	{
		mainInterface->SynchronizeInterface(params, fractalParams, qInterface::write);

		// show distance in statistics table
		const double distance =
			mainInterface->GetDistanceForPoint(params->Get<CVector3>("camera"), params, fractalParams);
		mainInterface->mainWindow->GetWidgetDockStatistics()->UpdateDistanceToFractal(distance);
	}
}

void cFlightAnimation::ConfirmAndSendRenderedFrames(
	const int frameIndex, const QStringList &listOfSavedFiles)
{
	emit NetRenderConfirmRendered(frameIndex, netRenderListOfFramesToRender.size());
	netRenderListOfFramesToRender.removeAll(frameIndex);
	emit NetRenderNotifyClientStatus();
	for (QString channelFileName : listOfSavedFiles)
	{
		emit NetRenderAddFileToSender(channelFileName);
	}
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

	if (!systemData.noGui && image->IsMainImage() && !gNetRender->IsClient())
	{
		mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
		gUndo->Store(params, fractalParams, frames, nullptr);
	}

	*stopRequest = false;
	animationStopRequest = false;

	std::shared_ptr<cRenderJob> renderJob = PrepareRenderJob(stopRequest);

	cRenderingConfiguration config;

	if (systemData.noGui)
	{
		config.DisableRefresh();
	}
	config.DisableProgressiveRender();
	config.EnableNetRender();

	renderJob->Init(cRenderJob::flightAnim, config);

	cProgressText progressText;
	progressText.ResetTimer();

	// range of keyframes to render
	sFrameRanges frameRanges;
	if (!InitFrameRanges(&frameRanges)) return false;

	if (!systemData.noGui && image->IsMainImage() && !gNetRender->IsClient())
	{
		mainInterface->mainWindow->GetWidgetDockNavigation()->LockAllFunctions();
		imageWidget->SetEnableClickModes(false);
	}

	InitFrameMarkers(frameRanges);

	try
	{
		// updating parameters
		if (!systemData.noGui && image->IsMainImage() && !gNetRender->IsClient())
		{
			mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
			gUndo->Store(params, fractalParams, frames, nullptr);
		}

		CheckWhichFramesAreAlreadyRendered(frameRanges);

		animationIsRendered = true;

		// count number of unrendered frames
		frameRanges.unrenderedTotalBeforeRender = 0;
		for (int i = 0; i < frames->GetNumberOfFrames(); i++)
		{
			if (!alreadyRenderedFrames[i]) frameRanges.unrenderedTotalBeforeRender++;
		}

		// message if all frames are already rendered
		bool startRenderKeyframesAgain = false;
		bool result = AllFramesAlreadyRendered(frameRanges, &startRenderKeyframesAgain);

		if (startRenderKeyframesAgain) return RenderFlight(stopRequest);
		if (!result) throw false;

		renderedFramesCount = 0;

		if (gNetRender->IsServer())
		{
			InitJobsForClients(frameRanges);
		}

		for (int index = 0; index < frames->GetNumberOfFrames(); ++index)
		{
			// skip already rendered frame
			if (alreadyRenderedFrames[index]) continue;
			if (reservedFrames[index]) continue;

			if (gNetRender->IsClient())
			{
				bool frameFound = false;
				for (int f = 0; f < netRenderListOfFramesToRender.size(); f++)
				{
					if (netRenderListOfFramesToRender[f] == index)
					{
						frameFound = true;
						break;
					}
				}
				if (!frameFound) continue;
			}

			if (gNetRender->IsServer())
			{
				reservedFrames[index] = true;
			}

			UpadeProgressInformation(frameRanges, &progressText, index);

			if (*stopRequest || systemData.globalStopRequest || animationStopRequest) throw false;

			frames->GetFrameAndConsolidate(index, params, fractalParams);

			// recalculation of camera rotation and distance (just for display purposes)
			UpdateCameraAndTarget();

			params->Set("frame_no", index);

			// render frame
			renderJob->UpdateParameters(params, fractalParams);
			const int result = renderJob->Execute();
			if (!result) throw false;

			// save frame
			QStringList listOfSavedFiles;
			const QString filename = GetFlightFilename(index, gNetRender->IsClient());
			const ImageFileSave::enumImageFileType fileType =
				ImageFileSave::enumImageFileType(params->Get<int>("flight_animation_image_type"));
			listOfSavedFiles = SaveImage(filename, fileType, image, gMainInterface->mainWindow);

			renderedFramesCount++;
			alreadyRenderedFrames[index] = true;

			// qDebug() << "Finished rendering frame" << index;

			if (gNetRender->IsClient())
			{
				ConfirmAndSendRenderedFrames(index, listOfSavedFiles);
			}

			gApplication->processEvents();
		}

		emit updateProgressAndStatus(QObject::tr("Animation finished"), progressText.getText(1.0), 1.0,
			cProgressText::progress_IMAGE);
		emit notifyRenderFlightRenderStatus(
			QObject::tr("Animation finished"), progressText.getText(1.0));
		emit updateProgressHide();
	}
	catch (bool ex)
	{
		if (gNetRender->IsServer())
		{
			emit NetRenderStopAllClients();
		}

		if (gNetRender->IsClient())
		{
			gNetRender->SetStatus(netRenderSts_READY);
			emit NetRenderNotifyClientStatus();
		}

		QString resultStatus = QObject::tr("Rendering terminated");
		if (ex) resultStatus += " - " + QObject::tr("Error occured, see log output");
		emit updateProgressAndStatus(
			resultStatus, progressText.getText(1.0), cProgressText::progress_ANIMATION);
		emit updateProgressHide();

		if (!systemData.noGui && image->IsMainImage())
		{
			mainInterface->mainWindow->GetWidgetDockNavigation()->UnlockAllFunctions();
			imageWidget->SetEnableClickModes(true);
		}

		animationIsRendered = false;
		return false;
	}

	if (!systemData.noGui && image->IsMainImage() && !gNetRender->IsClient())
	{
		mainInterface->mainWindow->GetWidgetDockNavigation()->UnlockAllFunctions();
		imageWidget->SetEnableClickModes(true);
	}

	if (gNetRender->IsClient())
	{
		gNetRender->SetStatus(netRenderSts_READY);
		emit NetRenderNotifyClientStatus();
	}

	animationIsRendered = false;
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

	auto tempPar = std::make_shared<cParameterContainer>();
	*tempPar = *params;
	auto tempFract = std::make_shared<cFractalContainer>();
	*tempFract = *fractalParams;

	table->setColumnCount(noOfFrames);

	for (int i = 0; i < noOfFrames; i++)
	{
		const int newColumn = AddColumn(frames->GetFrame(i), i);

		if (ui->checkBox_flight_show_thumbnails->isChecked())
		{
			cThumbnailWidget *thumbWidget =
				new cThumbnailWidget(previewSize.width(), previewSize.height(), 1, table);
			thumbWidget->UseOneCPUCore(true);
			frames->GetFrameAndConsolidate(i, tempPar, tempFract);
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

		if (systemData.globalStopRequest) break;
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
	gUndo->Store(params, fractalParams, frames, nullptr);
	for (int i = frames->GetNumberOfFrames() - 1; i >= index; i--)
		table->removeColumn(index);
	frames->DeleteFrames(index, frames->GetNumberOfFrames() - 1);
	UpdateLimitsForFrameRange();
}

void cFlightAnimation::DeleteFramesTo(int index) const
{
	gUndo->Store(params, fractalParams, frames, nullptr);
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
	QFileDialog dialog;
	dialog.setFileMode(QFileDialog::DirectoryOnly);
	dialog.setNameFilter(QObject::tr("Animation Image Folder"));
	dialog.setDirectory(QDir::toNativeSeparators(params->Get<QString>("anim_flight_dir")));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(QObject::tr("Choose Animation Image Folder"));
	dialog.setOption(QFileDialog::ShowDirsOnly);

	if (dialog.exec())
	{
		QStringList fileNames = dialog.selectedFiles();
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
			auto tempPar = std::make_shared<cParameterContainer>();
			*tempPar = *params;
			auto tempFract = std::make_shared<cFractalContainer>();
			*tempFract = *fractalParams;

			frames->GetFrameAndConsolidate(column, tempPar, tempFract);
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
	gUndo->Store(params, fractalParams, frames, nullptr);

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
			QLineEdit::Normal, QString("%L1").arg(valueDouble, 0, 'g', 15), &ok));
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
			newCellText = QString("%L1").arg(newValue, 0, 'g', 15);
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

QString cFlightAnimation::GetFlightFilename(int index, bool netRenderCache) const
{
	QString dir;
	if (netRenderCache)
	{
		dir = systemDirectories.GetNetrenderFolder() + QDir::separator()
					+ QString("pid%1_").arg(QCoreApplication::applicationPid());
	}
	else
	{
		dir = params->Get<QString>("anim_flight_dir");
	}

	QString filename = dir + "frame_" + QString("%1").arg(index, 7, 10, QChar('0'));
	filename += "."
							+ ImageFileSave::ImageFileExtension(ImageFileSave::enumImageFileType(
									params->Get<int>("flight_animation_image_type")));
	return filename;
}

void cFlightAnimation::slotExportFlightToKeyframes() const
{
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	gUndo->Store(params, fractalParams, gAnimFrames, gKeyframes);

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

void cFlightAnimation::SetNetRenderStartingFrames(const QVector<int> &startingFrames)
{
	netRenderListOfFramesToRender.clear();
	netRenderListOfFramesToRender.append(startingFrames.toList());
}

void cFlightAnimation::slotNetRenderFinishedFrame(
	int clientIndex, int frameIndex, int sizeOfToDoList)
{
	Q_UNUSED(frameIndex);

	renderedFramesCount++;
	if (frameIndex < alreadyRenderedFrames.size())
	{
		alreadyRenderedFrames[frameIndex] = true;
	}

	if (!animationStopRequest && animationIsRendered)
	{
		// qDebug() << "Server: got information about finished frame" << frameIndex << sizeOfToDoList;

		// counting left frames
		int countLeft = reservedFrames.count(false);

		// calculate maximum list size
		int numberOfFramesForNetRender = countLeft / gNetRender->GetClientCount() / 2;
		if (numberOfFramesForNetRender < minFramesForNetRender)
			numberOfFramesForNetRender = minFramesForNetRender;
		if (numberOfFramesForNetRender > maxFramesForNetRender)
			numberOfFramesForNetRender = maxFramesForNetRender;

		// calculate number for frames to supplement
		int numberOfNewFrames = numberOfFramesForNetRender - sizeOfToDoList;

		if (numberOfNewFrames > 0)
		{
			QList<int> toDoList;

			// looking for not reserved frame
			for (int f = 0; f < reservedFrames.size(); f++)
			{
				if (!reservedFrames[f])
				{
					toDoList.append(f);
					reservedFrames[f] = true;
				}
				if (toDoList.size() >= numberOfNewFrames) break;
			}
			NetRenderSendFramesToDoList(clientIndex, toDoList);
			// qDebug() << "Server: new toDo list" << toDoList;
		}
	}
}

void cFlightAnimation::slotNetRenderUpdateFramesToDo(QList<int> listOfFrames)
{
	if (animationIsRendered)
	{
		netRenderListOfFramesToRender.append(listOfFrames);
		// qDebug() << "Client: got frames toDo:" << listOfFrames;
	}
}

void cFlightAnimation::slotAnimationStopRequest()
{
	animationStopRequest = true;
}

cFligtAnimRenderThread::cFligtAnimRenderThread(QString &_settingsText) : QThread()
{
	settingsText = _settingsText;
}

void cFligtAnimRenderThread::startAnimationRender()
{
	cSettings parSettings(cSettings::formatFullText);
	parSettings.BeQuiet(true);
	parSettings.LoadFromString(settingsText);
	parSettings.Decode(gPar, gParFractal, gAnimFrames, nullptr);
	gNetRender->SetAnimation(true);
	gFlightAnimation->RenderFlight(&gMainInterface->stopRequest);
	emit renderingFinished();
}
