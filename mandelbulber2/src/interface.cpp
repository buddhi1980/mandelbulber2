/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cInterface class - operations connected directly with user interface
 */

#include "interface.hpp"

#include "ui_render_window.h"

#include "animation_flight.hpp"
#include "animation_keyframes.hpp"
#include "ao_modes.h"
#include "automated_widgets.hpp"
#include "calculate_distance.hpp"
#include "camera_movement_modes.h"
#include "camera_target.hpp"
#include "common_math.h"
#include "dof.hpp"
#include "error_message.hpp"
#include "fractparams.hpp"
#include "global_data.hpp"
#include "headless.h"
#include "initparameters.hpp"
#include "material_item_model.h"
#include "my_ui_loader.h"
#include "netrender.hpp"
#include "nine_fractals.hpp"
#include "opencl_engine_render_dof.h"
#include "opencl_engine_render_fractal.h"
#include "opencl_engine_render_ssao.h"
#include "opencl_global.h"
#include "post_effect_hdr_blur.h"
#include "queue.hpp"
#include "random.hpp"
#include "render_data.hpp"
#include "render_job.hpp"
#include "render_ssao.h"
#include "render_window.hpp"
#include "rendered_image_widget.hpp"
#include "rendering_configuration.hpp"
#include "settings.hpp"
#include "trace_behind.h"
#include "undo.h"

#include "qt/detached_window.h"
#include "qt/material_editor.h"
#include "qt/my_group_box.h"
#include "qt/my_progress_bar.h"
#include "qt/player_widget.hpp"
#include "qt/preview_file_dialog.h"
#include "qt/system_tray.hpp"

// custom includes
#ifdef USE_GAMEPAD
#include <QtGamepad/qgamepadmanager.h>
#endif // USE_GAMEPAD

cInterface *gMainInterface = nullptr;

// constructor of interface (loading of ui files)
cInterface::cInterface(QObject *parent) : QObject(parent)
{
	mainWindow = nullptr;
	detachedWindow = nullptr;
	headless = nullptr;
	qImage = nullptr;
	renderedImage = nullptr;
	imageSequencePlayer = nullptr;
	mainImage = nullptr;
	progressBar = nullptr;
	progressBarAnimation = nullptr;
	progressBarQueueImage = nullptr;
	progressBarQueueAnimation = nullptr;
	progressBarFrame = nullptr;
	progressBarLayout = nullptr;
	autoRefreshTimer = nullptr;
	stopRequestPulseTimer = nullptr;
	materialListModel = nullptr;
	materialEditor = nullptr;
	scrollAreaMaterialEditor = nullptr;
	systemTray = nullptr;
	stopRequest = false;
	repeatRequest = false;
	autoRefreshLastState = false;
	lockedDetailLevel = 1.0;
	lastSelectedMaterial = 1;
	numberOfStartedRenders = 0;
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
	if (qImage) delete qImage;
	if (mainImage) delete mainImage;
	if (headless) delete headless;
	if (mainWindow) delete mainWindow;
}

void cInterface::ShowUi()
{
	WriteLog("Prepare RenderWindow class", 2);

	QFont font = gApplication->font();
	font.setPointSizeF(gPar->Get<int>("ui_font_size"));
	gApplication->setFont(font);

	QFontMetrics fm(gApplication->font());
	int pixelFontSize = fm.height();

	int thumbnailSize = (pixelFontSize * 8);

	systemData.SetPreferredFontSize(pixelFontSize);
	systemData.SetPreferredThumbnailSize(thumbnailSize);

	mainWindow = new RenderWindow;

	WriteLog("Restoring window geometry", 2);

	if (gPar->Get<bool>("image_detached"))
	{
		DetachMainImageWidget();
		mainWindow->ui->actionDetach_image_from_main_window->setChecked(true);
	}

	mainWindow->restoreGeometry(settings.value("mainWindowGeometry").toByteArray());

	WriteLog("Restoring window state", 2);

	if (!mainWindow->restoreState(settings.value("mainWindowState").toByteArray()))
	{
		mainWindow->tabifyDockWidget(
			mainWindow->ui->dockWidget_materialEditor, mainWindow->ui->dockWidget_effects);
		mainWindow->tabifyDockWidget(
			mainWindow->ui->dockWidget_effects, mainWindow->ui->dockWidget_image_adjustments);
		mainWindow->tabifyDockWidget(
			mainWindow->ui->dockWidget_image_adjustments, mainWindow->ui->dockWidget_rendering_engine);
		mainWindow->tabifyDockWidget(
			mainWindow->ui->dockWidget_rendering_engine, mainWindow->ui->dockWidget_objects);
		mainWindow->ui->dockWidget_animation->hide();
		mainWindow->ui->dockWidget_info->hide();
		mainWindow->ui->dockWidget_gamepad_dock->hide();
		mainWindow->ui->dockWidget_histogram->hide();
		mainWindow->ui->dockWidget_queue_dock->hide();
	}

	mainWindow->setFont(font);

#ifdef __APPLE__
	mainWindow->ui->actionAbout_Qt->setText(QApplication::translate("RenderWindow", "Info &Qt", 0));
	mainWindow->ui->actionAbout_Manual->setText(
		QApplication::translate("RenderWindow", "About &User Manual", 0));
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
	mainImage->CreatePreview(1.0, 800, 600, renderedImage);
	mainImage->CompileImage();
	mainImage->ConvertTo8bit();
	mainImage->UpdatePreview();
	mainImage->SetAsMainImage();
	renderedImage->setMinimumSize(
		int(mainImage->GetPreviewWidth()), int(mainImage->GetPreviewHeight()));
	renderedImage->AssignImage(mainImage);
	renderedImage->AssignParameters(gPar, gParFractal);

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

	QFrame *progressBarFrameInternal = new QFrame;
	progressBarFrameInternal->setLayout(progressBarLayout);
	mainWindow->ui->statusbar->addPermanentWidget(progressBarFrameInternal);

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
		mainWindow->ui->dockWidget_gamepad_dock = nullptr;
	}
#endif

#ifndef USE_OPENCL
	mainWindow->GetWidgetDockNavigation()->EnableOpenCLModeComboBox(false);
#else
	mainWindow->GetWidgetDockNavigation()->EnableOpenCLModeComboBox(
		gPar->Get<bool>("opencl_enabled"));
#endif

	mainWindow->ui->actionImport_settings_from_Mandelbulb3d->setVisible(false);

	if (gPar->Get<bool>("ui_colorize"))
		ColorizeGroupBoxes(mainWindow, gPar->Get<int>("ui_colorize_random_seed"));

	renderedImage->show();

	mainWindow->setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
	mainWindow->setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
	mainWindow->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
	mainWindow->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);

	// loading default ui for all fractal components
	mainWindow->ui->widgetDockFractal->InitializeFractalUi();
	InitMaterialsUi();
	scrollAreaMaterialEditor = mainWindow->ui->scrollArea_material;

	// change some default settings with keeping compatibility with older versions
	StartupDefaultSettings();

	ComboMouseClickUpdate();

	mainWindow->slotPopulateRecentSettings();
	mainWindow->slotPopulateCustomWindowStates();
	systemTray = new cSystemTray(mainImage, mainWindow);

	// installing event filter for disabling tooltips
	gApplication->installEventFilter(mainWindow);

	WriteLog("cInterface::ConnectSignals(void)", 2);
	ConnectSignals();
	WriteLog("cInterface::ConnectSignals(void) finished", 2);
}

void cInterface::ConnectSignals() const
{
	// other

	connect(mainWindow->ui->checkBox_show_cursor, SIGNAL(stateChanged(int)), mainWindow,
		SLOT(slotChangedCheckBoxCursorVisibility(int)));

	connect(mainWindow->ui->comboBox_mouse_click_function, SIGNAL(currentIndexChanged(int)),
		mainWindow, SLOT(slotChangedComboMouseClickFunction(int)));

	connect(mainWindow->ui->comboBox_grid_type, SIGNAL(currentIndexChanged(int)), mainWindow,
		SLOT(slotChangedComboGridType(int)));

	connect(mainWindow, SIGNAL(AppendToLog(const QString &)), mainWindow->ui->log_text,
		SLOT(appendMessage(const QString &)));

	// menu actions
	connect(mainWindow->ui->actionQuit, SIGNAL(triggered()), mainWindow, SLOT(slotQuit()));
	connect(mainWindow->ui->actionSave_docks_positions, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuSaveDocksPositions()));
	connect(mainWindow->ui->actionDefault_docks_positions, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuResetDocksPositions()));
	connect(mainWindow->ui->actionAnimation_docks_positions, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuAnimationDocksPositions()));
	connect(mainWindow->ui->actionStack_all_docks, SIGNAL(triggered()), mainWindow,
		SLOT(slotStackAllDocks()));
	connect(mainWindow->ui->actionShow_animation_dock, SIGNAL(triggered()), mainWindow,
		SLOT(slotUpdateDocksAndToolbarByAction()));
	connect(mainWindow->ui->actionShow_toolbar, SIGNAL(triggered()), mainWindow,
		SLOT(slotUpdateDocksAndToolbarByAction()));
	connect(mainWindow->ui->actionShow_info_dock, SIGNAL(triggered()), mainWindow,
		SLOT(slotUpdateDocksAndToolbarByAction()));
	connect(mainWindow->ui->actionShow_statistics_dock, SIGNAL(triggered()), mainWindow,
		SLOT(slotUpdateDocksAndToolbarByAction()));
	connect(mainWindow->ui->actionShow_gamepad_dock, SIGNAL(triggered()), mainWindow,
		SLOT(slotUpdateDocksAndToolbarByAction()));
	connect(mainWindow->ui->actionShow_queue_dock, SIGNAL(triggered()), mainWindow,
		SLOT(slotUpdateDocksAndToolbarByAction()));
	connect(mainWindow->ui->actionShow_measurement_dock, SIGNAL(triggered()), mainWindow,
		SLOT(slotUpdateDocksAndToolbarByAction()));
	connect(mainWindow->ui->actionSave_settings, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuSaveSettings()));
	connect(mainWindow->ui->actionSave_settings_to_clipboard, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuSaveSettingsToClipboard()));
	connect(mainWindow->ui->actionLoad_settings, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuLoadSettings()));
	connect(mainWindow->ui->actionLoad_settings_from_clipboard, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuLoadSettingsFromClipboard()));
	connect(mainWindow->ui->actionLoad_example, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuLoadExample()));
	connect(mainWindow->ui->actionImport_settings_from_old_Mandelbulber, SIGNAL(triggered()),
		mainWindow, SLOT(slotImportOldSettings()));
	connect(mainWindow->ui->actionImport_settings_from_Mandelbulb3d, SIGNAL(triggered()), mainWindow,
		SLOT(slotImportMandelbulb3dSettings()));
	connect(mainWindow->ui->actionExportVoxelLayers, SIGNAL(triggered()), mainWindow,
		SLOT(slotExportVoxelLayers()));
	connect(
		mainWindow->ui->actionExport_Mesh, SIGNAL(triggered()), mainWindow, SLOT(slotExportMesh()));
	connect(mainWindow->ui->actionSave_as_JPG, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuSaveImageJPEG()));
	connect(mainWindow->ui->actionSave_as_PNG, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuSaveImagePNG()));
	connect(mainWindow->ui->actionSave_as_PNG_16_bit, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuSaveImagePNG16()));
	connect(mainWindow->ui->actionSave_as_PNG_16_bit_with_alpha_channel, SIGNAL(triggered()),
		mainWindow, SLOT(slotMenuSaveImagePNG16Alpha()));
#ifdef USE_EXR
	connect(mainWindow->ui->actionSave_as_EXR, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuSaveImageEXR()));
#endif // USE_EXR

#ifdef USE_TIFF
	connect(mainWindow->ui->actionSave_as_TIFF, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuSaveImageTIFF()));
#endif // USE_TIFF

	connect(mainWindow->ui->actionAbout_Qt, SIGNAL(triggered()), mainWindow, SLOT(slotMenuAboutQt()));
	connect(mainWindow->ui->actionUser_Manual, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuAboutManual()));
	connect(
		mainWindow->ui->actionUser_News, SIGNAL(triggered()), mainWindow, SLOT(slotMenuAboutNews()));
	connect(mainWindow->ui->actionUser_HotKeys, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuAboutHotKeys()));
	connect(mainWindow->ui->actionAbout_Mandelbulber, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuAboutMandelbulber()));
	connect(mainWindow->ui->actionAbout_ThirdParty, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuAboutThirdParty()));
	connect(mainWindow->ui->actionUndo, SIGNAL(triggered()), mainWindow, SLOT(slotMenuUndo()));
	connect(mainWindow->ui->actionRedo, SIGNAL(triggered()), mainWindow, SLOT(slotMenuRedo()));
	connect(mainWindow->ui->actionProgramPreferences, SIGNAL(triggered()), mainWindow,
		SLOT(slotMenuProgramPreferences()));
	connect(mainWindow->ui->actionDetach_image_from_main_window, SIGNAL(triggered()), mainWindow,
		SLOT(slotDetachMainImage()));

	connect(mainWindow->ui->scrollAreaForImage, SIGNAL(resized(int, int)), mainWindow,
		SLOT(slotResizedScrolledAreaImage(int, int)));
	connect(mainWindow->ui->comboBox_image_preview_scale, SIGNAL(currentIndexChanged(int)),
		mainWindow, SLOT(slotChangedComboImageScale(int)));

	// rendered image widget
	connect(
		renderedImage, SIGNAL(mouseMoved(int, int)), mainWindow, SLOT(slotMouseMovedOnImage(int, int)));
	connect(renderedImage, SIGNAL(singleClick(int, int, Qt::MouseButton)), mainWindow,
		SLOT(slotMouseClickOnImage(int, int, Qt::MouseButton)));
	connect(renderedImage, SIGNAL(keyPress(QKeyEvent *)), mainWindow,
		SLOT(slotKeyPressOnImage(QKeyEvent *)));
	connect(renderedImage, SIGNAL(keyRelease(QKeyEvent *)), mainWindow,
		SLOT(slotKeyReleaseOnImage(QKeyEvent *)));
	connect(renderedImage, SIGNAL(mouseWheelRotatedWithCtrl(int, int, int)), mainWindow,
		SLOT(slotMouseWheelRotatedWithCtrlOnImage(int, int, int)));
	connect(renderedImage, SIGNAL(mouseDragStart(int, int, Qt::MouseButtons)), mainWindow,
		SLOT(slotMouseDragStart(int, int, Qt::MouseButtons)));
	connect(renderedImage, SIGNAL(mouseDragFinish()), mainWindow, SLOT(slotMouseDragFinish()));
	connect(renderedImage, SIGNAL(mouseDragDelta(int, int)), mainWindow,
		SLOT(slotMouseDragDelta(int, int)));

	connect(mainWindow->ui->widgetDockRenderingEngine, SIGNAL(stateChangedConnectDetailLevel(int)),
		mainWindow->ui->widgetImageAdjustments, SLOT(slotCheckedDetailLevelLock(int)));

	// DockWidgets and Toolbar

	connect(mainWindow->ui->toolBar, SIGNAL(visibilityChanged(bool)), mainWindow,
		SLOT(slotUpdateDocksAndToolbarByView()));
	connect(mainWindow->ui->dockWidget_animation, SIGNAL(visibilityChanged(bool)), mainWindow,
		SLOT(slotUpdateDocksAndToolbarByView()));
	connect(mainWindow->ui->dockWidget_info, SIGNAL(visibilityChanged(bool)), mainWindow,
		SLOT(slotUpdateDocksAndToolbarByView()));
	connect(mainWindow->ui->dockWidget_histogram, SIGNAL(visibilityChanged(bool)), mainWindow,
		SLOT(slotUpdateDocksAndToolbarByView()));
#ifdef USE_GAMEPAD
	connect(mainWindow->ui->dockWidget_gamepad_dock, SIGNAL(visibilityChanged(bool)), mainWindow,
		SLOT(slotUpdateDocksAndToolbarByView()));
#endif
	connect(mainWindow->ui->dockWidget_queue_dock, SIGNAL(visibilityChanged(bool)), mainWindow,
		SLOT(slotUpdateDocksAndToolbarByView()));
	connect(mainWindow->ui->dockWidget_measurement, SIGNAL(visibilityChanged(bool)), mainWindow,
		SLOT(slotUpdateDocksAndToolbarByView()));

	connect(mainWindow->ui->actionAdd_Settings_to_Toolbar, SIGNAL(triggered()), mainWindow,
		SLOT(slotPresetAddToToolbar()));
	connect(mainWindow->ui->actionAdd_CustomWindowStateToMenu, SIGNAL(triggered()), mainWindow,
		SLOT(slotCustomWindowStateAddToMenu()));
	connect(mainWindow->ui->actionRemove_Window_settings, SIGNAL(triggered()), mainWindow,
		SLOT(slotCustomWindowRemovePopup()));

	//------------------------------------------------
	mainWindow->slotUpdateDocksAndToolbarByView();
}

// Reading ad writing parameters from/to ui to/from parameters container
void cInterface::SynchronizeInterface(
	cParameterContainer *par, cFractalContainer *parFractal, qInterface::enumReadWrite mode) const
{
	WriteLog(
		"cInterface::SynchronizeInterface(cParameterContainer *par, cFractalContainer *parFractal, "
		"enumReadWrite mode)",
		2);

	if (!gInterfaceReadyForSynchronization && mode == qInterface::read) return;

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

	if (detachedWindow)
	{
		WriteLog("cInterface::SynchronizeInterface: materialEditor", 3);
		SynchronizeInterfaceWindow(detachedWindow, par, mode);
	}

	mainWindow->ui->widgetDockFractal->SynchronizeInterfaceFractals(par, parFractal, mode);
}

void cInterface::StartRender(bool noUndo)
{
	numberOfStartedRenders++;
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

	gNetRender->SetAnimation(false);

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

	connect(renderJob, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
		mainWindow, SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
	connect(renderJob, SIGNAL(updateStatistics(cStatistics)), mainWindow->ui->widgetDockStatistics,
		SLOT(slotUpdateStatistics(cStatistics)));
	connect(renderJob, SIGNAL(fullyRendered(const QString &, const QString &)), systemTray,
		SLOT(showMessage(const QString &, const QString &)));
	connect(renderJob, SIGNAL(updateImage()), renderedImage, SLOT(update()));
	connect(renderJob, SIGNAL(sendRenderedTilesList(QList<sRenderedTileData>)), renderedImage,
		SLOT(showRenderedTilesList(QList<sRenderedTileData>)));

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
	gKeyframeAnimation->UpdateActualCameraPosition(gPar->Get<CVector3>("camera"));

	QThread *thread = new QThread; // deleted by deleteLater()
	renderJob->moveToThread(thread);
	QObject::connect(thread, SIGNAL(started()), renderJob, SLOT(slotExecute()));
	QObject::connect(renderJob, SIGNAL(finished()), thread, SLOT(quit()));
	QObject::connect(renderJob, SIGNAL(finished()), renderJob, SLOT(deleteLater()));
	QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

	thread->setObjectName("RenderJob");
	thread->start();
	numberOfStartedRenders--;
}

void cInterface::MoveCamera(QString buttonName, bool synchronizeAndRender)
{
	using namespace cameraMovementEnums;

	WriteLog("cInterface::MoveCamera(QString buttonName): button: " + buttonName, 2);

	// get data from interface
	if (synchronizeAndRender) SynchronizeInterface(gPar, gParFractal, qInterface::read);
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
		enumCameraMovementStepMode(gPar->Get<int>("camera_absolute_distance_mode"));
	enumCameraMovementMode movementMode =
		enumCameraMovementMode(gPar->Get<int>("camera_movement_mode"));

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
		cCameraTarget::enumRotationMode(gPar->Get<int>("camera_straight_rotation"));
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

	if (synchronizeAndRender) SynchronizeInterface(gPar, gParFractal, qInterface::write);
	if (synchronizeAndRender) StartRender();
}

void cInterface::CameraOrTargetEdited() const
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
		cCameraTarget::enumRotationMode(gPar->Get<int>("camera_straight_rotation"));
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

void cInterface::RotateCamera(QString buttonName, bool synchronizeAndRender)
{
	using namespace cameraMovementEnums;

	WriteLog("cInterface::RotateCamera(QString buttonName): button: " + buttonName, 2);

	// get data from interface
	if (synchronizeAndRender) SynchronizeInterface(gPar, gParFractal, qInterface::read);
	CVector3 camera = gPar->Get<CVector3>("camera");
	CVector3 target = gPar->Get<CVector3>("target");
	CVector3 topVector = gPar->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);
	double distance = cameraTarget.GetDistance();

	enumCameraRotationMode rotationMode =
		enumCameraRotationMode(gPar->Get<int>("camera_rotation_mode"));
	cCameraTarget::enumRotationMode rollMode =
		cCameraTarget::enumRotationMode(gPar->Get<int>("camera_straight_rotation"));

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

	if (synchronizeAndRender) SynchronizeInterface(gPar, gParFractal, qInterface::write);
	if (synchronizeAndRender) StartRender();
}

void cInterface::RotationEdited() const
{
	using namespace cameraMovementEnums;

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
		enumCameraRotationMode(gPar->Get<int>("camera_rotation_mode"));

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

void cInterface::CameraDistanceEdited() const
{
	using namespace cameraMovementEnums;

	WriteLog("cInterface::CameraDistanceEdited()", 2);

	SynchronizeInterfaceWindow(mainWindow->ui->dockWidget_navigation, gPar, qInterface::read);
	CVector3 camera = gPar->Get<CVector3>("camera");
	CVector3 target = gPar->Get<CVector3>("target");
	CVector3 topVector = gPar->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);
	CVector3 forwardVector = cameraTarget.GetForwardVector();

	double distance = gPar->Get<double>("camera_distance_to_target");

	enumCameraMovementMode movementMode =
		enumCameraMovementMode(gPar->Get<int>("camera_movement_mode"));
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

void cInterface::IFSDefaultsDodecahedron(cParameterContainer *parFractal) const
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

void cInterface::IFSDefaultsIcosahedron(cParameterContainer *parFractal) const
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
	if (!mainImage->IsUsed())
	{
		SynchronizeInterface(gPar, gParFractal, qInterface::read);
		sImageAdjustments imageAdjustments;
		imageAdjustments.brightness = gPar->Get<float>("brightness");
		imageAdjustments.contrast = gPar->Get<float>("contrast");
		imageAdjustments.imageGamma = gPar->Get<float>("gamma");
		imageAdjustments.saturation = gPar->Get<float>("saturation");
		imageAdjustments.hdrEnabled = gPar->Get<bool>("hdr");

		mainImage->SetImageParameters(imageAdjustments);
		mainImage->CompileImage();

		mainImage->ConvertTo8bit();
		mainImage->UpdatePreview();
		mainImage->GetImageWidget()->update();
	}
	else
	{
		cErrorMessage::showMessage(
			QObject::tr("You cannot apply changes during rendering. You will do this after rendering."),
			cErrorMessage::warningMessage, mainWindow);
	}
}

void cInterface::RefreshPostEffects()
{
	if (!mainImage->IsUsed())
	{
		mainImage->NullPostEffect();

		RefreshMainImage();

		// replace image size parameters in case if user changed image size just before image update
		gPar->Set("image_width", int(mainImage->GetWidth()));
		gPar->Set("image_height", int(mainImage->GetHeight()));

		stopRequest = false;
		if (gPar->Get<bool>("ambient_occlusion_enabled")
				&& gPar->Get<int>("ambient_occlusion_mode") == params::AOModeScreenSpace)
		{
			if (gPar->Get<bool>("opencl_enabled")
					&& cOpenClEngineRenderFractal::enumClRenderEngineMode(gPar->Get<int>("opencl_mode"))
							 != cOpenClEngineRenderFractal::clRenderEngineTypeNone)
			{
#ifdef USE_OPENCL
				sParamRender params(gPar);
				gOpenCl->openClEngineRenderSSAO->Lock();
				cRegion<int> region(0, 0, mainImage->GetWidth(), mainImage->GetHeight());
				gOpenCl->openClEngineRenderSSAO->SetParameters(&params, region);
				if (gOpenCl->openClEngineRenderSSAO->LoadSourcesAndCompile(gPar))
				{
					gOpenCl->openClEngineRenderSSAO->CreateKernel4Program(gPar);
					size_t neededMem = gOpenCl->openClEngineRenderSSAO->CalcNeededMemory();
					WriteLogDouble("OpenCl render SSAO - needed mem:", neededMem / 1048576.0, 2);
					if (neededMem / 1048576 < size_t(gPar->Get<int>("opencl_memory_limit")))
					{
						gOpenCl->openClEngineRenderSSAO->PreAllocateBuffers(gPar);
						gOpenCl->openClEngineRenderSSAO->CreateCommandQueue();
						gOpenCl->openClEngineRenderSSAO->Render(mainImage, &stopRequest);
					}
					else
					{
						cErrorMessage::showMessage(
							QObject::tr("Not enough free memory in OpenCL device to render SSAO effect!"),
							cErrorMessage::errorMessage, mainWindow);
					}
				}
				gOpenCl->openClEngineRenderSSAO->ReleaseMemory();
				gOpenCl->openClEngineRenderSSAO->Unlock();
#endif
			}
			else
			{
				sParamRender params(gPar);
				sRenderData data;
				data.stopRequest = &stopRequest;
				data.screenRegion = cRegion<int>(0, 0, mainImage->GetWidth(), mainImage->GetHeight());
				cRenderSSAO rendererSSAO(&params, &data, mainImage);
				QObject::connect(&rendererSSAO,
					SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), mainWindow,
					SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
				connect(&rendererSSAO, SIGNAL(updateImage()), renderedImage, SLOT(update()));

				rendererSSAO.RenderSSAO();

				mainImage->CompileImage();
				mainImage->ConvertTo8bit();
				mainImage->UpdatePreview();
				mainImage->GetImageWidget()->update();
			}
		}

		if (gPar->Get<bool>("DOF_enabled"))
		{
			if (gPar->Get<bool>("opencl_enabled")
					&& cOpenClEngineRenderFractal::enumClRenderEngineMode(gPar->Get<int>("opencl_mode"))
							 != cOpenClEngineRenderFractal::clRenderEngineTypeNone)
			{
#ifdef USE_OPENCL
				cRegion<int> screenRegion(0, 0, mainImage->GetWidth(), mainImage->GetHeight());
				sParamRender params(gPar);
				gOpenCl->openclEngineRenderDOF->RenderDOF(
					&params, gPar, mainImage, &stopRequest, screenRegion);
#endif
			}
			else
			{
				sParamRender params(gPar);
				// cRenderingConfiguration config;
				cPostRenderingDOF dof(mainImage);
				connect(&dof, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
					mainWindow, SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
				connect(&dof, SIGNAL(updateImage()), renderedImage, SLOT(update()));
				cRegion<int> screenRegion(0, 0, mainImage->GetWidth(), mainImage->GetHeight());
				dof.Render(screenRegion,
					params.DOFRadius * (mainImage->GetWidth() + mainImage->GetHeight()) / 2000.0,
					params.DOFFocus, params.DOFNumberOfPasses, params.DOFBlurOpacity, params.DOFMaxRadius,
					&stopRequest);
			}
		}

		if (gPar->Get<bool>("hdr_blur_enabled"))
		{
			cPostEffectHdrBlur *hdrBlur = new cPostEffectHdrBlur(mainImage);
			double blurRadius = gPar->Get<double>("hdr_blur_radius");
			double blurIntensity = gPar->Get<double>("hdr_blur_intensity");
			hdrBlur->SetParameters(blurRadius, blurIntensity);
			QObject::connect(hdrBlur,
				SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), mainWindow,
				SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
			hdrBlur->Render(&stopRequest);
			delete hdrBlur;
		}

		mainImage->CompileImage();

		mainImage->ConvertTo8bit();
		mainImage->UpdatePreview();
		mainImage->GetImageWidget()->update();
	}
	else
	{
		cErrorMessage::showMessage(
			QObject::tr("You cannot apply changes during rendering. You will do this after rendering."),
			cErrorMessage::warningMessage, mainWindow);
	}
}

void cInterface::AutoFog() const
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
	sParamRender *params = new sParamRender(par);
	cNineFractals *fractals = new cNineFractals(parFractal, par);
	sDistanceIn in(point, 0, false);
	sDistanceOut out;
	double distance = CalculateDistance(*params, *fractals, in, &out);
	delete params;
	delete fractals;
	return distance;
}

double cInterface::GetDistanceForPoint(CVector3 point) const
{
	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	double distance = GetDistanceForPoint(point, gPar, gParFractal);
	return distance;
}

void cInterface::SetByMouse(
	CVector2<double> screenPoint, Qt::MouseButton button, const QList<QVariant> &mode)
{
	using namespace cameraMovementEnums;

	WriteLog(
		QString("MoveCameraByMouse(CVector2<double> screenPoint, Qt::MouseButton button): button: ")
			+ button,
		2);
	// get data from interface

	RenderedImage::enumClickMode clickMode = RenderedImage::enumClickMode(mode.at(0).toInt());

	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	CVector3 camera = gPar->Get<CVector3>("camera");
	CVector3 target = gPar->Get<CVector3>("target");
	CVector3 topVector = gPar->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);

	enumCameraMovementStepMode stepMode =
		enumCameraMovementStepMode(gPar->Get<int>("camera_absolute_distance_mode"));
	enumCameraMovementMode movementMode =
		enumCameraMovementMode(gPar->Get<int>("camera_movement_mode"));
	params::enumPerspectiveType perspType =
		params::enumPerspectiveType(gPar->Get<int>("perspective_type"));
	cCameraTarget::enumRotationMode rollMode =
		cCameraTarget::enumRotationMode(gPar->Get<int>("camera_straight_rotation"));
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
					mainWindow->ui->widgetEffects->SynchronizeInterfaceBasicFogEnabled(gPar);
					StartRender();
					break;
				}
				case RenderedImage::clickDOFFocus:
				{
					DisablePeriodicRefresh();
					double DOF = depth;
					gPar->Set("DOF_focus", DOF);
					mainWindow->ui->widgetEffects->SynchronizeInterfaceDOFEnabled(gPar);
					gUndo.Store(gPar, gParFractal);
					RefreshPostEffects();
					ReEnablePeriodicRefresh();
					break;
				}
				case RenderedImage::clickPlaceLight:
				{
					double frontDist = gPar->Get<double>("aux_light_manual_placement_dist");
					bool placeBehind = gPar->Get<bool>("aux_light_place_behind");
					double distanceLimit = gPar->Get<double>("view_distance_max");
					CVector3 pointCorrected;
					if (!placeBehind)
					{
						pointCorrected = point - viewVector * frontDist;
					}
					else
					{
						double distanceBehind = traceBehindFractal(gPar, gParFractal, frontDist, viewVector,
							depth, 1.0 / mainImage->GetHeight(), distanceLimit);
						pointCorrected = point + viewVector * distanceBehind;
					}
					double estDistance = GetDistanceForPoint(pointCorrected, gPar, gParFractal);
					double intensity = estDistance * estDistance;
					int lightNumber = mode.at(1).toInt();
					gPar->Set("aux_light_position", lightNumber, pointCorrected);
					gPar->Set("aux_light_intensity", lightNumber, intensity);
					mainWindow->ui->widgetEffects->SynchronizeInterfaceLights(gPar);
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
					mainWindow->ui->widgetEffects->SynchronizeInterfaceRandomLights(gPar);
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
						mainWindow->slotUpdateDocksAndToolbarByAction();
					}
					ReEnablePeriodicRefresh();
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
					SetBoundingBoxAsLimits(limitMin, limitMax);
					break;
				}
			}
		}
	}
}

void cInterface::MouseDragStart(
	CVector2<double> screenPoint, Qt::MouseButtons button, const QList<QVariant> &mode)
{
	RenderedImage::enumClickMode clickMode = RenderedImage::enumClickMode(mode.at(0).toInt());

	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	CVector3 camera = gPar->Get<CVector3>("camera");
	CVector3 target = gPar->Get<CVector3>("target");
	CVector3 topVector = gPar->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);

	params::enumPerspectiveType perspType =
		params::enumPerspectiveType(gPar->Get<int>("perspective_type"));
	double sweetSpotHAngle = gPar->Get<double>("sweet_spot_horizontal_angle") / 180.0 * M_PI;
	double sweetSpotVAngle = gPar->Get<double>("sweet_spot_vertical_angle") / 180.0 * M_PI;
	double fov = gPar->Get<double>("fov");
	bool legacyCoordinateSystem = gPar->Get<bool>("legacy_coordinate_system");
	double reverse = legacyCoordinateSystem ? -1.0 : 1.0;

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

			cameraDragData.startCamera = camera;
			cameraDragData.startTarget = target;
			cameraDragData.startTopVector = topVector;
			cameraDragData.startIndicatedPoint = point;
			cameraDragData.button = button;
			cameraDragData.startScreenPoint = screenPoint;
			cameraDragData.startNormalizedPoint = normalizedPoint;
			cameraDragData.startZ = depth;
			cameraDragData.lastRefreshTime.restart();
			cameraDragData.lastStartRenderingTime = 0;

			if (clickMode == RenderedImage::clickMoveCamera)
			{
				cameraDragData.cameraDraggingStarted = true;
			}
		}
	}
}

void cInterface::MouseDragFinish()
{
	cameraDragData.cameraDraggingStarted = false;
}

void cInterface::MouseDragDelta(int dx, int dy)
{
	if (cameraDragData.cameraDraggingStarted)
	{
		if (numberOfStartedRenders > 1) stopRequest = true;

		if (cameraDragData.lastRefreshTime.elapsed()
					> gPar->Get<double>("auto_refresh_period") * 1000 + cameraDragData.lastStartRenderingTime
				&& numberOfStartedRenders < 2)
		{
			cameraDragData.lastRefreshTime.restart();
			params::enumPerspectiveType perspType =
				params::enumPerspectiveType(gPar->Get<int>("perspective_type"));
			double sweetSpotHAngle = gPar->Get<double>("sweet_spot_horizontal_angle") / 180.0 * M_PI;
			double sweetSpotVAngle = gPar->Get<double>("sweet_spot_vertical_angle") / 180.0 * M_PI;
			bool legacyCoordinateSystem = gPar->Get<bool>("legacy_coordinate_system");
			double reverse = legacyCoordinateSystem ? -1.0 : 1.0;
			double fov = gPar->Get<double>("fov");
			cCameraTarget::enumRotationMode rollMode =
				cCameraTarget::enumRotationMode(gPar->Get<int>("camera_straight_rotation"));

			CVector2<double> newScreenPoint(
				cameraDragData.startScreenPoint.x - dx, cameraDragData.startScreenPoint.y - dy);
			CVector2<double> imagePoint = newScreenPoint / mainImage->GetPreviewScale();

			int width = mainImage->GetWidth();
			int height = mainImage->GetHeight();
			double aspectRatio = double(width) / height;
			if (perspType == params::perspEquirectangular) aspectRatio = 2.0;

			switch (cameraDragData.button)
			{
				case Qt::LeftButton:
				{
					CVector3 camera = cameraDragData.startCamera;

					cCameraTarget cameraTarget(
						camera, cameraDragData.startTarget, cameraDragData.startTopVector);

					CVector3 angles = cameraTarget.GetRotation();
					CRotationMatrix mRot;
					mRot.SetRotation(angles);
					mRot.RotateZ(-sweetSpotHAngle);
					mRot.RotateX(sweetSpotVAngle);

					CVector2<double> normalizedPoint;
					normalizedPoint.x = (imagePoint.x / width - 0.5) * aspectRatio;
					normalizedPoint.y = (imagePoint.y / height - 0.5) * (-1.0) * reverse;

					CVector3 viewVector = CalculateViewVector(normalizedPoint, fov, perspType, mRot);

					CVector3 point = camera + viewVector * cameraDragData.startZ;
					CVector3 deltaPoint = point - cameraDragData.startIndicatedPoint;

					CVector3 pointCamera = camera - point;
					pointCamera.Normalize();
					CVector3 relativeVector;
					relativeVector.z = pointCamera.Dot(deltaPoint);
					relativeVector.x = pointCamera.Cross(cameraTarget.GetTopVector()).Dot(deltaPoint);
					relativeVector.y = pointCamera.Cross(cameraTarget.GetRightVector()).Dot(deltaPoint);
					double ratio = (camera - cameraDragData.startTarget).Length() / (camera - point).Length();

					if (perspType == params::perspThreePoint)
					{
						ratio /= -pointCamera.Dot(cameraTarget.GetForwardVector());
					}
					relativeVector *= ratio;

					CVector3 newTarget = cameraDragData.startTarget;
					newTarget -= relativeVector.x * cameraTarget.GetRightVector();
					newTarget += relativeVector.y * cameraTarget.GetTopVector();
					newTarget += relativeVector.z * cameraTarget.GetForwardVector();
					cameraTarget.SetTarget(newTarget, rollMode);

					gPar->Set("camera", camera);
					gPar->Set("target", newTarget);

					CVector3 topVector = cameraTarget.GetTopVector();
					gPar->Set("camera_top", topVector);
					CVector3 rotation = cameraTarget.GetRotation();
					gPar->Set("camera_rotation", rotation * (180.0 / M_PI));
					double dist = cameraTarget.GetDistance();
					gPar->Set("camera_distance_to_target", dist);
					break;
				}
				case Qt::RightButton:
				{
					cCameraTarget cameraTarget(
						cameraDragData.startCamera, cameraDragData.startTarget, cameraDragData.startTopVector);

					CVector3 shiftedCamera = cameraDragData.startCamera - cameraDragData.startIndicatedPoint;
					CVector3 shiftedTarget = cameraDragData.startTarget - cameraDragData.startIndicatedPoint;

					shiftedCamera = shiftedCamera.RotateAroundVectorByAngle(
						cameraTarget.GetTopVector(), (double)dx / mainImage->GetPreviewWidth() * M_PI_2);
					shiftedTarget = shiftedTarget.RotateAroundVectorByAngle(
						cameraTarget.GetTopVector(), (double)dx / mainImage->GetPreviewWidth() * M_PI_2);

					CVector3 newCamera = shiftedCamera + cameraDragData.startIndicatedPoint;
					CVector3 newTarget = shiftedTarget + cameraDragData.startIndicatedPoint;
					cameraTarget.SetCamera(newCamera, rollMode);
					cameraTarget.SetTarget(newTarget, rollMode);

					shiftedCamera = shiftedCamera.RotateAroundVectorByAngle(
						cameraTarget.GetRightVector(), (double)dy / mainImage->GetPreviewHeight() * M_PI_2);
					shiftedTarget = shiftedTarget.RotateAroundVectorByAngle(
						cameraTarget.GetRightVector(), (double)dy / mainImage->GetPreviewHeight() * M_PI_2);

					newCamera = shiftedCamera + cameraDragData.startIndicatedPoint;
					newTarget = shiftedTarget + cameraDragData.startIndicatedPoint;
					cameraTarget.SetCamera(newCamera, rollMode);
					cameraTarget.SetTarget(newTarget, rollMode);

					gPar->Set("camera", cameraTarget.GetCamera());
					gPar->Set("target", cameraTarget.GetTarget());

					CVector3 topVector = cameraTarget.GetTopVector();
					gPar->Set("camera_top", topVector);
					CVector3 rotation = cameraTarget.GetRotation();
					gPar->Set("camera_rotation", rotation * (180.0 / M_PI));
					double dist = cameraTarget.GetDistance();
					gPar->Set("camera_distance_to_target", dist);
					break;
				}
				case Qt::MiddleButton:
				{
					double angle = -(double)dx / mainImage->GetPreviewHeight() * M_PI_2;
					cCameraTarget cameraTarget(
						cameraDragData.startCamera, cameraDragData.startTarget, cameraDragData.startTopVector);
					CVector3 newTopVector = cameraDragData.startTopVector.RotateAroundVectorByAngle(
						cameraTarget.GetForwardVector(), angle);
					cameraTarget.SetCameraTargetTop(
						cameraDragData.startCamera, cameraDragData.startTarget, newTopVector);
					gPar->Set("camera_top", newTopVector);
					CVector3 rotation = cameraTarget.GetRotation();
					gPar->Set("camera_rotation", rotation * (180.0 / M_PI));
					break;
				}

				case (Qt::LeftButton | Qt::RightButton):
				{
					CVector3 camera = cameraDragData.startCamera;

					cCameraTarget cameraTarget(
						camera, cameraDragData.startTarget, cameraDragData.startTopVector);

					CVector3 angles = cameraTarget.GetRotation();
					CRotationMatrix mRot;
					mRot.SetRotation(angles);
					mRot.RotateZ(-sweetSpotHAngle);
					mRot.RotateX(sweetSpotVAngle);

					CVector2<double> normalizedPoint;
					normalizedPoint.x = (imagePoint.x / width - 0.5) * aspectRatio;
					normalizedPoint.y = (imagePoint.y / height - 0.5) * (-1.0) * reverse;

					CVector3 viewVector = CalculateViewVector(normalizedPoint, fov, perspType, mRot);

					CVector3 point = camera + viewVector * cameraDragData.startZ;
					CVector3 deltaPoint = point - cameraDragData.startIndicatedPoint;

					CVector3 newTarget = cameraDragData.startTarget + deltaPoint;
					CVector3 newCamera = cameraDragData.startCamera + deltaPoint;

					cameraTarget.SetCameraTargetTop(newCamera, newTarget, cameraTarget.GetTopVector());

					gPar->Set("camera", newCamera);
					gPar->Set("target", newTarget);

					CVector3 topVector = cameraTarget.GetTopVector();
					gPar->Set("camera_top", topVector);
					CVector3 rotation = cameraTarget.GetRotation();
					gPar->Set("camera_rotation", rotation * (180.0 / M_PI));
					double dist = cameraTarget.GetDistance();
					gPar->Set("camera_distance_to_target", dist);
					break;
				}

				default: break;
			}

			SynchronizeInterface(gPar, gParFractal, qInterface::write);

			QElapsedTimer timerStartRender;
			timerStartRender.start();
			StartRender();
			cameraDragData.lastStartRenderingTime = timerStartRender.elapsed();
		}
	}
}

void cInterface::MovementStepModeChanged(int mode) const
{
	using namespace cameraMovementEnums;

	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	enumCameraMovementStepMode stepMode = enumCameraMovementStepMode(mode);
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
	DisablePeriodicRefresh();
	gInterfaceReadyForSynchronization = false;
	if (gUndo.Undo(gPar, gParFractal, gAnimFrames, gKeyframes, &refreshFrames, &refreshKeyframes))
	{
		RebuildPrimitives(gPar);
		materialListModel->Regenerate();
		gInterfaceReadyForSynchronization = false;
		SynchronizeInterface(gPar, gParFractal, qInterface::write);
		if (refreshFrames) gFlightAnimation->RefreshTable();
		if (refreshKeyframes) gKeyframeAnimation->RefreshTable();
		StartRender(true);
	}
	gInterfaceReadyForSynchronization = true;
	ReEnablePeriodicRefresh();
}

void cInterface::Redo()
{
	bool refreshFrames = false;
	bool refreshKeyframes = false;
	DisablePeriodicRefresh();
	gInterfaceReadyForSynchronization = false;
	if (gUndo.Redo(gPar, gParFractal, gAnimFrames, gKeyframes, &refreshFrames, &refreshKeyframes))
	{
		RebuildPrimitives(gPar);
		materialListModel->Regenerate();
		gInterfaceReadyForSynchronization = true;
		SynchronizeInterface(gPar, gParFractal, qInterface::write);
		if (refreshFrames) gFlightAnimation->RefreshTable();
		if (refreshKeyframes) gKeyframeAnimation->RefreshTable();
		StartRender(true);
	}
	gInterfaceReadyForSynchronization = true;
	ReEnablePeriodicRefresh();
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
		params::enumPerspectiveType(gPar->Get<int>("perspective_type"));
	double DEFactor = gPar->Get<double>("DE_factor");

	cParameterContainer parTemp = *gPar;
	parTemp.Set("limits_enabled", false);
	parTemp.Set("interior_mode", false);

	// calculate size of the fractal in random directions
	double maxDist = 0.0;

	sParamRender *params = new sParamRender(gPar);
	cNineFractals *fractals = new cNineFractals(gParFractal, gPar);

	for (int i = 0; i < 50; i++)
	{
		cProgressText::ProgressStatusText(QObject::tr("Resetting view"),
			QObject::tr("Fractal size calculation"), i / 50.0, cProgressText::progress_IMAGE);
		CVector3 direction(
			Random(1000) / 500.0 - 1.0, Random(1000) / 500.0 - 1.0, Random(1000) / 500.0 - 1.0);
		direction.Normalize();
		double distStep;
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
			distStep = dist * DEFactor * 0.5;
			if (distStep > 5.0) distStep = 5.0;
			// qDebug() << "i" << i << "scan" << scan << "direction" << direction.Debug();
		}
		if (scan > maxDist) maxDist = scan;
	}
	cProgressText::ProgressStatusText(
		QObject::tr("Resetting view"), QObject::tr("Done"), 1.0, cProgressText::progress_IMAGE);
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

void cInterface::BoundingBoxMove(char dimension, double moveLower, double moveUpper)
{
	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	CVector3 limitMin = gPar->Get<CVector3>("limit_min");
	CVector3 limitMax = gPar->Get<CVector3>("limit_max");
	CVector3 limitDifference = limitMax - limitMin;
	switch (dimension)
	{
		case 'x':
			limitMin.x -= moveLower * limitDifference.x;
			limitMax.x += moveUpper * limitDifference.x;
			break;
		case 'y':
			limitMin.y -= moveLower * limitDifference.y;
			limitMax.y += moveUpper * limitDifference.y;
			break;
		case 'z':
			limitMin.z -= moveLower * limitDifference.z;
			limitMax.z += moveUpper * limitDifference.z;
			break;
	}
	gPar->Set("limit_min", limitMin);
	gPar->Set("limit_max", limitMax);
	SynchronizeInterface(gPar, gParFractal, qInterface::write);
}

void cInterface::SetBoundingBoxAsLimitsTotal()
{
	double outerBounding = gPar->Get<double>("limit_outer_bounding");
	CVector3 outerBoundingMin(-outerBounding, -outerBounding, -outerBounding);
	CVector3 outerBoundingMax(outerBounding, outerBounding, outerBounding);
	SetBoundingBoxAsLimits(outerBoundingMin, outerBoundingMax);
}

void cInterface::SetBoundingBoxAsLimits(CVector3 outerBoundingMin, CVector3 outerBoundingMax)
{
	CVector3 boundingCenter = (outerBoundingMin + outerBoundingMax) / 2;

	SynchronizeInterface(gPar, gParFractal, qInterface::read);

	cParameterContainer parTemp = *gPar;
	parTemp.Set("limits_enabled", false);
	parTemp.Set("interior_mode", false);

	sParamRender *params = new sParamRender(&parTemp);
	cNineFractals *fractals = new cNineFractals(gParFractal, &parTemp);

	CVector3 direction;
	CVector3 orthDirection;
	CVector3 point;
	double dist;

	stopRequest = false;

	// negative x limit
	cProgressText::ProgressStatusText(
		QObject::tr("bounding box as limit"), QObject::tr("Negative X Limit"), 0.0 / 6.0);
	direction = CVector3(1, 0, 0);
	orthDirection = CVector3(0, 1, 0);
	point = CVector3(outerBoundingMin.x, boundingCenter.y, boundingCenter.z);
	dist =
		CalculateDistanceMinPlane(*params, *fractals, point, direction, orthDirection, &stopRequest);
	double minX = point.x + dist;

	// negative y limit
	cProgressText::ProgressStatusText(
		QObject::tr("bounding box as limit"), QObject::tr("Negative Y Limit"), 1.0 / 6.0);
	direction = CVector3(0, 1, 0);
	orthDirection = CVector3(0, 0, 1);
	point = CVector3(boundingCenter.x, outerBoundingMin.y, boundingCenter.z);
	dist =
		CalculateDistanceMinPlane(*params, *fractals, point, direction, orthDirection, &stopRequest);
	double minY = point.y + dist;

	// negative z limit
	cProgressText::ProgressStatusText(
		QObject::tr("bounding box as limit"), QObject::tr("Negative Z Limit"), 2.0 / 6.0);
	direction = CVector3(0, 0, 1);
	orthDirection = CVector3(1, 0, 0);
	point = CVector3(boundingCenter.x, boundingCenter.y, outerBoundingMin.z);
	dist =
		CalculateDistanceMinPlane(*params, *fractals, point, direction, orthDirection, &stopRequest);
	double minZ = point.z + dist;

	// positive x limit
	cProgressText::ProgressStatusText(
		QObject::tr("bounding box as limit"), QObject::tr("Positive X Limit"), 3.0 / 6.0);
	direction = CVector3(-1, 0, 0);
	orthDirection = CVector3(0, -1, 0);
	point = CVector3(outerBoundingMax.x, boundingCenter.y, boundingCenter.z);
	dist =
		CalculateDistanceMinPlane(*params, *fractals, point, direction, orthDirection, &stopRequest);
	double maxX = point.x - dist;

	// positive y limit
	cProgressText::ProgressStatusText(
		QObject::tr("bounding box as limit"), QObject::tr("Positive Y Limit"), 4.0 / 6.0);
	direction = CVector3(0, -1, 0);
	orthDirection = CVector3(0, 0, -1);
	point = CVector3(boundingCenter.x, outerBoundingMax.y, boundingCenter.z);
	dist =
		CalculateDistanceMinPlane(*params, *fractals, point, direction, orthDirection, &stopRequest);
	double maxY = point.y - dist;

	// positive z limit
	cProgressText::ProgressStatusText(
		QObject::tr("bounding box as limit"), QObject::tr("Positive Z Limit"), 5.0 / 6.0);
	direction = CVector3(0, 0, -1);
	orthDirection = CVector3(-1, 0, 0);
	point = CVector3(boundingCenter.x, boundingCenter.y, outerBoundingMax.z);
	dist =
		CalculateDistanceMinPlane(*params, *fractals, point, direction, orthDirection, &stopRequest);
	double maxZ = point.z - dist;

	double medX = (maxX + minX) / 2.0;
	double medY = (maxY + minY) / 2.0;
	double medZ = (maxZ + minZ) / 2.0;
	double rangeX = maxX - minX;
	double rangeY = maxY - minY;
	double rangeZ = maxZ - minZ;

	gPar->Set("limit_min", CVector3(medX - rangeX * 0.6, medY - rangeY * 0.6, medZ - rangeZ * 0.6));
	gPar->Set("limit_max", CVector3(medX + rangeX * 0.6, medY + rangeY * 0.6, medZ + rangeZ * 0.6));

	cProgressText::ProgressStatusText(QObject::tr("bounding box as limit"), QObject::tr("Done"), 1.0);
	delete params;
	delete fractals;
	SynchronizeInterface(gPar, gParFractal, qInterface::write);
}

void cInterface::NewPrimitive(const QString &primitiveType, int index)
{
	QString primitiveName = QString("primitive_") + primitiveType;
	QString uiFileName = systemData.sharedDir + "formula" + QDir::separator() + "ui"
											 + QDir::separator() + primitiveName + ".ui";
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
			for (const auto &primitiveItem : listOfPrimitives)
			{
				if (objectType == primitiveItem.type && newId == primitiveItem.id) occupied = true;
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

	QHBoxLayout *buttonsLayout2 = new QHBoxLayout();
	layout->addLayout(buttonsLayout2);

	QPushButton *alignButton = new QPushButton(QObject::tr("Align rotation to camera"), mainWidget);
	alignButton->setObjectName(QString("alignButton_") + primitiveFullName);
	buttonsLayout2->addWidget(alignButton);

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
		for (auto &widget : listOfWidgets)
		{
			QString name = widget->objectName();
			int firstDash = name.indexOf('_');
			QString newName = name.insert(firstDash + 1, primitiveFullName + "_");
			widget->setObjectName(newName);
		}

		connect(deleteButton, SIGNAL(clicked()), mainWindow, SLOT(slotPressedButtonDeletePrimitive()));
		connect(setPositionButton, SIGNAL(clicked()), mainWindow,
			SLOT(slotPressedButtonSetPositionPrimitive()));
		connect(
			alignButton, SIGNAL(clicked()), mainWindow, SLOT(slotPressedButtonAlignPrimitiveAngle()));

		// adding parameters
		if (index == 0) // for only new primitive
		{
			InitPrimitiveParams(objectType, primitiveFullName, gPar);
			gPar->Set(primitiveFullName + "_enabled", true);
		}

		mainWindow->automatedWidgets->ConnectSignalsForSlidersInWindow(mainWidget);
		SynchronizeInterfaceWindow(mainWidget, gPar, qInterface::write);

		if (gPar->Get<bool>("ui_colorize"))
		{
			cInterface::ColorizeGroupBoxes(
				groupBox, gPar->Get<int>("ui_colorize_random_seed") + Random(65535));
		}

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
	for (const auto &primitiveItem : listOfPrimitives)
	{
		QString widgetName = QString("widgetmain_") + primitiveItem.name;
		QWidget *widget =
			mainWindow->ui->widgetDockFractal->GetContainerWithPrimitives()->findChild<QWidget *>(
				widgetName);
		delete widget;
	}
	listOfPrimitives.clear();

	QList<QString> listOfParameters = par->GetListOfParameters();
	for (auto &parameterName : listOfParameters)
	{
		if (parameterName.left(parameterName.indexOf('_')) == "primitive")
		{
			QStringList split = parameterName.split('_');
			QString primitiveName = split.at(0) + "_" + split.at(1) + "_" + split.at(2);
			QString objectTypeString = split.at(1);
			int index = split.at(2).toInt();

			bool found = false;
			for (const auto &listOfPrimitive : listOfPrimitives)
			{
				if (listOfPrimitive.name == primitiveName)
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

void cInterface::ComboMouseClickUpdate() const
{
	QComboBox *combo = mainWindow->ui->comboBox_mouse_click_function;
	int lastIndex = combo->currentIndex();

	combo->clear();
	QList<QVariant> item;

	item.clear();
	item.append(int(RenderedImage::clickDoNothing));
	combo->addItem(QObject::tr("No action"), item);

	item.clear();
	item.append(int(RenderedImage::clickMoveCamera));
	combo->addItem(QObject::tr("Move the camera"), item);

	item.clear();
	item.append(int(RenderedImage::clickFogVisibility));
	combo->addItem(QObject::tr("Set fog visibility"), item);

	item.clear();
	item.append(int(RenderedImage::clickDOFFocus));
	combo->addItem(QObject::tr("Set DOF focus"), item);

	item.clear();
	item.append(int(RenderedImage::clickGetJuliaConstant));
	combo->addItem(QObject::tr("Get Julia constant"), item);

	item.clear();
	item.append(int(RenderedImage::clickPlaceLight));
	item.append(1);
	combo->addItem(QObject::tr("Place light #1"), item);

	item.clear();
	item.append(int(RenderedImage::clickPlaceLight));
	item.append(2);
	combo->addItem(QObject::tr("Place light #2"), item);

	item.clear();
	item.append(int(RenderedImage::clickPlaceLight));
	item.append(3);
	combo->addItem(QObject::tr("Place light #3"), item);

	item.clear();
	item.append(int(RenderedImage::clickPlaceLight));
	item.append(4);
	combo->addItem(QObject::tr("Place light #4"), item);

	item.clear();
	item.append(int(RenderedImage::clickPlaceRandomLightCenter));
	combo->addItem(QObject::tr("Place random light center"), item);

	item.clear();
	item.append(int(RenderedImage::clickGetPoint));
	combo->addItem(QObject::tr("Get point coordinates"), item);

	item.clear();
	item.append(int(RenderedImage::clickWrapLimitsAroundObject));
	combo->addItem(QObject::tr("Wrap Limits around object"), item);

	if (listOfPrimitives.size() > 0)
	{
		for (const auto &primitiveItem : listOfPrimitives)
		{
			QString primitiveName = PrimitiveNames(primitiveItem.type);
			int index = primitiveItem.id;
			QString comboItemString =
				QString(QObject::tr("Place ")) + primitiveName + QString(" #") + QString::number(index);
			item.clear();
			item.append(int(RenderedImage::clickPlacePrimitive));
			item.append(int(primitiveItem.type));
			item.append(primitiveItem.id);
			item.append(primitiveItem.name);
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
	int closeResult;
	bool quitDoNotAskAgain = gPar->Get<bool>("quit_do_not_ask_again");
	QMessageBox *messageBox = nullptr;
	QAbstractButton *btnYesAndDoNotAskAgain = nullptr;
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
			SynchronizeInterface(gPar, gParFractal, qInterface::read);
			parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
			parSettings.SaveToFile(systemData.GetIniFile());

			while (cRenderJob::GetRunningJobCount() > 0)
			{
				gApplication->processEvents();
			}

			QFile::remove(systemData.GetAutosaveFile());

			if (detachedWindow)
			{
				settings.setValue("detachedWindowGeometry", detachedWindow->saveGeometry());
			}

			gApplication->quit();
			quit = true;
			break;
		}
	}
	if (messageBox) delete messageBox;
	return quit;
}

void cInterface::AutoRecovery() const
{
	if (QFile::exists(systemData.GetAutosaveFile()))
	{
		// auto recovery dialog
		QMessageBox::StandardButton reply;
		reply = QMessageBox::question(mainWindow->ui->centralwidget, QObject::tr("Auto recovery"),
			QObject::tr(
				"Application has not been closed properly\nDo you want to recover your latest work?"),
			QMessageBox::Yes | QMessageBox::No);

		if (reply == QMessageBox::Yes)
		{
			cSettings parSettings(cSettings::formatFullText);
			gInterfaceReadyForSynchronization = false;
			parSettings.LoadFromFile(systemData.GetAutosaveFile());
			parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
			gMainInterface->RebuildPrimitives(gPar);
			materialListModel->Regenerate();
			gInterfaceReadyForSynchronization = true;
			SynchronizeInterface(gPar, gParFractal, qInterface::write);
			gFlightAnimation->RefreshTable();
			gKeyframeAnimation->RefreshTable();
		}
		else
		{
			return;
		}
	}
}

// ReSharper disable once CppMemberFunctionMayBeStatic
bool cInterface::DataFolderUpgrade() const
{
#ifndef _WIN32
	if (systemData.IsUpgraded()) return false; // already upgraded, nothing to do
	bool upgradeDoNotAskAgain = gPar->Get<bool>("upgrade_do_not_ask_again");
	if (upgradeDoNotAskAgain) return false; // user does not want to upgrade ever

	QAbstractButton *btnNoAndDoNotAskAgain = nullptr;
	QMessageBox *messageBox = new QMessageBox(mainWindow);
	QString messageText = QObject::tr(
		"In Mandelbulber 2.10 the default data structure changed for linux and MacOS:\n"
		"Instead of keeping all working folders/files in ~/.mandelbulber these are now split into"
		"<ul><li><b>.mandelbulber</b> for program internal folders/files:<br>"
		"undo, toolbar, queue, thumbnails, mandelbulber.ini, miscellaneous meta files</li>"
		"<li><b>mandelbulber</b> for user defined folders/files:<br>"
		"settings, images, materials, slices, animation, textures</li></ul>\n"
		"Do you want to upgrade now to this new structure? Program will restart after upgrade.");
	messageBox->setText(messageText);
	messageBox->setTextFormat(Qt::RichText);
	messageBox->setWindowTitle(QObject::tr("Data folder upgrade"));
	messageBox->setIcon(QMessageBox::Question);
	messageBox->addButton(QMessageBox::Ok);
	messageBox->addButton(QMessageBox::Cancel);
	btnNoAndDoNotAskAgain =
		messageBox->addButton(QObject::tr("No, don't ask again"), QMessageBox::YesRole);
	messageBox->setDefaultButton(QMessageBox::Ok);
	int dialogResult = messageBox->exec();

	switch (dialogResult)
	{
		case QMessageBox::Cancel:
		default:
		{
			if (messageBox->clickedButton() == btnNoAndDoNotAskAgain)
				gPar->Set("upgrade_do_not_ask_again", true);
			break;
		}
		case QMessageBox::Ok:
		{
			systemData.Upgrade();
			// Needs restart
			QProcess::startDetached(qApp->arguments()[0]);
			delete messageBox;
			return true;
			break;
		}
	}
	delete messageBox;
#endif
	return false;
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
	tempParam.Set("opencl_mode", 0); // disable OpenCL
	for (int i = 1; i <= 4; i++)
	{
		tempParam.Set("aux_light_enabled", i, false);
		tempParam.Set("aux_light_volumetric_enabled", i, false);
	}

	int maxDimension = max(gPar->Get<int>("image_width"), gPar->Get<int>("image_height"));
	int newWidth = double(gPar->Get<int>("image_width")) / maxDimension * 256.0;
	int newHeight = double(gPar->Get<int>("image_height")) / maxDimension * 256.0;

	tempParam.Set("image_width", newWidth);
	tempParam.Set("image_height", newHeight);
	tempParam.Set("detail_level", 4.0);

	int scanCount = 0;
	double DEFactor = 1.0;
	double step = 1.0;

	cRenderJob *renderJob =
		new cRenderJob(&tempParam, &tempFractal, mainImage, &stopRequest, renderedImage);
	QObject::connect(renderJob, SIGNAL(updateStatistics(cStatistics)),
		mainWindow->ui->widgetDockStatistics, SLOT(slotUpdateStatistics(cStatistics)));
	connect(renderJob, SIGNAL(updateImage()), renderedImage, SLOT(update()));

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
		if (stopRequest || systemData.globalStopRequest) return;
		scanCount++;
		tempParam.Set("DE_factor", DEFactor);

		gPar->Set("DE_factor", DEFactor);
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
					DEFactor = DEFactor * 2.0;
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
				if (step < 0.05 * DEFactor) break;
				DEFactor += step;
			}
		}

		double progress = 1.0 - 1.0 / (1.0 + pow(qualityTarget / (missedDE - qualityTarget), 2.0));
		cProgressText::ProgressStatusText(QObject::tr("Looking for optimal DE factor"),
			QObject::tr("Percentage of wrong distance estimations: %1").arg(missedDE), progress,
			cProgressText::progress_IMAGE);

		step /= 2.0;
		DEFactor -= step;
	}

	gPar->Set("DE_factor", DEFactor);
	SynchronizeInterface(gPar, gParFractal, qInterface::write);
	cProgressText::ProgressStatusText(QObject::tr("Idle"),
		QObject::tr("Optimal DE factor is: %1 which gives %2% of bad distance estimations")
			.arg(DEFactor)
			.arg(missedDE),
		1.0, cProgressText::progress_IMAGE);

	ReEnablePeriodicRefresh();

	delete renderJob;
}

void cInterface::ResetFormula(int fractalNumber) const
{
	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	gUndo.Store(gPar, gParFractal, gAnimFrames, gKeyframes);
	cParameterContainer *fractal = &gParFractal->at(fractalNumber);
	fractal->ResetAllToDefault();

	QStringList listToReset = {"formula_iterations", "formula_weight", "formula_start_iteration",
		"formula_stop_iteration", "julia_mode", "julia_c", "fractal_constant_factor", "initial_waxis",
		"formula_position", "formula_rotation", "formula_repeat", "formula_scale",
		"dont_add_c_constant", "check_for_bailout"};

	for (int i = 0; i < listToReset.size(); i++)
	{
		cOneParameter oneParameter =
			gPar->GetAsOneParameter(listToReset[i] + QString("_%1").arg(fractalNumber + 1));
		oneParameter.SetMultiVal(oneParameter.GetMultiVal(valueDefault), valueActual);
		gPar->SetFromOneParameter(listToReset[i] + QString("_%1").arg(fractalNumber + 1), oneParameter);
	}

	gUndo.Store(gPar, gParFractal, gAnimFrames, gKeyframes);
	SynchronizeInterface(gPar, gParFractal, qInterface::write);
}

void cInterface::PeriodicRefresh()
{
	if (!cameraDragData.cameraDraggingStarted)
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
	}

	autoRefreshTimer->start(int(gPar->Get<double>("auto_refresh_period") * 1000.0));
}

void cInterface::DisablePeriodicRefresh()
{
	if (mainWindow)
	{
		autoRefreshLastState = mainWindow->ui->widgetDockNavigation->AutoRefreshIsChecked();
		mainWindow->ui->widgetDockNavigation->AutoRefreshSetChecked(false);
		gPar->Set("auto_refresh", false);
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
		gPar->Set("auto_refresh", true);
	}
}

void cInterface::InitPeriodicRefresh()
{
	autoRefreshTimer = new QTimer(mainWindow);
	autoRefreshTimer->setSingleShot(true);
	connect(autoRefreshTimer, SIGNAL(timeout()), mainWindow, SLOT(slotAutoRefresh()));
	autoRefreshTimer->start(int(gPar->Get<double>("auto_refresh_period") * 1000.0));
}

void cInterface::InitMaterialsUi()
{
	materialEditor = new cMaterialEditor(mainWindow->ui->scrollArea_material);

	if (gPar->Get<bool>("ui_colorize"))
		materialEditor->Colorize(gPar->Get<int>("ui_colorize_random_seed"));

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
	connect(materialEditor, SIGNAL(materialChanged(int)), materialListModel,
		SLOT(slotMaterialChanged(int)));

	connect(mainWindow->ui->widget_material_list_view, SIGNAL(materialSelected(int)), mainWindow,
		SLOT(slotMaterialSelected(int)));

	connect(mainWindow->ui->widget_material_list_view, SIGNAL(materialEdited()), mainWindow,
		SLOT(slotMaterialEdited()));
}

void cInterface::MaterialSelected(int matIndex)
{
	if (materialEditor)
	{
		delete materialEditor;
		materialEditor = new cMaterialEditor(mainWindow->ui->scrollArea_material);

		if (gPar->Get<bool>("ui_colorize"))
			materialEditor->Colorize(gPar->Get<int>("ui_colorize_random_seed"));

		mainWindow->ui->verticalLayout_materials->addWidget(materialEditor);
		materialEditor->AssignMaterial(gPar, matIndex);
		connect(materialEditor, SIGNAL(materialChanged(int)), materialListModel,
			SLOT(slotMaterialChanged(int)));

		if (matIndex != lastSelectedMaterial)
		{
			mainWindow->ui->dockWidget_materialEditor->raise();
			lastSelectedMaterial = matIndex;
		}
	}
}

void cInterface::StartupDefaultSettings()
{
	gPar->Set("DE_factor", 1.0);
	gPar->Set("ambient_occlusion_enabled", true);
	gPar->Set("ambient_occlusion_mode", int(params::AOModeScreenSpace));
	gPar->Set("ambient_occlusion_quality", 4);
	gPar->Set("shadows_enabled", true);
	gPar->Set("raytraced_reflections", true);
	gPar->Set("detail_level", 1.0);
}

void cInterface::DisableJuliaPointMode() const
{
	QList<QVariant> itemMouseMove;
	itemMouseMove.append(int(RenderedImage::clickMoveCamera));

	QList<QVariant> itemJuliaMode;
	itemJuliaMode.append(int(RenderedImage::clickGetJuliaConstant));

	if (mainWindow->ui->comboBox_mouse_click_function->currentData() == itemJuliaMode)
	{
		int index = mainWindow->ui->comboBox_mouse_click_function->findData(itemMouseMove);
		mainWindow->ui->comboBox_mouse_click_function->setCurrentIndex(index);
		renderedImage->setClickMode(itemMouseMove);
	}
}
// function to create icons with actual color in ColorButtons

void cInterface::ConnectProgressAndStatisticsSignals() const
{
	QObject::connect(gFlightAnimation,
		SIGNAL(updateProgressAndStatus(
			const QString &, const QString &, double, cProgressText::enumProgressType)),
		mainWindow,
		SLOT(slotUpdateProgressAndStatus(
			const QString &, const QString &, double, cProgressText::enumProgressType)));
	QObject::connect(gFlightAnimation, SIGNAL(updateProgressHide(cProgressText::enumProgressType)),
		mainWindow, SLOT(slotUpdateProgressHide(cProgressText::enumProgressType)));
	QObject::connect(gFlightAnimation, SIGNAL(updateStatistics(cStatistics)),
		mainWindow->ui->widgetDockStatistics, SLOT(slotUpdateStatistics(cStatistics)));
	QObject::connect(gKeyframeAnimation,
		SIGNAL(updateProgressAndStatus(
			const QString &, const QString &, double, cProgressText::enumProgressType)),
		mainWindow,
		SLOT(slotUpdateProgressAndStatus(
			const QString &, const QString &, double, cProgressText::enumProgressType)));
	QObject::connect(gKeyframeAnimation, SIGNAL(updateProgressHide(cProgressText::enumProgressType)),
		mainWindow, SLOT(slotUpdateProgressHide(cProgressText::enumProgressType)));
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

void cInterface::DetachMainImageWidget()
{
	if (!detachedWindow)
	{
		detachedWindow = new cDetachedWindow;
		connect(detachedWindow, SIGNAL(ReturnToOrigin()),
			mainWindow->ui->actionDetach_image_from_main_window, SLOT(toggle()));
		connect(detachedWindow, SIGNAL(ReturnToOrigin()), mainWindow, SLOT(slotDetachMainImage()));
	}
	mainWindow->ui->verticalLayout->removeWidget(mainWindow->ui->widgetWithImage);
	detachedWindow->InstallImageWidget(mainWindow->ui->widgetWithImage);
	detachedWindow->restoreGeometry(settings.value("detachedWindowGeometry").toByteArray());
	detachedWindow->show();
	// mainWindow->centralWidget()->hide();
	gPar->Set("image_detached", true);
}

void cInterface::AttachMainImageWidget()
{
	if (detachedWindow)
	{
		detachedWindow->RemoveImageWidget(mainWindow->ui->widgetWithImage);
		mainWindow->ui->verticalLayout->addWidget(mainWindow->ui->widgetWithImage);
		settings.setValue("detachedWindowGeometry", detachedWindow->saveGeometry());
		// mainWindow->centralWidget()->show();
		gPar->Set("image_detached", false);
	}
}

void cInterface::CameraMovementModeChanged(int index)
{
	renderedImage->SetCameraMovementMode(index);
}

void cInterface::ColorizeGroupBoxes(QWidget *window, int randomSeed)
{
	QList<QGroupBox *> widgets;
	widgets = window->findChildren<QGroupBox *>();
	if (qobject_cast<QGroupBox *>(window)) // check if QGroupBox
	{
		widgets.append(static_cast<QGroupBox *>(window));
	}
	QPalette palette = window->palette();
	QColor globalColor = palette.background().color();
	int brightness = globalColor.value();

	int rBase = globalColor.red();
	int gBase = globalColor.green();
	int bBase = globalColor.blue();

	cRandom random;
	random.Initialize(randomSeed);

	foreach (QGroupBox *groupbox, widgets)
	{
		QColor buttonColor;
		if (brightness > 20)
		{
			int r = random.Random(40) + rBase - 20;
			r = clamp(r, 0, 255);
			int g = random.Random(40) + gBase - 20;
			g = clamp(g, 0, 255);
			int b = random.Random(40) + bBase - 20;
			b = clamp(b, 0, 255);
			buttonColor = QColor(r, g, b);
		}
		else
		{
			int r = random.Random(40) + rBase;
			r = clamp(r, 0, 255);
			int g = random.Random(40) + gBase;
			g = clamp(g, 0, 255);
			int b = random.Random(40) + bBase;
			b = clamp(b, 0, 255);
			buttonColor = QColor(r, g, b);
		}

		QPalette newPalette(buttonColor);
		groupbox->setPalette(newPalette);
		groupbox->setAutoFillBackground(true);
	}
}

// selective saving of settings (from selected widget)
void cInterface::SaveLocalSettings(const QWidget *widget)
{
	QStringList listOfParameters = CreateListOfParametersInWidget(widget);

	cSettings parSettings(cSettings::formatCondensedText);
	parSettings.SetListOfParametersToProcess(listOfParameters);

	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);

	QString proposedFilename = widget->objectName();

	QFileDialog dialog(mainWindow);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::AnyFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(
		QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(proposedFilename));
	dialog.setAcceptMode(QFileDialog::AcceptSave);
	dialog.setWindowTitle(tr("Save settings from %1").arg(widget->objectName()));
	dialog.setDefaultSuffix("fract");
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		parSettings.SaveToFile(filename);
	}
}

void cInterface::LoadLocalSettings(const QWidget *widget)
{
	QString proposedFilename = widget->objectName();

	PreviewFileDialog dialog(mainWindow);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("Fractals (*.txt *.fract)"));
	dialog.setDirectory(
		QDir::toNativeSeparators(QFileInfo(systemData.lastSettingsFile).absolutePath()));
	dialog.selectFile(QDir::toNativeSeparators(proposedFilename));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Load settings to %1").arg(widget->objectName()));
	QStringList filenames;

	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());

		for (int i = 0; i < 2; i++) // repeat twice to refresh comboboxes and get new list of widgets
		{
			QStringList listOfParameters = CreateListOfParametersInWidget(widget);

			SynchronizeInterface(
				gPar, gParFractal, qInterface::read); // update appParam before loading new settings

			cSettings parSettings(cSettings::formatFullText);
			parSettings.SetListOfParametersToProcess(listOfParameters);

			DisablePeriodicRefresh();
			gInterfaceReadyForSynchronization = false;
			parSettings.LoadFromFile(filename);
			parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
			gInterfaceReadyForSynchronization = true;
			SynchronizeInterface(gPar, gParFractal, qInterface::write);
			ComboMouseClickUpdate();
			ReEnablePeriodicRefresh();
		}
	}
}

void cInterface::ResetLocalSettings(const QWidget *widget)
{
	QStringList listOfParameters = CreateListOfParametersInWidget(widget);

	for (QString fullParameterName : listOfParameters)
	{
		const int firstUnderscore = fullParameterName.indexOf('_');
		const QString containerName = fullParameterName.left(firstUnderscore);
		const QString parameterName = fullParameterName.mid(firstUnderscore + 1);

		cParameterContainer *container = nullptr;
		if (containerName == "main")
		{
			container = gPar;
		}
		else if (containerName.indexOf("fractal") >= 0)
		{
			const int index = containerName.rightRef(1).toInt();
			if (index < 4)
			{
				container = &gParFractal->at(index);
			}
		}

		if (container)
		{
			cOneParameter oneParam = container->GetAsOneParameter(parameterName);
			oneParam.SetMultiVal(oneParam.GetMultiVal(valueDefault), valueActual);
			container->SetFromOneParameter(parameterName, oneParam);
		}
	}
	SynchronizeInterface(gPar, gParFractal, qInterface::write);
}

QStringList cInterface::CreateListOfParametersInWidget(const QWidget *widget)
{
	QList<QWidget *> listOfWidgets = widget->findChildren<QWidget *>();
	QSet<QString> listOfParameters;

	foreach (QWidget *widget, listOfWidgets)
	{
		CommonMyWidgetWrapper *myWidget = dynamic_cast<CommonMyWidgetWrapper *>(widget);
		if (myWidget)
		{
			QString parameterName = myWidget->getFullParameterName();
			QString containerName = myWidget->getParameterContainerName();
			QString fullParameterName = containerName + "_" + parameterName;
			listOfParameters.insert(fullParameterName);
		}
	}
	QStringList list(listOfParameters.toList());
	return list;
}

void cInterface::GlobalStopRequest()
{
	systemData.globalStopRequest = true;
	if (!stopRequestPulseTimer)
	{
		stopRequestPulseTimer = new QTimer(mainWindow); // will be deleted with parent
		connect(stopRequestPulseTimer, SIGNAL(timeout()), mainWindow, SLOT(ResetGlobalStopRequest()));
		stopRequestPulseTimer->setSingleShot(true);
	}

	stopRequestPulseTimer->start(1000);
}

void cInterface::ResetGlobalStopRequest()
{
	while (cRenderJob::GetRunningJobCount() > 0)
	{
		gApplication->processEvents();
	}

	systemData.globalStopRequest = false;
}
