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
	cScheduler(int _numberOfLines, int progressive);
	~cScheduler();
	int NextLine(int threadId, int actualLine);
	bool ShouldIBreak(int threadId, int actualLine);
	bool ThereIsStillSomethingToDo(int ThreadId);
	bool AllLinesDone();
	void InitFirstLine(int threadId, int firstLine);
	QList<int> GetLastRenderedLines(void);
	double PercentDone();
	void Stop() {stopRequest = true;}

	int GetProgresiveStep() {return progressiveStep;}
	int GetProgresivePass() {return progressivePass;}
	bool ProgresiveNextStep();

private:
	void Reset(void);

	int *linePendingThreadId;
	bool *lineDone;
	bool *lastLinesDone;
	int numberOfLines;
	volatile bool stopRequest;
	int progressiveStep;
	int progressivePass;
	bool progressiveEnabled;
};



#endif /* SCHEDULER_HPP_ */
