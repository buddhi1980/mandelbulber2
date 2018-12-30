/*
 * opencl_worker_thread.cpp
 *
 *  Created on: 29 gru 2018
 *      Author: krzysztof
 */

#ifdef USE_OPENCL

#include <algorithm>
#include <QtCore>

#include "opencl_engine.h"
#include "opencl_worker_therad.h"
#include "opencl_scheduler.h"

using std::min;

cOpenClWorkerThread::cOpenClWorkerThread(
	cOpenClEngine *engine, const QSharedPointer<cOpenClScheduler> scheduler, int _deviceIndex)
		: QObject(), deviceIndex(_deviceIndex)
{
	this->scheduler = scheduler;
	repeatMCLoop = false;
	optimalStepX = 0;
	optimalStepY = 0;
	this->engine = engine;
	imageWidth = 0;
	imageHeight = 0;
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

	while (repeatMCLoop)
	{
		for (int tile = startTile; !scheduler->AllDone(); scheduler->GetNextTileToRender(tile))
		{
			// refresh parameters (needed to update random seed)
			engine->AssignParametersToKernel(deviceIndex);

			int gridX = scheduler->getTileSequence()->at(tile).x();
			int gridY = scheduler->getTileSequence()->at(tile).y();
			qint64 jobX = gridX * optimalStepX;
			qint64 jobY = gridY * optimalStepY;

			qint64 pixelsLeftX = imageWidth - jobX;
			qint64 pixelsLeftY = imageHeight - jobY;
			qint64 jobWidth2 = min(optimalStepX, pixelsLeftX);
			qint64 jobHeight2 = min(optimalStepY, pixelsLeftY);

			int result = ProcessClQueue(jobX, jobY, pixelsLeftX, pixelsLeftY);
			// TODO: add error handling

			// TODO: add reading buffers and transfer to output queue
			// TODO: write code for scheduler;
		}
	}
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
