/*
 * render_worker.cpp
 *
 *  Created on: May 4, 2014
 *      Author: krzysztof
 */


#include "render_worker.hpp"
#include <QtCore>
#include "calculate_distance.hpp"

cRenderWorker::cRenderWorker(const cParamRender *_params, const cFourFractals *_fractal, sThreadData *_threadData, const sRenderData *_data, cImage *_image)
{
	params = _params;
	fractal = _fractal;
	data = _data;
	image = _image;
	threadData = _threadData;
}

void cRenderWorker::doWork(void)
{
	// here will be rendering thread
	QTextStream out(stdout);
	out << "Thread start confirmed - id: " << threadData->id << endl;

	int width = image->GetWidth();
	int height = image->GetHeight();

	cScheduler *scheduler = threadData->scheduler;

	scheduler->InitFirstLine(threadData->id, threadData->startLine);
	for (int ys = threadData->startLine; scheduler->ThereIsStillSomethingToDo(threadData->id); ys = scheduler->NextLine(threadData->id, ys))
	{
		out << "Thread id: " << threadData->id << ", line number: " << ys << endl;
		for (int xs = 0; xs < width; xs++)
		{
			if(scheduler->ShouldIBreak(threadData->id, ys))
			{
				break;
			}
			double x = (((double)xs / width) - 0.5) * 2.0;
			double y = (((double)ys / height) - 0.5) * 2.0;

			CVector3 point(x, y, 0.0);
			sDistanceIn in(point, 0.01);
			sDistanceOut out;

			double dist = CalculateDistance(*params, *fractal, in, &out);

			dist = dist * 500.0;

			sRGBfloat color = sRGBfloat(dist, dist, dist);

			image->PutPixelImage(xs, ys, color);
		}
	}

	out << endl;

	emit finished();

	out << "Finished: " << threadData->id << endl;;

	return;
}

