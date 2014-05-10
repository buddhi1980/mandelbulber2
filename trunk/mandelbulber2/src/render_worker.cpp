/*
 * render_worker.cpp
 *
 *  Created on: May 4, 2014
 *      Author: krzysztof
 */


#include "render_worker.hpp"
#include <QtCore>
#include "calculate_distance.hpp"
#include "region.hpp"

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
	double aspectRatio = (double)height / width;

	cScheduler *scheduler = threadData->scheduler;

	scheduler->InitFirstLine(threadData->id, threadData->startLine);
	for (int ys = threadData->startLine; scheduler->ThereIsStillSomethingToDo(threadData->id); ys = scheduler->NextLine(threadData->id, ys))
	{
		out << "Thread id: " << threadData->id << ", line number: " << ys << endl;
		if(ys < data->screenRegion.y1 || ys > data->screenRegion.y2) continue;

		for (int xs = 0; xs < width; xs++)
		{
			if(scheduler->ShouldIBreak(threadData->id, ys))
			{
				break;
			}

			if(xs < data->screenRegion.x1 || xs > data->screenRegion.x2) continue;

			CVector2<int> screenPoint(xs, ys);
			CVector2<double> imagePoint = data->screenRegion.transpose(data->imageRegion, screenPoint);

			imagePoint.y *= aspectRatio;

			CVector3 point(imagePoint.x, imagePoint.y, 0.0);
			sDistanceIn in(point, 0.01);
			sDistanceOut out;

			double dist = CalculateDistance(*params, *fractal, in, &out);

			dist = dist * width;

			sRGBfloat color = sRGBfloat(dist, dist, dist);

			image->PutPixelImage(screenPoint.x, screenPoint.y, color);
		}
	}

	out << endl;

	emit finished();

	out << "Finished: " << threadData->id << endl;;

	return;
}

