/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * thread for one of GPUs in multi-gpu rendering
 */

#ifdef USE_OPENCL

#include "opencl_worker_thread.h"

#include <algorithm>

#include <QtCore>

#include "opencl_engine.h"
#include "opencl_scheduler.h"
#include "opencl_worker_output_queue.h"
#include "system.hpp"

using std::min;

cOpenClWorkerThread::cOpenClWorkerThread(
	cOpenClEngine *engine, const QSharedPointer<cOpenClScheduler> scheduler, int _deviceIndex)
		: QObject(), deviceIndex(_deviceIndex)
{
	this->scheduler = scheduler;
	optimalStepX = 0;
	optimalStepY = 0;
	this->engine = engine;
	imageWidth = 0;
	imageHeight = 0;
	maxMonteCarloSamples = 1;
	stopRequest = nullptr;
	reservedGpuTime = 0.0;
	finishedWithSuccess = false;
}

cOpenClWorkerThread::~cOpenClWorkerThread()
{
	// nothing to destroy
}

bool cOpenClWorkerThread::checkErr(cl_int err, QString functionName)
{
	if (err != CL_SUCCESS)
	{
		qCritical() << "OpenCl ERROR: " << functionName << " (" << err << ")";
		return false;
	}
	else
		return true;
}

void cOpenClWorkerThread::ProcessRenderingLoop()
{
	int startTile = deviceIndex;
	if (startTile >= scheduler->getTileSequence()->length())
	{
		finishedWithSuccess = true;
		emit finished();
		return;
	}

	scheduler->ReserveTile(startTile);

	QElapsedTimer openclProcessingTime;
	qint64 openclprocessingTimeNanoSeconds = 0;

	for (int monteCarloLoop = 1; monteCarloLoop <= maxMonteCarloSamples; monteCarloLoop++)
	{
		for (int tile = startTile; !scheduler->AllDone(monteCarloLoop);
				 tile = scheduler->GetNextTileToRender(tile, monteCarloLoop))
		{
			if (tile < 0) break;
			if (!scheduler->IsTileEnabled(tile)) continue;

			// refresh parameters (needed to update random seed)
			engine->AssignParametersToKernel(deviceIndex);

			quint64 gridX = scheduler->getTileSequence()->at(tile).x();
			quint64 gridY = scheduler->getTileSequence()->at(tile).y();
			quint64 jobX = gridX * optimalStepX;
			quint64 jobY = gridY * optimalStepY;
			quint64 pixelsLeftX = imageWidth - jobX;
			quint64 pixelsLeftY = imageHeight - jobY;
			quint64 jobWidth = min(optimalStepX, pixelsLeftX);
			quint64 jobHeight = min(optimalStepY, pixelsLeftY);

			if (jobX < imageWidth && jobY < imageHeight)
			{
				openclProcessingTime.restart();
				int result = ProcessClQueue(jobX, jobY, pixelsLeftX, pixelsLeftY);
				if (!result)
				{
					emit finished();
					finishedWithSuccess = false;
					return;
				}

				result = engine->ReadBuffersFromQueue(deviceIndex);
				if (!result)
				{
					emit finished();
					finishedWithSuccess = false;
					return;
				}

				openclprocessingTimeNanoSeconds = openclProcessingTime.nsecsElapsed();

				quint64 outputItemSize = outputBuffers.at(outputIndex).itemSize;
				quint64 outputItemlength = outputBuffers.at(outputIndex).length;
				cOpenCLWorkerOutputQueue::sClDataBuffer dataBuffer(outputItemSize, outputItemlength);

				char *startPtr = outputBuffers.at(outputIndex).ptr.data();
				char *endPtr = startPtr + outputBuffers.at(outputIndex).size();
				dataBuffer.data.assign(startPtr, endPtr);

				cOpenCLWorkerOutputQueue::sClSingleOutput outputDataForQueue;
				outputDataForQueue.jobX = jobX;
				outputDataForQueue.jobY = jobY;
				outputDataForQueue.gridX = gridX;
				outputDataForQueue.gridY = gridY;
				outputDataForQueue.tileIndex = tile;
				outputDataForQueue.jobWidth = jobWidth;
				outputDataForQueue.jobHeight = jobHeight;
				outputDataForQueue.monteCarloLoop = monteCarloLoop;
				outputDataForQueue.outputBuffers.append(dataBuffer);

				outputQueue->AddToQueue(&outputDataForQueue);

				// reserve GPU time for the system
				if (reservedGpuTime > 0.0)
				{
					unsigned long int waitTime =
						reservedGpuTime * openclprocessingTimeNanoSeconds / 1000.0 / 100.0;
					if (waitTime == 0) waitTime = 1;
					thread()->usleep(waitTime);
				}
			}

			if (*stopRequest || systemData.globalStopRequest)
			{
				emit finished();
				finishedWithSuccess = false;
				return;
			}

			// slow down to reduce length of queue
			int queueLength = outputQueue->getQueueLength();
			if (queueLength > 100)
			{
				Wait((queueLength - 100));
			}
		} // next tile

		if (*stopRequest || systemData.globalStopRequest)
		{
			emit finished();
			finishedWithSuccess = false;
			return;
		}
	} // next monteCarloLoop

	finishedWithSuccess = true;
	emit finished();
}

bool cOpenClWorkerThread::ProcessClQueue(
	quint64 jobX, quint64 jobY, quint64 pixelsLeftX, quint64 pixelsLeftY)
{
	size_t stepSizeX = optimalStepX;
	if (pixelsLeftX < stepSizeX) stepSizeX = pixelsLeftX;
	size_t stepSizeY = optimalStepY;
	if (pixelsLeftY < stepSizeY) stepSizeY = pixelsLeftY;

	cl_int err = clQueue->enqueueNDRangeKernel(
		*clKernel, cl::NDRange(jobX, jobY), cl::NDRange(stepSizeX, stepSizeY), cl::NullRange);
	if (!checkErr(err, "CommandQueue::enqueueNDRangeKernel()"))
	{
		emit showErrorMessage(
			QObject::tr("Cannot enqueue OpenCL rendering jobs"), cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

#endif // USE_OPENCL
