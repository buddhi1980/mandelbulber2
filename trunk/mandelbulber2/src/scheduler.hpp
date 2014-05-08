/*
 * scheduler.hpp
 *
 *  Created on: May 7, 2014
 *      Author: krzysztof
 */

#ifndef SCHEDULER_HPP_
#define SCHEDULER_HPP_

#include <qvector.h>

class cScheduler
{
public:
	cScheduler(int _numberOfLines);
	~cScheduler();
	int NextLine(int threadId, int actualLine);
	bool ShouldIBreak(int threadId, int actualLine);
	bool ThereIsStillSomethingToDo(int ThreadId);
	bool AllLinesDone();
	void InitFirstLine(int threadId, int firstLine);
	QVector<int> GetLastRenderedLines(void);

private:
	int *linePendingThreadId;
	bool *lineDone;
	bool *lastLinesDone;
	int numberOfLines;
};



#endif /* SCHEDULER_HPP_ */
