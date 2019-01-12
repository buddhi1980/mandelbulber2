/*
 * opencl_worker_thread.cpp
 *
 *  Created on: 29 gru 2018
 *      Author: krzysztof
 */

#ifdef USE_OPENCL

#include "opencl_worker_output_queue.h"
#include "system.hpp"

#include <algorithm>
#include <QtCore>

#include "opencl_engine.h"
#include "opencl_worker_thread.h"
#include "opencl_scheduler.h"

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
	// TODO Auto-generated destructor stub
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
	if (startTile >= scheduler->getTileSequence()->length()) return;
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

			int gridX = scheduler->getTileSequence()->at(tile).x();
			int gridY = scheduler->getTileSequence()->at(tile).y();
			qint64 jobX = gridX * optimalStepX;
			qint64 jobY = gridY * optimalStepY;
			qint64 pixelsLeftX = imageWidth - jobX;
			qint64 pixelsLeftY = imageHeight - jobY;
			qint64 jobWidth = min(optimalStepX, pixelsLeftX);
			qint64 jobHeight = min(optimalStepY, pixelsLeftY);

			if (jobX >= 0 && jobX < imageWidth && jobY >= 0 && jobY < imageHeight)
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

				qint64 outputItemSize = outputBuffers.at(outputIndex).itemSize;
				qint64 outputItemlength = outputBuffers.at(outputIndex).length;
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

			if (*stopRequest)
			{
				finishedWithSuccess = false;
				break;
			}

			// slow down to reduce length of queue
			int queueLength = outputQueue->getQueueLength();
			if (queueLength > 100)
			{
				Wait((queueLength - 100));
			}
		} // next tile

		if (*stopRequest) break;
	} // next monteCarloLoop

	finishedWithSuccess = true;
	emit finished();
}

bool cOpenClWorkerThread::ProcessClQueue(
	size_t jobX, size_t jobY, size_t pixelsLeftX, size_t pixelsLeftY)
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
