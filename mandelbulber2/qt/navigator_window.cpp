/*
 * navigator_window.cpp
 *
 *  Created on: 24 wrz 2021
 *      Author: krzysztof
 */

#include "navigator_window.h"

#include <QThread>

#include "src/render_window.hpp"
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

	image.reset(new cImage(800, 600, false));
	ui->widgetRenderedImage->AssignImage(image);
	image->SetFastPreview(true);
	image->CreatePreview(1.0, 800, 600, ui->widgetRenderedImage);
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

	*params = *_params;
	*fractalParams = *_fractalParams;

	ui->widgetRenderedImage->AssignParameters(params, fractalParams);
	ui->widgetNavigationButtons->AssignParameterContainers(params, fractalParams, &stopRequest);
	manipulations->AssignParameterContainers(params, fractalParams);
	manipulations->AssingImage(image);
	manipulations->AssignWidgets(ui->widgetRenderedImage, ui->widgetNavigationButtons, nullptr);

	SynchronizeInterfaceWindow(ui->frameNavigationButtons, params, qInterface::write);

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

	// check if something was changed in settings
	cSettings tempSettings(cSettings::formatCondensedText);
	tempSettings.CreateText(params, fractalParams);
	autoRefreshLastHash = tempSettings.GetHashCode();

	cRenderJob *renderJob = new cRenderJob(params, fractalParams, image, &stopRequest,
		ui->widgetRenderedImage); // deleted by deleteLater()

	connect(renderJob, SIGNAL(updateImage()), ui->widgetRenderedImage, SLOT(update()));
	connect(renderJob, &cRenderJob::sendRenderedTilesList, ui->widgetRenderedImage,
		&RenderedImage::showRenderedTilesList);

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
