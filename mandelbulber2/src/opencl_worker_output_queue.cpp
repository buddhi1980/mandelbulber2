/*
 * opencl_worker_output_queue.cpp
 *
 *  Created on: 28 gru 2018
 *      Author: krzysztof
 */

#ifdef USE_OPENCL
#include "opencl_worker_output_queue.h"

cOpenCLWorkerOutputQueue::cOpenCLWorkerOutputQueue()
{
	// nothing to initialize
}

cOpenCLWorkerOutputQueue::~cOpenCLWorkerOutputQueue()
{
	// nothing to delete
}

void cOpenCLWorkerOutputQueue::AddToQueue(const sClSingleOutput *data)
{
	lock.lock();
	queue.enqueue(*data);
	qDebug() << "Queue size after add" << queue.size();
	lock.unlock();
}

cOpenCLWorkerOutputQueue::sClSingleOutput cOpenCLWorkerOutputQueue::GetFromQueue()
{
	lock.lock();
	if (!queue.isEmpty())
	{
		return queue.dequeue();
	}
	else
	{
		qCritical() << "cOpenCLWorkerOutputQueue::GetFromQueue(): queue is empty!";
		return sClSingleOutput();
	}
	lock.unlock();
}

#endif // USE_OPENCL
