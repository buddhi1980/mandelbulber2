/*
 * navigator_window.cpp
 *
 *  Created on: 24 wrz 2021
 *      Author: krzysztof
 */

#include "navigator_window.h"
#include "ui_navigator_window.h"

#include "tab_fractal.h"
#include "dock_effects.h"

#include <QThread>

#include "../src/image_scale.hpp"
#include "../src/shortcuts.h"
#include "src/ao_modes.h"
#include "src/cimage.hpp"
#include "src/common_math.h"
#include "src/error_message.hpp"
#include "src/fractal_container.hpp"
#include "src/fractal_container.hpp"
#include "src/global_data.hpp"
#include "src/interface.hpp"
#include "src/initparameters.hpp"
#include "src/light.h"
#include "src/lights.hpp"
#include "src/manipulations.h"
#include "src/parameters.hpp"
#include "src/render_job.hpp"
#include "src/render_window.hpp"
#include "src/rendering_configuration.hpp"
#include "src/settings.hpp"
#include "src/synchronize_interface.hpp"
#include "src/write_log.hpp"

cNavigatorWindow::cNavigatorWindow(QWidget *parent) : QDialog(parent), ui(new Ui::cNavigatorWindow)
{
	ui->setupUi(this);
	ui->widgetNavigationButtons->HideSomeButtons();
	setModal(false);
	setWindowTitle(tr("Navigator"));

	manipulations = new cManipulations(this);

	connect(ui->widgetNavigationButtons, &cDockNavigation::signalRender, this,
		&cNavigatorWindow::StartRender);
	connect(manipulations, &cManipulations::signalRender, this, &cNavigatorWindow::StartRender);
	connect(manipulations, &cManipulations::signalStop, this, &cNavigatorWindow::StopRender);

	connect(ui->widgetNavigationButtons, &cDockNavigation::signalCameraMovementModeChanged, this,
		&cNavigatorWindow::slotCameraMovementModeChanged);

	connect(ui->widgetRenderedImage, &RenderedImage::singleClick, this,
		&cNavigatorWindow::slotMouseClickOnImage);
	connect(ui->widgetRenderedImage, &RenderedImage::mouseWheelRotatedWithKey, this,
		&cNavigatorWindow::slotMouseWheelRotatedWithKeyOnImage);

	connect(ui->widgetRenderedImage, &RenderedImage::mouseDragStart, this,
		&cNavigatorWindow::slotMouseDragStart);
	connect(ui->widgetRenderedImage, &RenderedImage::mouseDragFinish, this,
		&cNavigatorWindow::slotMouseDragFinish);
	connect(ui->widgetRenderedImage, &RenderedImage::mouseDragDelta, this,
		&cNavigatorWindow::slotMouseDragDelta);

	connect(
		ui->pushButtonUse, &QPushButton::pressed, this, &cNavigatorWindow::slotButtonUseParameters);
	connect(ui->pushButtonCancel, &QPushButton::pressed, this, &cNavigatorWindow::slotButtonCancel);

	connect(manipulations, &cManipulations::signalDisablePeriodicRefresh, this,
		&cNavigatorWindow::slotDisablePeriodicRefresh);
	connect(manipulations, &cManipulations::signalReEnablePeriodicRefresh, this,
		&cNavigatorWindow::slotReEnablePeriodicRefresh);
	connect(manipulations, &cManipulations::signalRefreshPostEffects, this,
		&cNavigatorWindow::slotRefreshMainImage);

	connect(ui->comboBox_mouse_click_function, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotChangedComboMouseClickFunction(int)));

	connect(ui->checkBox_navigator_dark_glow, &QCheckBox::stateChanged, this,
		&cNavigatorWindow::slotDarkGlowEnabled);

	connect(ui->widgetRenderedImage, &RenderedImage::keyPress, this,
		&cNavigatorWindow::slotKeyPressOnImage);
	connect(ui->widgetRenderedImage, &RenderedImage::keyRelease, this,
		&cNavigatorWindow::slotKeyReleaseOnImage);

	buttonPressTimer = new QTimer(this);
	connect(buttonPressTimer, &QTimer::timeout, this, &cNavigatorWindow::slotButtonLongPress);
	buttonPressTimer->start(100);

	SynchronizeInterfaceWindow(ui->groupBox_navigator_options, gPar, qInterface::write);
}

void cNavigatorWindow::AddLeftWidget(QWidget *widget)
{
	leftWidget = widget;
	ui->scrollAreaParameterSetContents->layout()->addWidget(widget);

	if (leftWidget)
	{
		cDockEffects *dockEffects = dynamic_cast<cDockEffects *>(leftWidget);
		if (dockEffects)
		{
			connect(manipulations, &cManipulations::signalWriteInterfaceBasicFog, dockEffects,
				&cDockEffects::slotSynchronizeInterfaceBasicFog);
			connect(manipulations, &cManipulations::signalWriteInterfaceDOF, dockEffects,
				&cDockEffects::slotSynchronizeInterfaceDOF);
			connect(manipulations, &cManipulations::signalWriteInterfaceRandomLights, dockEffects,
				&cDockEffects::slotSynchronizeInterfaceRandomLights);
			connect(dockEffects, &cDockEffects::signalRefreshPostEffects, this,
				&cNavigatorWindow::slotRefreshMainImage);
			connect(manipulations, &cManipulations::signalWriteInterfaceLights, dockEffects,
				&cDockEffects::slotSynchronizeInterfaceLights);
		}
	}
}

cNavigatorWindow::~cNavigatorWindow()
{
	delete ui;
}

void cNavigatorWindow::SetInitialParameters(
	std::shared_ptr<cParameterContainer> _params, std::shared_ptr<cFractalContainer> _fractalParams)
{
	if (!leftWidget)
	{
		ui->groupBoxParameterSet->hide();
	}

	params.reset(new cParameterContainer());
	fractalParams.reset(new cFractalContainer());

	sourceParams = _params;
	sourceFractalParams = _fractalParams;

	*params = *_params;
	if (_fractalParams) *fractalParams = *_fractalParams;

	QRect availableScreenGeometry = QGuiApplication::screens().first()->availableGeometry();
	int maxWindowWidth = availableScreenGeometry.width();
	int maxWindowHeight = availableScreenGeometry.height();

	imageProportion = double(params->Get<int>("image_width")) / params->Get<int>("image_height");

	initImageWidth = maxWindowWidth / (4 - ui->comboBox_navigator_preview_size->currentIndex());
	if (!leftWidget) initImageWidth = initImageWidth * 120 / 100;
	initImageHeight = initImageWidth / imageProportion;

	image.reset(new cImage(initImageWidth, initImageHeight, false));
	ui->widgetRenderedImage->AssignImage(image);
	image->SetResizeOnChangeSize(true);
	image->SetFastPreview(true);
	image->CreatePreview(1.0, initImageWidth, initImageHeight, ui->widgetRenderedImage);
	image->UpdatePreview();

	ui->widgetRenderedImage->AssignParameters(params, fractalParams);
	ui->widgetNavigationButtons->AssignParameterContainers(params, fractalParams, &stopRequest);

	cMyWidgetWithParams *widgetWithParams = dynamic_cast<cMyWidgetWithParams *>(leftWidget);

	if (widgetWithParams)
	{
		widgetWithParams->AssignSpecialWidgets(
			ui->widgetRenderedImage, ui->comboBox_mouse_click_function);
		widgetWithParams->AssignParameterContainers(params, fractalParams);
	}

	manipulations->AssignParameterContainers(params, fractalParams);
	manipulations->AssingImage(image);

	cDockEffects *widgetDockEfects = nullptr;
	if (leftWidget)
	{
		widgetDockEfects = qobject_cast<cDockEffects *>(leftWidget);
	}

	manipulations->AssignWidgets(
		ui->widgetRenderedImage, ui->widgetNavigationButtons, widgetDockEfects);

	InitPeriodicRefresh();

	cInterface::ComboMouseClickUpdate(ui->comboBox_mouse_click_function, params);

	connect(ui->comboBox_navigator_preview_size, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotChangedPreviewSize()));

	// StartRender();
}

void cNavigatorWindow::SynchronizeInterface(qInterface::enumReadWrite mode)
{
	SynchronizeInterfaceWindow(ui->frameNavigationButtons, params, mode);

	if (leftWidget)
	{
		if (cTabFractal *fractalWidget = qobject_cast<cTabFractal *>(leftWidget))
		{
			int tabIndex = fractalWidget->GetTabIndex();
			fractalWidget->SynchronizeFractal(fractalParams->at(tabIndex), mode);
			fractalWidget->SynchronizeInterface(params, mode);
		}
		else
		{
			SynchronizeInterfaceWindow(ui->groupBoxParameterSet, params, mode);
		}
	}

	SynchronizeInterfaceWindow(ui->groupBox_navigator_options, gPar, qInterface::read);
}

void cNavigatorWindow::StartRender()
{
	manipulations->IncreaseNumberOfStartedRenders();
	if (!image->IsUsed())
	{
		image->BlockImage();
		WriteLog("cInterface::StartRender(void) - image was free", 2);
	}
	else
	{
		WriteLog("cInterface::StartRender(void) - image was used by another instance", 2);
		stopRequest = true;
		while (image->IsUsed())
		{
			gApplication->processEvents();
			stopRequest = true;
		}
		image->BlockImage();
	}

	SynchronizeInterface(qInterface::read);

	// check if something was changed in settings
	cSettings tempSettings(cSettings::formatCondensedText);
	tempSettings.CreateText(params, fractalParams);
	autoRefreshLastHash = tempSettings.GetHashCode();

	std::shared_ptr<cParameterContainer> tempParams(new cParameterContainer());
	std::shared_ptr<cFractalContainer> tempFractalParams(new cFractalContainer());
	*tempParams = *params;
	*tempFractalParams = *fractalParams;

	int width = 0;
	int height = 0;
	int intSizeFactor = 0;

	QRect availableScreenGeometry = QGuiApplication::screens().first()->availableGeometry();
	int maxWindowWidth = availableScreenGeometry.width();
	int maxWindowHeight = availableScreenGeometry.height();
	int newInitImageWidth =
		maxWindowWidth / (4 - ui->comboBox_navigator_preview_size->currentIndex());
	if (!leftWidget) newInitImageWidth = newInitImageWidth * 120 / 100;
	int newInitImageHeight = initImageWidth / imageProportion;
	if ((newInitImageHeight != initImageHeight) || newInitImageWidth != initImageWidth)
	{
		initImageHeight = newInitImageHeight;
		initImageWidth = newInitImageWidth;
		ui->widgetRenderedImage->setFixedWidth(initImageWidth);
		ui->widgetRenderedImage->setFixedHeight(initImageHeight);
		image->ChangeSize(initImageWidth, initImageHeight, sImageOptional());
		image->CreatePreview(1.0, initImageWidth, initImageHeight, ui->widgetRenderedImage);
	}

	if (forcedSizeFactor > 0)
	{
		intSizeFactor = forcedSizeFactor;
		forcedSizeFactor = 0;
	}
	else
	{
		if (ui->comboBox_navigator_preview_quality->currentIndex() == 0)
		{
			double sizeFactor = 8 * (lastRenderedTimeOfSmallPart + 0.001) * lastSizefactor;

			intSizeFactor = lastSizefactor;
			if (sizeFactor > (lastSizefactor - 1) * 2.0 || sizeFactor < (lastSizefactor - 1) * 0.5)
			{
				intSizeFactor = int(sizeFactor) + 1;
			}
			intSizeFactor = clamp(intSizeFactor, 1, 16);

			lastSizefactor = lastSizefactor + (intSizeFactor - lastSizefactor) * 0.3;
			intSizeFactor = lastSizefactor;
		}
		else
		{
			lastSizefactor = intSizeFactor =
				pow(2, ui->comboBox_navigator_preview_quality->currentIndex() - 1);
		}
	}

	if (params->Get<int>("opencl_mode") == 0) lastSizefactor = intSizeFactor = 1;

	width = clamp(int(initImageWidth / intSizeFactor), 64, initImageWidth);
	height = width / imageProportion;

	tempParams->Set("image_width", width);
	tempParams->Set("image_height", height);
	tempParams->Set("detail_level", params->Get<double>("detail_level") * intSizeFactor);

	//	if (intSizeFactor != lastIntSizeFactor)
	//	{
	//		image->FastResize(width, height);
	//		double scale = CalcMainImageScale(0.0, initImageWidth, initImageHeight, image);
	//		image->CreatePreview(scale, initImageWidth, initImageWidth, ui->widgetRenderedImage);
	//		image->CompileImage();
	//		image->ConvertTo8bitChar();
	//		image->UpdatePreview();
	//	}

	lastIntSizeFactor = intSizeFactor;

	if (params::enumAOMode(params->Get<int>("ambient_occlusion_mode")) == params::AOModeScreenSpace
			|| ui->checkBox_navigator_dark_glow->isChecked())
	{
		tempParams->Set("ambient_occlusion_mode", int(params::AOModeFast));
		tempParams->Set("ambient_occlusion_fast_tune", 0.5);
	}

	if (!ui->checkBox_navigator_shadows->isChecked())
	{
		QList<int> listOfLights = cLights::GetListOfLights(tempParams);
		for (int lightIndex : listOfLights)
		{
			tempParams->Set(cLight::Name("cast_shadows", lightIndex), false);
		}
	}

	if (!ui->checkBox_navigator_reflections->isChecked())
		tempParams->Set("raytraced_reflections", false);

	if (!ui->checkBox_navigator_volumetrics->isChecked())
	{
		tempParams->Set("volumetric_fog_enabled", false);
		tempParams->Set("iteration_fog_enable", false);
		tempParams->Set("clouds_enable", false);
		tempParams->Set("glow_enabled", false);
		tempParams->Set("basic_fog_enabled", false);
	}

	if (ui->checkBox_navigator_dark_glow->isChecked())
	{
		tempParams->Set("glow_enabled", true);
		tempParams->Set("glow_intensity", 4.0);
		tempParams->Set("glow_color_1", sRGB(0, 0, 0));
		tempParams->Set("glow_color_2", sRGB(0, 0, 0));
		tempParams->Set("ambient_occlusion_fast_tune", 1.0);
	}

	cRenderJob *renderJob = new cRenderJob(tempParams, tempFractalParams, image, &stopRequest,
		ui->widgetRenderedImage); // deleted by deleteLater()

	connect(renderJob, SIGNAL(updateImage()), ui->widgetRenderedImage, SLOT(update()));
	connect(renderJob, &cRenderJob::sendRenderedTilesList, ui->widgetRenderedImage,
		&RenderedImage::showRenderedTilesList);
	connect(renderJob, &cRenderJob::signalSmallPartRendered, manipulations,
		&cManipulations::slotSmallPartRendered);
	connect(renderJob, &cRenderJob::signalSmallPartRendered, this,
		&cNavigatorWindow::slotSmallPartRendered);
	connect(renderJob, &cRenderJob::fullyRendered, this, &cNavigatorWindow::slotFullImageRendered);

	cRenderingConfiguration config;
	config.DisableNetRender();
	config.ForceFastPreview();

	if (!renderJob->Init(cRenderJob::still, config))
	{
		image->ReleaseImage();
		cErrorMessage::showMessage(
			QObject::tr("Cannot init renderJob, see log output for more information."),
			cErrorMessage::errorMessage);
		return;
	}

	QThread *thread = new QThread; // deleted by deleteLater()
	renderJob->moveToThread(thread);
	QObject::connect(thread, SIGNAL(started()), renderJob, SLOT(slotExecute()));
	QObject::connect(renderJob, SIGNAL(finished()), thread, SLOT(quit()));
	QObject::connect(renderJob, SIGNAL(finished()), renderJob, SLOT(deleteLater()));
	QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

	thread->setObjectName("RenderJob");
	thread->start();
	manipulations->DecreaseNumberOfStartedRenders();
}

void cNavigatorWindow::StopRender()
{
	stopRequest = true;
}

void cNavigatorWindow::slotCameraMovementModeChanged(int index)
{
	ui->widgetRenderedImage->SetCameraMovementMode(index);
}

void cNavigatorWindow::slotMouseClickOnImage(int x, int y, Qt::MouseButton button) const
{
	RenderedImage::enumClickMode clickMode =
		RenderedImage::enumClickMode(mouseClickFunction.at(0).toInt());
	switch (clickMode)
	{
		case RenderedImage::clickMoveCamera:
		case RenderedImage::clickFogVisibility:
		case RenderedImage::clickDOFFocus:
		case RenderedImage::clickPlaceLight:
		case RenderedImage::clickGetJuliaConstant:
		case RenderedImage::clickPlacePrimitive:
		case RenderedImage::clickPlaceRandomLightCenter:
		case RenderedImage::clickGetPoint:
		case RenderedImage::clickWrapLimitsAroundObject:
		{
			manipulations->SetByMouse(CVector2<double>(x, y), button, mouseClickFunction);
			break;
		}
		case RenderedImage::clickDoNothing:
		case RenderedImage::clickFlightSpeedControl:
			// nothing
			break;
	}
}

void cNavigatorWindow::slotMouseDragStart(int x, int y, Qt::MouseButtons buttons)
{
	RenderedImage::enumClickMode clickMode =
		RenderedImage::enumClickMode(mouseClickFunction.at(0).toInt());

	switch (clickMode)
	{
		case RenderedImage::clickMoveCamera:
		case RenderedImage::clickPlaceLight:
		{
			if (gMainInterface->renderedImage->GetEnableClickModes())
			{
				manipulations->MouseDragStart(CVector2<double>(x, y), buttons, mouseClickFunction);
			}
			break;
		}
		default:
			// nothing
			break;
	}
}

void cNavigatorWindow::slotMouseDragFinish()
{
	manipulations->MouseDragFinish();
}

void cNavigatorWindow::slotMouseDragDelta(int dx, int dy)
{
	manipulations->MouseDragDelta(dx, dy);
}

void cNavigatorWindow::slotMouseWheelRotatedWithKeyOnImage(
	int x, int y, int delta, Qt::KeyboardModifiers keyModifiers)
{
	QList<QVariant> mode = mouseClickFunction;
	RenderedImage::enumClickMode clickMode = RenderedImage::enumClickMode(mode.at(0).toInt());
	switch (clickMode)
	{
		case RenderedImage::clickPlaceLight:
		{
			if (keyModifiers & Qt::AltModifier)
			{
				if (leftWidget)
				{
					if (cDockEffects *widgetEffects = dynamic_cast<cDockEffects *>(leftWidget))
					{
						double deltaLog = exp(delta * 0.001);
						double dist = widgetEffects->GetAuxLightManualPlacementDistance();
						dist *= deltaLog;
						widgetEffects->slotSetAuxLightManualPlacementDistance(dist);
					}
				}
			}
			else if (keyModifiers == Qt::NoModifier)
			{
				manipulations->MoveLightByWheel(delta);
			}
			break;
		}
		case RenderedImage::clickMoveCamera:
		{
			Qt::MouseButton button = (delta > 0) ? Qt::LeftButton : Qt::RightButton;
			mode.append(QVariant(delta));
			manipulations->SetByMouse(CVector2<double>(x, y), button, mode);
			break;
		}
		default: break;
	}
}

void cNavigatorWindow::slotButtonUseParameters()
{
	stopRequest = true;
	*sourceParams = *params;
	*sourceFractalParams = *fractalParams;
	gMainInterface->SynchronizeInterface(sourceParams, sourceFractalParams, qInterface::write);
	gMainInterface->StartRender();
	emit signalChangesAccepted();
	emit close();
}

void cNavigatorWindow::slotButtonCancel()
{
	stopRequest = true;
	emit close();
}

void cNavigatorWindow::closeEvent(QCloseEvent *event)
{
	stopRequest = true;
	QWidget::closeEvent(event);
}

void cNavigatorWindow::slotSmallPartRendered(double time)
{
	lastRenderedTimeOfSmallPart = time;
}

void cNavigatorWindow::InitPeriodicRefresh()
{
	autoRefreshTimer = new QTimer(this);
	autoRefreshTimer->setSingleShot(true);
	connect(autoRefreshTimer, &QTimer::timeout, this, &cNavigatorWindow::slotPeriodicRefresh);
	autoRefreshTimer->start(int(params->Get<double>("auto_refresh_period") * 1000.0));
	autoRefreshEnabled = true;
}

void cNavigatorWindow::slotPeriodicRefresh()
{
	if (!manipulations->isDraggingStarted() && autoRefreshEnabled)
	{
		// check if something was changed in settings
		SynchronizeInterface(qInterface::read);
		cSettings tempSettings(cSettings::formatCondensedText);
		tempSettings.CreateText(params, fractalParams);
		QString newHash = tempSettings.GetHashCode();

		if (newHash != autoRefreshLastHash)
		{
			autoRefreshLastHash = newHash;
			stopRequest = true;
			StartRender();
		}
	}

	int period = lastRenderedTimeOfSmallPart * 1000 / 2;
	period = clamp(period, 10, 1000);
	autoRefreshTimer->start(period);
}

void cNavigatorWindow::slotDisablePeriodicRefresh()
{
	autoRefreshEnabled = false;
}

void cNavigatorWindow::slotReEnablePeriodicRefresh()
{
	SynchronizeInterface(qInterface::read);
	cSettings tempSettings(cSettings::formatCondensedText);
	tempSettings.CreateText(params, fractalParams);
	autoRefreshLastHash = tempSettings.GetHashCode();
	autoRefreshEnabled = true;
}

void cNavigatorWindow::SetMouseClickFunction(QList<QVariant> _clickMode)
{
	mouseClickFunction = _clickMode;
	int index = ui->comboBox_mouse_click_function->findData(mouseClickFunction);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
}

void cNavigatorWindow::slotChangedComboMouseClickFunction(int index)
{
	if (index >= 0) // if list is empty, then index = -1
	{
		QComboBox *comboBox = static_cast<QComboBox *>(sender());
		QList<QVariant> item = comboBox->itemData(index).toList();
		mouseClickFunction = item;
		ui->widgetRenderedImage->setClickMode(item);
	}
}

void cNavigatorWindow::slotRefreshMainImage()
{
	SynchronizeInterface(qInterface::read);
	std::unique_ptr<cRenderJob> renderJob(
		new cRenderJob(params, fractalParams, image, &stopRequest, ui->widgetRenderedImage));
	renderJob->RefreshPostEffects();
}

void cNavigatorWindow::slotChangedPreviewSize()
{
	StartRender();
}

void cNavigatorWindow::slotDarkGlowEnabled(int state)
{
	if (state)
	{
		ui->checkBox_navigator_reflections->setChecked(false);
		ui->checkBox_navigator_shadows->setChecked(false);
		ui->checkBox_navigator_volumetrics->setChecked(false);
	}
}

void cNavigatorWindow::slotFullImageRendered()
{
	if (lastIntSizeFactor > 1 && ui->comboBox_navigator_preview_quality->currentIndex() == 0
			&& params->Get<int>("opencl_mode") > 0 && !stopRequest)

	{
		forcedSizeFactor = lastIntSizeFactor / 2;
		StartRender();
	}
}

void cNavigatorWindow::slotKeyPressOnImage(QKeyEvent *event)
{
	currentKeyEvents.append(event->key());
	lastKeyEventModifiers = event->modifiers();
	slotKeyHandle();
	buttonPressTimer->start();
}

void cNavigatorWindow::slotKeyReleaseOnImage(QKeyEvent *event)
{
	currentKeyEvents.removeOne(event->key());

	lastKeyEventModifiers = event->modifiers();
	slotKeyHandle();
	buttonPressTimer->stop();
}

void cNavigatorWindow::slotButtonLongPress()
{
	slotKeyHandle();
}

void cNavigatorWindow::slotKeyHandle()
{
	if (currentKeyEvents.size() == 0) return;

	SynchronizeInterface(qInterface::read);

	bool render = false;

	for (int i = 0; i < currentKeyEvents.size(); i++)
	{
		enumShortcuts shortcut =
			cShortcuts::getShortcut(gPar.get(), currentKeyEvents.at(i), lastKeyEventModifiers);

		switch (shortcut)
		{
			case enumShortcuts::moveForward:
				manipulations->MoveCamera("bu_move_forward");
				render = true;
				break;
			case enumShortcuts::moveBackward:
				manipulations->MoveCamera("bu_move_backward");
				render = true;
				break;
			case enumShortcuts::moveLeft:
				manipulations->MoveCamera("bu_move_left");
				render = true;
				break;
			case enumShortcuts::moveRight:
				manipulations->MoveCamera("bu_move_right");
				render = true;
				break;
			case enumShortcuts::moveUp:
				manipulations->MoveCamera("bu_move_up");
				render = true;
				break;
			case enumShortcuts::moveDown:
				manipulations->MoveCamera("bu_move_down");
				render = true;
				break;
			case enumShortcuts::rotateLeft:
				manipulations->RotateCamera("bu_rotate_left");
				render = true;
				break;
			case enumShortcuts::rotateRight:
				manipulations->RotateCamera("bu_rotate_right");
				render = true;
				break;
			case enumShortcuts::rotateUp:
				manipulations->RotateCamera("bu_rotate_up");
				render = true;
				break;
			case enumShortcuts::rotateDown:
				manipulations->RotateCamera("bu_rotate_down");
				render = true;
				break;
			case enumShortcuts::rollLeft:
				manipulations->RotateCamera("bu_rotate_roll_left");
				render = true;
				break;
			case enumShortcuts::rollRight:
				manipulations->RotateCamera("bu_rotate_roll_right");
				render = true;
				break;
			case enumShortcuts::render:
				currentKeyEvents.removeOne(currentKeyEvents.at(i)); // long press not allowed
				StartRender();
				break;
			case enumShortcuts::stop:
				currentKeyEvents.removeOne(currentKeyEvents.at(i)); // long press not allowed
				stopRequest = true;
				break;

			default: break;
		}
	}

	SynchronizeInterface(qInterface::write);

	if (render)
	{
		StartRender();
	}
}
