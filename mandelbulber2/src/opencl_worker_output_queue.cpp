/*
 * opencl_worker_output_queue.cpp
 *
 *  Created on: 28 gru 2018
 *      Author: krzysztof
 */

#include "opencl_worker_output_queue.h"

cOpenCLWorkerOutputQueue::cOpenCLWorkerOutputQueue()
{
	// nothing to initialize
}

void cOpenCLWorkerOutputQueue::AddToQueue(const sClSingleOutput *data)
{
	queue.enqueue(*data);
	qDebug() << "Queue size after add" << queue.size();
}

cOpenCLWorkerOutputQueue::sClSingleOutput cOpenCLWorkerOutputQueue::GetFromQueue()
{
	if (!queue.isEmpty())
	{
		return queue.dequeue();
	}
	else
	{
		qCritical() << "cOpenCLWorkerOutputQueue::GetFromQueue(): queue is empty!";
		return sClSingleOutput();
	}
}
