/*
 * ssao_worker.h
 *
 *  Created on: Aug 2, 2014
 *      Author: krzysztof
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
