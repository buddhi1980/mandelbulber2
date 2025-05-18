/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
#include "lights.hpp"
#include "manipulations.h"
#include "material_item_model.h"
#include "my_ui_loader.h"
#include "netrender.hpp"
#include "nine_fractals.hpp"
#include "opencl_engine_render_dof.h"
#include "opencl_engine_render_fractal.h"
#include "opencl_engine_render_post_filter.h"
#include "opencl_engine_render_ssao.h"
#include "opencl_global.h"
#include "post_effect_hdr_blur.h"
#include "queue.hpp"
#include "random.hpp"
#include "randomizer_dialog.h"
#include "render_data.hpp"
#include "render_job.hpp"
#include "render_ssao.h"
#include "render_window.hpp"
#include "rendered_image_widget.hpp"
#include "rendering_configuration.hpp"
#include "scripts.h"
#include "settings.hpp"
#include "synchronize_interface.hpp"
#include "system_data.hpp"
#include "trace_behind.h"
#include "tree_string_list.h"
#include "undo.h"
#include "wait.hpp"
#include "write_log.hpp"

#include "qt/detached_window.h"
#include "qt/dock_effects.h"
#include "qt/dock_navigation.h"
#include "qt/material_editor.h"
#include "qt/my_group_box.h"
#include "qt/my_progress_bar.h"
#include "qt/player_widget.hpp"
#include "qt/preview_file_dialog.h"
#include "qt/settings_cleaner.h"
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
	renderedImage = nullptr;
	imageSequencePlayer = nullptr;
	mainImage = nullptr;
	progressBar = nullptr;
	progressBarAnimation = nullptr;
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
}

cInterface::~cInterface()
{
	if (imageSequencePlayer) delete imageSequencePlayer;
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

	systemData.setPreferredCustomFormulaFontSize(gPar->Get<int>("custom_formula_font_size"));

	mainWindow = new RenderWindow();

	QSize iconSize(int(pixelFontSize * 1.0), int(pixelFontSize * 1.0));
	mainWindow->ui->toolButton_moveMode->setIconSize(iconSize);
	mainWindow->ui->toolButton_multiMode->setIconSize(iconSize);
	mainWindow->ui->toolButton_rollMode->setIconSize(iconSize);
	mainWindow->ui->toolButton_rotAroundMode->setIconSize(iconSize);
	mainWindow->ui->toolButton_rotMode->setIconSize(iconSize);

	if (gPar->Get<bool>("image_detached"))
	{
		DetachMainImageWidget();
		mainWindow->ui->actionDetach_image_from_main_window->setChecked(true);
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
	mainImage.reset(new cImage(gPar->Get<int>("image_width"), gPar->Get<int>("image_height")));
	mainImage->CreatePreview(1.0, 800, 600, renderedImage);
	mainImage->CompileImage();
	mainImage->ConvertTo8bitChar();
	mainImage->UpdatePreview();
	mainImage->SetAsMainImage();
	renderedImage->setMinimumSize(
		int(mainImage->GetPreviewWidth()), int(mainImage->GetPreviewHeight()));
	renderedImage->AssignImage(mainImage);
	renderedImage->AssignParameters(gPar, gParFractal);

	mainWindow->manipulations->AssignParameterContainers(gPar, gParFractal);
	mainWindow->manipulations->AssingImage(mainImage);
	mainWindow->manipulations->AssignWidgets(renderedImage, mainWindow->GetWidgetDockNavigation(),
		mainWindow->GetWidgetDockEffects(), mainWindow->GetWidgetDockFractal());

	mainWindow->ui->widgetDockNavigation->AssignParameterContainers(gPar, gParFractal);

	mainWindow->ui->widgetEffects->AssignParameterContainers(gPar, gParFractal);
	mainWindow->ui->widgetEffects->AssignSpecialWidgets(
		renderedImage, mainWindow->ui->comboBox_mouse_click_function);

	mainWindow->ui->widgetDockFractal->AssignParameterContainers(gPar, gParFractal);
	mainWindow->ui->widgetDockFractal->AssignSpecialWidgets(
		renderedImage, mainWindow->ui->comboBox_mouse_click_function);

	WriteLog("Prepare progress and status bar", 2);
	progressBarLayout = new QVBoxLayout();
	progressBarLayout->setSpacing(0);
	progressBarLayout->setContentsMargins(0, 0, 0, 0);
	progressBarFrame = new QFrame(mainWindow->ui->statusbar);

	progressBarAnimation = new MyProgressBar(progressBarFrame);
	progressBarAnimation->setMaximum(1000);
	progressBarAnimation->setAlignment(Qt::AlignCenter);
	progressBarAnimation->hide();
	progressBarLayout->addWidget(progressBarAnimation);

	progressBar = new MyProgressBar(progressBarFrame);
	progressBar->setMaximum(1000);
	progressBar->setAlignment(Qt::AlignCenter);
	progressBarLayout->addWidget(progressBar);

	QFrame *progressBarFrameInternal = new QFrame(mainWindow->ui->statusbar);
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

	if (gPar->Get<bool>("opencl_enabled") && gPar->Get<bool>("opencl_mode") > 0)
		mainWindow->GetWidgetDockImageAdjustments()->SetAntialiasingOpenCL(true);
#endif

	QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_F11), mainWindow);
	shortcut->setContext(Qt::ApplicationShortcut);
	connect(shortcut, &QShortcut::activated, mainWindow, &RenderWindow::ToggleFullScreen);

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

	sPrimitiveBasic::InitPrimitiveWireframeShapes();

	// change some default settings with keeping compatibility with older versions
	StartupDefaultSettings();

	systemData.numberOfThreads = gPar->Get<int>("limit_CPU_cores");
	systemData.threadsPriority = enumRenderingThreadPriority(gPar->Get<int>("threads_priority"));
	systemData.enableConsoleOutput = gPar->Get<bool>("console_output_enable");

	ComboMouseClickUpdate();

	mainWindow->slotPopulateRecentSettings();
	mainWindow->slotPopulateCustomWindowStates();
	systemTray = new cSystemTray(mainImage, mainWindow);

	WriteLog("Restoring window geometry", 2);

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
		if (mainWindow->ui->dockWidget_gamepad_dock != nullptr)
			mainWindow->ui->dockWidget_gamepad_dock->hide();
		mainWindow->ui->dockWidget_histogram->hide();
		mainWindow->ui->dockWidget_queue_dock->hide();
	}

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

	connect(mainWindow->ui->widgetDockNavigation, &cDockNavigation::signalRender, mainWindow,
		&RenderWindow::slotStartRender);

	connect(mainWindow->ui->widgetDockNavigation, &cDockNavigation::signalCameraMovementModeChanged,
		mainWindow, &RenderWindow::slotCameraMovementModeChanged);
	connect(mainWindow->ui->widgetDockNavigation, &cDockNavigation::signalStop, mainWindow,
		&RenderWindow::slotMenuStopRendering);

	// manipulations class
	connect(mainWindow->manipulations, &cManipulations::signalRender, mainWindow,
		&RenderWindow::slotStartRender);
	connect(mainWindow->manipulations, &cManipulations::signalStop, mainWindow,
		&RenderWindow::slotMenuStopRendering);
	connect(mainWindow->manipulations, &cManipulations::signalWriteInterfaceBasicFog,
		mainWindow->ui->widgetEffects, &cDockEffects::slotSynchronizeInterfaceBasicFog);
	connect(mainWindow->manipulations, &cManipulations::signalWriteInterfaceDOF,
		mainWindow->ui->widgetEffects, &cDockEffects::slotSynchronizeInterfaceDOF);
	connect(mainWindow->manipulations, &cManipulations::signalWriteInterfaceLights,
		mainWindow->ui->widgetEffects, &cDockEffects::slotSynchronizeInterfaceLights);
	connect(mainWindow->manipulations, &cManipulations::signalWriteInterfaceRandomLights,
		mainWindow->ui->widgetEffects, &cDockEffects::slotSynchronizeInterfaceRandomLights);
	connect(mainWindow->manipulations, &cManipulations::signalWriteInterfaceJulia,
		mainWindow->ui->widgetDockFractal, &cDockFractal::slotSynchronizeInterfaceJulia);
	connect(mainWindow->manipulations, &cManipulations::signalWriteInterfacePrimitives,
		mainWindow->ui->widgetDockFractal, &cDockFractal::slotSynchronizeInterfacePrimitives);
	connect(mainWindow->manipulations, &cManipulations::signalEnableJuliaMode,
		mainWindow->ui->widgetDockFractal, &cDockFractal::slotEnableJuliaMode);
	connect(mainWindow->manipulations, &cManipulations::signalWriteInterfaceMeasuremets,
		mainWindow->ui->widgetDockMeasurements, &cDockMeasurements::slotSynchronizeInterface);
	connect(mainWindow->manipulations, &cManipulations::signalDisablePeriodicRefresh, this,
		&cInterface::slotDisablePeriodicRefresh);
	connect(mainWindow->manipulations, &cManipulations::signalReEnablePeriodicRefresh, this,
		&cInterface::slotReEnablePeriodicRefresh);
	connect(mainWindow->manipulations, &cManipulations::signalRefreshPostEffects, this,
		&cInterface::slotRefreshPostEffects);
	connect(mainWindow->manipulations, &cManipulations::signalShowMeasuremetsDock, mainWindow,
		&RenderWindow::slotShowMeasuremetDock);
	connect(mainWindow->ui->toolButton_multiMode, &QToolButton::toggled, mainWindow->manipulations,
		&cManipulations::slotToggledOtpionMulti);
	connect(mainWindow->ui->toolButton_rotMode, &QToolButton::toggled, mainWindow->manipulations,
		&cManipulations::slotToggledOtpionRotate);
	connect(mainWindow->ui->toolButton_rotAroundMode, &QToolButton::toggled,
		mainWindow->manipulations, &cManipulations::slotToggledOtpionRotateAround);
	connect(mainWindow->ui->toolButton_rollMode, &QToolButton::toggled, mainWindow->manipulations,
		&cManipulations::slotToggledOtpionRoll);
	connect(mainWindow->ui->toolButton_moveMode, &QToolButton::toggled, mainWindow->manipulations,
		&cManipulations::slotToggledOtpionMove);
	connect(mainWindow->ui->toolButton_preciseRotation, &QToolButton::toggled,
		mainWindow->manipulations, &cManipulations::slotToggledOPtionPreciseRotation);

	// menu actions
	connect(mainWindow->ui->actionQuit, &QAction::triggered, mainWindow, &RenderWindow::slotQuit);
	connect(mainWindow->ui->actionSave_docks_positions, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuSaveDocksPositions);
	connect(mainWindow->ui->actionDefault_docks_positions, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuResetDocksPositions);
	connect(mainWindow->ui->actionAnimation_docks_positions, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuAnimationDocksPositions);
	connect(mainWindow->ui->actionStack_all_docks, &QAction::triggered, mainWindow,
		&RenderWindow::slotStackAllDocks);
	connect(mainWindow->ui->actionShow_animation_dock, &QAction::triggered, mainWindow,
		&RenderWindow::slotUpdateDocksAndToolbarByAction);
	connect(mainWindow->ui->actionShow_toolbar, &QAction::triggered, mainWindow,
		&RenderWindow::slotUpdateDocksAndToolbarByAction);
	connect(mainWindow->ui->actionShow_info_dock, &QAction::triggered, mainWindow,
		&RenderWindow::slotUpdateDocksAndToolbarByAction);
	connect(mainWindow->ui->actionShow_statistics_dock, &QAction::triggered, mainWindow,
		&RenderWindow::slotUpdateDocksAndToolbarByAction);
	connect(mainWindow->ui->actionShow_gamepad_dock, &QAction::triggered, mainWindow,
		&RenderWindow::slotUpdateDocksAndToolbarByAction);
	connect(mainWindow->ui->actionShow_queue_dock, &QAction::triggered, mainWindow,
		&RenderWindow::slotUpdateDocksAndToolbarByAction);
	connect(mainWindow->ui->actionShow_measurement_dock, &QAction::triggered, mainWindow,
		&RenderWindow::slotUpdateDocksAndToolbarByAction);
	connect(mainWindow->ui->actionSave_settings, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuSaveSettings);
	connect(mainWindow->ui->actionSave_settings_to_clipboard, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuSaveSettingsToClipboard);
	connect(mainWindow->ui->actionLoad_settings, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuLoadSettings);
	connect(mainWindow->ui->actionLoad_settings_from_clipboard, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuLoadSettingsFromClipboard);
	connect(mainWindow->ui->actionLoad_example, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuLoadExample);
	connect(mainWindow->ui->actionSettings_browser, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuSettingsBrowser);
	connect(mainWindow->ui->actionSave_as_default_settings, &QAction::triggered, mainWindow,
		&RenderWindow::slotSaveSettingsAsDefaut);
	connect(mainWindow->ui->actionReset_to_default, &QAction::triggered, mainWindow,
		&RenderWindow::slotResetToDefault);
	connect(mainWindow->ui->actionDelete_default_settings, &QAction::triggered, mainWindow,
		&RenderWindow::slotDeleteDefaultSettings);
	connect(mainWindow->ui->actionImport_settings_from_old_Mandelbulber, &QAction::triggered,
		mainWindow, &RenderWindow::slotImportOldSettings);
	connect(mainWindow->ui->actionImport_settings_from_Mandelbulb3d, &QAction::triggered, mainWindow,
		&RenderWindow::slotImportMandelbulb3dSettings);
	connect(mainWindow->ui->actionExportVoxelLayers, &QAction::triggered, mainWindow,
		&RenderWindow::slotExportVoxelLayers);
	connect(mainWindow->ui->actionExport_Mesh, &QAction::triggered, mainWindow,
		&RenderWindow::slotExportMesh);
	connect(mainWindow->ui->actionSave_as_JPG, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuSaveImageJPEG);
	connect(mainWindow->ui->actionSave_as_IMAGE, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuSaveImageAll);
	connect(mainWindow->ui->actionSave_as_PNG, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuSaveImagePNG);
	connect(mainWindow->ui->actionSave_as_PNG_16_bit, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuSaveImagePNG16);
	connect(mainWindow->ui->actionSave_as_PNG_16_bit_with_alpha_channel, &QAction::triggered,
		mainWindow, &RenderWindow::slotMenuSaveImagePNG16Alpha);
#ifdef USE_EXR
	connect(mainWindow->ui->actionSave_as_EXR, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuSaveImageEXR);
#endif // USE_EXR

#ifdef USE_TIFF
	connect(mainWindow->ui->actionSave_as_TIFF, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuSaveImageTIFF);
#endif // USE_TIFF

	connect(mainWindow->ui->actionAbout_Qt, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuAboutQt);
	connect(mainWindow->ui->actionUser_Manual, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuAboutManual);
	connect(mainWindow->ui->actionUser_News, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuAboutNews);
	connect(mainWindow->ui->actionUser_HotKeys, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuAboutHotKeys);
	connect(mainWindow->ui->actionAbout_Mandelbulber, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuAboutMandelbulber);
	connect(mainWindow->ui->actionAbout_ThirdParty, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuAboutThirdParty);
	connect(mainWindow->ui->actionRender_Image, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuRenderImage);
	connect(mainWindow->ui->actionStop_rendering, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuStopRendering);
	connect(mainWindow->ui->actionUndo, &QAction::triggered, mainWindow, &RenderWindow::slotMenuUndo);
	connect(mainWindow->ui->actionRedo, &QAction::triggered, mainWindow, &RenderWindow::slotMenuRedo);
	connect(mainWindow->ui->actionRandomizeAll, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuRandomizeAll);
	connect(mainWindow->ui->actionCleanSettings, &QAction::triggered, mainWindow,
		&RenderWindow::slotCleanSettings);

	connect(mainWindow->ui->actionProgramPreferences, &QAction::triggered, mainWindow,
		&RenderWindow::slotMenuProgramPreferences);
	connect(mainWindow->ui->actionDetach_image_from_main_window, &QAction::triggered, mainWindow,
		&RenderWindow::slotDetachMainImage);

	connect(mainWindow->ui->scrollAreaForImage, SIGNAL(resized(int, int)), mainWindow,
		SLOT(slotResizedScrolledAreaImage(int, int)));
	connect(mainWindow->ui->comboBox_image_preview_scale, SIGNAL(currentIndexChanged(int)),
		mainWindow, SLOT(slotChangedComboImageScale(int)));

	// rendered image widget
	connect(
		renderedImage, SIGNAL(mouseMoved(int, int)), mainWindow, SLOT(slotMouseMovedOnImage(int, int)));
	connect(
		renderedImage, &RenderedImage::singleClick, mainWindow, &RenderWindow::slotMouseClickOnImage);
	connect(renderedImage, &RenderedImage::keyPress, mainWindow, &RenderWindow::slotKeyPressOnImage);
	connect(
		renderedImage, &RenderedImage::keyRelease, mainWindow, &RenderWindow::slotKeyReleaseOnImage);
	connect(renderedImage, &RenderedImage::mouseWheelRotatedWithKey, mainWindow,
		&RenderWindow::slotMouseWheelRotatedWithKeyOnImage);
	connect(
		renderedImage, &RenderedImage::mouseDragStart, mainWindow, &RenderWindow::slotMouseDragStart);
	connect(
		renderedImage, &RenderedImage::mouseDragFinish, mainWindow, &RenderWindow::slotMouseDragFinish);
	connect(
		renderedImage, &RenderedImage::mouseDragDelta, mainWindow, &RenderWindow::slotMouseDragDelta);

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

	connect(mainWindow->ui->actionAdd_Settings_to_Toolbar, &QAction::triggered, mainWindow,
		&RenderWindow::slotPresetAddToToolbar);
	connect(mainWindow->ui->actionAdd_CustomWindowStateToMenu, &QAction::triggered, mainWindow,
		&RenderWindow::slotCustomWindowStateAddToMenu);
	connect(mainWindow->ui->actionRemove_Window_settings, &QAction::triggered, mainWindow,
		&RenderWindow::slotCustomWindowRemovePopup);

	connect(mainWindow->ui->widgetEffects, &cDockEffects::signalRefreshPostEffects, this,
		&cInterface::slotRefreshPostEffects);

	connect(mainWindow->ui->widgetDockFractal, &cDockFractal::signalUpdatePrimitivesCombos,
		mainWindow->ui->widgetEffects, &cDockEffects::slotUpdatePrimitivesCombos);

	//------------------------------------------------
	mainWindow->slotUpdateDocksAndToolbarByView();
}

// Reading ad writing parameters from/to ui to/from parameters container
void cInterface::SynchronizeInterface(std::shared_ptr<cParameterContainer> par,
	std::shared_ptr<cFractalContainer> parFractal, qInterface::enumReadWrite mode) const
{
	WriteLog(
		"cInterface::SynchronizeInterface(std::shared_ptr<cParameterContainer> par, cFractalContainer "
		"*parFractal, "
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
	mainWindow->manipulations->IncreaseNumberOfStartedRenders();
	if (!mainImage->IsUsed())
	{
		mainImage->BlockImage();
		WriteLog("cInterface::StartRender(void) - image was free", 2);
	}
	else
	{
		WriteLog("cInterface::StartRender(void) - image was used by another instance", 2);
		bool isStopped = StopRender();
		if (!isStopped)
		{
			mainWindow->manipulations->DecreaseNumberOfStartedRenders();
			mainWindow->currentKeyEvents.clear();
			return;
		}

		while (mainImage->IsUsed())
		{
			Wait(10);
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

	cScripts::EvaluateAll(gPar, gParFractal);

	UpdateCameraRotation(gPar);

	SynchronizeInterface(gPar, gParFractal, qInterface::write);

	if (!noUndo) gUndo->Store(gPar, gParFractal);

	DisableJuliaPointMode();

	int temporaryScale = int(pow(int(2), gPar->Get<int>("temporary_scale")));
	timerForAbortWarnings.start();

	cRenderJob *renderJob = new cRenderJob(gPar, gParFractal, mainImage, temporaryScale, &stopRequest,
		renderedImage); // deleted by deleteLater()

	connect(renderJob, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)),
		mainWindow, SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
	connect(renderJob, SIGNAL(updateStatistics(cStatistics)), mainWindow->ui->widgetDockStatistics,
		SLOT(slotUpdateStatistics(cStatistics)));
	connect(renderJob, &cRenderJob::fullyRendered, systemTray, &cSystemTray::showMessage);
	connect(renderJob, SIGNAL(updateImage()), renderedImage, SLOT(update()));
	connect(renderJob, SIGNAL(sendRenderedTilesList(QList<sRenderedTileData>)), renderedImage,
		SLOT(showRenderedTilesList(QList<sRenderedTileData>)));
	connect(renderJob, &cRenderJob::fullyRenderedTime, this, &cInterface::slotAutoSaveImage);
	connect(renderJob, &cRenderJob::signalSmallPartRendered, mainWindow->manipulations,
		&cManipulations::slotSmallPartRendered);

	cRenderingConfiguration config;
	config.EnableNetRender();
	if (gPar->Get<bool>("nebula_mode")) config.SetNebulaMode();

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

	mainWindow->manipulations->DecreaseNumberOfStartedRenders();
}

void cInterface::RenderFlame()
{
	// TODO:	render flame code
}

bool cInterface::StopRender()
{
	int closeResult = 0;

	// if render time is lomger than 10 minutes
	if (mainImage->IsUsed() && timerForAbortWarnings.elapsed() > 1000 * 60 * 10)
	{
		QMessageBox *messageBox = new QMessageBox(mainWindow);
		QString messageText = QObject::tr("Are you sure to stop this long render?");
		messageBox->setText(messageText);
		messageBox->setWindowTitle(QObject::tr("Abort?"));
		messageBox->setIcon(QMessageBox::Warning);
		messageBox->addButton(QMessageBox::Ok);
		messageBox->addButton(QMessageBox::Cancel);
		messageBox->setDefaultButton(QMessageBox::Ok);
		closeResult = messageBox->exec();

		switch (closeResult)
		{
			case QMessageBox::Cancel:
			{
				// nothing
				stopRequest = false;
				return false;
				break;
			}
			case QMessageBox::Yes:
			default:
			{
				stopRequest = true;
				return true;
				break;
			}
		}
	}
	else
	{
		stopRequest = true;
		return true;
	}
}

void cInterface::IFSDefaultsDodecahedron(std::shared_ptr<cParameterContainer> parFractal) const
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

void cInterface::IFSDefaultsIcosahedron(std::shared_ptr<cParameterContainer> parFractal) const
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

void cInterface::IFSDefaultsOctahedron(std::shared_ptr<cParameterContainer> parFractal)
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

void cInterface::IFSDefaultsMengerSponge(std::shared_ptr<cParameterContainer> parFractal)
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

void cInterface::IFSDefaultsReset(std::shared_ptr<cParameterContainer> parFractal)
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

void cInterface::AutoFog(std::shared_ptr<cParameterContainer> _params,
	std::shared_ptr<cFractalContainer> _fractalParams) const
{

	double distance = GetDistanceForPoint(_params->Get<CVector3>("camera"), _params, _fractalParams);
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

double cInterface::GetDistanceForPoint(CVector3 point, std::shared_ptr<cParameterContainer> par,
	std::shared_ptr<cFractalContainer> parFractal)
{
	std::shared_ptr<sParamRender> params(new sParamRender(par));
	std::shared_ptr<cNineFractals> fractals(new cNineFractals(parFractal, par));
	sDistanceIn in(point, 0, false);
	sDistanceOut out;

	bool openClEnabled = false;
#ifdef USE_OPENCL
	openClEnabled = par->Get<bool>("opencl_enabled") && parFractal->isUsedCustomFormula();

	if (openClEnabled)
	{
		gOpenCl->openClEngineRenderFractal->Lock();
		gOpenCl->openClEngineRenderFractal->SetDistanceMode();
		gOpenCl->openClEngineRenderFractal->SetParameters(
			par, parFractal, params, fractals, nullptr, false);
		if (gOpenCl->openClEngineRenderFractal->LoadSourcesAndCompile(par))
		{
			gOpenCl->openClEngineRenderFractal->CreateKernel4Program(par);
			gOpenCl->openClEngineRenderFractal->PreAllocateBuffers(par);
			gOpenCl->openClEngineRenderFractal->CreateCommandQueue();
		}
		else
		{
			gOpenCl->openClEngineRenderFractal->ReleaseMemory();
			gOpenCl->openClEngineRenderFractal->Unlock();
			return 0.0;
		}
	}
#endif

	double dist;
	if (openClEnabled)
	{
#ifdef USE_OPENCL
		dist = gOpenCl->openClEngineRenderFractal->CalculateDistance(point);
#endif
	}
	else
	{
		dist = CalculateDistance(*params, *fractals, in, &out);
	}

#ifdef USE_OPENCL
	if (openClEnabled)
	{
		gOpenCl->openClEngineRenderFractal->ReleaseMemory();
		gOpenCl->openClEngineRenderFractal->Unlock();
	}
#endif

	return dist;
}

double cInterface::GetDistanceForPoint(CVector3 point) const
{
	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	double distance = GetDistanceForPoint(point, gPar, gParFractal);
	return distance;
}

void cInterface::Undo()
{
	bool refreshFrames = false;
	bool refreshKeyframes = false;
	DisablePeriodicRefresh();
	gInterfaceReadyForSynchronization = false;
	if (gUndo->Undo(gPar, gParFractal, gAnimFrames, gKeyframes, &refreshFrames, &refreshKeyframes))
	{
		materialListModel->Regenerate();
		mainWindow->ui->widgetEffects->RegenerateLights();
		mainWindow->ui->widgetDockFractal->RegeneratePrimitives();
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
	if (gUndo->Redo(gPar, gParFractal, gAnimFrames, gKeyframes, &refreshFrames, &refreshKeyframes))
	{
		materialListModel->Regenerate();
		mainWindow->ui->widgetEffects->RegenerateLights();
		mainWindow->ui->widgetDockFractal->RegeneratePrimitives();
		gInterfaceReadyForSynchronization = false;
		SynchronizeInterface(gPar, gParFractal, qInterface::write);
		if (refreshFrames) gFlightAnimation->RefreshTable();
		if (refreshKeyframes) gKeyframeAnimation->RefreshTable();
		StartRender(true);
	}
	gInterfaceReadyForSynchronization = true;
	ReEnablePeriodicRefresh();
}

void cInterface::ResetView(QWidget *navigationWidget,
	std::shared_ptr<cParameterContainer> paramsContainer,
	std::shared_ptr<cFractalContainer> parFractalContainer)
{
	SynchronizeInterfaceWindow(navigationWidget, paramsContainer, qInterface::read);

	CVector3 camera = paramsContainer->Get<CVector3>("camera");
	CVector3 target = paramsContainer->Get<CVector3>("target");
	CVector3 topVector = paramsContainer->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, topVector);
	CVector3 forwardVector = cameraTarget.GetForwardVector();
	params::enumPerspectiveType perspType =
		params::enumPerspectiveType(paramsContainer->Get<int>("perspective_type"));
	double fov = CalcFOV(paramsContainer->Get<double>("fov"), perspType);
	double DEFactor = paramsContainer->Get<double>("DE_factor");

	std::shared_ptr<cParameterContainer> parTemp(new cParameterContainer);
	*parTemp = *paramsContainer;
	parTemp->Set("limits_enabled", false);
	parTemp->Set("interior_mode", false);

	QStringList listOfParameters = parTemp->GetListOfParameters();
	for (const QString &parameterName : listOfParameters)
	{
		if (parameterName.contains("primitive_plane") && parameterName.contains("enabled"))
		{
			parTemp->Set(parameterName, false);
		}

		if (parameterName.contains("primitive_water") && parameterName.contains("enabled"))
		{
			parTemp->Set(parameterName, false);
		}
	}

	// calculate size of the fractal in random directions
	double maxDist = 0.0;

	std::shared_ptr<sParamRender> params(new sParamRender(parTemp));
	std::shared_ptr<cNineFractals> fractals(new cNineFractals(parFractalContainer, parTemp));

	bool openClEnabled = false;
#ifdef USE_OPENCL
	openClEnabled =
		parTemp->Get<bool>("opencl_enabled") && parFractalContainer->isUsedCustomFormula();

	if (openClEnabled)
	{
		gOpenCl->openClEngineRenderFractal->Lock();
		gOpenCl->openClEngineRenderFractal->SetDistanceMode();
		gOpenCl->openClEngineRenderFractal->SetParameters(
			parTemp, gParFractal, params, fractals, nullptr, false);
		if (gOpenCl->openClEngineRenderFractal->LoadSourcesAndCompile(parTemp))
		{
			gOpenCl->openClEngineRenderFractal->CreateKernel4Program(parTemp);
			gOpenCl->openClEngineRenderFractal->PreAllocateBuffers(parTemp);
			gOpenCl->openClEngineRenderFractal->CreateCommandQueue();
		}
		else
		{
			gOpenCl->openClEngineRenderFractal->ReleaseMemory();
			gOpenCl->openClEngineRenderFractal->Unlock();
			return;
		}
	}
#endif

	for (int i = 0; i < 100; i++)
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

			double dist;
			if (openClEnabled)
			{
#ifdef USE_OPENCL
				dist = gOpenCl->openClEngineRenderFractal->CalculateDistance(point);
#endif
			}
			else
			{
				dist = CalculateDistance(*params, *fractals, in, &out);
			}
			if (dist < 0.1)
			{
				break;
			}
			distStep = dist * DEFactor * 0.5;
			if (distStep > 1.0) distStep = 1.0;
			// qDebug() << "i" << i << "scan" << scan << "direction" << direction.Debug();
		}
		if (scan > maxDist) maxDist = scan;
	}
	cProgressText::ProgressStatusText(
		QObject::tr("Resetting view"), QObject::tr("Done"), 1.0, cProgressText::progress_IMAGE);

	double newCameraDist;

#ifdef USE_OPENCL
	if (openClEnabled)
	{
		gOpenCl->openClEngineRenderFractal->ReleaseMemory();
		gOpenCl->openClEngineRenderFractal->Unlock();
	}
#endif

	if (perspType == params::perspThreePoint)
	{
		newCameraDist = maxDist / fov * 2.0 * sqrt(2);
	}
	else if (perspType == params::perspEquirectangular)
	{
		newCameraDist = maxDist / fov * 4.0;
	}
	else
	{
		newCameraDist = maxDist / fov * 2.0;
	}

	if (newCameraDist < 0.1) newCameraDist = 0.1;

	paramsContainer->Set("target", CVector3(0.0, 0.0, 0.0));
	CVector3 newCamera = forwardVector * newCameraDist * (-1.0);
	paramsContainer->Set("camera", newCamera);
	paramsContainer->Set("camera_distance_to_target", newCameraDist);
	paramsContainer->Set("view_distance_max", (newCameraDist + maxDist) * 2.0);
	SynchronizeInterfaceWindow(navigationWidget, paramsContainer, qInterface::write);
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
		default: break;
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
	SetBoundingBoxAsLimits(outerBoundingMin, outerBoundingMax, gPar, gParFractal);
}

void cInterface::SetBoundingBoxAsLimits(CVector3 outerBoundingMin, CVector3 outerBoundingMax,
	std::shared_ptr<cParameterContainer> par, std::shared_ptr<cFractalContainer> parFractal)
{
	CVector3 boundingCenter = (outerBoundingMin + outerBoundingMax) / 2;

	SynchronizeInterface(par, parFractal, qInterface::read);

	auto parTemp = std::make_shared<cParameterContainer>();
	*parTemp = *par;

	parTemp->Set("limits_enabled", false);
	parTemp->Set("interior_mode", false);

	std::shared_ptr<sParamRender> params(new sParamRender(parTemp));
	std::shared_ptr<cNineFractals> fractals(new cNineFractals(parFractal, parTemp));

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
	dist = CalculateDistanceMinPlane(params, fractals, point, direction, orthDirection, &stopRequest);
	double minX = point.x + dist;

	// negative y limit
	cProgressText::ProgressStatusText(
		QObject::tr("bounding box as limit"), QObject::tr("Negative Y Limit"), 1.0 / 6.0);
	direction = CVector3(0, 1, 0);
	orthDirection = CVector3(0, 0, 1);
	point = CVector3(boundingCenter.x, outerBoundingMin.y, boundingCenter.z);
	dist = CalculateDistanceMinPlane(params, fractals, point, direction, orthDirection, &stopRequest);
	double minY = point.y + dist;

	// negative z limit
	cProgressText::ProgressStatusText(
		QObject::tr("bounding box as limit"), QObject::tr("Negative Z Limit"), 2.0 / 6.0);
	direction = CVector3(0, 0, 1);
	orthDirection = CVector3(1, 0, 0);
	point = CVector3(boundingCenter.x, boundingCenter.y, outerBoundingMin.z);
	dist = CalculateDistanceMinPlane(params, fractals, point, direction, orthDirection, &stopRequest);
	double minZ = point.z + dist;

	// positive x limit
	cProgressText::ProgressStatusText(
		QObject::tr("bounding box as limit"), QObject::tr("Positive X Limit"), 3.0 / 6.0);
	direction = CVector3(-1, 0, 0);
	orthDirection = CVector3(0, -1, 0);
	point = CVector3(outerBoundingMax.x, boundingCenter.y, boundingCenter.z);
	dist = CalculateDistanceMinPlane(params, fractals, point, direction, orthDirection, &stopRequest);
	double maxX = point.x - dist;

	// positive y limit
	cProgressText::ProgressStatusText(
		QObject::tr("bounding box as limit"), QObject::tr("Positive Y Limit"), 4.0 / 6.0);
	direction = CVector3(0, -1, 0);
	orthDirection = CVector3(0, 0, -1);
	point = CVector3(boundingCenter.x, outerBoundingMax.y, boundingCenter.z);
	dist = CalculateDistanceMinPlane(params, fractals, point, direction, orthDirection, &stopRequest);
	double maxY = point.y - dist;

	// positive z limit
	cProgressText::ProgressStatusText(
		QObject::tr("bounding box as limit"), QObject::tr("Positive Z Limit"), 5.0 / 6.0);
	direction = CVector3(0, 0, -1);
	orthDirection = CVector3(-1, 0, 0);
	point = CVector3(boundingCenter.x, boundingCenter.y, outerBoundingMax.z);
	dist = CalculateDistanceMinPlane(params, fractals, point, direction, orthDirection, &stopRequest);
	double maxZ = point.z - dist;

	double medX = (maxX + minX) / 2.0;
	double medY = (maxY + minY) / 2.0;
	double medZ = (maxZ + minZ) / 2.0;
	double rangeX = maxX - minX;
	double rangeY = maxY - minY;
	double rangeZ = maxZ - minZ;

	par->Set("limit_min", CVector3(medX - rangeX * 0.6, medY - rangeY * 0.6, medZ - rangeZ * 0.6));
	par->Set("limit_max", CVector3(medX + rangeX * 0.6, medY + rangeY * 0.6, medZ + rangeZ * 0.6));

	cProgressText::ProgressStatusText(QObject::tr("bounding box as limit"), QObject::tr("Done"), 1.0);
	SynchronizeInterface(par, parFractal, qInterface::write);
}

void cInterface::ComboMouseClickUpdate()
{
	ComboMouseClickUpdate(mainWindow->ui->comboBox_mouse_click_function, gPar);
}

void cInterface::ComboMouseClickUpdate(
	QComboBox *combo, std::shared_ptr<cParameterContainer> params)
{
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

	QList<int> listOfLights = cLights::GetListOfLights(params);

	for (int lightIndex : listOfLights)
	{
		item.clear();
		item.append(int(RenderedImage::clickPlaceLight));
		item.append(lightIndex);
		combo->addItem(QObject::tr("Place light #%1").arg(lightIndex), item);
	}

	item.clear();
	item.append(int(RenderedImage::clickPlaceRandomLightCenter));
	combo->addItem(QObject::tr("Place random light center"), item);

	item.clear();
	item.append(int(RenderedImage::clickGetPoint));
	combo->addItem(QObject::tr("Get point coordinates"), item);

	item.clear();
	item.append(int(RenderedImage::clickWrapLimitsAroundObject));
	combo->addItem(QObject::tr("Wrap Limits around object"), item);

	QList<sPrimitiveItem> listOfPrimitives = cPrimitives::GetListOfPrimitives(params);

	if (listOfPrimitives.size() > 0)
	{
		for (const auto &primitiveItem : listOfPrimitives)
		{
			QString primitiveName = cPrimitives::PrimitiveNames(primitiveItem.type);
			int index = primitiveItem.id;
			QString comboItemString =
				QString(QObject::tr("Place ")) + primitiveName + QString(" #") + QString::number(index);
			item.clear();
			item.append(int(RenderedImage::clickPlacePrimitive));
			item.append(int(primitiveItem.type));
			item.append(primitiveItem.id);
			item.append(primitiveItem.fullName);
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

			QFile::remove(systemDirectories.GetAutosaveFile());

			if (detachedWindow)
			{
				settings.setValue("detachedWindowGeometry", detachedWindow->saveGeometry());
			}

			ClearNetRenderCache();

			gApplication->quit();
			quit = true;
			break;
		}
	}
	if (messageBox) delete messageBox;
	return quit;
}

bool cInterface::AutoRecovery() const
{
	if (QFile::exists(systemDirectories.GetAutosaveFile()))
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
			parSettings.LoadFromFile(systemDirectories.GetAutosaveFile());
			parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
			materialListModel->Regenerate();
			mainWindow->ui->widgetEffects->RegenerateLights();
			mainWindow->ui->widgetDockFractal->RegeneratePrimitives();
			SynchronizeInterface(gPar, gParFractal, qInterface::write);
			gInterfaceReadyForSynchronization = true;
			gFlightAnimation->RefreshTable();
			gKeyframeAnimation->RefreshTable();
			return true;
		}
		else
		{
			return false;
		}
	}
	return false;
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
			QProcess::startDetached(qApp->arguments()[0], QStringList());
			delete messageBox;
			return true;
			break;
		}
	}
	delete messageBox;
#endif
	return false;
}

void cInterface::SetQuickRenderParameters(const std::shared_ptr<cParameterContainer> &tempParam)
{
	// disabling all slow effects
	tempParam->Set("ambient_occlusion", false);
	tempParam->Set("DOF_enabled", false);
	tempParam->Set("iteration_threshold_mode", false);
	tempParam->Set("raytraced_reflections", false);
	tempParam->Set("textured_background", false);
	tempParam->Set("iteration_fog_enable", false);
	tempParam->Set("fake_lights_enabled", false);
	tempParam->Set("DOF_monte_carlo", false);
	tempParam->Set("clouds_enable", false);
	tempParam->Set("hdr_blur_enabled", false);
	tempParam->Set("post_chromatic_aberration_enabled", false);
	tempParam->Set("textured_background", false);
	tempParam->Set("opencl_mode", 0); // disable OpenCL
	QList<int> listOfLights = cLights::GetListOfLights(tempParam);
	for (int lightIndex : listOfLights)
	{
		tempParam->Set(cLight::Name("cast_shadows", lightIndex), false);
		tempParam->Set(cLight::Name("volumetric", lightIndex), false);
	}
	int maxDimension = max(tempParam->Get<int>("image_width"), tempParam->Get<int>("image_height"));
	if (maxDimension == 0) maxDimension = 1;

	int newWidth = double(tempParam->Get<int>("image_width")) / maxDimension * 256.0;
	int newHeight = double(tempParam->Get<int>("image_height")) / maxDimension * 256.0;
	tempParam->Set("image_width", newWidth);
	tempParam->Set("image_height", newHeight);
	tempParam->Set("detail_level", 4.0);
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
	gUndo->Store(gPar, gParFractal);

	auto tempParam = std::make_shared<cParameterContainer>();
	*tempParam = *gPar;
	auto tempFractal = std::make_shared<cFractalContainer>();
	*tempFractal = *gParFractal;

	// disabling all slow effects
	SetQuickRenderParameters(tempParam);

	int scanCount = 0;
	double DEFactor = 1.0;
	double step = 1.0;

	std::unique_ptr<cRenderJob> renderJob(
		new cRenderJob(tempParam, tempFractal, mainImage, 1, &stopRequest, renderedImage));
	QObject::connect(renderJob.get(), SIGNAL(updateStatistics(cStatistics)),
		mainWindow->ui->widgetDockStatistics, SLOT(slotUpdateStatistics(cStatistics)));
	connect(renderJob.get(), SIGNAL(updateImage()), renderedImage, SLOT(update()));

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
		tempParam->Set("DE_factor", DEFactor);

		gPar->Set("DE_factor", DEFactor);
		SynchronizeInterface(gPar, gParFractal, qInterface::write);

		renderJob->UpdateParameters(tempParam, tempFractal);
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
}

void cInterface::ResetFormula(int fractalNumber) const
{
	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	gUndo->Store(gPar, gParFractal, gAnimFrames, gKeyframes);
	std::shared_ptr<cParameterContainer> fractal = gParFractal->at(fractalNumber);

	QStringList exclude = {"formula_code"};
	fractal->ResetAllToDefault(exclude);

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

	gUndo->Store(gPar, gParFractal, gAnimFrames, gKeyframes);
	SynchronizeInterface(gPar, gParFractal, qInterface::write);
}

void cInterface::PeriodicRefresh()
{
	if (!mainWindow->manipulations->isDraggingStarted())
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
	connect(autoRefreshTimer, &QTimer::timeout, mainWindow, &RenderWindow::slotAutoRefresh);
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
	gPar->Set("raytraced_reflections", true);
	gPar->Set("detail_level", 2.0);
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

	QObject::connect(gKeyframeAnimation, &cKeyframeAnimation::updateProgressAndStatus, mainWindow,
		&RenderWindow::slotUpdateProgressAndStatus);
	QObject::connect(gKeyframeAnimation, &cKeyframeAnimation::updateProgressHide, mainWindow,
		&RenderWindow::slotUpdateProgressHide);
	QObject::connect(gKeyframeAnimation, &cKeyframeAnimation::updateStatistics,
		mainWindow->ui->widgetDockStatistics, &cDockStatistics::slotUpdateStatistics);
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

void cInterface::ColorizeGroupBoxes(QWidget *window, int randomSeed)
{
	QList<QGroupBox *> widgets;
	widgets = window->findChildren<QGroupBox *>();
	if (qobject_cast<QGroupBox *>(window)) // check if QGroupBox
	{
		widgets.append(static_cast<QGroupBox *>(window));
	}
	QPalette palette = window->palette();
	QColor globalColor = palette.window().color();
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

			if (widget->objectName().contains("widgetTabFractal"))
			{
				int fractalIndex = widget->objectName().right(1).toInt();
				parSettings.SetFractalFormulaIndex(fractalIndex);
			}

			DisablePeriodicRefresh();
			gInterfaceReadyForSynchronization = false;
			parSettings.LoadFromFile(filename);
			parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);

			SynchronizeInterface(gPar, gParFractal, qInterface::write);
			gInterfaceReadyForSynchronization = true;

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

		std::shared_ptr<cParameterContainer> container = nullptr;
		if (containerName == "main")
		{
			container = gPar;
		}
		else if (containerName.indexOf("fractal") >= 0)
		{
			const int index = containerName.right(1).toInt();
			if (index < 4)
			{
				container = gParFractal->at(index);
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

void cInterface::RandomizeLocalSettings(const QWidget *widget)
{
	cRandomizerDialog *randomizer = new cRandomizerDialog();
	randomizer->setAttribute(Qt::WA_DeleteOnClose);
	randomizer->AssignSourceWidget(widget);
	randomizer->show();
}

QStringList cInterface::CreateListOfParametersInWidget(const QWidget *inputWidget)
{
	QList<QWidget *> listOfWidgets = inputWidget->findChildren<QWidget *>();
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
#if QT_VERSION < QT_VERSION_CHECK(5, 14, 0)
	QStringList list(listOfParameters.toList());
#else
	QStringList list(listOfParameters.values());
#endif
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

void cInterface::CleanSettings()
{
	if (QMessageBox::Yes
			== QMessageBox::question(mainWindow, tr("Cleaning up"),
				tr("Do you want to clean up settings?\nIt will take a while"),
				QMessageBox::Yes | QMessageBox::No))
	{
		SynchronizeInterface(gPar, gParFractal, qInterface::read);
		gUndo->Store(gPar, gParFractal);
		gUndo->Store(gPar, gParFractal);
		cSettingsCleaner *cleaner = new cSettingsCleaner(mainWindow);
		cleaner->show();
		cleaner->runCleaner();
		cleaner->exec();
		delete cleaner;
		SynchronizeInterface(gPar, gParFractal, qInterface::write);
	}
}

void cInterface::slotAutoSaveImage(double timeSeconds)
{
	if (timeSeconds > 600)
	{
		QString filename = systemDirectories.GetImagesFolder() + QDir::separator() + "autosave.png";
		SaveImage(filename, ImageFileSave::IMAGE_FILE_TYPE_PNG, gMainInterface->mainImage, nullptr);
		mainWindow->ui->statusbar->showMessage(tr("Image auto-saved to %1").arg(filename), 0);
	}
}

void cInterface::UpdateMainImagePreview()
{
	renderedImage->update();
}

void cInterface::ChangeLightWireframeVisibility(bool enable)
{
	renderedImage->SetLightsVisibility(enable);
	renderedImage->update();
}

QList<QVariant> cInterface::GetMouseClickFunction()
{
	int index = mainWindow->ui->comboBox_mouse_click_function->currentIndex();
	return mainWindow->ui->comboBox_mouse_click_function->itemData(index).toList();
}

void cInterface::RefreshMainImage()
{
	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	std::unique_ptr<cRenderJob> renderJob(
		new cRenderJob(gPar, gParFractal, mainImage, 1, &stopRequest, renderedImage));
	renderJob->RefreshPostEffects();
}

void cInterface::RefreshImageAdjustments()
{
	SynchronizeInterface(gPar, gParFractal, qInterface::read);
	std::unique_ptr<cRenderJob> renderJob(
		new cRenderJob(gPar, gParFractal, mainImage, 1, &stopRequest, renderedImage));
	renderJob->RefreshImageAdjustments();
}

void cInterface::slotDisablePeriodicRefresh()
{
	DisablePeriodicRefresh();
}

void cInterface::slotReEnablePeriodicRefresh()
{
	ReEnablePeriodicRefresh();
}

void cInterface::slotRefreshPostEffects()
{
	RefreshMainImage();
}

void cInterface::LoadDefaultSettings()
{
	QString previousSettings = systemData.lastSettingsFile;
	QString filename = QDir::toNativeSeparators(systemDirectories.GetDefaultSettingsFile());
	if (QFile::exists(filename))
	{
		gMainInterface->SynchronizeInterface(
			gPar, gParFractal, qInterface::read); // update appParam before loading new settings
		mainWindow->slotMenuLoadSettingsFromFile(filename);
	}

	// to not modify last settings file in the file browser
	systemData.lastSettingsFile = previousSettings;
}

bool cInterface::CheckForMissingTextures()
{
	bool correctionApplied = false;
	QStringList listOfAllParameters = gPar->GetListOfParameters();
	QStringList listOfTextureParameters;
	for (const QString &parameterName : listOfAllParameters)
	{
		if (parameterName.contains("file_") && parameterName.contains("texture")
				&& !parameterName.contains("animsound_"))
			listOfTextureParameters.append(parameterName);
	}
	listOfTextureParameters.append("file_background");
	listOfTextureParameters.append("file_envmap");
	listOfTextureParameters.append("file_lightmap");

	for (const QString &parameterName : listOfTextureParameters)
	{
		QString textureFile = gPar->Get<QString>(parameterName);
		QFileInfo fi(textureFile);
		if (!textureFile.contains("%") && !textureFile.contains("https://")
				&& !textureFile.contains("http://") && (!fi.exists(textureFile) || fi.isDir()))
		{
			QMessageBox msgBox;
			msgBox.setText(tr("Missing texture"));
			msgBox.setWindowTitle("Missing texture");
			msgBox.setInformativeText(
				tr("Parameter %1\nuses texture file\n%2\nwhich is missing.\n\nWhat you would like to do?")
					.arg(parameterName)
					.arg(textureFile));
			QPushButton *buttonDefault = msgBox.addButton(tr("Use default"), QMessageBox::ActionRole);
			QPushButton *buttonSelect = msgBox.addButton(tr("Select file..."), QMessageBox::ActionRole);
			QPushButton *buttonSearch = msgBox.addButton(tr("Search..."), QMessageBox::ActionRole);
			QPushButton *buttonIgnore = msgBox.addButton(tr("Ignore"), QMessageBox::DestructiveRole);
			int ret = msgBox.exec();

			msgBox.exec();

			if (msgBox.clickedButton() == buttonDefault) // use default
			{
				gPar->Set(parameterName, gPar->GetDefault<QString>(parameterName));
				correctionApplied = true;
			}
			else if (msgBox.clickedButton() == buttonSelect) // select file
			{
				QFileInfo fileInfo(textureFile);
				QString dirName = fileInfo.absolutePath();
				if (!fileInfo.exists())
				{
					dirName = gPar->Get<QString>("default_user_textures_path");
				}

				PreviewFileDialog dialog(mainWindow);
				dialog.setFileMode(QFileDialog::ExistingFile);
				dialog.setNameFilter(tr("Images (*.jpg *.jpeg *.png *.bmp *.hdr)"));
				dialog.setDirectory(QDir::toNativeSeparators(dirName));
				dialog.selectFile(QDir::toNativeSeparators(textureFile));
				dialog.setAcceptMode(QFileDialog::AcceptOpen);
				dialog.setWindowTitle(tr("Select file for %1").arg(parameterName));
				QStringList filenames;
				if (dialog.exec())
				{
					filenames = dialog.selectedFiles();
					QString fileName = QDir::toNativeSeparators(filenames.first());
					gPar->Set(parameterName, fileName);
					correctionApplied = true;
				}
			}
			else if (msgBox.clickedButton() == buttonSearch) // searching
			{
				QStringList foundFiles;

				QFileInfo fileInfo(textureFile);
				QString onlyFileName(fileInfo.fileName());

				QStringList searchFolders;
				searchFolders.append(gPar->Get<QString>("default_user_textures_path"));
				searchFolders.append(gPar->Get<QString>("default_textures_path"));
				searchFolders.append(QDir(systemData.lastSettingsFile).dirName());

				for (const QString &searchFolder : searchFolders)
				{
					if (QDir(searchFolder).exists())
					{
						QDirIterator it(
							searchFolder, QStringList() << "*.jpg", QDir::Files, QDirIterator::Subdirectories);

						while (it.hasNext())
						{
							QString fileSearch = it.next();
							QFileInfo fileInfoSearch(fileSearch);
							QString onlyFileNameSearch(fileInfoSearch.fileName());
							if (onlyFileNameSearch == onlyFileName)
							{
								foundFiles.append(fileSearch);
							}
						}
					}
				}

				if (foundFiles.size() > 0)
				{
					bool ok;
					QString substitution =
						QInputDialog::getItem(mainWindow, tr("Some files of the same were found:"),
							tr("Select one from proposed files"), foundFiles, 0, false, &ok);
					if (ok)
					{
						qDebug() << "substitution" << substitution;
						gPar->Set(parameterName, substitution);
						correctionApplied = true;
					}
				}
			}
			else if (msgBox.clickedButton() == buttonIgnore) // ignore
			{
				// do nothing
			}
		}
	}
	return correctionApplied;
}

void cInterface::UpdateCameraRotation(std::shared_ptr<cParameterContainer> params)
{
	// recalculation of camera rotation and distance (just for display purposes)
	const CVector3 camera = params->Get<CVector3>("camera");
	const CVector3 target = params->Get<CVector3>("target");
	const CVector3 top = params->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, top);
	params->Set("camera_rotation", cameraTarget.GetRotation() * 180.0 / M_PI);
	params->Set("camera_distance_to_target", cameraTarget.GetDistance());
}
