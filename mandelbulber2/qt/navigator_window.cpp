/*
 * navigator_window.cpp
 *
 *  Created on: 24 wrz 2021
 *      Author: krzysztof
 */

#include "navigator_window.h"
#include "tab_fractal.h"

#include <QThread>

#include "src/common_math.h"
#include "src/lights.hpp"
#include "src/light.h"
#include "src/ao_modes.h"
#include "src/render_window.hpp"
#include "src/fractal_container.hpp"
#include "ui_navigator_window.h"
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
}

void cNavigatorWindow::AddLeftWidget(QWidget *widget)
{
	leftWidget = widget;
	ui->scrollAreaParameterSetContents->layout()->addWidget(widget);
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
	manipulations->AssignParameterContainers(params, fractalParams);
	manipulations->AssingImage(image);
	manipulations->AssignWidgets(ui->widgetRenderedImage, ui->widgetNavigationButtons, nullptr);

	SynchronizeInterfaceWindow(ui->frameNavigationButtons, params, qInterface::write);

	if (leftWidget)
	{
		if (cTabFractal *fractalWidget = qobject_cast<cTabFractal *>(leftWidget))
		{
			int tabIndex = fractalWidget->GetTabIndex();
			fractalWidget->SynchronizeFractal(fractalParams->at(tabIndex), qInterface::write);
			fractalWidget->SynchronizeInterface(params, qInterface::write);
		}
		else
		{
			SynchronizeInterfaceWindow(ui->groupBoxParameterSet, params, qInterface::write);
		}
	}

	StartRender();
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

	SynchronizeInterfaceWindow(ui->frameNavigationButtons, params, qInterface::read);

	if (leftWidget)
	{
		if (cTabFractal *fractalWidget = qobject_cast<cTabFractal *>(leftWidget))
		{
			int tabIndex = fractalWidget->GetTabIndex();
			fractalWidget->SynchronizeFractal(fractalParams->at(tabIndex), qInterface::read);
			fractalWidget->SynchronizeInterface(params, qInterface::write);
		}
		else
		{
			SynchronizeInterfaceWindow(ui->groupBoxParameterSet, params, qInterface::read);
		}
	}

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
