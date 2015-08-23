/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cScheduler class - class to schedule rendering job between CPU cores
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

#ifndef SCHEDULER_HPP_
#define SCHEDULER_HPP_

#include <qvector.h>

class cScheduler
{
public:
	cScheduler(int _numberOfLines, int progressive);
	~cScheduler();
	int NextLine(int threadId, int actualLine, bool lastLineWasBroken);
	bool ShouldIBreak(int threadId, int actualLine);
	bool ThereIsStillSomethingToDo(int ThreadId);
	bool AllLinesDone();
	void InitFirstLine(int threadId, int firstLine);
	QList<int> GetLastRenderedLines(void);
	double PercentDone();
	void Stop() {stopRequest = true;}
	void MarkReceivedLines(const QList<int> &lineNumbers);
	void UpdateDoneLines(const QList<int> &done);

	int GetProgressiveStep() {return progressiveStep;}
	int GetProgressivePass() {return progressivePass;}
	bool ProgressiveNextStep();
	QList<int> CreateDoneList();
	bool IsLineDoneByServer(int line);

private:
	void Reset(void);
	int FindBiggestGap();

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
