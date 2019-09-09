/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * cRenderSSAO class - renders Screen Space Ambient Occlusion effect
 */

#include "render_ssao.h"

#include "cimage.hpp"
#include "fractparams.hpp"
#include "global_data.hpp"
#include "progress_text.hpp"
#include "render_data.hpp"
#include "ssao_worker.h"
#include "system.hpp"

cRenderSSAO::cRenderSSAO(
	const sParamRender *_params, const sRenderData *_renderData, cImage *_image)
		: QObject()
{
	params = _params;
	data = _renderData;
	image = _image;
	qualityFactor = 1.0;
	progressive = 0;
	startLine = data->screenRegion.y1;
	endLine = data->screenRegion.y2;
	height = data->screenRegion.height;
	numberOfThreads = min(data->configuration.GetNumberOfThreads(), height);
	region = data->screenRegion;
}

cRenderSSAO::~cRenderSSAO()
{
	// nothing to destroy
}

void cRenderSSAO::SetRegion(const cRegion<int> &_region)
{
	region = _region;
	startLine = region.y1;
	endLine = region.y2;
	height = region.height;
	numberOfThreads = min(data->configuration.GetNumberOfThreads(), height);
}

void cRenderSSAO::RenderSSAO(QList<int> *list)
{
	WriteLog("cRenderSSAO::RenderSSAO()", 2);
	// prepare multiple threads
	QThread **thread = new QThread *[numberOfThreads];
	cSSAOWorker::sThreadData *threadData = new cSSAOWorker::sThreadData[numberOfThreads];
	cSSAOWorker **worker = new cSSAOWorker *[numberOfThreads];

	cProgressText progressText;
	progressText.ResetTimer();

	// create list of lines to render for each CPU core
	QList<int> *lists = nullptr;
	if (list)
	{
		lists = new QList<int>[numberOfThreads];
		for (int y : *list)
		{
			int mod = (y - startLine) % numberOfThreads;
			lists[mod].append(y);
		}
	}

	double qualityFactorCalculated;
	if (progressive > 0)
	{
		qualityFactorCalculated = sqrt(1.0 / (progressive * 2.0));
	}
	else
	{
		qualityFactorCalculated = 1.0;
	}
	int quality = int(
		params->ambientOcclusionQuality * params->ambientOcclusionQuality * qualityFactorCalculated);
	if (quality < 3) quality = 3;

	for (int i = 0; i < numberOfThreads; i++)
	{
		threadData[i].startLine = startLine + i;
		threadData[i].noOfThreads = numberOfThreads;
		threadData[i].quality = quality;
		threadData[i].color = params->ambientOcclusionColor;
		threadData[i].done = 0;
		threadData[i].progressive = progressive;
		threadData[i].stopRequest = false;
		threadData[i].region = region;

		if (list)
			threadData[i].list = &lists[i];
		else
			threadData[i].list = nullptr;
	}

	QString statusText;
	QString progressTxt;

	WriteLog("Start rendering SSAO", 2);

	for (int i = 0; i < numberOfThreads; i++)
	{
		WriteLog(QString("Thread ") + QString::number(i) + " create", 3);
		thread[i] = new QThread;
		worker[i] =
			new cSSAOWorker(params, &threadData[i], data, image); // Warning! not needed to delete object
		worker[i]->moveToThread(thread[i]);
		QObject::connect(thread[i], SIGNAL(started()), worker[i], SLOT(doWork()));
		QObject::connect(worker[i], SIGNAL(finished()), thread[i], SLOT(quit()));
		QObject::connect(worker[i], SIGNAL(finished()), worker[i], SLOT(deleteLater()));
		thread[i]->setObjectName("SSAOWorker #" + QString::number(i));
		thread[i]->start();
		thread[i]->setPriority(GetQThreadPriority(systemData.threadsPriority));
		WriteLog(QString("Thread ") + QString::number(i) + " started", 3);
	}

	int totalDone = 0;
	int toDo;
	if (list)
	{
		toDo = list->size();
	}
	else
	{
		toDo = height;
	}

	while (totalDone < toDo)
	{
		gApplication->processEvents();

		Wait(10);

		if (*data->stopRequest || systemData.globalStopRequest)
		{
			for (int i = 0; i < numberOfThreads; i++)
				threadData[i].stopRequest = true;
			if (!list)
			{
				//*data->stopRequest = false;
			}
			break;
		}

		totalDone = 0;
		for (int i = 0; i < numberOfThreads; i++)
		{
			totalDone += threadData[i].done;
		}

		double percentDone = double(totalDone) / toDo;
		statusText = QObject::tr("Rendering SSAO effect in progress");
		progressTxt = progressText.getText(percentDone);

		emit updateProgressAndStatus(statusText, progressTxt, percentDone);
	}

	for (int i = 0; i < numberOfThreads; i++)
	{
		while (thread[i]->isRunning())
		{
			gApplication->processEvents();
		};
		WriteLog(QString("Thread ") + QString::number(i) + " finished", 3);
		delete thread[i];
	}

	// status bar and progress bar
	double percentDone = 1.0;
	statusText = QObject::tr("Idle");
	progressTxt = progressText.getText(percentDone);

	emit updateProgressAndStatus(statusText, progressTxt, percentDone);

	delete[] thread;
	delete[] threadData;
	delete[] worker;
	if (list) delete[] lists;

	WriteLog("cRenderSSAO::RenderSSAO(): memory released", 2);

	WriteLog("Rendering SSAO finished", 2);
}
