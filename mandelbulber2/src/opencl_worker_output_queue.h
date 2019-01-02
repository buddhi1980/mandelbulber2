/*
 * opencl_worker_output_queue.h
 *
 *  Created on: 28 gru 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPENCL_WORKER_OUTPUT_QUEUE_H_
#define MANDELBULBER2_SRC_OPENCL_WORKER_OUTPUT_QUEUE_H_

#include <QtCore>

#ifdef USE_OPENCL
class cOpenCLWorkerOutputQueue
{
public:
	struct sClDataBuffer
	{
		sClDataBuffer(qint64 itemSize, qint64 length) : itemSize(itemSize), length(length)
		{
			data.reserve(itemSize * length);
		}

		qint64 size() const { return itemSize * length; }
		qint64 itemSize;
		qint64 length;
		std::vector<char> data;
	};

	struct sClSingleOutput
	{
		qint64 jobX;
		qint64 jobY;
		qint64 jobWidth;
		qint64 jobHeight;
		int gridX;
		int gridY;
		int tileIndex;
		int monteCarloLoop;
		QList<sClDataBuffer> outputBuffers;
	};

	cOpenCLWorkerOutputQueue();
	~cOpenCLWorkerOutputQueue();
	void AddToQueue(const sClSingleOutput *data);
	sClSingleOutput GetFromQueue();
	bool isEmpty();

private:
	QQueue<sClSingleOutput> queue;
	QMutex lock;
};
#endif // USE_OPENCL

#endif /* MANDELBULBER2_SRC_OPENCL_WORKER_OUTPUT_QUEUE_H_ */
