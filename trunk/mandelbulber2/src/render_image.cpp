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

	//prepare multiple threads
	QThread **thread = new QThread*[data->numberOfThreads];
	sThreadData *threadData = new sThreadData[data->numberOfThreads];
	cRenderWorker **worker= new cRenderWorker*[data->numberOfThreads];
	QEventLoop loop;
	cScheduler *scheduler = new cScheduler(image->GetHeight());

	for(int i=0; i < data->numberOfThreads; i++)
	{
		threadData[i].id = i + 1;
		threadData[i].startLine = image->GetHeight()/4 * i;
		threadData[i].scheduler = scheduler;

		thread[i] = new QThread; //Warning! not needed to delete object
		worker[i] = new cRenderWorker(params, fractal, &threadData[i], data, image); //Warning! not needed to delete object
		worker[i]->moveToThread(thread[i]);
		QObject::connect(thread[i], SIGNAL(started()), worker[i], SLOT(doWork()));
		QObject::connect(worker[i], SIGNAL(finished()), thread[i], SLOT(quit()));
		QObject::connect(worker[i], SIGNAL(finished()), worker[i], SLOT(deleteLater()));
		QObject::connect(thread[i], SIGNAL(finished()), thread[i], SLOT(deleteLater()));
		QObject::connect(worker[i], SIGNAL(finished()), &loop, SLOT(quit()));
		thread[i]->start();
		out << "Thread " << i << " started" << endl;
	}

	while(!scheduler->AllLinesDone())
	{
		mainInterface->application->processEvents();

		//get list of last rendered lines
		QVector<int> list = scheduler->GetLastRenderedLines();
		qDebug() << "Lines rendered: " << list;
		image->CompileImage(&list);
		image->ConvertTo8bit();
		image->UpdatePreview(&list);
		image->GetImageWidget()->update();
	}

	image->CompileImage();
	image->ConvertTo8bit();
	image->UpdatePreview();
	image->GetImageWidget()->update();

	out << "All threads finished" << endl;

	delete[] thread;
	delete[] threadData;
	delete[] worker;

	return true;
}
