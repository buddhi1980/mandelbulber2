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
	int progressive = pow(2, (int)(log(max(image->GetWidth(), image->GetHeight())) / log(2))-4);
	if (progressive == 0) progressive = 1;
	cScheduler *scheduler = new cScheduler(image->GetHeight(), progressive);

	qDebug() << "progressive" << progressive;

	cProgressText progressText;
	progressText.ResetTimer();

	for(int i=0; i < data->numberOfThreads; i++)
	{
		threadData[i].id = i + 1;
		threadData[i].startLine = (image->GetHeight()/4 * i) / scheduler->GetProgresiveStep() * scheduler->GetProgresiveStep();
		threadData[i].scheduler = scheduler;
	}

	QString statusText;
	QString progressTxt;

	do
	{
		for (int i = 0; i < data->numberOfThreads; i++)
		{
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

		QElapsedTimer timerRefresh;
		timerRefresh.start();
		qint64 lastRefreshTime = 0;
		QVector<int> listToRefresh;

		while (!scheduler->AllLinesDone())
		{
			mainInterface->application->processEvents();
			if (mainInterface->stopRequest)
			{
				scheduler->Stop();
				mainInterface->stopRequest = false;
			}

			Wait(100); //wait 100ms

			//get list of last rendered lines
			QVector<int> list = scheduler->GetLastRenderedLines();
			//create list of lines for image refresh
			listToRefresh += list;

			//qDebug() << "Lines rendered: " << list;

			//status bar and progress bar
			double percentDone = scheduler->PercentDone();
			statusText = "Rendering image in progress";
			progressTxt = progressText.getText(percentDone);
			mainInterface->StatusText(statusText, progressTxt, percentDone);

			//refresh image
			if (listToRefresh.size() > 0)
			{
				if (timerRefresh.elapsed() > lastRefreshTime * 100)
				{
					timerRefresh.restart();
					qSort(listToRefresh);
					image->CompileImage(&listToRefresh);
					if (image->IsPreview())
					{
						image->ConvertTo8bit();
						image->UpdatePreview(&listToRefresh);
						image->GetImageWidget()->update();
					}
					lastRefreshTime = timerRefresh.elapsed() / (listToRefresh.size());
					listToRefresh.clear();
				}
			}
		}
	}
	while(scheduler->ProgresiveNextStep());

	//refresh image at end
	image->CompileImage();
	if(image->IsPreview())
	{
		image->ConvertTo8bit();
		image->UpdatePreview();
		image->GetImageWidget()->update();
	}
	//status bar and progress bar
	double percentDone = scheduler->PercentDone();
	statusText = "Idle";
	progressTxt = progressText.getText(percentDone);
	mainInterface->StatusText(statusText, progressTxt, percentDone);

	out << "All threads finished" << endl;

	delete[] thread;
	delete[] threadData;
	delete[] worker;
	delete scheduler;

	return true;
}
