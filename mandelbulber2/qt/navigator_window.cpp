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

#include "src/common_math.h"
#include "src/lights.hpp"
#include "src/light.h"
#include "src/ao_modes.h"
#include "src/render_window.hpp"
#include "src/fractal_container.hpp"

#include "src/parameters.hpp"
#include "src/fractal_container.hpp"
#include "src/cimage.hpp"
#include "src/synchronize_interface.hpp"
#include "src/global_data.hpp"
#include "src/write_log.hpp"
#include "src/settings.hpp"
#include "src/render_job.hpp"
#include "src/error_message.hpp"
#include "src/rendering_configuration.hpp"
#include "src/manipulations.h"
#include "src/interface.hpp"

cNavigatorWindow::cNavigatorWindow(QWidget *parent) : QDialog(parent), ui(new Ui::cNavigatorWindow)
{
	ui->setupUi(this);
	ui->widgetNavigationButtons->HideSomeButtons();
	setModal(false);
	setWindowTitle(tr("Navigator"));

	manipulations = new cManipulations(this);

	image.reset(new cImage(initImageWidgth, initImageHeight, false));
	ui->widgetRenderedImage->AssignImage(image);
	image->SetFastPreview(true);
	image->CreatePreview(1.0, initImageWidgth, initImageHeight, ui->widgetRenderedImage);
	image->UpdatePreview();

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
	params.reset(new cParameterContainer());
	fractalParams.reset(new cFractalContainer());

	sourceParams = _params;
	sourceFractalParams = _fractalParams;

	*params = *_params;
	*fractalParams = *_fractalParams;

	ui->widgetRenderedImage->AssignParameters(params, fractalParams);
	ui->widgetNavigationButtons->AssignParameterContainers(params, fractalParams, &stopRequest);

	cMyWidgetWithParams *widgetWithParams = dynamic_cast<cMyWidgetWithParams *>(leftWidget);
	if (widgetWithParams)
	{
		widgetWithParams->AssignParameterContainers(params, fractalParams);
		widgetWithParams->AssignSpecialWidgets(
			ui->widgetRenderedImage, ui->comboBox_mouse_click_function);
	}

	manipulations->AssignParameterContainers(params, fractalParams);
	manipulations->AssingImage(image);
	manipulations->AssignWidgets(ui->widgetRenderedImage, ui->widgetNavigationButtons, nullptr);

	InitPeriodicRefresh();

	cInterface::ComboMouseClickUpdate(ui->comboBox_mouse_click_function, params);

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

	if (params::enumAOMode(params->Get<int>("ambient_occlusion_mode")) == params::AOModeScreenSpace)
	{
		tempParams->Set("ambient_occlusion_mode", int(params::AOModeFast));
		tempParams->Set("ambient_occlusion_fast_tune", 0.5);
	}

	double sizeFactor = 1.0 * sqrt(lastRenderedTimeOfSmallPart + 0.0001) * lastSizefactor;

	int intSizeFactor = lastSizefactor;
	if (sizeFactor > (lastSizefactor - 1) * 2.0 || sizeFactor < (lastSizefactor - 1) * 0.5)
	{
		intSizeFactor = int(sizeFactor) + 1;
	}
	intSizeFactor = clamp(intSizeFactor, 1, 8);

	lastSizefactor = intSizeFactor;

	int width = clamp(int(1200 / intSizeFactor), 64, 1200);
	int height = width * 3 / 4;

	tempParams->Set("image_width", width);
	tempParams->Set("image_height", height);
	tempParams->Set("detail_level", params->Get<double>("detail_level") * intSizeFactor);

	//	QList<int> listOfLights = cLights::GetListOfLights(tempParams);
	//	for (int lightIndex : listOfLights)
	//	{
	//		tempParams->Set(cLight::Name("cast_shadows", lightIndex), false);
	//	}

	cRenderJob *renderJob = new cRenderJob(tempParams, tempFractalParams, image, &stopRequest,
		ui->widgetRenderedImage); // deleted by deleteLater()

	connect(renderJob, SIGNAL(updateImage()), ui->widgetRenderedImage, SLOT(update()));
	connect(renderJob, &cRenderJob::sendRenderedTilesList, ui->widgetRenderedImage,
		&RenderedImage::showRenderedTilesList);
	connect(renderJob, &cRenderJob::signalSmallPartRendered, manipulations,
		&cManipulations::slotSmallPartRendered);
	connect(renderJob, &cRenderJob::signalSmallPartRendered, this,
		&cNavigatorWindow::slotSmallPartRendered);

	cRenderingConfiguration config;
	config.DisableNetRender();

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
			// FIXME:			if (keyModifiers & Qt::AltModifier)
			//			{
			//				double deltaLog = exp(delta * 0.001);
			//				double dist = ui->widgetEffects->GetAuxLightManualPlacementDistance();
			//				dist *= deltaLog;
			//				ui->widgetEffects->slotSetAuxLightManualPlacementDistance(dist);
			//			}
			//			else if (keyModifiers & Qt::ControlModifier)
			//			{
			//				manipulations->MoveLightByWheel(delta);
			//			}
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
	*sourceParams = *params;
	*sourceFractalParams = *fractalParams;
	gMainInterface->SynchronizeInterface(sourceParams, sourceFractalParams, qInterface::write);
	stopRequest = true;
	gMainInterface->StartRender();
	emit close();
}

void cNavigatorWindow::slotButtonCancel()
{
	stopRequest = true;
	emit close();
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
