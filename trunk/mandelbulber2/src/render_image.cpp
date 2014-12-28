/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cRenderer class - calculates image using multiple CPU cores and does post processing
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "render_image.hpp"

#include <QtCore>

#include "dof.hpp"
#include "system.hpp"
#include "render_worker.hpp"
#include "progress_text.hpp"
#include "error_message.hpp"
#include "render_ssao.h"
#include "global_data.hpp"

cRenderer::cRenderer(const cParamRender *_params, const cFourFractals *_fractal, sRenderData *_renderData, cImage *_image)
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

	int progressiveSteps;
	if(data->doNotRefresh && data->maxRenderTime > 1e10)
			progressiveSteps = 0;
	else
		progressiveSteps = (int)(log((double)max(image->GetWidth(), image->GetHeight())) / log(2.0))-3;

	if(progressiveSteps < 0) progressiveSteps = 0;
	int progressive = pow(2.0, (double)progressiveSteps - 1);
	if (progressive == 0) progressive = 1;

	//prepare multiple threads
	QThread **thread = new QThread*[data->numberOfThreads];
	cRenderWorker::sThreadData *threadData = new cRenderWorker::sThreadData[data->numberOfThreads];
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

	WriteLog("Start rendering");
	do
	{
		WriteLogDouble("Progressive loop", scheduler->GetProgresiveStep());
		for (int i = 0; i < data->numberOfThreads; i++)
		{
			WriteLog(QString("Thread ") + QString::number(i) + " create");
			thread[i] = new QThread;
			worker[i] = new cRenderWorker(params, fractal, &threadData[i], data, image); //Warning! not needed to delete object
			worker[i]->moveToThread(thread[i]);
			QObject::connect(thread[i], SIGNAL(started()), worker[i], SLOT(doWork()));
			QObject::connect(worker[i], SIGNAL(finished()), thread[i], SLOT(quit()));
			QObject::connect(worker[i], SIGNAL(finished()), worker[i], SLOT(deleteLater()));

			thread[i]->start();
			WriteLog(QString("Thread ") + QString::number(i) + " started");
		}

		while (!scheduler->AllLinesDone())
		{
			application->processEvents();

			if (*data->stopRequest || progressText.getTime() > data->maxRenderTime)
			{
				scheduler->Stop();
			}

			Wait(100); //wait 100ms

			if(!data->doNotRefresh)
			{
				//get list of last rendered lines
				QList<int> list = scheduler->GetLastRenderedLines();
				//create list of lines for image refresh
				listToRefresh += list;
			}

			//status bar and progress bar
			double percentDone = scheduler->PercentDone();
			data->lastPercentage = percentDone;
			statusText = QObject::tr("Rendering image in progress");
			progressTxt = progressText.getText(percentDone);
			ProgressStatusText(statusText, progressTxt, percentDone, data->statusBar, data->progressBar);

			//refresh image
			if (!data->doNotRefresh && image->IsPreview() && listToRefresh.size() > 0)
			{
				if (timerRefresh.elapsed() > lastRefreshTime && scheduler->GetProgresivePass() > 1)
				{
					timerRefresh.restart();
					QSet<int> set_listTorefresh = listToRefresh.toSet(); //removing duplicates
					listToRefresh = set_listTorefresh.toList();
					qSort(listToRefresh);

					image->CompileImage(&listToRefresh);

					if (params->ambientOcclusionEnabled && params->ambientOcclusionMode == params::AOmodeScreenSpace)
					{
						cRenderSSAO rendererSSAO(params, data, image);
						rendererSSAO.setProgressive(scheduler->GetProgresiveStep());
						rendererSSAO.RenderSSAO(&listToRefresh);
					}

					image->ConvertTo8bit();
					image->UpdatePreview(&listToRefresh);
					image->GetImageWidget()->update();

					lastRefreshTime = timerRefresh.elapsed() * 1000 / (listToRefresh.size());
					timerRefresh.restart();
					listToRefresh.clear();
				} //timerRefresh
			} //isPreview
		}//while scheduler
		for (int i = 0; i < data->numberOfThreads; i++)
		{
			while(thread[i]->isRunning())
			{
				application->processEvents();
			};
			WriteLog(QString("Thread ") + QString::number(i) + " finished");
			delete thread[i];
		}
	}
	while(scheduler->ProgresiveNextStep());

	//refresh image at end
	WriteLog("image->CompileImage()");
	image->CompileImage();

	if(params->ambientOcclusionEnabled && params->ambientOcclusionMode == params::AOmodeScreenSpace)
	{
		cRenderSSAO rendererSSAO(params, data, image);
		rendererSSAO.RenderSSAO();
	}
	if(params->DOFEnabled && !*data->stopRequest)
	{
		PostRendering_DOF(image, params->DOFRadius * (image->GetWidth() + image->GetHeight()) / 2000.0, params->DOFFocus, data->statusBar, data->progressBar, data->stopRequest);
	}

	if(image->IsPreview())
	{
		WriteLog("image->ConvertTo8bit()");
		image->ConvertTo8bit();
		WriteLog("image->UpdatePreview()");
		image->UpdatePreview();
		WriteLog("image->GetImageWidget()->update()");
		image->GetImageWidget()->update();
	}

	WriteLog("Rendering finished");

	//status bar and progress bar
	double percentDone = 1.0;
	statusText = QObject::tr("Idle");
	progressTxt = progressText.getText(percentDone);
	ProgressStatusText(statusText, progressTxt, percentDone, data->statusBar, data->progressBar);

	delete[] thread;
	delete[] threadData;
	delete[] worker;
	delete scheduler;

	WriteLog("cRenderer::RenderImage(): memory released");

	return true;
}
