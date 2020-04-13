/*
 * wait.cpp
 *
 *  Created on: 13 kwi 2020
 *      Author: krzysztof
 */

#include <QMutex>
#include <QWaitCondition>

void Wait(long int time)
{
	QMutex dummy;
	dummy.lock();
	QWaitCondition waitCondition;
	waitCondition.wait(&dummy, time);
	dummy.unlock();
}
