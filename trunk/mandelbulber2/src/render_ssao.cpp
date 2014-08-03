/*
 * render_ssao.cpp
 *
 *  Created on: Aug 3, 2014
 *      Author: krzysztof
 */

#include "render_ssao.h"
#include "progress_text.hpp"
#include "ssao_worker.h"
#include "interface.hpp"
#include "system.hpp"

cRenderSSAO::cRenderSSAO(const cParamRender *_params, const sRenderData *_renderData, cImage *_image)
{
	params = _params;
	data = _renderData;
	image = _image;
	quiet = false;
	qualityFactor = 1.0;
	progressive = 0;
}

cRenderSSAO::~cRenderSSAO()
{
	//nothing to destroy
}

void cRenderSSAO::RenderSSAO(QList<int> *list)
{
	WriteLog("cRenderSSAO::RenderSSAO()");
	//prepare multiple threads
	QThread **thread = new QThread*[data->numberOfThreads];
	cSSAOWorker::sThreadData *threadData = new cSSAOWorker::sThreadData[data->numberOfThreads];
	cSSAOWorker **worker= new cSSAOWorker*[data->numberOfThreads];

	cProgressText progressText;
	progressText.ResetTimer();

	int quality = params->ambientOcclusion * params->ambientOcclusion / qualityFactor;
	if (quality < 1) quality = 1;

	QList<int> *lists = NULL;
	if(list)
	{
		lists = new QList<int>[data->numberOfThreads];
		for(int i=0; i<list->size(); i++)
		{
			int y = list->at(i);
			int mod = y % data->numberOfThreads;
			lists[mod].append(y);
		}
	}


	double qualityFactor;
	if(progressive > 0)
	{
		qualityFactor = sqrt(1.0/(progressive*2.0));
	}
	else
	{
		qualityFactor = 1.0;
	}

	for(int i=0; i < data->numberOfThreads; i++)
	{
		threadData[i].startLine = i;
		threadData[i].noOfThreads = data->numberOfThreads;
		threadData[i].quality = params->ambientOcclusionQuality * params->ambientOcclusionQuality * qualityFactor;
		threadData[i].done = 0;
		threadData[i].progressive = progressive;
		threadData[i].stopRequest = false;

		if(list)
			threadData[i].list = &lists[i];
		else
			threadData[i].list = NULL;
	}

	QString statusText;
	QString progressTxt;

	WriteLog("Start rendering SSAO");

	for (int i = 0; i < data->numberOfThreads; i++)
	{
		WriteLog(QString("Thread ") + QString::number(i) + " create");
		thread[i] = new QThread;
		worker[i] = new cSSAOWorker(params, &threadData[i], data, image); //Warning! not needed to delete object
		worker[i]->moveToThread(thread[i]);
		QObject::connect(thread[i], SIGNAL(started()), worker[i], SLOT(doWork()));
		QObject::connect(worker[i], SIGNAL(finished()), thread[i], SLOT(quit()));
		QObject::connect(worker[i], SIGNAL(finished()), worker[i], SLOT(deleteLater()));

		thread[i]->start();
		WriteLog(QString("Thread ") + QString::number(i) + " started");
	}

	int totalDone = 0;
	int toDo;
	if(list)
	{
		toDo = list->size();
	}
	else
	{
		toDo = image->GetHeight();
	}

	while (totalDone < toDo)
	{
		mainInterface->application->processEvents();

		Wait(100);

		if (mainInterface->stopRequest)
		{
			for(int i = 0; i < data->numberOfThreads; i++)
				threadData[i].stopRequest = true;
			if(!list)
			{
				mainInterface->stopRequest = false;
			}
			break;
		}

		totalDone = 0;
		for(int i = 0; i < data->numberOfThreads; i++)
		{
			totalDone += threadData[i].done;
		}

		double percentDone = (double) totalDone / toDo;
		statusText = "Rendering SSAO effect in progress";
		progressTxt = progressText.getText(percentDone);
		if(!quiet)
		{
			mainInterface->StatusText(statusText, progressTxt, percentDone);
		}
	}

	for (int i = 0; i < data->numberOfThreads; i++)
	{
		while(thread[i]->isRunning())
		{
			mainInterface->application->processEvents();
		};
		WriteLog(QString("Thread ") + QString::number(i) + " finished");
		delete thread[i];
	}

	//status bar and progress bar
	double percentDone = 1.0;
	statusText = "Idle";
	progressTxt = progressText.getText(percentDone);
	if(!quiet)
	{
		mainInterface->StatusText(statusText, progressTxt, percentDone);
	}
	delete[] thread;
	delete[] threadData;
	delete[] worker;
	if(list) delete[] lists;

	WriteLog("cRenderSSAO::RenderSSAO(): memory released");

	WriteLog("Rendering SSAO finished");
}
