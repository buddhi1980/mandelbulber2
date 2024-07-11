/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
	if (mainInterface && mainInterface->mainWindow)
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
	// Check if the 'frames' member variable is valid (not null or zero)
	if (frames)
	{
		// If 'frames' is valid, call the RecordFlight function with 'false' as an argument.
		RecordFlight(false);
	}
	else
	{
		// If 'frames' is not valid, log a critical error message to the console.
		qCritical() << "gAnimFrames not allocated";
	}
}

void cFlightAnimation::slotContinueRecording()
{
	// Check if the 'frames' member variable is valid (not null or zero)
	if (frames)
	{
		// If 'frames' is valid, call the RecordFlight function with 'true' as an argument
		RecordFlight(true);
	}
	else
	{
		// If 'frames' is not valid, log a critical error message to the console.
		qCritical() << "gAnimFrames not allocated";
	}
}

bool cFlightAnimation::slotRenderFlight()
{
	// Synchronize Parameters (if main window exists)
	if (mainInterface->mainWindow)
	{
		// Synchronize the animation parameters, fractal parameters, and interface settings
		mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	}

	// Create and Save Parameter Text
	cSettings parSettings(cSettings::formatCondensedText);

	// Create a text representation of the current parameter settings and save it to a history file
	parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
	parSettings.SaveToFile(systemDirectories.GetHistoryFileName());

	// Validate and Render Flight
	if (frames)
	{
		// Check for Empty Frames
		if (frames->GetNumberOfFrames() == 0)
		{
			// Display an error message to the user indicating no frames are present
			emit showErrorMessage(QObject::tr("No frames to render"), cErrorMessage::errorMessage,
				mainInterface->mainWindow->GetCentralWidget());
		}

		// Check for Valid Output Directory
		else if (!QDir(params->Get<QString>("anim_flight_dir")).exists())
		{
			// Display an error message to the user indicating the specified directory is invalid
			emit showErrorMessage(
				QObject::tr("The folder %1 does not exist. Please specify a valid location.")
					.arg(params->Get<QString>("anim_flight_dir")),
				cErrorMessage::errorMessage, mainInterface->mainWindow->GetCentralWidget());
		}

		// Perform Rendering
		else
		{
			// Render the flight animation, potentially with stop request handling
			return RenderFlight(&gMainInterface->stopRequest);
		}
	}

	// Handle Missing Frame Data
	else
	{
		// Log a critical error if the animation frame data is not allocated
		qCritical() << "gAnimFrames not allocated";
	}

	// Indicate Failure
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
		frames->AddFrame(params, fractalParams, 0);

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
	// Block table's signals temporarily to prevent unnecessary updates during thumbnail generation.
	table->blockSignals(true);

	// Convert the internal image data (image->ConvertTo8bitChar()) to a QImage in RGB888 format.
	const QImage qImage(static_cast<const uchar *>(image->ConvertTo8bitChar()),
		int(image->GetWidth()), int(image->GetHeight()), int(image->GetWidth() * sizeof(sRGB8)),
		QImage::Format_RGB888);

	// Create a QPixmap from the QImage.
	QPixmap pixmap;
	pixmap.convertFromImage(qImage);

	// Scale the pixmap to the desired preview size while maintaining aspect ratio and using smooth
	// transformation.
	const QIcon icon(
		pixmap.scaled(previewSize, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

	// Create a new QTableWidgetItem with the scaled icon and an empty string as text.
	table->setItem(0, index, new QTableWidgetItem(icon, QString()));

	// Re-enable table's signals for normal behavior.
	table->blockSignals(false);
}

void cFlightAnimation::PrepareTable()
{
	// Clear existing table data and structure
	table->setRowCount(0);
	table->setColumnCount(0);
	table->clear();
	tableRowNames.clear();

	// Adjust row height based on font metrics
	QFontMetrics fm(mainInterface->mainWindow->font());
	table->verticalHeader()->setDefaultSectionSize(fm.height() + 6);

	// Create new rows in the table
	CreateRowsInTable();
}

void cFlightAnimation::CreateRowsInTable()
{
	// Get a list of parameters used in the animation frames.
	QList<cAnimationFrames::sParameterDescription> parList = frames->GetListOfUsedParameters();

	// Set the icon size for the table.
	table->setIconSize(previewSize);

	// Insert a row at index 0 for the preview.
	table->insertRow(0);
	table->setVerticalHeaderItem(0, new QTableWidgetItem(tr("preview")));
	table->setRowHeight(0, previewSize.height());
	tableRowNames.append(tr("preview"));

	// Clear the row parameter list and append -1 as the initial value.
	rowParameter.clear();
	rowParameter.append(-1);

	// Clear the list of parameter rows.
	parameterRows.clear();

	// Iterate through each parameter in the list.
	for (int i = 0; i < parList.size(); ++i)
	{
		// Add a row to the table for the current parameter and get its row index.
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
	// Create a new render job, passing in the necessary parameters and objects.
	std::shared_ptr<cRenderJob> renderJob(
		new cRenderJob(params, fractalParams, image, stopRequest, imageWidget));

	// Connect the updateProgressAndStatus signal to propagate progress updates to a higher level.
	connect(renderJob.get(),
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));

	// Connect the updateStatistics signal to propagate statistics updates to a higher level.
	connect(renderJob.get(), SIGNAL(updateStatistics(cStatistics)), this,
		SIGNAL(updateStatistics(cStatistics)));

	// If we're not running in a GUI-less environment (systemData.noGui is false)...
	if (!systemData.noGui)
	{
		// Connect the updateImage signal to the mainInterface's renderedImage slot to update
		connect(renderJob.get(), SIGNAL(updateImage()), mainInterface->renderedImage, SLOT(update()));

		// Connect the sendRenderedTilesList signal to the mainInterface's renderedImage slot
		connect(renderJob.get(), SIGNAL(sendRenderedTilesList(QList<sRenderedTileData>)),
			mainInterface->renderedImage, SLOT(showRenderedTilesList(QList<sRenderedTileData>)));
	}

	// Return the prepared renderJob as a shared_ptr, allowing multiple objects to manage it
	return renderJob;
}

bool cFlightAnimation::InitFrameRanges(sFrameRanges *frameRanges)
{
	// Get the starting frame to render from the parameters
	frameRanges->startFrame = params->Get<int>("flight_first_to_render");

	// Get the ending frame to render from the parameters
	frameRanges->endFrame = params->Get<int>("flight_last_to_render");

	// Get the total number of frames in the animation
	frameRanges->totalFrames = frames->GetNumberOfFrames();

	// Ensure that the totalFrames isn't invalid (less than 1)
	if (frameRanges->totalFrames < 1) frameRanges->totalFrames = 1;

	// If the end frame is not specified (0), set it to the total number of frames
	if (frameRanges->endFrame == 0) frameRanges->endFrame = frameRanges->totalFrames;

	// Check if the start and end frames are the same (meaning there's nothing to render)
	if (frameRanges->startFrame == frameRanges->endFrame)
	{
		// Emit an error message indicating that there are no frames to render
		emit showErrorMessage(
			QObject::tr(
				"There is no frame to render: first frame to render and last frame to render are equals."),
			cErrorMessage::warningMessage);

		// Return false to indicate an error (initialization failed)
		return false;
	}

	// Return true to indicate successful initialization of frame ranges
	return true;
}

void cFlightAnimation::InitFrameMarkers(const sFrameRanges &frameRanges)
{
	// Resize the `alreadyRenderedFrames` vector to match the total number of frames
	alreadyRenderedFrames.resize(frameRanges.totalFrames);

	// Fill it with 'false' (indicating no frames have been rendered yet)
	alreadyRenderedFrames.fill(false);

	// Clear the `reservedFrames` vector (in case it was used previously)
	reservedFrames.clear();

	// Resize the `reservedFrames` vector to match the total number of frames
	reservedFrames.resize(frameRanges.totalFrames);

	// Fill it with 'false' (indicating no frames are currently reserved)
	reservedFrames.fill(false);
}

void cFlightAnimation::CheckWhichFramesAreAlreadyRendered(const sFrameRanges &frameRanges)
{
	// Iterate through each frame in the flight animation
	for (int index = 0; index < frames->GetNumberOfFrames(); ++index)
	{
		// Construct the filename for the current frame
		const QString filename = GetFlightFilename(index, false);

		// Check if the frame file exists, or if it's outside the specified rendering range
		alreadyRenderedFrames[index] =
			QFile(filename).exists() || index < frameRanges.startFrame || index >= frameRanges.endFrame;

		// If in a net-rendering client environment
		if (gNetRender->IsClient())
		{
			// If there are frames to render in the network list
			if (netRenderListOfFramesToRender.size() > 0)
			{
				// If the current frame index is before the first frame in the net render list, mark it as
				// rendered
				if (index < netRenderListOfFramesToRender[0]) alreadyRenderedFrames[index] = true;
			}
		}

		// Mark the frame as reserved if it's already rendered
		reservedFrames[index] = alreadyRenderedFrames[index];
	}
}

bool cFlightAnimation::AllFramesAlreadyRendered(
	const sFrameRanges &frameRanges, bool *startRenderKeyframesAgain)
{
	// Assume all frames are rendered initially
	bool result = true;

	// If not in a net-rendering client environment
	if (!gNetRender->IsClient())
	{
		// If there are frames and no unrendered frames before the render
		if (frames->GetNumberOfFrames() > 0 && frameRanges.unrenderedTotalBeforeRender == 0)
		{
			// Ask the user if they want to delete the existing render
			bool deletePreviousRender;
			const QString questionTitle = QObject::tr("Truncate Image Folder");
			const QString questionText =
				QObject::tr(
					"The animation has already been rendered completely.\n Do you want to purge "
					"the output "
					"folder?\n")
				+ QObject::tr("This will delete all images in the image folder.\nProceed?");

			// If not in GUI-less mode, display a confirmation dialog
			if (!systemData.noGui)
			{
				QMessageBox::StandardButton reply = QMessageBox::NoButton;
				emit QuestionMessage(
					questionTitle, questionText, QMessageBox::Yes | QMessageBox::No, &reply);
				while (reply == QMessageBox::NoButton)
				{
					// Process events until the user answers
					gApplication->processEvents();
				}
				deletePreviousRender = (reply == QMessageBox::Yes);
			}
			else
			{
				// Exit silently if in silent mode
				if (systemData.silent)
				{
					exit(0);
				}

				// Get confirmation in headless mode
				deletePreviousRender = cHeadless::ConfirmMessage(questionTitle + "\n" + questionText);
			}

			// If the user confirms deletion
			if (deletePreviousRender)
			{
				// Delete the existing frames
				cAnimationFrames::WipeFramesFromFolder(params->Get<QString>("anim_flight_dir"));

				// If not in GUI-less mode and this is the main image
				if (!systemData.noGui && image->IsMainImage())
				{
					// Unlock interface functions and enable click modes
					mainInterface->mainWindow->GetWidgetDockNavigation()->UnlockAllFunctions();
					imageWidget->SetEnableClickModes(true);
				}

				// Set flag to start rendering keyframes again
				*startRenderKeyframesAgain = true;
			}
			// If the user doesn't confirm deletion
			else
			{
				// Indicate that not all frames are rendered
				result = false;
			}
		}
	}

	// Return whether all frames are rendered
	return result;
}

void cFlightAnimation::InitJobsForClients(const sFrameRanges &frameRanges)
{
	// Determine the number of frames to assign to each net-render client
	int numberOfFramesForNetRender;
	if (gNetRender->GetClientCount() == 0)
	{
		numberOfFramesForNetRender = 0;
	}
	else
	{
		// Divide the total unrendered frames among clients, adjusted to avoid small workloads
		numberOfFramesForNetRender =
			frameRanges.unrenderedTotalBeforeRender / gNetRender->GetClientCount() / 2 + 1;
		if (numberOfFramesForNetRender < minFramesForNetRender)
			numberOfFramesForNetRender = minFramesForNetRender;
	}

	// Enforce a maximum number of frames per client
	if (numberOfFramesForNetRender > maxFramesForNetRender)
		numberOfFramesForNetRender = maxFramesForNetRender;

	// Generate a random ID for this render job
	qint32 renderId = rand();
	gNetRender->SetCurrentRenderId(renderId);
	gNetRender->SetAnimation(true);

	// Iterate through each connected net-render client
	int frameIndex = 0;
	for (int i = 0; i < gNetRender->GetClientCount(); i++)
	{
		// Create a list of frame indices to assign to this client
		QList<int> startingFrames;
		for (int i = 0; i < numberOfFramesForNetRender; i++)
		{
			// Find the next unrendered frame
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
			// If we found an unrendered frame within the range
			else
			{
				startingFrames.append(frameIndex);
				reservedFrames[frameIndex] = true;
				frameIndex++;
			}
		}

		// If we assigned any frames to this client
		if (startingFrames.size() > 0)
		{
			// Send the frame indices to the client for rendering
			emit SendNetRenderSetup(i, startingFrames);
		}
	}

	// Send the animation parameters to all clients
	emit NetRenderCurrentAnimation(params, fractalParams, true);
}

void cFlightAnimation::UpadeProgressInformation(
	const sFrameRanges &frameRanges, cProgressText *progressText, int index)
{
	// Variable to store the percentage of frames rendered
	double percentDoneFrame;

	// Calculate the percentage of frames rendered
	if (frameRanges.unrenderedTotalBeforeRender > 0)
	{
		// If there are still frames to render, calculate percentage
		percentDoneFrame = (double(renderedFramesCount)) / frameRanges.unrenderedTotalBeforeRender;
	}
	else
	{
		// If all frames are rendered, set percentage to 100% (1.0)
		percentDoneFrame = 1.0;
	}

	// Get the formatted progress text based on the percentage done
	const QString progressTxt = progressText->getText(percentDoneFrame);

	// Emit a signal to update the progress and status information:
	emit updateProgressAndStatus(QObject::tr("Animation start"),
		// Detailed progress text: Frame number, total frames, and progress
		QObject::tr("Frame %1 of %2").arg((index + 1)).arg(frames->GetNumberOfFrames()) + " "
			+ progressTxt,
		percentDoneFrame, cProgressText::progress_ANIMATION);
}

void cFlightAnimation::UpdateCameraAndTarget()
{
	// Get camera, target, and top vector parameters from animation configuration
	const CVector3 camera = params->Get<CVector3>("camera");
	const CVector3 target = params->Get<CVector3>("target");
	const CVector3 top = params->Get<CVector3>("camera_top");

	// Create a helper object for camera-target calculations
	cCameraTarget cameraTarget(camera, target, top);

	// Update the camera rotation parameter (in degrees) in the animation configuration
	params->Set("camera_rotation", cameraTarget.GetRotation() * 180.0 / M_PI);

	// Update the distance between camera and target in the animation configuration
	params->Set("camera_distance_to_target", cameraTarget.GetDistance());

	// Check if GUI is enabled, image is the main image, and not in network rendering client mode
	if (!systemData.noGui && image->IsMainImage() && !gNetRender->IsClient())
	{
		// Synchronize interface parameters with animation and fractal parameters
		mainInterface->SynchronizeInterface(params, fractalParams, qInterface::write);

		// Calculate the distance between the camera and the fractal
		const double distance =
			mainInterface->GetDistanceForPoint(params->Get<CVector3>("camera"), params, fractalParams);

		// Update the displayed distance in the statistics table within the main window
		mainInterface->mainWindow->GetWidgetDockStatistics()->UpdateDistanceToFractal(distance);
	}
}

void cFlightAnimation::ConfirmAndSendRenderedFrames(
	const int frameIndex, const QStringList &listOfSavedFiles)
{
	// Signal the completion of rendering a frame to the network renderer.
	emit NetRenderConfirmRendered(frameIndex, netRenderListOfFramesToRender.size());

	// Remove the completed frame index from the list of frames to be rendered.
	netRenderListOfFramesToRender.removeAll(frameIndex);

	// Notify the network rendering client about the updated status.
	emit NetRenderNotifyClientStatus();

	// Iterate through the list of saved files (rendered channels).
	for (QString channelFileName : listOfSavedFiles)
	{
		// Signal that a file (rendered channel) should be sent to the client.
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
	// Remove parameters that are not found in both 'params' and 'fractalParams' from 'frames'.
	frames->RemoveMissingParameters(params, fractalParams);

	// Prepare the table for data insertion.
	PrepareTable();

	// Process any pending events in the application's event queue.
	gApplication->processEvents();

	// Get the total number of frames in the 'frames' container.
	const int noOfFrames = frames->GetNumberOfFrames();

	// Update limits for the frame range (needed after loading settings).
	UpdateLimitsForFrameRange();

	// Synchronize the interface window's values with the 'params' container.
	SynchronizeInterfaceWindow(ui->tab_flight_animation, params, qInterface::read);

	// Create temporary copies of 'params' and 'fractalParams' using shared pointers.
	auto tempPar = std::make_shared<cParameterContainer>();
	*tempPar = *params;
	auto tempFract = std::make_shared<cFractalContainer>();
	*tempFract = *fractalParams;

	// Set the number of columns in the table to the number of frames.
	table->setColumnCount(noOfFrames);

	// Iterate through each frame.
	for (int i = 0; i < noOfFrames; i++)
	{
		// Add a new column for the current frame.
		const int newColumn = AddColumn(frames->GetFrame(i), i);

		// If the "show thumbnails" checkbox is checked.
		if (ui->checkBox_flight_show_thumbnails->isChecked())
		{
			// Create a new thumbnail widget for the current frame.
			cThumbnailWidget *thumbWidget =
				new cThumbnailWidget(previewSize.width(), previewSize.height(), 1, table);

			// Configure the thumbnail widget to use one CPU core.
			thumbWidget->UseOneCPUCore(true);

			// Get the parameters and consolidate them for the current frame.
			frames->GetFrameAndConsolidate(i, tempPar, tempFract);

			// Assign the consolidated parameters to the thumbnail widget.
			thumbWidget->AssignParameters(tempPar, tempFract);

			// Set the thumbnail widget as the cell widget for the current column and row 0.
			table->setCellWidget(0, newColumn, thumbWidget);
		}

		// Every 100 frames, update the progress and status.
		if (i % 100 == 0)
		{
			emit updateProgressAndStatus(QObject::tr("Refreshing animation"),
				tr("Refreshing animation frames"), double(i) / noOfFrames,
				cProgressText::progress_ANIMATION);

			// Process any pending events in the application's event queue.
			gApplication->processEvents();
		}

		// If a global stop request has been made, exit the loop.
		if (systemData.globalStopRequest) break;
	}

	// Update limits for the frame range.
	UpdateLimitsForFrameRange();

	// Hide the progress update signal.
	emit updateProgressHide();
}

QString cFlightAnimation::GetParameterName(int rowNumber)
{
	// Get the parameter number associated with the given row number from the rowParameter array.
	const int parameterNumber = rowParameter[rowNumber];

	// Initialize a string to store the full parameter name.
	QString fullParameterName;

	// Get the list of used parameters from the animation frames container.
	QList<cAnimationFrames::sParameterDescription> list = frames->GetListOfUsedParameters();

	// If the parameter number is valid (>= 0), construct the full parameter name.
	if (parameterNumber >= 0)
	{
		fullParameterName =
			list[parameterNumber].containerName + "_" + list[parameterNumber].parameterName;
	}
	// If the parameter number is invalid, log an error message.
	else
	{
		qCritical() << "cFlightAnimation::GetParameterNumber(int rowNumber): row not found";
	}

	// Return the full parameter name.
	return fullParameterName;
}

void cFlightAnimation::RenderFrame(int index) const
{
	// Read the parameter values from the main interface and update `params` and `fractalParams`
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);

	// Retrieve the parameters for the specified frame `index` and consolidate them
	frames->GetFrameAndConsolidate(index, params, fractalParams);

	// Write the updated parameters back to the main interface to reflect the current
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::write);

	// Start rendering the frame using the updated parameter values.
	mainInterface->StartRender();
}

// Method to delete animation frames from a specified index onward
void cFlightAnimation::DeleteFramesFrom(int index) const
{
	// Store Undo Information
	gUndo->Store(params, fractalParams, frames, nullptr);

	// Remove Frames from Table
	for (int i = frames->GetNumberOfFrames() - 1; i >= index; i--)
	{
		table->removeColumn(index);
	}

	// Delete Frames from Storage
	frames->DeleteFrames(index, frames->GetNumberOfFrames() - 1);

	// Update Limits
	UpdateLimitsForFrameRange();
}

// Method to delete animation frames up to a specified index
void cFlightAnimation::DeleteFramesTo(int index) const
{
	// Store current state of animation parameters, fractal parameters, and frames for undo
	gUndo->Store(params, fractalParams, frames, nullptr);

	// Remove frame data from the table
	for (int i = 0; i <= index; i++)
	{
		// Remove the column at index 0 (since frames are removed, shifting the data)
		table->removeColumn(0);
	}

	// Delete the frames themselves from the internal data structure
	frames->DeleteFrames(0, index);

	// Update any limits or constraints based on the new range of frames
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

// Slot function to change the speed of the flight animation
void cFlightAnimation::slotFlightChangeSpeed(double amount)
{
	// Read and synchronize current parameters from the user interface
	SynchronizeInterfaceWindow(
		ui->scrollAreaWidgetContents_flightAnimationParameters, params, qInterface::read);

	// Calculate the new linear speed based on the current speed and the change amount
	linearSpeedSp = params->Get<double>("flight_speed") * amount;

	// Update the 'flight_speed' parameter in the parameter set
	params->Set("flight_speed", linearSpeedSp);

	// Write and synchronize the updated parameters back to the user interface
	SynchronizeInterfaceWindow(
		ui->scrollAreaWidgetContents_flightAnimationParameters, params, qInterface::write);
}

void cFlightAnimation::slotFlightSetSpeed(double amount)
{
	// Synchronize the interface window with the flight animation parameters
	SynchronizeInterfaceWindow(
		ui->scrollAreaWidgetContents_flightAnimationParameters, params, qInterface::read);

	// Determine if the flight is in reverse (negative speed)
	negativeFlightSpeed = (amount < 0) ? true : false;

	// Get the absolute value of the speed (magnitude)
	linearSpeedSp = fabs(amount);

	// Update the 'flight_speed' parameter in the local object
	params->Set("flight_speed", linearSpeedSp);

	// Synchronize the interface window again, this time writing the updated parameters back to the UI
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
	// Create a file dialog for selecting a directory
	QFileDialog dialog;

	// Set the file dialog mode to directory selection
	dialog.setFileMode(QFileDialog::Directory);

	// Set the name filter for the dialog to "Animation Image Folder"
	dialog.setNameFilter(QObject::tr("Animation Image Folder"));

	// Set the initial directory of the dialog to the previously selected animation image directory
	dialog.setDirectory(QDir::toNativeSeparators(params->Get<QString>("anim_flight_dir")));

	// Set the accept mode to open, indicating that the user should select an existing folder
	dialog.setAcceptMode(QFileDialog::AcceptOpen);

	// Set the window title of the dialog to "Choose Animation Image Folder"
	dialog.setWindowTitle(QObject::tr("Choose Animation Image Folder"));

	// Set the option to show only directories in the dialog
	dialog.setOption(QFileDialog::ShowDirsOnly);

	// Execute the file dialog and check if the user selected a directory
	if (dialog.exec())
	{
		// Get the list of selected files (which should contain only the selected directory)
		QStringList fileNames = dialog.selectedFiles();

		// Get the first selected file (the directory), convert it to native separators, and append a
		// directory separator
		const QString filename = QDir::toNativeSeparators(fileNames.first() + QDir::separator());

		// Set the text of the ui->text_anim_flight_dir widget to the selected directory
		ui->text_anim_flight_dir->setText(filename);

		// Update the "anim_flight_dir" parameter in the params object with the selected directory
		params->Set("anim_flight_dir", filename);
	}
}

void cFlightAnimation::slotTableCellChanged(int row, int column)
{
	// Check if the changed cell is not in the header row (row 0)
	if (row > 0)
	{
		// Temporarily block signals from the table to prevent recursive updates
		table->blockSignals(true);

		// Get a pointer to the modified table cell item
		QTableWidgetItem *cell = table->item(row, column);
		QString cellText = cell->text();

		// Retrieve the corresponding animation frame
		cAnimationFrames::sAnimationFrame frame = frames->GetFrame(column);

		// Get the name of the parameter being edited based on the row
		const QString parameterName = GetParameterName(row);

		// Determine the first row associated with this parameter and calculate the vector index
		const int parameterFirstRow = parameterRows[rowParameter[row]];
		const int vectIndex = row - parameterFirstRow;

		// Use the parameterContainer namespace for convenient access to types
		using namespace parameterContainer;

		// Get the data type of the parameter being edited
		const enumVarType type = frame.parameters.GetVarType(parameterName);

		// Handle updates based on the parameter type
		if (type == typeVector3)
		{
			// Update a CVector3 parameter based on the vector index
			CVector3 vect = frame.parameters.Get<CVector3>(parameterName);
			if (vectIndex == 0) vect.x = systemData.locale.toDouble(cellText);
			if (vectIndex == 1) vect.y = systemData.locale.toDouble(cellText);
			if (vectIndex == 2) vect.z = systemData.locale.toDouble(cellText);
			frame.parameters.Set(parameterName, vect);
		}
		else if (type == typeVector4)
		{
			// Update a CVector4 parameter based on the vector index
			CVector4 vect = frame.parameters.Get<CVector4>(parameterName);
			if (vectIndex == 0) vect.x = systemData.locale.toDouble(cellText);
			if (vectIndex == 1) vect.y = systemData.locale.toDouble(cellText);
			if (vectIndex == 2) vect.z = systemData.locale.toDouble(cellText);
			if (vectIndex == 3) vect.w = systemData.locale.toDouble(cellText);
			frame.parameters.Set(parameterName, vect);
		}
		else if (type == typeRgb)
		{
			// Update an sRGB (color) parameter based on the vector index
			sRGB col = frame.parameters.Get<sRGB>(parameterName);
			if (vectIndex == 0) col.R = cellText.toInt();
			if (vectIndex == 1) col.G = cellText.toInt();
			if (vectIndex == 2) col.B = cellText.toInt();
			frame.parameters.Set(parameterName, col);
		}
		else
		{
			// Update a simple string parameter
			frame.parameters.Set(parameterName, cellText);
		}

		// Store the modified frame back into the animation frames
		frames->ModifyFrame(column, frame);

		// Update thumbnail if the option is enabled
		if (ui->checkBox_flight_show_thumbnails->isChecked())
		{
			// Create temporary parameter and fractal containers
			auto tempPar = std::make_shared<cParameterContainer>();
			*tempPar = *params;
			auto tempFract = std::make_shared<cFractalContainer>();
			*tempFract = *fractalParams;

			// Get and consolidate the frame data
			frames->GetFrameAndConsolidate(column, tempPar, tempFract);

			// Get or create a thumbnail widget for the cell
			cThumbnailWidget *thumbWidget = static_cast<cThumbnailWidget *>(table->cellWidget(0, column));

			if (!thumbWidget)
			{
				// Create a new thumbnail widget if it doesn't exist
				thumbWidget = new cThumbnailWidget(previewSize.width(), previewSize.height(), 1, table);
				thumbWidget->UseOneCPUCore(true);
				thumbWidget->AssignParameters(tempPar, tempFract);
				table->setCellWidget(0, column, thumbWidget);
			}
			else
			{
				// Update the existing thumbnail widget
				thumbWidget->AssignParameters(tempPar, tempFract);
			}
		}

		// Unblock signals to allow further updates
		table->blockSignals(false);
	}
}

void cFlightAnimation::slotDeleteAllImages() const
{
	// Ensure the user interface parameters are synchronized with the underlying data model
	SynchronizeInterfaceWindow(
		ui->scrollAreaWidgetContents_flightAnimationParameters, params, qInterface::read);

	// Display a confirmation dialog asking the user if they want to proceed
	const QMessageBox::StandardButton reply = QMessageBox::question(
		mainInterface->mainWindow->GetCentralWidget(), QObject::tr("Truncate Image Folder"),
		QObject::tr("This will delete all images in the image folder.\nProceed?"),
		QMessageBox::Yes | QMessageBox::No);

	// If the user clicks "Yes", delete all images in the specified folder
	if (reply == QMessageBox::Yes)
	{
		cAnimationFrames::WipeFramesFromFolder(params->Get<QString>("anim_flight_dir"));
	}
}

void cFlightAnimation::slotShowAnimation() const
{
	// Log a message indicating preparation of the image sequence player
	WriteLog("Prepare PlayerWidget class", 2);

	// Synchronize the user interface parameters with the underlying data model
	SynchronizeInterfaceWindow(
		ui->scrollAreaWidgetContents_keyframeAnimationParameters, params, qInterface::read);

	// If the image sequence player doesn't exist, create a new one
	if (!mainInterface->imageSequencePlayer)
	{
		mainInterface->imageSequencePlayer = new PlayerWidget;
	}

	// Set the file path for the image sequence player to the animation directory
	mainInterface->imageSequencePlayer->SetFilePath(params->Get<QString>("anim_flight_dir"));

	// Show, raise, and activate the image sequence player window
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
	// Store the current state (parameters, fractal params, frames) for undo functionality
	gUndo->Store(params, fractalParams, frames, nullptr);

	// Get the value in the selected cell of the animation table
	QTableWidgetItem *cell = ui->tableWidget_flightAnimation->item(row, column);
	QString cellText = cell->text();

	// Determine the name of the parameter being edited (e.g., "Zoom", "Rotation", etc.)
	const QString parameterName = GetParameterName(row);

	// Retrieve the animation frame at the specified column
	cAnimationFrames::sAnimationFrame frame = frames->GetFrame(column);

	// Get the data type of the parameter (e.g., integer, double, text)
	using namespace parameterContainer;
	const enumVarType type = frame.parameters.GetVarType(parameterName);

	// Initialize variables to hold the value depending on its type
	bool valueIsInteger = false;
	bool valueIsDouble = false;
	bool valueIsText = false;
	int valueInteger = 0;
	double valueDouble = 0.0;
	QString valueText;

	// Prompt the user for the last frame number to interpolate to
	bool ok;
	const int lastFrame = QInputDialog::getInt(mainInterface->mainWindow, "Parameter interpolation",
		"Enter last frame number", column + 1, column + 2, frames->GetNumberOfFrames(), 1, &ok);
	if (!ok) return;

	// Calculate the total number of frames to interpolate across
	const int numberOfFrames = (lastFrame - column - 1);

	// Determine the appropriate value type based on the parameter type
	switch (type)
	{
		case typeBool:
		case typeInt:
		case typeRgb:
		{
			valueIsInteger = true;
			valueInteger = cellText.toInt();
			break;
		}
		case typeDouble:
		case typeVector3:
		case typeVector4:
		{
			valueIsDouble = true;
			valueDouble = systemData.locale.toDouble(cellText);
			break;
		}
		default:
		{
			valueIsText = true;
			valueText = cellText;
			break;
		}
	}

	// Calculate interpolation steps for integer and double values
	double integerStep = 0.0;
	double doubleStep = 0.0;

	// If the value is an integer, get the final integer value and calculate the step
	if (valueIsInteger)
	{
		const int finalInteger = QInputDialog::getInt(mainInterface->mainWindow,
			"Parameter interpolation", "Enter value for last frame", valueInteger, 0, 2147483647, 1, &ok);
		integerStep = double(finalInteger - valueInteger) / numberOfFrames;
	}
	// If the value is a double, get the final double value and calculate the step
	else if (valueIsDouble)
	{
		const double finalDouble = systemData.locale.toDouble(QInputDialog::getText(
			mainInterface->mainWindow, "Parameter interpolation", "Enter value for last frame",
			QLineEdit::Normal, QString("%L1").arg(valueDouble, 0, 'g', 15), &ok));
		doubleStep = (finalDouble - valueDouble) / numberOfFrames;
	}

	// If the user didn't cancel, perform the interpolation
	if (!ok) return;

	// Interpolate across the specified frames
	for (int i = column; i < lastFrame; i++)
	{
		// String to hold the interpolated value for the current frame
		QString newCellText;

		// Calculate and format the interpolated value based on the type
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

		// Update the cell in the table with the interpolated value
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
	// Determine the directory where the flight animation images will be stored
	QString dir;

	// If using network rendering cache, store images in a temporary network folder
	if (netRenderCache)
	{
		dir = systemDirectories.GetNetrenderFolder() + QDir::separator()
					+ QString("pid%1_").arg(QCoreApplication::applicationPid());
	}
	// Otherwise, use the user-specified animation directory
	else
	{
		dir = params->Get<QString>("anim_flight_dir");
	}

	// Construct the filename using a standardized format
	QString filename = dir + "frame_" + QString("%1").arg(index, 7, 10, QChar('0'));

	// Append the appropriate file extension based on the image type specified in the parameters
	filename += "."
							+ ImageFileSave::ImageFileExtension(
								ImageFileSave::enumImageFileType(params->Get<int>("flight_animation_image_type")));

	return filename;
}

void cFlightAnimation::slotExportFlightToKeyframes() const
{
	// Ensure the main interface parameters and fractal parameters are synchronized for reading
	mainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);

	// Store the current state of parameters, fractal parameters, animation frames, and keyframes
	gUndo->Store(params, fractalParams, gAnimFrames, gKeyframes);

	// Check if there are already captured keyframes
	if (gKeyframes->GetFrames().size() > 0)
	{
		// Ask the user if they want to discard the existing keyframes
		const QMessageBox::StandardButton reply = QMessageBox::question(
			mainInterface->mainWindow->GetCentralWidget(), QObject::tr("Export flight to keyframes"),
			QObject::tr("There are already captured keyframes present.\nDiscard current keyframes?"),
			QMessageBox::Yes | QMessageBox::No);

		// If the user chooses not to discard, exit the function
		if (reply == QMessageBox::No) return;
	}

	// Clear all existing keyframes and associated morph cache
	gKeyframes->ClearAll();
	gKeyframes->ClearMorphCache();

	// Set the list of parameters for the keyframes and clear them, using the animation frames
	gKeyframes->SetListOfParametersAndClear(gAnimFrames->GetListOfParameters(), params);

	// Determine the step size for capturing keyframes (frames per keyframe)
	const int step = params->Get<int>("frames_per_keyframe");

	// Iterate through the animation frames and capture keyframes at the specified intervals
	for (int i = 0; i < frames->GetNumberOfFrames(); i += step)
	{
		cAnimationFrames::sAnimationFrame frame = frames->GetFrame(i);
		frame.numberOfSubFrames = step;
		gKeyframes->AddFrame(frame);
	}

	// Update the indexes table for the keyframes
	gKeyframes->UpdateFramesIndexesTable();

	// Switch to the keyframe tab in the UI
	ui->tabWidgetFlightKeyframe->setCurrentIndex(1);

	// Trigger an animated click on the refresh keyframe table button
	ui->pushButton_refresh_keyframe_table->animateClick();
}

void cFlightAnimation::UpdateLimitsForFrameRange() const
{
	// Get the total number of frames in the animation
	const int noOfFrames = frames->GetNumberOfFrames();

	// Set the maximum value for the "first frame to render" spinbox
	ui->spinboxInt_flight_first_to_render->setMaximum(noOfFrames);

	// Set the maximum value for the "last frame to render" spinbox
	ui->spinboxInt_flight_last_to_render->setMaximum(noOfFrames);

	// Synchronize the interface window with the updated parameters, allowing writing
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
	// Indicates that frameIndex is not used in this function
	Q_UNUSED(frameIndex);

	// Increment the count of rendered frames
	renderedFramesCount++;

	// Mark the frame as already rendered if it's within the range of alreadyRenderedFrames
	if (frameIndex < alreadyRenderedFrames.size())
	{
		alreadyRenderedFrames[frameIndex] = true;
	}

	// Check if animation is still running and hasn't been stopped or completed
	if (!animationStopRequest && animationIsRendered)
	{
		// qDebug() << "Server: got information about finished frame" << frameIndex << sizeOfToDoList;

		// Count the number of frames left to render
		int countLeft = reservedFrames.count(false);

		// Calculate the maximum list size for net rendering
		int numberOfFramesForNetRender = countLeft / gNetRender->GetClientCount() / 2;
		if (numberOfFramesForNetRender < minFramesForNetRender)
			numberOfFramesForNetRender = minFramesForNetRender;
		if (numberOfFramesForNetRender > maxFramesForNetRender)
			numberOfFramesForNetRender = maxFramesForNetRender;

		// Calculate the number of new frames to add to the to-do list
		int numberOfNewFrames = numberOfFramesForNetRender - sizeOfToDoList;

		// If there are new frames to add...
		if (numberOfNewFrames > 0)
		{
			QList<int> toDoList;

			// Look for frames that are not yet reserved
			for (int f = 0; f < reservedFrames.size(); f++)
			{
				if (!reservedFrames[f])
				{
					toDoList.append(f);
					reservedFrames[f] = true;
				}
				if (toDoList.size() >= numberOfNewFrames) break;
			}

			// Send the updated to-do list to the client
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
	// Create a settings object with full text formatting
	cSettings parSettings(cSettings::formatFullText);

	// Suppress console output from settings operations
	parSettings.BeQuiet(true);

	// Load settings from the 'settingsText' string
	parSettings.LoadFromString(settingsText);

	// Decode settings into global parameter structures (gPar, gParFractal, gAnimFrames)
	parSettings.Decode(gPar, gParFractal, gAnimFrames, nullptr);

	// Set the global network renderer to animation mode
	gNetRender->SetAnimation(true);

	// Execute the flight animation rendering process, monitoring for stop requests
	gFlightAnimation->RenderFlight(&gMainInterface->stopRequest);

	// Signal the completion of the rendering process (using Qt's signal-slot mechanism)
	emit renderingFinished();
}
