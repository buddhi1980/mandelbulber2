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
#include "progress_text.hpp"

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

	cProgressText progressText;
	progressText.ResetTimer();

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

	QString statusText;
	QString progressTxt;

	QElapsedTimer timerRefresh;
	timerRefresh.start();
	qint64 lastRefreshTime = 0;
	QVector<int> listToRefresh;

	while(!scheduler->AllLinesDone())
	{
		mainInterface->application->processEvents();
		Wait(100);

		//get list of last rendered lines
		QVector<int> list = scheduler->GetLastRenderedLines();
		listToRefresh += list;

		//qDebug() << "Lines rendered: " << list;

		double percentDone = scheduler->PercentDone();
		statusText = "Rendering image in progress";
		progressTxt = progressText.getText(percentDone);
		mainInterface->StatusText(statusText, progressTxt, percentDone);

		if(timerRefresh.elapsed() > lastRefreshTime * 100)
		{
			timerRefresh.restart();
			qSort(listToRefresh);
			image->CompileImage(&listToRefresh);
			image->ConvertTo8bit();
			image->UpdatePreview(&listToRefresh);
			image->GetImageWidget()->update();
			lastRefreshTime = timerRefresh.elapsed();
			listToRefresh.clear();
		}
	}

	image->CompileImage();
	image->ConvertTo8bit();
	image->UpdatePreview();
	image->GetImageWidget()->update();

	double percentDone = scheduler->PercentDone();
	statusText = "Idle";
	progressTxt = progressText.getText(percentDone);
	mainInterface->StatusText(statusText, progressTxt, percentDone);

	out << "All threads finished" << endl;

	delete[] thread;
	delete[] threadData;
	delete[] worker;

	return true;
}
