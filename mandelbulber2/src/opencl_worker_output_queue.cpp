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
		sClSingleOutput output = queue.dequeue();
		lock.unlock();
		return output;
	}
	else
	{
		qCritical() << "cOpenCLWorkerOutputQueue::GetFromQueue(): queue is empty!";
		lock.unlock();
		return sClSingleOutput();
	}
}

bool cOpenCLWorkerOutputQueue::isEmpty()
{
	lock.lock();
	bool isempty = queue.isEmpty();
	lock.unlock();

	return isempty;
}

#endif // USE_OPENCL
