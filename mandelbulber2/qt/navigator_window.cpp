/*
 * navigator_window.cpp
 *
 *  Created on: 24 wrz 2021
 *      Author: krzysztof
 */

#include "navigator_window.h"

#include "ui_navigator_window.h"
#include "src/parameters.hpp"
#include "src/fractal_container.hpp"
#include "src/cimage.hpp"
#include "src/synchronize_interface.hpp"
#include "src/global_data.hpp"
#include "src/write_log.hpp"
#include "src/settings.hpp"
#include "src/render_job.hpp"
#include <src/error_message.hpp>
#include <src/rendering_configuration.hpp>

cNavigatorWindow::cNavigatorWindow(QWidget *parent) : QDialog(parent), ui(new Ui::cNavigatorWindow)
{
	ui->setupUi(this);
	ui->widgetNavigationButtons->HideSomeButtons();
	setModal(false);

	image.reset(new cImage(800, 600, false));
	ui->widgetRenderedImage->AssignImage(image);
	image->SetFastPreview(true);
	image->CreatePreview(1.0, 800, 600, ui->widgetRenderedImage);
	image->UpdatePreview();
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
	SynchronizeInterfaceWindow(ui->frameNavigationButtons, params, qInterface::write);

	StartRender();
}

void cNavigatorWindow::StartRender()
{
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
	connect(renderJob, SIGNAL(sendRenderedTilesList(QList<sRenderedTileData>)),
		ui->widgetRenderedImage, SLOT(showRenderedTilesList(QList<sRenderedTileData>)));

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
}
