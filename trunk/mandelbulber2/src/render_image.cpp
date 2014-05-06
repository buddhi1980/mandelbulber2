/*
 * render_image.cpp
 *
 *  Created on: May 3, 2014
 *      Author: krzysztof
 */

#include "render_image.hpp"
#include <QtCore>

#include "system.hpp"
#include "render_worker.hpp"
#include "interface.hpp"

cRenderer::cRenderer(const cParamRender *_params, const cFourFractals *_fractal, const sRenderData *_renderData, cImage *_image)
{
	params = _params;
	fractal = _fractal;
	data = _renderData;
	image = _image;
}

bool cRenderer::RenderImage()
{
	QTextStream out(stdout);
	WriteLog("cRenderer::RenderImage()");

	image->SetImageParameters(params->imageAdjustments);

	out << "Start rendering" << endl;

	QThread *thread = new QThread;

	sThreadData threadData;
	threadData.id = 1;
	threadData.startLine = 0;

	QEventLoop loop;

	cRenderWorker *worker = new cRenderWorker(params, fractal, &threadData, data, image);
	worker->moveToThread(thread);
	QObject::connect(worker, SIGNAL(error(const QString)), mainInterface->application, SLOT(errorString(QString)));
	QObject::connect(thread, SIGNAL(started()), worker, SLOT(doWork()));
	QObject::connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
	QObject::connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
	QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
	QObject::connect(worker, SIGNAL(finished()), &loop, SLOT(quit()));
	thread->start();
	out << "Thread started" << endl;

	loop.exec();

	image->CompileImage();
	image->ConvertTo8bit();
	image->UpdatePreview();

	mainInterface->mainWindow->update();
	//mainInterface->mainWindow->ui->scrollAreaWidgetContents->update();

	out << "Thread finished";

	return true;
}
