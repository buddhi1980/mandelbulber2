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

	int progressiveSteps = (int)(log(max(image->GetWidth(), image->GetHeight())) / log(2))-3;
	if(progressiveSteps < 0) progressiveSteps = 0;
	int progressive = pow(2, progressiveSteps - 1);
	if (progressive == 0) progressive = 1;

	//prepare multiple threads
	QThread **thread = new QThread*[data->numberOfThreads];
	sThreadData *threadData = new sThreadData[data->numberOfThreads];
	cRenderWorker **worker= new cRenderWorker*[data->numberOfThreads];

	cScheduler *scheduler = new cScheduler(image->GetHeight(), progressive);

	cProgressText progressText;
	progressText.ResetTimer();

	for(int i=0; i < data->numberOfThreads; i++)
	{
		threadData[i].id = i + 1;
		threadData[i].startLine = (image->GetHeight()/data->numberOfThreads * i) / scheduler->GetProgresiveStep() * scheduler->GetProgresiveStep();
		threadData[i].scheduler = scheduler;
	}

	QString statusText;
	QString progressTxt;

	QElapsedTimer timerRefresh;
	timerRefresh.start();
	qint64 lastRefreshTime = 0;
	QList<int> listToRefresh;

	do
	{
		for (int i = 0; i < data->numberOfThreads; i++)
		{
			thread[i] = new QThread;
			worker[i] = new cRenderWorker(params, fractal, &threadData[i], data, image); //Warning! not needed to delete object
			worker[i]->moveToThread(thread[i]);
			QObject::connect(thread[i], SIGNAL(started()), worker[i], SLOT(doWork()));
			QObject::connect(worker[i], SIGNAL(finished()), thread[i], SLOT(quit()));
			QObject::connect(worker[i], SIGNAL(finished()), worker[i], SLOT(deleteLater()));

			thread[i]->start();
			out << "Thread " << i << " started" << endl;
		}

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
			QList<int> list = scheduler->GetLastRenderedLines();

			//create list of lines for image refresh
			listToRefresh += list;

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
					QSet<int> set_listTorefresh = listToRefresh.toSet(); //removing duplicates
					listToRefresh = set_listTorefresh.toList();
					qSort(listToRefresh);

					image->CompileImage(&listToRefresh);
					if (image->IsPreview())
					{
						image->ConvertTo8bit();
						image->UpdatePreview(&listToRefresh);
						image->GetImageWidget()->update();
					}
					lastRefreshTime = timerRefresh.elapsed() / (listToRefresh.size());
					timerRefresh.restart();
					listToRefresh.clear();
				}
			}
		}
		for (int i = 0; i < data->numberOfThreads; i++)
		{
			while(thread[i]->isRunning())
			{
				mainInterface->application->processEvents();
			};
			delete thread[i];
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
