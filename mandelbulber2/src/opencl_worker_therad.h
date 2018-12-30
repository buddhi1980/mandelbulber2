/*
 * opencl_worker_therad.h
 *
 *  Created on: 29 gru 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPENCL_WORKER_THERAD_H_
#define MANDELBULBER2_SRC_OPENCL_WORKER_THERAD_H_
#include "opencl_input_output_buffer.h"

#ifdef USE_OPENCL

#include <QObject>

#include "error_message.hpp"
#include "include_header_wrapper.hpp"

class cOpenCLWorkerOutputQueue;
class cOpenClScheduler;
class cOpenClEngine;

class cOpenClWorkerThread : public QObject
{
	Q_OBJECT

public:
	cOpenClWorkerThread(
		cOpenClEngine *engine, const QSharedPointer<cOpenClScheduler> scheduler, int deviceIndex);
	~cOpenClWorkerThread();
	void ProcessRenderingLoop();
	bool ProcessClQueue(size_t jobX, size_t jobY, size_t pixelsLeftX, size_t pixelsLeftY);

	void setImageHeight(qint64 imageHeight) { this->imageHeight = imageHeight; }
	void setImageWidth(qint64 imageWidth) { this->imageWidth = imageWidth; }
	void setOptimalStepX(qint64 optimalStepX) { this->optimalStepX = optimalStepX; }
	void setOptimalStepY(qint64 optimalStepY) { this->optimalStepY = optimalStepY; }

private:
	static bool checkErr(cl_int err, QString functionName);

	QSharedPointer<cl::Kernel> clKernel;
	QSharedPointer<cl::CommandQueue> clQueue;

	QSharedPointer<cOpenCLWorkerOutputQueue> outputQueue;
	QSharedPointer<cOpenClScheduler> scheduler;
	QList<sClInputOutputBuffer> outputBuffers;
	QList<sClInputOutputBuffer> inputAndOutputBuffers;

	const int outputIndex = 0;

	cOpenClEngine *engine;

	qint64 optimalStepX;
	qint64 optimalStepY;
	qint64 imageWidth;
	qint64 imageHeight;

	const int deviceIndex;

	bool repeatMCLoop;

signals:
	void showErrorMessage(QString, cErrorMessage::enumMessageType, QWidget *);
};

#endif // USE_OPENCL

#endif /* MANDELBULBER2_SRC_OPENCL_WORKER_THERAD_H_ */
