/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-16 Krzysztof Marczak     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen
 *
 * cInterface class - operations connected directly with user interface
 */

#include "interface.hpp"

#include "../qt/my_group_box.h"
#include "animation_flight.hpp"
#include "animation_keyframes.hpp"
#include "calculate_distance.hpp"
#include "camera_target.hpp"
#include "common_math.h"
#include "dof.hpp"
#include "error_message.hpp"
#include "global_data.hpp"
#include "initparameters.hpp"
#include "my_ui_loader.h"
#include "queue.hpp"
#include "render_job.hpp"
#include "render_ssao.h"
#include "settings.hpp"
#include "undo.h"

#ifdef USE_GAMEPAD
#include <QtGamepad/qgamepadmanager.h>
#endif // USE_GAMEPAD

cInterface *gMainInterface = NULL;

// constructor of interface (loading of ui files)
cInterface::cInterface()
{
	mainWindow = NULL;
	headless = NULL;
	qimage = NULL;
	renderedImage = NULL;
	imageSequencePlayer = NULL;
	mainImage = NULL;
	progressBar = NULL;
	progressBarAnimation = NULL;
	progressBarQueueImage = NULL;
	progressBarQueueAnimation = NULL;
	progressBarFrame = NULL;
	progressBarLayout = NULL;
	autoRefreshTimer = NULL;
	materialListModel = NULL;
	materialEditor = NULL;
	scrollAreaMaterialEditor = NULL;
	systemTray = NULL;
	stopRequest = false;
	repeatRequest = false;
	interfaceReady = false;
	autoRefreshLastState = false;
	lockedDetailLevel = 1.0;
}

cInterface::~cInterface()
{
	if (renderedImage) delete renderedImage;
	if (imageSequencePlayer) delete imageSequencePlayer;
	if (progressBar) delete progressBar;
	if (progressBarAnimation) delete progressBarAnimation;
	if (progressBarQueueImage) delete progressBarQueueImage;
	if (progressBarQueueAnimation) delete progressBarQueueAnimation;
	if (progressBarFrame) delete progressBarFrame;
	if (progressBarLayout) delete progressBarLayout;
	if (qimage) delete qimage;
	if (mainImage) delete mainImage;
	if (headless) delete headless;
	if (mainWindow) delete mainWindow;
}

void cInterface::ShowUi(void)
{
	WriteLog("Prepare RenderWindow class", 2);

	mainWindow = new RenderWindow;

	WriteLog("Restoring window geometry", 2);

	mainWindow->restoreGeometry(mainWindow->settings.value("mainWindowGeometry").toByteArray());

	WriteLog("Restoring window state", 2);

	if (!mainWindow->restoreState(mainWindow->settings.value("mainWindowState").toByteArray()))
	{
		mainWindow->tabifyDockWidget(
			mainWindow->ui->dockWidget_materialEditor, mainWindow->ui->dockWidget_effects);
		mainWindow->tabifyDockWidget(
			mainWindow->ui->dockWidget_effects, mainWindow->ui->dockWidget_image_adjustments);
		mainWindow->tabifyDockWidget(
			mainWindow->ui->dockWidget_image_adjustments, mainWindow->ui->dockWidget_rendering_engine);
		mainWindow->tabifyDockWidget(
			mainWindow->ui->dockWidget_rendering_engine, mainWindow->ui->dockWidget_fractal);
		mainWindow->ui->dockWidget_animation->hide();
		mainWindow->ui->dockWidget_info->hide();
		mainWindow->ui->dockWidget_gamepad_dock->hide();
		mainWindow->ui->dockWidget_histogram->hide();
		mainWindow->ui->dockWidget_queue_dock->hide();
	}

	QFont font = mainWindow->font();
	font.setPixelSize(gPar->Get<int>("ui_font_size"));
	mainWindow->setFont(font);

#ifdef __APPLE__
	mainWindow->ui->actionAbout_Qt->setText(QApplication::translate("RenderWindow", "Info &Qt", 0));
	mainWindow->ui->actionAbout_Mandelbulber->setText(
		QApplication::translate("RenderWindow", "&Info Mandelbulber", 0));
	mainWindow->ui->actionAbout_ThirdParty->setText(
		QApplication::translate("RenderWindow", "Info &Third Party", 0));
#endif

	WriteLog("mainWindow->show()", 2);
	mainWindow->show();

	WriteLog("Prepare RenderedImage class", 2);
	renderedImage = new RenderedImage(mainWindow);

	mainWindow->ui->scrollAreaLayoutRenderedImage->addWidget(renderedImage);

	// setup main image
	WriteLog("Setup of main image", 2);
	mainImage = new cImage(gPar->Get<int>("image_width"), gPar->Get<int>("image_height"));
	mainImage->CreatePreview(1.0, 800, 600, gMainInterface->renderedImage);
	mainImage->CompileImage();
	mainImage->ConvertTo8bit();
	mainImage->UpdatePreview();
	mainImage->SetAsMainImage();
	renderedImage->setMinimumSize(
		gMainInterface->mainImage->GetPreviewWidth(), gMainInterface->mainImage->GetPreviewHeight());
	renderedImage->AssignImage(gMainInterface->mainImage);
	renderedImage->AssignParameters(gPar);

	WriteLog("Prepare progress and status bar", 2);
	progressBarLayout = new QVBoxLayout();
	progressBarLayout->setSpacing(0);
	progressBarLayout->setContentsMargins(0, 0, 0, 0);
	progressBarFrame = new QFrame(mainWindow->ui->statusbar);

	progressBarQueueImage = mainWindow->ui->widgetDockQueue->GetProgressBarImage();
	progressBarQueueAnimation = mainWindow->ui->widgetDockQueue->GetProgressBarAnimation();

	progressBarAnimation = new MyProgressBar(progressBarFrame);
	progressBarAnimation->setMaximum(1000);
	progressBarAnimation->setAlignment(Qt::AlignCenter);
	progressBarAnimation->hide();
	progressBarLayout->addWidget(progressBarAnimation);

	progressBar = new MyProgressBar(progressBarFrame);
	progressBar->setMaximum(1000);
	progressBar->setAlignment(Qt::AlignCenter);
	progressBarLayout->addWidget(progressBar);

	QFrame *progressBarFrame = new QFrame;
	progressBarFrame->setLayout(progressBarLayout);
	mainWindow->ui->statusbar->addPermanentWidget(progressBarFrame);

	mainWindow->setWindowTitle(QString("Mandelbulber (") + systemData.lastSettingsFile + ")");

#ifndef USE_EXR
	{
		mainWindow->ui->actionSave_as_EXR->setVisible(false);
		mainWindow->ui->widgetDockAnimation->DisableEXR();
	}
#endif

#ifndef USE_TIFF
	{
		mainWindow->ui->actionSave_as_TIFF->setVisible(false);
		mainWindow->ui->widgetDockAnimation->DisableTIFF();
	}
#endif

#ifndef USE_GAMEPAD
	{
		delete mainWindow->ui->dockWidget_gamepad_dock;
		mainWindow->ui->dockWidget_gamepad_dock = NULL;
	}
#endif

	renderedImage->show();

	mainWindow->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	mainWindow->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	mainWindow->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	mainWindow->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	// loading default ui for all fractal components
	QString uiFilename =
		systemData.sharedDir + "qt_data" + QDir::separator() + "fractal_mandelbulb.ui";
	mainWindow->ui->widgetDockFractal->InitializeFractalUi(uiFilename);
	InitMaterialsUi();
	scrollAreaMaterialEditor = mainWindow->ui->scrollArea_material;

	// change sone default settings with keeping compatibility with older versions
	StartupDefaultSettings();

	ComboMouseClickUpdate();

	mainWindow->slotPopulateToolbar();

	systemTray = new cSystemTray(mainImage, mainWindow);

	WriteLog("cInterface::ConnectSignals(void)", 2);
	ConnectSignals();
	WriteLog("cInterface::ConnectSignals(void) finished", 2);
}

void cInterface::ConnectSignals(void)
{
	// other

	QApplication::connect(mainWindow->ui->checkBox_show_cursor, SIGNAL(stateChanged(int)), mainWindow,
		SLOT(slotChangedCheckBoxCursorVisibility(int)));

	QApplication::connect(mainWindow->ui->comboBox_mouse_click_function,
		SIGNAL(currentIndexChanged(int)), mainWindow, SLOT(slotChangedComboMouseClickFunction(int)));

	QApplication::connect(mainWindow, SIGNAL(AppendToLog(const QString &)), mainWindow->ui->log_text,
		SLOT(appendMessage(const QString &)));

	// menu actions
	QApplication::connect(
		mainWindow->ui->actionQuit, SIGNAL(triggered()), mainWindow, SLOT(slotQuit()));
	QApplication::connect(mainWindow->ui->actionSave_docks_positions, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuSaveDocksPositions()));
	QApplication::connect(mainWindow->ui->actionDefault_docks_positions, SIGNAL(triggered()),
		mainWindow, SLOT(slotMenuResetDocksPositions()));
	QApplication::connect(mainWindow->ui->actionStack_all_docks, SIGNAL(triggered()), mainWindow,
		SLOT(slotStackAllDocks()));
	QApplication::connect(mainWindow->ui->actionShow_animation_dock, SIGNAL(triggered()), mainWindow,
		SLOT(slotUpdateDocksandToolbarbyAction()));
	QApplication::connect(mainWindow->ui->actionShow_toolbar, SIGNAL(triggered()), mainWindow,
		SLOT(slotUpdateDocksandToolbarbyAction()));
	QApplication::connect(mainWindow->ui->actionShow_info_dock, SIGNAL(triggered()), mainWindow,
		SLOT(slotUpdateDocksandToolbarbyAction()));
	QApplication::connect(mainWindow->ui->actionShow_statistics_dock, SIGNAL(triggered()), mainWindow,
		SLOT(slotUpdateDocksandToolbarbyAction()));
	QApplication::connect(mainWindow->ui->actionShow_gamepad_dock, SIGNAL(triggered()), mainWindow,
		SLOT(slotUpdateDocksandToolbarbyAction()));
	QApplication::connect(mainWindow->ui->actionShow_queue_dock, SIGNAL(triggered()), mainWindow,
		SLOT(slotUpdateDocksandToolbarbyAction()));
	QApplication::connect(mainWindow->ui->actionShow_measurement_dock, SIGNAL(triggered()),
		mainWindow, SLOT(slotUpdateDocksandToolbarbyAction()));
	QApplication::connect(mainWindow->ui->actionSave_settings, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuSaveSettings()));
	QApplication::connect(mainWindow->ui->actionSave_settings_to_clipboard, SIGNAL(triggered()),
		mainWindow, SLOT(slotMenuSaveSettingsToClipboard()));
	QApplication::connect(mainWindow->ui->actionLoad_settings, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuLoadSettings()));
	QApplication::connect(mainWindow->ui->actionLoad_settings_from_clipboard, SIGNAL(triggered()),
		mainWindow, SLOT(slotMenuLoadSettingsFromClipboard()));
	QApplication::connect(mainWindow->ui->actionLoad_example, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuLoadExample()));
	QApplication::connect(mainWindow->ui->actionImport_settings_from_old_Mandelbulber,
		SIGNAL(triggered()), mainWindow, SLOT(slotImportOldSettings()));
	QApplication::connect(mainWindow->ui->actionExportVoxelLayers, SIGNAL(triggered()), mainWindow,
		SLOT(slotExportVoxelLayers()));
	QApplication::connect(mainWindow->ui->actionSave_as_JPG, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuSaveImageJPEG()));
	QApplication::connect(mainWindow->ui->actionSave_as_PNG, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuSaveImagePNG()));
	QApplication::connect(mainWindow->ui->actionSave_as_PNG_16_bit, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuSaveImagePNG16()));
	QApplication::connect(mainWindow->ui->actionSave_as_PNG_16_bit_with_alpha_channel,
		SIGNAL(triggered()), mainWindow, SLOT(slotMenuSaveImagePNG16Alpha()));
#ifdef USE_EXR
	QApplication::connect(mainWindow->ui->actionSave_as_EXR, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuSaveImageEXR()));
#endif // USE_EXR

#ifdef USE_TIFF
	QApplication::connect(mainWindow->ui->actionSave_as_TIFF, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuSaveImageTIFF()));
#endif // USE_TIFF

	QApplication::connect(
		mainWindow->ui->actionAbout_Qt, SIGNAL(triggered()), mainWindow, SLOT(slotMenuAboutQt()));
	QApplication::connect(mainWindow->ui->actionAbout_Mandelbulber, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuAboutMandelbulber()));
	QApplication::connect(mainWindow->ui->actionAbout_ThirdParty, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuAboutThirdParty()));
	QApplication::connect(
		mainWindow->ui->actionUndo, SIGNAL(triggered()), mainWindow, SLOT(slotMenuUndo()));
	QApplication::connect(
		mainWindow->ui->actionRedo, SIGNAL(triggered()), mainWindow, SLOT(slotMenuRedo()));
	QApplication::connect(mainWindow->ui->actionProgramSettings, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuProgramSettings()));

	QApplication::connect(mainWindow->ui->scrollAreaForImage, SIGNAL(resized(int, int)), mainWindow,
		SLOT(slotResizedScrolledAreaImage(int, int)));
	QApplication::connect(mainWindow->ui->comboBox_image_preview_scale,
		SIGNAL(currentIndexChanged(int)), mainWindow, SLOT(slotChangedComboImageScale(int)));

	// rendered image widget
	QApplication::connect(
		renderedImage, SIGNAL(mouseMoved(int, int)), mainWindow, SLOT(slotMouseMovedOnImage(int, int)));
	QApplication::connect(renderedImage, SIGNAL(singleClick(int, int, Qt::MouseButton)), mainWindow,
		SLOT(slotMouseClickOnImage(int, int, Qt::MouseButton)));
	QApplication::connect(
		renderedImage, SIGNAL(keyPress(Qt::Key)), mainWindow, SLOT(slotKeyPressOnImage(Qt::Key)));
	QApplication::connect(
		renderedImage, SIGNAL(keyRelease(Qt::Key)), mainWindow, SLOT(slotKeyReleaseOnImage(Qt::Key)));
	QApplication::connect(renderedImage, SIGNAL(mouseWheelRotated(int)), mainWindow,
		SLOT(slotMouseWheelRotatedOnImage(int)));

	QApplication::connect(mainWindow->ui->widgetDockRenderingEngine,
		SIGNAL(stateChangedConnectDetailLevel(int)),
		gMainInterface->mainWindow->ui->widgetImageAjustments, SLOT(slotCheckedDetailLevelLock(int)));

	// DockWidgets and Toolbar

	QApplication::connect(mainWindow->ui->toolBar, SIGNAL(visibilityChanged(bool)), mainWindow,
		SLOT(slotUpdateDocksandToolbarbyView()));
	QApplication::connect(mainWindow->ui->dockWidget_animation, SIGNAL(visibilityChanged(bool)),
		mainWindow, SLOT(slotUpdateDocksandToolbarbyView()));
	QApplication::connect(mainWindow->ui->dockWidget_info, SIGNAL(visibilityChanged(bool)),
		mainWindow, SLOT(slotUpdateDocksandToolbarbyView()));
	QApplication::connect(mainWindow->ui->dockWidget_histogram, SIGNAL(visibilityChanged(bool)),
		mainWindow, SLOT(slotUpdateDocksandToolbarbyView()));
#ifdef USE_GAMEPAD
	QApplication::connect(mainWindow->ui->dockWidget_gamepad_dock, SIGNAL(visibilityChanged(bool)),
		mainWindow, SLOT(slotUpdateDocksandToolbarbyView()));
#endif
	QApplication::connect(mainWindow->ui->dockWidget_queue_dock, SIGNAL(visibilityChanged(bool)),
		mainWindow, SLOT(slotUpdateDocksandToolbarbyView()));
	QApplication::connect(mainWindow->ui->dockWidget_measurement, SIGNAL(visibilityChanged(bool)),
		mainWindow, SLOT(slotUpdateDocksandToolbarbyView()));

	QApplication::connect(mainWindow->ui->actionAdd_Settings_to_Toolbar, SIGNAL(triggered()),
		mainWindow, SLOT(slotPresetAddToToolbar()));

	//------------------------------------------------
	mainWindow->slotUpdateDocksandToolbarbyView();
}

// Reading ad writing parameters from/to ui to/from parameters container
void cInterface::SynchronizeInterface(
	cParameterContainer *par, cFractalContainer *parFractal, qInterface::enumReadWrite mode)
{
	WriteLog(
		"cInterface::SynchronizeInterface(cParameterContainer *par, cFractalContainer *parFractal, "
		"enumReadWrite mode)",
		2);

	if (!interfaceReady && mode == qInterface::read) return;

	WriteLog("cInterface::SynchronizeInterface: dockWidget_effects", 3);
	SynchronizeInterfaceWindow(mainWindow->ui->dockWidget_effects, par, mode);
	WriteLog("cInterface::SynchronizeInterface: dockWidget_image_adjustments", 3);
	SynchronizeInterfaceWindow(mainWindow->ui->dockWidget_image_adjustments, par, mode);
	WriteLog("cInterface::SynchronizeInterface: dockWidget_navigation", 3);
	SynchronizeInterfaceWindow(mainWindow->ui->dockWidget_navigation, par, mode);
	WriteLog("cInterface::SynchronizeInterface: dockWidget_rendering_engine", 3);
	SynchronizeInterfaceWindow(mainWindow->ui->dockWidget_rendering_engine, par, mode);
	WriteLog("cInterface::SynchronizeInterface: dockWidget_queue_dock", 3);
	SynchronizeInterfaceWindow(mainWindow->ui->dockWidget_queue_dock, par, mode);
	WriteLog("cInterface::SynchronizeInterface: centralwidget", 3);
	SynchronizeInterfaceWindow(mainWindow->ui->centralwidget, par, mode);
	WriteLog("cInterface::SynchronizeInterface: dockWidgetContents_animation", 3);
	SynchronizeInterfaceWindow(mainWindow->ui->dockWidgetContents_animation, par, mode);
	WriteLog("cInterface::SynchronizeInterface: dockWidget_measurement", 3);
	SynchronizeInterfaceWindow(mainWindow->ui->dockWidget_measurement, par, mode);
	WriteLog("cInterface::SynchronizeInterface: materialEditor", 3);
	SynchronizeInterfaceWindow(materialEditor, par, mode);

	mainWindow->ui->widgetDockFractal->SynchronizeInterfaceFractals(par, parFractal, mode);
}

void cInterface::StartRender(bool noUndo)
{
	if (!mainImage->IsUsed())
	{
		mainImage->BlockImage();
		WriteLog("cInterface::StartRender(void) - image was free", 2);
	}
	else
	{
		WriteLog("cInterface::StartRender(void) - image was used by another instance", 2);
		stopRequest = true;
		while (mainImage->IsUsed())
		{
			gApplication->processEvents();
			stopRequest = true;
		}
		mainImage->BlockImage();
	}

	repeatRequest = false;
	progressBarAnimation->hide();
	SynchronizeInterface(gPar, gParFractal, qInterface::read);

	if (mainWindow->ui->widgetDockNavigation->AutoRefreshIsChecked())
	{
		// check if something was changed in settings
		cSettings tempSettings(cSettings::formatCondensedText);
		tempSettings.CreateText(gPar, gParFractal);
		autoRefreshLastHash = tempSettings.GetHashCode();
	}

	if (!noUndo) gUndo.Store(gPar, gParFractal);

	DisableJuliaPointMode();

	cRenderJob *renderJob = new cRenderJob(
		gPar, gParFractal, mainImage, &stopRequest, renderedImage); // deleted by deleteLater()

	QObject::connect(renderJob,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), mainWindow,
		SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
	QObject::connect(renderJob, SIGNAL(updateStatistics(cStatistics)),
		mainWindow->ui->widgetDockStatistics, SLOT(slotUpdateStatistics(cStatistics)));
	QObject::connect(renderJob, SIGNAL(fullyRendered(const QString &, const QString &)), systemTray,
		SLOT(showMessage(const QString &, const QString &)));

	cRenderingConfiguration config;
	config.EnableNetRender();

	if (!renderJob->Init(cRenderJob::still, config))
	{
		mainImage->ReleaseImage();
		cErrorMessage::showMessage(
			QObject::tr("Cannot init renderJob, see log output for more information."),
			cErrorMessage::errorMessage);
		return;
	}

	// show distance in statistics table
	double distance = GetDistanceForPoint(gPar->Get<CVector3>("camera"), gPar, gParFractal);
	mainWindow->ui->widgetDockStatistics->UpdateDistanceToFractal(distance);

	QThread *thread = new QThread; // deleted by deleteLater()
	renderJob->moveToThread(thread);
	QObject::connect(thread, SIGNAL(started()), renderJob, SLOT(slotExecute()));
	thread->setObjectName("RenderJob");
	thread->start();

	QObject::connect(renderJob, SIGNAL(finished()), renderJob, SLOT(deleteLater()));
	QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
}

void cInterface::MoveCamera(QString buttonName)
{
	WriteLog("cInterface::MoveCamera(QString buttonName): button: " + buttonName, 2);
	// get data from interface
	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	CVector3 camera = gPar->Get<CVector3>("camera");
	CVector3 target = gPar->Get<CVector3>("target");
	CVector3 topVector = gPar->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);

	bool legacyCoordinateSystem = gPar->Get<bool>("legacy_coordinate_system");
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
		(enumCameraMovementStepMode)gPar->Get<int>("camera_absolute_distance_mode");
	enumCameraMovementMode movementMode =
		(enumCameraMovementMode)gPar->Get<int>("camera_movement_mode");

	// movement step
	double step;
	if (stepMode == absolute)
	{
		step = gPar->Get<double>("camera_movement_step");
	}
	else
	{
		double relativeStep = gPar->Get<double>("camera_movement_step");

		CVector3 point;
		if (movementMode == moveTarget)
			point = target;
		else
			point = camera;

		double distance = GetDistanceForPoint(point, gPar, gParFractal);

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
	gPar->Set("camera", camera);
	gPar->Set("target", target);

	// recalculation of camera-target
	cCameraTarget::enumRotationMode rollMode =
		(cCameraTarget::enumRotationMode)gPar->Get<int>("camera_straight_rotation");
	if (movementMode == moveCamera)
		cameraTarget.SetCamera(camera, rollMode);
	else if (movementMode == moveTarget)
		cameraTarget.SetTarget(target, rollMode);
	else if (movementMode == fixedDistance)
		cameraTarget.SetCameraTargetTop(camera, target, topVector);

	topVector = cameraTarget.GetTopVector();
	gPar->Set("camera_top", topVector);
	CVector3 rotation = cameraTarget.GetRotation();
	gPar->Set("camera_rotation", rotation * (180.0 / M_PI));
	double dist = cameraTarget.GetDistance();
	gPar->Set("camera_distance_to_target", dist);

	SynchronizeInterface(gPar, gParFractal, qInterface::write);

	StartRender();
}

void cInterface::CameraOrTargetEdited(void)
{
	WriteLog("cInterface::CameraOrTargetEdited(void)", 2);

	// get data from interface before synchronization
	CVector3 camera = gPar->Get<CVector3>("camera");
	CVector3 target = gPar->Get<CVector3>("target");
	CVector3 topVector = gPar->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);

	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	camera = gPar->Get<CVector3>("camera");
	target = gPar->Get<CVector3>("target");

	// recalculation of camera-target
	cCameraTarget::enumRotationMode rollMode =
		(cCameraTarget::enumRotationMode)gPar->Get<int>("camera_straight_rotation");
	cameraTarget.SetCamera(camera, rollMode);
	cameraTarget.SetTarget(target, rollMode);

	topVector = cameraTarget.GetTopVector();
	gPar->Set("camera_top", topVector);
	CVector3 rotation = cameraTarget.GetRotation();
	gPar->Set("camera_rotation", rotation * (180.0 / M_PI));
	double dist = cameraTarget.GetDistance();
	gPar->Set("camera_distance_to_target", dist);

	SynchronizeInterface(gPar, gParFractal, qInterface::write);
}

void cInterface::RotateCamera(QString buttonName)
{
	WriteLog("cInterface::RotateCamera(QString buttonName): button: " + buttonName, 2);

	// get data from interface
	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	CVector3 camera = gPar->Get<CVector3>("camera");
	CVector3 target = gPar->Get<CVector3>("target");
	CVector3 topVector = gPar->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);
	double distance = cameraTarget.GetDistance();

	enumCameraRotationMode rotationMode =
		(enumCameraRotationMode)gPar->Get<int>("camera_rotation_mode");
	cCameraTarget::enumRotationMode rollMode =
		(cCameraTarget::enumRotationMode)gPar->Get<int>("camera_straight_rotation");

	bool legacyCoordinateSystem = gPar->Get<bool>("legacy_coordinate_system");
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
	double rotationStep = gPar->Get<double>("camera_rotation_step") * (M_PI / 180.0);
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

	gPar->Set("camera", camera);
	gPar->Set("target", target);
	gPar->Set("camera_top", topVector);
	CVector3 rotation = cameraTarget.GetRotation();
	gPar->Set("camera_rotation", rotation * (180.0 / M_PI));
	double dist = cameraTarget.GetDistance();
	gPar->Set("camera_distance_to_target", dist);

	SynchronizeInterface(gPar, gParFractal, qInterface::write);

	StartRender();
}

void cInterface::RotationEdited(void)
{
	WriteLog("cInterface::RotationEdited(void)", 2);
	// get data from interface before synchronization
	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	CVector3 camera = gPar->Get<CVector3>("camera");
	CVector3 target = gPar->Get<CVector3>("target");
	CVector3 topVector = gPar->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);
	double distance = cameraTarget.GetDistance();
	CVector3 rotation = gPar->Get<CVector3>("camera_rotation") * (M_PI / 180.0);

	enumCameraRotationMode rotationMode =
		(enumCameraRotationMode)gPar->Get<int>("camera_rotation_mode");

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
	gPar->Set("camera", camera);
	gPar->Set("target", target);
	gPar->Set("camera_top", cameraTarget.GetTopVector());
	SynchronizeInterface(gPar, gParFractal, qInterface::write);
}

void cInterface::CameraDistanceEdited()
{
	WriteLog("cInterface::CameraDistanceEdited()", 2);

	SynchronizeInterfaceWindow(mainWindow->ui->dockWidget_navigation, gPar, qInterface::read);
	CVector3 camera = gPar->Get<CVector3>("camera");
	CVector3 target = gPar->Get<CVector3>("target");
	CVector3 topVector = gPar->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);
	CVector3 forwardVector = cameraTarget.GetForwardVector();

	double distance = gPar->Get<double>("camera_distance_to_target");

	enumCameraMovementMode movementMode =
		(enumCameraMovementMode)gPar->Get<int>("camera_movement_mode");
	if (movementMode == moveTarget)
	{
		target = camera + forwardVector * distance;
	}
	else
	{
		camera = target - forwardVector * distance;
	}

	cameraTarget.SetCameraTargetTop(camera, target, topVector);
	gPar->Set("camera", camera);
	gPar->Set("target", target);
	gPar->Set("camera_top", cameraTarget.GetTopVector());
	SynchronizeInterfaceWindow(mainWindow->ui->dockWidget_navigation, gPar, qInterface::write);
}

void cInterface::IFSDefaultsDodecahedron(cParameterContainer *parFractal)
{
	double phi = (1 + sqrt(5.0)) / 2.0;
	parFractal->Set("IFS_scale", phi * phi);
	parFractal->Set("IFS_direction_0", CVector3(phi * phi, 1.0, -phi));
	parFractal->Set("IFS_direction_1", CVector3(-phi, phi * phi, 1.0));
	parFractal->Set("IFS_direction_2", CVector3(1.0, -phi, phi * phi));
	parFractal->Set("IFS_enabled_0", true);
	parFractal->Set("IFS_enabled_1", true);
	parFractal->Set("IFS_enabled_2", true);
	parFractal->Set("IFS_offset", CVector3(1.0, 1.0, 1.0));
	parFractal->Set("IFS_abs_x", true);
	parFractal->Set("IFS_abs_y", true);
	parFractal->Set("IFS_abs_z", true);
	parFractal->Set("IFS_menger_sponge_mode", false);
}

void cInterface::IFSDefaultsIcosahedron(cParameterContainer *parFractal)
{
	double phi = (1 + sqrt(5.0)) / 2.0;
	parFractal->Set("IFS_scale", 2.0);
	parFractal->Set("IFS_direction_3", CVector3(-phi * phi, 1.0, phi));
	parFractal->Set("IFS_direction_4", CVector3(phi, -phi * phi, 1.0));
	parFractal->Set("IFS_enabled_3", true);
	parFractal->Set("IFS_enabled_4", true);
	parFractal->Set("IFS_offset", CVector3(1.0, 0.0, phi));
	parFractal->Set("IFS_abs_x", true);
	parFractal->Set("IFS_abs_y", true);
	parFractal->Set("IFS_abs_z", true);
	parFractal->Set("IFS_menger_sponge_mode", false);
}

void cInterface::IFSDefaultsOctahedron(cParameterContainer *parFractal)
{
	parFractal->Set("IFS_scale", 2.0);
	parFractal->Set("IFS_direction_5", CVector3(1.0, -1.0, 0));
	parFractal->Set("IFS_direction_6", CVector3(1.0, 0.0, -1.0));
	parFractal->Set("IFS_direction_7", CVector3(0.0, 1.0, -1.0));
	parFractal->Set("IFS_enabled_5", true);
	parFractal->Set("IFS_enabled_6", true);
	parFractal->Set("IFS_enabled_7", true);
	parFractal->Set("IFS_offset", CVector3(1.0, 0.0, 0.0));
	parFractal->Set("IFS_abs_x", true);
	parFractal->Set("IFS_abs_y", true);
	parFractal->Set("IFS_abs_z", true);
	parFractal->Set("IFS_menger_sponge_mode", false);
}

void cInterface::IFSDefaultsMengerSponge(cParameterContainer *parFractal)
{
	parFractal->Set("IFS_scale", 3.0);
	parFractal->Set("IFS_direction_5", CVector3(1.0, -1.0, 0));
	parFractal->Set("IFS_direction_6", CVector3(1.0, 0.0, -1.0));
	parFractal->Set("IFS_direction_7", CVector3(0.0, 1.0, -1.0));
	parFractal->Set("IFS_enabled_5", true);
	parFractal->Set("IFS_enabled_6", true);
	parFractal->Set("IFS_enabled_7", true);
	parFractal->Set("IFS_offset", CVector3(1.0, 1.0, 1.0));
	parFractal->Set("IFS_abs_x", true);
	parFractal->Set("IFS_abs_y", true);
	parFractal->Set("IFS_abs_z", true);
	parFractal->Set("IFS_menger_sponge_mode", true);
}

void cInterface::IFSDefaultsReset(cParameterContainer *parFractal)
{
	for (int i = 0; i < 9; i++)
	{
		parFractal->Set("IFS_direction", i, CVector3(1.0, 0.0, 0.0));
		parFractal->Set("IFS_rotations", i, CVector3(0.0, 0.0, 0.0));
		parFractal->Set("IFS_distance", i, 0.0);
		parFractal->Set("IFS_enabled", i, false);
		parFractal->Set("IFS_intensity", i, 1.0);
	}
	parFractal->Set("IFS_offset", CVector3(1.0, 0.0, 0.0));
	parFractal->Set("IFS_abs_x", false);
	parFractal->Set("IFS_abs_y", false);
	parFractal->Set("IFS_abs_z", false);
	parFractal->Set("IFS_menger_sponge_mode", false);
	parFractal->Set("IFS_scale", 2.0);
	parFractal->Set("IFS_rotation", CVector3(0.0, 0.0, 0.0));
	parFractal->Set("IFS_rotation_enabled", false);
	parFractal->Set("IFS_edge", CVector3(0.0, 0.0, 0.0));
	parFractal->Set("IFS_edge_enabled", false);
}

void cInterface::RefreshMainImage()
{
	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	sImageAdjustments imageAdjustments;
	imageAdjustments.brightness = gPar->Get<double>("brightness");
	imageAdjustments.contrast = gPar->Get<double>("contrast");
	imageAdjustments.imageGamma = gPar->Get<double>("gamma");
	imageAdjustments.hdrEnabled = gPar->Get<bool>("hdr");

	mainImage->SetImageParameters(imageAdjustments);
	mainImage->CompileImage();

	stopRequest = false;
	bool ssaoUsed = false;
	if (gPar->Get<bool>("ambient_occlusion_enabled")
			&& gPar->Get<int>("ambient_occlusion_mode") == params::AOmodeScreenSpace)
	{
		cParamRender params(gPar);
		sRenderData data;
		// cRenderingConfiguration config;
		data.stopRequest = &stopRequest;
		cRenderSSAO rendererSSAO(&params, &data, mainImage);
		QObject::connect(&rendererSSAO,
			SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
			gMainInterface->mainWindow,
			SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));

		rendererSSAO.RenderSSAO();
		ssaoUsed = true;
	}

	if (gPar->Get<bool>("DOF_enabled"))
	{
		cParamRender params(gPar);
		// cRenderingConfiguration config;
		cPostRenderingDOF dof(mainImage);
		QObject::connect(&dof,
			SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
			gMainInterface->mainWindow,
			SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
		cRegion<int> screenRegion(0, 0, mainImage->GetWidth(), mainImage->GetHeight());
		dof.Render(screenRegion,
			params.DOFRadius * (mainImage->GetWidth() + mainImage->GetPreviewHeight()) / 2000.0,
			params.DOFFocus, !ssaoUsed && gPar->Get<bool>("DOF_HDR"), params.DOFNumberOfPasses,
			params.DOFBlurOpacity, &stopRequest);
	}

	mainImage->ConvertTo8bit();
	mainImage->UpdatePreview();
	mainImage->GetImageWidget()->update();
}

void cInterface::AutoFog()
{
	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	double distance = GetDistanceForPoint(gPar->Get<CVector3>("camera"), gPar, gParFractal);
	double fogDensity = 0.5;
	double fogDistanceFactor = distance;
	double fogColour1Distance = distance * 0.5;
	double fogColour2Distance = distance;
	gPar->Set("volumetric_fog_distance_factor", fogDistanceFactor);
	gPar->Set("volumetric_fog_colour_1_distance", fogColour1Distance);
	gPar->Set("volumetric_fog_colour_2_distance", fogColour2Distance);
	gPar->Set("volumetric_fog_density", fogDensity);
	SynchronizeInterface(gPar, gParFractal, qInterface::write);
}

double cInterface::GetDistanceForPoint(
	CVector3 point, cParameterContainer *par, cFractalContainer *parFractal)
{
	cParamRender *params = new cParamRender(par);
	cNineFractals *fractals = new cNineFractals(parFractal, par);
	sDistanceIn in(point, 0, false);
	sDistanceOut out;
	double distance = CalculateDistance(*params, *fractals, in, &out);
	delete params;
	delete fractals;
	return distance;
}

double cInterface::GetDistanceForPoint(CVector3 point)
{
	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	double distance = GetDistanceForPoint(point, gPar, gParFractal);
	return distance;
}

void cInterface::SetByMouse(
	CVector2<double> screenPoint, Qt::MouseButton button, const QList<QVariant> &mode)
{
	WriteLog(
		QString("MoveCameraByMouse(CVector2<double> screenPoint, Qt::MouseButton button): button: ")
			+ button,
		2);
	// get data from interface

	RenderedImage::enumClickMode clickMode = (RenderedImage::enumClickMode)mode.at(0).toInt();

	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	CVector3 camera = gPar->Get<CVector3>("camera");
	CVector3 target = gPar->Get<CVector3>("target");
	CVector3 topVector = gPar->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);

	enumCameraMovementStepMode stepMode =
		(enumCameraMovementStepMode)gPar->Get<int>("camera_absolute_distance_mode");
	enumCameraMovementMode movementMode =
		(enumCameraMovementMode)gPar->Get<int>("camera_movement_mode");
	params::enumPerspectiveType perspType =
		(params::enumPerspectiveType)gPar->Get<int>("perspective_type");
	cCameraTarget::enumRotationMode rollMode =
		(cCameraTarget::enumRotationMode)gPar->Get<int>("camera_straight_rotation");
	double movementStep = gPar->Get<double>("camera_movement_step");
	double fov = gPar->Get<double>("fov");
	bool legacyCoordinateSystem = gPar->Get<bool>("legacy_coordinate_system");
	double reverse = legacyCoordinateSystem ? -1.0 : 1.0;

	double sweetSpotHAngle = gPar->Get<double>("sweet_spot_horizontal_angle") / 180.0 * M_PI;
	double sweetSpotVAngle = gPar->Get<double>("sweet_spot_vertical_angle") / 180.0 * M_PI;

	CVector2<double> imagePoint;
	imagePoint = screenPoint / mainImage->GetPreviewScale();

	int width = mainImage->GetWidth();
	int height = mainImage->GetHeight();

	if (imagePoint.x >= 0 && imagePoint.x < mainImage->GetWidth() && imagePoint.y >= 0
			&& imagePoint.y < mainImage->GetHeight())
	{
		double depth = mainImage->GetPixelZBuffer(imagePoint.x, imagePoint.y);
		if (depth < 1e10)
		{
			CVector3 viewVector;
			double aspectRatio = (double)width / height;

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

			switch (clickMode)
			{
				case RenderedImage::clickMoveCamera:
				{
					double distance = (camera - point).Length();
					double moveDistance = (stepMode == absolute) ? movementStep : distance * movementStep;
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

					gPar->Set("camera", camera);
					gPar->Set("target", target);

					topVector = cameraTarget.GetTopVector();
					gPar->Set("camera_top", topVector);
					CVector3 rotation = cameraTarget.GetRotation();
					gPar->Set("camera_rotation", rotation * (180.0 / M_PI));
					double dist = cameraTarget.GetDistance();
					gPar->Set("camera_distance_to_target", dist);

					SynchronizeInterface(gPar, gParFractal, qInterface::write);
					renderedImage->setNewZ(depth - moveDistance);

					StartRender();

					break;
				}
				case RenderedImage::clickFogVisibility:
				{
					double fogDepth = depth;
					gPar->Set("basic_fog_visibility", fogDepth);
					gMainInterface->mainWindow->ui->widgetEffects->SynchronizeInterfaceBasicFogEnabled(gPar);
					StartRender();
					break;
				}
				case RenderedImage::clickDOFFocus:
				{
					DisablePeriodicRefresh();
					double DOF = depth;
					gPar->Set("DOF_focus", DOF);
					gMainInterface->mainWindow->ui->widgetEffects->SynchronizeInterfaceDOFEnabled(gPar);
					gUndo.Store(gPar, gParFractal);
					RefreshMainImage();
					ReEnablePeriodicRefresh();
					break;
				}
				case RenderedImage::clickPlaceLight:
				{
					double frontDist = gPar->Get<double>("aux_light_manual_placement_dist");
					CVector3 pointCorrected = point - viewVector * frontDist;
					double estDistance = GetDistanceForPoint(pointCorrected, gPar, gParFractal);
					double intensity = estDistance * estDistance;
					int lightNumber = mode.at(1).toInt();
					gPar->Set("aux_light_position", lightNumber, pointCorrected);
					gPar->Set("aux_light_intensity", lightNumber, intensity);
					gMainInterface->mainWindow->ui->widgetEffects->SynchronizeInterfaceLights(gPar);
					StartRender();
					break;
				}
				case RenderedImage::clickGetJuliaConstant:
				{
					gPar->Set("julia_c", point);
					mainWindow->ui->widgetDockFractal->EnableJuliaMode();
					mainWindow->ui->widgetDockFractal->SynchronizeInterfaceJulia();

					// StartRender();
					break;
				}
				case RenderedImage::clickPlacePrimitive:
				{
					QString parameterName = mode.at(3).toString() + "_position";
					gPar->Set(parameterName, point);
					mainWindow->ui->widgetDockFractal->SynchronizeInterfacePrimitives();
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
					gPar->Set("random_lights_distribution_center", point);
					gPar->Set("random_lights_distribution_radius", 0.5 * distanceCameraToCenter);
					gPar->Set("random_lights_max_distance_from_fractal", 0.1 * distanceCameraToCenter);
					gMainInterface->mainWindow->ui->widgetEffects->SynchronizeInterfacerandomLights(gPar);
					StartRender();
					break;
				}
				case RenderedImage::clickGetPoint:
				{
					DisablePeriodicRefresh();
					SynchronizeInterface(gPar, gParFractal, qInterface::read);
					CVector3 oldPoint = gPar->Get<CVector3>("meas_point");
					double distanceFromLast = (point - oldPoint).Length();
					double distanceFromCamera = (point - camera).Length();
					gPar->Set("meas_point", point);
					gPar->Set("meas_distance_from_last", distanceFromLast);
					gPar->Set("meas_distance_from_camera", distanceFromCamera);
					SynchronizeInterfaceWindow(
						mainWindow->ui->dockWidget_measurement, gPar, qInterface::write);
					if (!mainWindow->ui->actionShow_measurement_dock->isChecked())
					{
						mainWindow->ui->actionShow_measurement_dock->setChecked(true);
						mainWindow->slotUpdateDocksandToolbarbyAction();
					}
					ReEnablePeriodicRefresh();
					break;
				}
			}
		}
	}
}

void cInterface::MovementStepModeChanged(int mode)
{
	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	enumCameraMovementStepMode stepMode = (enumCameraMovementStepMode)mode;
	double distance = GetDistanceForPoint(gPar->Get<CVector3>("camera"), gPar, gParFractal);
	double oldStep = gPar->Get<double>("camera_movement_step");
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
	gPar->Set("camera_movement_step", newStep);
	SynchronizeInterfaceWindow(mainWindow->ui->dockWidget_navigation, gPar, qInterface::write);
}

void cInterface::Undo()
{
	bool refreshFrames = false;
	bool refreshKeyframes = false;
	if (gUndo.Undo(gPar, gParFractal, gAnimFrames, gKeyframes, &refreshFrames, &refreshKeyframes))
	{
		gMainInterface->RebuildPrimitives(gPar);
		gMainInterface->materialListModel->Regenerate();
		SynchronizeInterface(gPar, gParFractal, qInterface::write);
		if (refreshFrames) gFlightAnimation->RefreshTable();
		if (refreshKeyframes) gKeyframeAnimation->RefreshTable();
		StartRender(true);
	}
}

void cInterface::Redo()
{
	bool refreshFrames = false;
	bool refreshKeyframes = false;
	if (gUndo.Redo(gPar, gParFractal, gAnimFrames, gKeyframes, &refreshFrames, &refreshKeyframes))
	{
		gMainInterface->RebuildPrimitives(gPar);
		gMainInterface->materialListModel->Regenerate();
		SynchronizeInterface(gPar, gParFractal, qInterface::write);
		if (refreshFrames) gFlightAnimation->RefreshTable();
		if (refreshKeyframes) gKeyframeAnimation->RefreshTable();
		StartRender(true);
	}
}

void cInterface::ResetView()
{
	SynchronizeInterface(gPar, gParFractal, qInterface::read);

	CVector3 camera = gPar->Get<CVector3>("camera");
	CVector3 target = gPar->Get<CVector3>("target");
	CVector3 topVector = gPar->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);
	CVector3 forwardVector = cameraTarget.GetForwardVector();
	double fov = gPar->Get<double>("fov");
	params::enumPerspectiveType perspType =
		(params::enumPerspectiveType)gPar->Get<int>("perspective_type");
	double DEactor = gPar->Get<double>("DE_factor");

	cParameterContainer parTemp = *gPar;
	parTemp.Set("limits_enabled", false);
	parTemp.Set("interior_mode", false);

	// calculate size of the fractal in random directions
	double maxDist = 0.0;

	cParamRender *params = new cParamRender(gPar);
	cNineFractals *fractals = new cNineFractals(gParFractal, gPar);

	for (int i = 0; i < 50; i++)
	{
		cProgressText::ProgressStatusText(QObject::tr("Reseting view"),
			QObject::tr("Fractal size calculation"), i / 50.0, cProgressText::progress_IMAGE);
		CVector3 direction(
			Random(1000) / 500.0 - 1.0, Random(1000) / 500.0 - 1.0, Random(1000) / 500.0 - 1.0);
		direction.Normalize();
		double distStep = 0.0;
		double scan;

		for (scan = 100.0; scan > 0; scan -= distStep)
		{
			CVector3 point = direction * scan;
			sDistanceIn in(point, 0, false);
			sDistanceOut out;
			double dist = CalculateDistance(*params, *fractals, in, &out);
			if (dist < 0.1)
			{
				break;
			}
			distStep = dist * DEactor * 0.5;
			if (distStep > 5.0) distStep = 5.0;
			// qDebug() << "i" << i << "scan" << scan << "direction" << direction.Debug();
		}
		if (scan > maxDist) maxDist = scan;
	}
	cProgressText::ProgressStatusText(
		QObject::tr("Reseting view"), QObject::tr("Done"), 1.0, cProgressText::progress_IMAGE);
	delete params;
	delete fractals;

	double newCameraDist = maxDist / fov * 2.0 * sqrt(2);
	if (perspType == params::perspFishEye || perspType == params::perspFishEyeCut
			|| perspType == params::perspEquirectangular)
		newCameraDist /= M_PI;

	if (newCameraDist < 0.1) newCameraDist = 0.1;

	gPar->Set("target", CVector3(0.0, 0.0, 0.0));
	CVector3 newCamera = forwardVector * newCameraDist * (-1.0);
	gPar->Set("camera", newCamera);
	gPar->Set("camera_distance_to_target", newCameraDist);
	gPar->Set("view_distance_max", (newCameraDist + maxDist) * 2.0);
	SynchronizeInterface(gPar, gParFractal, qInterface::write);
	StartRender();
}

void cInterface::NewPrimitive(const QString &primitiveType, int index)
{
	QString primitiveName = QString("primitive_") + primitiveType;
	QString uiFileName = systemData.sharedDir + "qt_data" + QDir::separator() + primitiveName + ".ui";
	fractal::enumObjectType objectType = PrimitiveNameToEnum(primitiveType);

	int newId = 0;
	if (index == 0)
	{
		// look for the lowest free id
		bool occupied = true;

		while (occupied)
		{
			newId++;
			occupied = false;
			for (int i = 0; i < listOfPrimitives.size(); i++)
			{
				if (objectType == listOfPrimitives.at(i).type && newId == listOfPrimitives.at(i).id)
					occupied = true;
			}
		}
	}
	else
	{
		newId = index; // use given index
	}

	// name for new primitive
	QString primitiveFullName = primitiveName + "_" + QString::number(newId);

	sPrimitiveItem newItem(objectType, newId, primitiveFullName);
	listOfPrimitives.append(newItem);

	// main widget for primitive
	QWidget *mainWidget =
		new QWidget(mainWindow->ui->widgetDockFractal->GetContainerWithPrimitives());
	mainWidget->setObjectName(QString("widgetmain_") + primitiveFullName);
	QVBoxLayout *layout = new QVBoxLayout();
	mainWidget->setLayout(layout);

	QHBoxLayout *buttonsLayout = new QHBoxLayout();
	layout->addLayout(buttonsLayout);

	QPushButton *setPositionButton = new QPushButton(
		QObject::tr("Set position of %1 # %2 by mouse pointer").arg(primitiveType).arg(newId),
		mainWidget);
	setPositionButton->setObjectName(QString("setPositionButton_") + primitiveFullName);
	buttonsLayout->addWidget(setPositionButton);

	QPushButton *deleteButton = new QPushButton(
		QObject::tr("Delete ") + primitiveType + " # " + QString::number(newId), mainWidget);
	deleteButton->setObjectName(QString("deleteButton_") + primitiveFullName);
	buttonsLayout->addWidget(deleteButton);

	MyGroupBox *groupBox = new MyGroupBox(mainWidget);
	groupBox->setObjectName(QString("groupCheck_") + primitiveFullName + "_enabled");
	groupBox->setCheckable(true);
	groupBox->setTitle(primitiveType + " # " + QString::number(newId));
	layout->addWidget(groupBox);

	QVBoxLayout *layoutGroupBox = new QVBoxLayout();
	groupBox->setLayout(layoutGroupBox);

	// load ui
	MyUiLoader loader;
	QFile uiFile(uiFileName);
	if (uiFile.exists())
	{
		uiFile.open(QFile::ReadOnly);
		QWidget *primitiveWidget = loader.load(&uiFile);
		uiFile.close();
		primitiveWidget->setObjectName(QString("widgetui_") + primitiveFullName);
		layoutGroupBox->addWidget(primitiveWidget);

		// put widget into layout
		QVBoxLayout *primitivesLayout = mainWindow->ui->widgetDockFractal->GetLayoutWithPrimitives();
		primitivesLayout->addWidget(mainWidget);

		// rename widgets
		QList<QWidget *> listOfWidgets = primitiveWidget->findChildren<QWidget *>();
		for (int i = 0; i < listOfWidgets.size(); i++)
		{
			QString name = listOfWidgets[i]->objectName();
			int firstDash = name.indexOf('_');
			QString newName = name.insert(firstDash + 1, primitiveFullName + "_");
			listOfWidgets[i]->setObjectName(newName);
		}

		QApplication::connect(
			deleteButton, SIGNAL(clicked()), mainWindow, SLOT(slotPressedButtonDeletePrimitive()));
		QApplication::connect(setPositionButton, SIGNAL(clicked()), mainWindow,
			SLOT(slotPressedButtonSetPositionPrimitive()));

		// adding parameters
		if (index == 0) // for only new primitive
		{
			InitPrimitiveParams(objectType, primitiveFullName, gPar);
		}

		gPar->Set(primitiveFullName + "_enabled", true);

		mainWindow->automatedWidgets->ConnectSignalsForSlidersInWindow(mainWidget);
		SynchronizeInterfaceWindow(mainWidget, gPar, qInterface::write);

		ComboMouseClickUpdate();
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("Can't open file ") + uiFileName
																 + QObject::tr(" Primitive object ui file can't be loaded"),
			cErrorMessage::errorMessage, mainWindow);
	}
}

void cInterface::DeletePrimitive(const QString &primitiveName)
{
	QString primitiveWidgetName = QString("widgetmain_") + primitiveName;
	fractal::enumObjectType objectType = fractal::objNone;

	// delete widget
	QWidget *primitiveWidget =
		mainWindow->ui->widgetDockFractal->GetContainerWithPrimitives()->findChild<QWidget *>(
			primitiveWidgetName);
	delete primitiveWidget;

	// remove item from list
	for (int i = 0; i < listOfPrimitives.size(); i++)
	{
		if (primitiveName == listOfPrimitives.at(i).name)
		{
			objectType = listOfPrimitives.at(i).type;
			listOfPrimitives.removeAt(i);
		}
	}

	DeletePrimitiveParams(objectType, primitiveName, gPar);
	ComboMouseClickUpdate();
}

void cInterface::RebuildPrimitives(cParameterContainer *par)
{
	// clear all widgets
	for (int i = 0; i < listOfPrimitives.size(); i++)
	{
		QString widgetName = QString("widgetmain_") + listOfPrimitives.at(i).name;
		QWidget *widget =
			mainWindow->ui->widgetDockFractal->GetContainerWithPrimitives()->findChild<QWidget *>(
				widgetName);
		delete widget;
	}
	listOfPrimitives.clear();

	QList<QString> listOfParameters = par->GetListOfParameters();
	for (int i = 0; i < listOfParameters.size(); i++)
	{
		QString parameterName = listOfParameters.at(i);
		if (parameterName.left(parameterName.indexOf('_')) == "primitive")
		{
			QStringList split = parameterName.split('_');
			QString primitiveName = split.at(0) + "_" + split.at(1) + "_" + split.at(2);
			QString objectTypeString = split.at(1);
			int index = split.at(2).toInt();

			bool found = false;
			for (int l = 0; l < listOfPrimitives.size(); l++)
			{
				if (listOfPrimitives.at(l).name == primitiveName)
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				NewPrimitive(objectTypeString, index);
			}
		}
	}
}

void cInterface::ComboMouseClickUpdate()
{
	QComboBox *combo = mainWindow->ui->comboBox_mouse_click_function;
	int lastIndex = combo->currentIndex();

	combo->clear();
	QList<QVariant> item;

	item.clear();
	item.append((int)RenderedImage::clickDoNothing);
	combo->addItem(QObject::tr("No action"), item);

	item.clear();
	item.append((int)RenderedImage::clickMoveCamera);
	combo->addItem(QObject::tr("Move the camera"), item);

	item.clear();
	item.append((int)RenderedImage::clickFogVisibility);
	combo->addItem(QObject::tr("Set fog visibility"), item);

	item.clear();
	item.append((int)RenderedImage::clickDOFFocus);
	combo->addItem(QObject::tr("Set DOF focus"), item);

	item.clear();
	item.append((int)RenderedImage::clickGetJuliaConstant);
	combo->addItem(QObject::tr("Get Julia constant"), item);

	item.clear();
	item.append((int)RenderedImage::clickPlaceLight);
	item.append(1);
	combo->addItem(QObject::tr("Place light #1"), item);

	item.clear();
	item.append((int)RenderedImage::clickPlaceLight);
	item.append(2);
	combo->addItem(QObject::tr("Place light #2"), item);

	item.clear();
	item.append((int)RenderedImage::clickPlaceLight);
	item.append(3);
	combo->addItem(QObject::tr("Place light #3"), item);

	item.clear();
	item.append((int)RenderedImage::clickPlaceLight);
	item.append(4);
	combo->addItem(QObject::tr("Place light #4"), item);

	item.clear();
	item.append((int)RenderedImage::clickPlaceRandomLightCenter);
	combo->addItem(QObject::tr("Place random light center"), item);

	item.clear();
	item.append((int)RenderedImage::clickGetPoint);
	combo->addItem(QObject::tr("Get point coordinates"), item);

	if (listOfPrimitives.size() > 0)
	{
		for (int i = 0; i < listOfPrimitives.size(); i++)
		{
			QString primitiveName = PrimitiveNames(listOfPrimitives.at(i).type);
			int index = listOfPrimitives.at(i).id;
			QString comboItemString =
				QString(QObject::tr("Place ")) + primitiveName + QString(" #") + QString::number(index);
			item.clear();
			item.append((int)RenderedImage::clickPlacePrimitive);
			item.append((int)listOfPrimitives.at(i).type);
			item.append(listOfPrimitives.at(i).id);
			item.append(listOfPrimitives.at(i).name);
			combo->addItem(comboItemString, item);
		}
	}

	if (lastIndex < combo->count())
	{
		combo->setCurrentIndex(lastIndex);
	}
}

bool cInterface::QuitApplicationDialog()
{
	bool quit = false;
	int closeResult = 0;
	bool quitDoNotAskAgain = gPar->Get<bool>("quit_do_not_ask_again");
	QMessageBox *messageBox = NULL;
	QAbstractButton *btnYesAndDoNotAskAgain = NULL;
	if (quitDoNotAskAgain)
	{
		closeResult = QMessageBox::Ok;
	}
	else
	{
		messageBox = new QMessageBox(mainWindow);
		QString messageText = QObject::tr("Are you sure to close the application?");
		messageBox->setText(messageText);
		messageBox->setWindowTitle(QObject::tr("Quit?"));
		messageBox->setIcon(QMessageBox::Question);
		messageBox->addButton(QMessageBox::Ok);
		messageBox->addButton(QMessageBox::Cancel);
		btnYesAndDoNotAskAgain =
			messageBox->addButton(QObject::tr("Yes, don't ask again"), QMessageBox::YesRole);
		messageBox->setDefaultButton(QMessageBox::Ok);
		closeResult = messageBox->exec();
	}

	switch (closeResult)
	{
		case QMessageBox::Cancel:
		{
			// nothing
			break;
		}
		case QMessageBox::Yes:
		default:
		{
			if (messageBox && messageBox->clickedButton() == btnYesAndDoNotAskAgain)
				gPar->Set("quit_do_not_ask_again", true);
			DisablePeriodicRefresh();
			stopRequest = true;
			systemData.globalStopRequest = true;
			gQueue->stopRequest = true;
			WriteLog("Quit application", 2);
			// save applications settings
			cSettings parSettings(cSettings::formatAppSettings);
			gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::read);
			parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
			parSettings.SaveToFile(systemData.dataDirectory + "mandelbulber.ini");

			while (cRenderJob::GetRunningJobCount() > 0)
			{
				gApplication->processEvents();
			}

			QFile::remove(systemData.autosaveFile);

			gApplication->quit();
			quit = true;
			break;
		}
	}
	if (messageBox) delete messageBox;
	return quit;
}

void cInterface::AutoRecovery()
{
	if (QFile::exists(systemData.autosaveFile))
	{
		// autorecovery dialog
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(
			mainWindow->ui->centralwidget, QObject::tr("Auto recovery"),
			QObject::tr(
				"Application has not been closed properly\nDo you want to recover your latest work?"),
			QMessageBox::Yes | QMessageBox::No);

		if (reply == QMessageBox::Yes)
		{
			cSettings parSettings(cSettings::formatFullText);
			parSettings.LoadFromFile(systemData.autosaveFile);
			parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
			gMainInterface->RebuildPrimitives(gPar);
			gMainInterface->materialListModel->Regenerate();
			gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
			gFlightAnimation->RefreshTable();
			gKeyframeAnimation->RefreshTable();
		}
		else
		{
			return;
		}
	}
}

void cInterface::OptimizeStepFactor(double qualityTarget)
{
	DisablePeriodicRefresh();

	// check if main image is not used by other rendering process
	if (mainImage->IsUsed())
	{
		cErrorMessage::showMessage(
			QObject::tr("Rendering engine is busy. Stop unfinished rendering before starting new one"),
			cErrorMessage::errorMessage);
		return;
	}

	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	gUndo.Store(gPar, gParFractal);

	cParameterContainer tempParam = *gPar;
	cFractalContainer tempFractal = *gParFractal;

	// disabling all slow effects
	tempParam.Set("shadows_enabled", false);
	tempParam.Set("ambient_occlusion", false);
	tempParam.Set("DOF_enabled", false);
	tempParam.Set("iteration_threshold_mode", false);
	tempParam.Set("raytraced_reflections", false);
	tempParam.Set("textured_background", false);
	tempParam.Set("iteration_fog_enable", false);
	tempParam.Set("fake_lights_enabled", false);
	tempParam.Set("main_light_volumetric_enabled", false);
	for (int i = 1; i <= 4; i++)
	{
		tempParam.Set("aux_light_enabled", i, false);
		tempParam.Set("aux_light_volumetric_enabled", i, false);
	}

	int maxDimension = max(gPar->Get<int>("image_width"), gPar->Get<int>("image_height"));
	int newWidth = (double)gPar->Get<int>("image_width") / maxDimension * 256.0;
	int newHeight = (double)gPar->Get<int>("image_height") / maxDimension * 256.0;

	tempParam.Set("image_width", newWidth);
	tempParam.Set("image_height", newHeight);
	tempParam.Set("detail_level", 4.0);

	int scanCount = 0;
	double DEfactor = 1.0;
	double step = 1.0;

	cRenderJob *renderJob =
		new cRenderJob(&tempParam, &tempFractal, mainImage, &stopRequest, renderedImage);
	QObject::connect(renderJob, SIGNAL(updateStatistics(cStatistics)),
		mainWindow->ui->widgetDockStatistics, SLOT(slotUpdateStatistics(cStatistics)));

	cRenderingConfiguration config;
	config.DisableRefresh();
	config.DisableProgressiveRender();
	config.DisableNetRender();
	config.SetMaxRenderTime(5.0);

	renderJob->Init(cRenderJob::still, config);

	cProgressText::ProgressStatusText(QObject::tr("Looking for optimal DE factor"),
		QString("Percentage of wrong distance estimations: ") + QString::number(0.0), 0.0,
		cProgressText::progress_IMAGE);

	stopRequest = false;

	double missedDE = 0.0;

	for (int i = 0; i < 100; i++)
	{
		if (stopRequest) return;
		scanCount++;
		tempParam.Set("DE_factor", DEfactor);

		gPar->Set("DE_factor", DEfactor);
		SynchronizeInterface(gPar, gParFractal, qInterface::write);

		renderJob->UpdateParameters(&tempParam, &tempFractal);
		renderJob->Execute();
		missedDE = renderJob->GetStatistics().GetMissedDEPercentage();

		if (missedDE < qualityTarget)
		{
			if (scanCount == 1)
			{
				if (step < 10000)
				{
					DEfactor = DEfactor * 2.0;
					step = step * 2.0;
					scanCount = 0;
					continue;
				}
				else
				{
					return;
				}
			}
			else
			{
				if (step < 0.05 * DEfactor) break;
				DEfactor += step;
			}
		}

		double progress = 1.0 - 1.0 / (1.0 + pow(qualityTarget / (missedDE - qualityTarget), 2.0));
		cProgressText::ProgressStatusText(QObject::tr("Looking for optimal DE factor"),
			QObject::tr("Percentage of wrong distance estimations: %1").arg(missedDE), progress,
			cProgressText::progress_IMAGE);

		step /= 2.0;
		DEfactor -= step;
	}

	gPar->Set("DE_factor", DEfactor);
	SynchronizeInterface(gPar, gParFractal, qInterface::write);
	cProgressText::ProgressStatusText(QObject::tr("Idle"),
		QObject::tr("Optimal DE factor is: %1 which gives %2% of bad distance estimations")
			.arg(DEfactor)
			.arg(missedDE),
		1.0, cProgressText::progress_IMAGE);

	ReEnablePeriodicRefresh();

	delete renderJob;
}

void cInterface::ResetFormula(int fractalNumber)
{
	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	gUndo.Store(gPar, gParFractal, gAnimFrames, gKeyframes);
	cParameterContainer *fractal = &gParFractal->at(fractalNumber);
	fractal->ResetAllToDefault();
	gUndo.Store(gPar, gParFractal, gAnimFrames, gKeyframes);
	SynchronizeInterface(gPar, gParFractal, qInterface::write);
}

void cInterface::PeriodicRefresh()
{
	if (mainWindow->ui->widgetDockNavigation->AutoRefreshIsChecked())
	{
		// check if something was changed in settings
		SynchronizeInterface(gPar, gParFractal, qInterface::read);
		cSettings tempSettings(cSettings::formatCondensedText);
		tempSettings.CreateText(gPar, gParFractal);
		QString newHash = tempSettings.GetHashCode();

		if (newHash != autoRefreshLastHash)
		{
			autoRefreshLastHash = newHash;
			StartRender();
		}
	}

	autoRefreshTimer->start(2000);
}

void cInterface::DisablePeriodicRefresh()
{
	if (mainWindow)
	{
		autoRefreshLastState = mainWindow->ui->widgetDockNavigation->AutoRefreshIsChecked();
		mainWindow->ui->widgetDockNavigation->AutoRefreshSetChecked(false);
	}
}

void cInterface::ReEnablePeriodicRefresh()
{
	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	cSettings tempSettings(cSettings::formatCondensedText);
	tempSettings.CreateText(gPar, gParFractal);
	autoRefreshLastHash = tempSettings.GetHashCode();
	if (autoRefreshLastState)
	{
		mainWindow->ui->widgetDockNavigation->AutoRefreshSetChecked(true);
	}
}

void cInterface::InitPeriodicRefresh()
{
	autoRefreshTimer = new QTimer(mainWindow);
	autoRefreshTimer->setSingleShot(true);
	QApplication::connect(autoRefreshTimer, SIGNAL(timeout()), mainWindow, SLOT(slotAutoRefresh()));
	autoRefreshTimer->start(2000);
}

void cInterface::InitMaterialsUi()
{
	materialEditor = new cMaterialEditor(mainWindow->ui->scrollArea_material);
	mainWindow->ui->verticalLayout_materials->addWidget(materialEditor);

	materialListModel = new cMaterialItemModel(mainWindow->ui->scrollArea_material);
	materialListModel->AssignContainer(gPar);
	mainWindow->ui->widget_material_list_view->SetModel(materialListModel);

	if (systemData.settingsLoadedFromCLI)
	{
		materialListModel->Regenerate();
	}
	else
	{
		materialListModel->insertRows(0, 1, QModelIndex());
	}

	materialEditor->AssignMaterial(gPar, 1);
	QApplication::connect(materialEditor, SIGNAL(materialChanged(int)), materialListModel,
		SLOT(slotMaterialChanged(int)));

	QApplication::connect(mainWindow->ui->widget_material_list_view, SIGNAL(materialSelected(int)),
		mainWindow, SLOT(slotMaterialSelected(int)));

	QApplication::connect(mainWindow->ui->widget_material_list_view, SIGNAL(materialEdited()),
		mainWindow, SLOT(slotMaterialEdited()));
}

void cInterface::MaterialSelected(int matIndex)
{
	if (materialEditor)
	{
		delete materialEditor;
		materialEditor = new cMaterialEditor(mainWindow->ui->scrollArea_material);
		mainWindow->ui->verticalLayout_materials->addWidget(materialEditor);
		materialEditor->AssignMaterial(gPar, matIndex);
		QApplication::connect(materialEditor, SIGNAL(materialChanged(int)), materialListModel,
			SLOT(slotMaterialChanged(int)));
	}
}

void cInterface::StartupDefaultSettings(void)
{
	gPar->Set("DE_factor", 1.0);
	gPar->Set("ambient_occlusion_enabled", true);
	gPar->Set("ambient_occlusion_mode", (int)params::AOmodeScreenSpace);
	gPar->Set("ambient_occlusion_quality", 4);
	gPar->Set("shadows_enabled", true);
	gPar->Set("raytraced_reflections", true);
	gPar->Set("detail_level", 1.0);
}

void cInterface::DisableJuliaPointMode()
{
	QList<QVariant> itemMouseMove;
	itemMouseMove.append((int)RenderedImage::clickMoveCamera);

	QList<QVariant> itemJuliaMode;
	itemJuliaMode.append((int)RenderedImage::clickGetJuliaConstant);

	if (mainWindow->ui->comboBox_mouse_click_function->currentData() == itemJuliaMode)
	{
		int index = mainWindow->ui->comboBox_mouse_click_function->findData(itemMouseMove);
		mainWindow->ui->comboBox_mouse_click_function->setCurrentIndex(index);
		gMainInterface->renderedImage->setClickMode(itemMouseMove);
	}
}
// function to create icons with actual color in ColorButtons

void cInterface::ConnectProgressAndStatisticsSignals()
{
	QObject::connect(gFlightAnimation, SIGNAL(updateProgressAndStatus(const QString &,
																			 const QString &, double, cProgressText::enumProgressType)),
		mainWindow, SLOT(slotUpdateProgressAndStatus(
									const QString &, const QString &, double, cProgressText::enumProgressType)));
	QObject::connect(gFlightAnimation, SIGNAL(updateProgressHide(cProgressText::enumProgressType)),
		mainWindow, SLOT(slotUpdateProgressHide(cProgressText::enumProgressType)));
	QObject::connect(gFlightAnimation, SIGNAL(updateStatistics(cStatistics)),
		mainWindow->ui->widgetDockStatistics, SLOT(slotUpdateStatistics(cStatistics)));
	QObject::connect(gKeyframeAnimation, SIGNAL(updateProgressAndStatus(const QString &,
																				 const QString &, double, cProgressText::enumProgressType)),
		mainWindow, SLOT(slotUpdateProgressAndStatus(
									const QString &, const QString &, double, cProgressText::enumProgressType)));
	QObject::connect(gKeyframeAnimation, SIGNAL(updateProgressHide(cProgressText::enumProgressType)),
		gMainInterface->mainWindow, SLOT(slotUpdateProgressHide(cProgressText::enumProgressType)));
	QObject::connect(gKeyframeAnimation, SIGNAL(updateStatistics(cStatistics)),
		mainWindow->ui->widgetDockStatistics, SLOT(slotUpdateStatistics(cStatistics)));
}

void MakeIconForButton(QColor &color, QPushButton *pushbutton)
{
	const int w = 40;
	const int h = 15;
	QPixmap pix(w, h);
	QPainter painter(&pix);
	painter.fillRect(QRect(0, 0, w, h), color);
	painter.drawRect(0, 0, w - 1, h - 1);
	QIcon icon(pix);
	pushbutton->setIcon(icon);
	pushbutton->setIconSize(QSize(w, h));
}
