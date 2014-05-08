/*
 * render_worker.hpp
 *
 *  Created on: May 4, 2014
 *      Author: krzysztof
 */

#ifndef RENDER_WORKER_HPP_
#define RENDER_WORKER_HPP_

#include <QThread>

#include "cimage.hpp"
#include "fractparams.hpp"
#include "fractal.h"
#include "render_image.hpp"
#include "scheduler.hpp"

struct sThreadData
{
	int id;
	int startLine;
	cScheduler *scheduler;
};

class cRenderWorker: public QObject
{
Q_OBJECT

public:
cRenderWorker(const cParamRender *_params, const cFourFractals *_fractal, sThreadData *_threadData, const sRenderData *_data, cImage *_image);

QThread workerThread;

const cParamRender *params;
const cFourFractals *fractal;
const sRenderData *data;
sThreadData *threadData;
cImage *image;

public slots:
	void doWork(void);

signals:
	void finished();
	void error(const QString &result);
};



#endif /* RENDER_WORKER_HPP_ */
