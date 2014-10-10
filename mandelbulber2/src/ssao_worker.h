/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cSSAOWorker class - worker for multithread SSAO effect
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

#ifndef SSAO_WORKER_H_
#define SSAO_WORKER_H_

#include <qobject.h>
#include "fractparams.hpp"
#include "render_data.hpp"
#include <QList>

class cSSAOWorker: public QObject
{
	Q_OBJECT
public:

	struct sThreadData
	{
		int startLine;
		int noOfThreads;
		int quality;
		int progressive;
		unsigned int done;
		bool stopRequest;
		QList<int> *list;
	};

	cSSAOWorker(const cParamRender *_params, sThreadData *_threadData, const sRenderData *_data, cImage *_image);
	~cSSAOWorker();

	QThread workerThread;

	//data got from main thread
	const cParamRender *params;
	const sRenderData *data;
	sThreadData *threadData;
	cImage *image;

	public slots:
		void doWork(void);

	signals:
		void finished();
};

#endif /* SSAO_WORKER_H_ */
