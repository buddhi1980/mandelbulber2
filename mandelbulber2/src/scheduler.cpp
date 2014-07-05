/*
 * scheduler.cpp
 *
 *  Created on: May 7, 2014
 *      Author: krzysztof
 */

#include "scheduler.hpp"

#include <string.h>
#include <QtCore>

cScheduler::cScheduler(int _numberOfLines, int progressive)
{
	numberOfLines = _numberOfLines;
	linePendingThreadId = new int[numberOfLines];
	lineDone = new bool[numberOfLines];
	lastLinesDone = new bool[numberOfLines];
	stopRequest = false;
	progressiveStep = progressive;
	progressivePass = 1;
	if(progressive > 1) progressiveEnabled = true;
	else progressiveEnabled = false;
	Reset();
}

cScheduler::~cScheduler()
{
	delete[] lineDone;
	delete[] linePendingThreadId;
	delete[] lastLinesDone;
}

void cScheduler::Reset()
{
	memset(linePendingThreadId, 0, sizeof(int) * numberOfLines);
	memset(lineDone, 0, sizeof(bool) * numberOfLines);
	memset(lastLinesDone, 0, sizeof(bool) * numberOfLines);
}

//coś wymyslić, żeby nie zostawała jedna linia na koniec - oznaczać linie w toku i zakończone oddzielnie
bool cScheduler::ThereIsStillSomethingToDo(int threadId)
{
	bool result = false;
	for(int i = 0; i<numberOfLines; i++)
	{
		if(lineDone[i] == false && linePendingThreadId[i] == threadId)
		{
			result = true;
			break;
		}
	}

	if(stopRequest) result = false;

	return result;
}

bool cScheduler::AllLinesDone(void)
{
	bool result = true;
	for(int i = 0; i<numberOfLines; i++)
	{
		if(lineDone[i] == false)
		{
			result = false;
			break;
		}
	}

	if(stopRequest) result = true;

	return result;
}

bool cScheduler::ShouldIBreak(int threadId, int actualLine)
{
	return threadId != linePendingThreadId[actualLine] || stopRequest;
}

int cScheduler::NextLine(int threadId, int actualLine)
{
	QTextStream out(stdout);

	int nextLine = -1;
	int firstFree = -1;
	int lastFree = -1;
	int maxHole = 0;
	bool firstFreeFound = false;

	for(int i=0; i<progressiveStep; i++)
	{
		if(actualLine + i < numberOfLines)
		{
			lineDone[actualLine + i] = true;
			lastLinesDone[actualLine + i] = true;
		}
	}

	//next line is not occupied by any thread
	if(actualLine < numberOfLines - progressiveStep && linePendingThreadId[actualLine + progressiveStep] == 0)
	{
		nextLine = actualLine + progressiveStep;
	}
	else
	//next line is occupied or it's last line. There is needed to find new optimal line for rendering
	{
		for(int i = 0; i < numberOfLines; i++)
		{
			if(!firstFreeFound && linePendingThreadId[i] == 0)
			{
				firstFreeFound = true;
				firstFree = i;
				continue;
			}

			if(firstFreeFound && (linePendingThreadId[i] > 0 || i == numberOfLines - 1))
			{
				lastFree = i;
				int holeSize = lastFree - firstFree;
				firstFreeFound = false;

				if(holeSize > maxHole)
				{
					maxHole = holeSize;
					//next line should be in the middle of the biggest gap
					nextLine = (lastFree + firstFree) / 2;
					nextLine /= progressiveStep;
					nextLine *= progressiveStep;
					//out << "Jump Id: " << threadId  << " first: " << firstFree << " last: " << lastFree << endl;
				}
			}
		}
	}
	if(nextLine >= 0)
	{
		linePendingThreadId[nextLine] = threadId;
	}
	return nextLine;
}

void cScheduler::InitFirstLine(int threadId, int firstLine)
{
	linePendingThreadId[firstLine] = threadId;
}

QList<int> cScheduler::GetLastRenderedLines(void)
{
	QList<int> list;
	for(int i = 0; i < numberOfLines; i++)
	{
		if(lastLinesDone[i])
		{
			list.append(i);
			lastLinesDone[i] = false;
		}
	}
	return list;
}

double cScheduler::PercentDone()
{
	int count = 0;
	for(int i = 0; i < numberOfLines; i++)
	{
		if(lineDone[i]) count++;
	}

	double progressiveDone, percent_done;
	if(progressivePass == 1) progressiveDone = 0;
	else progressiveDone = 0.25 / (progressiveStep * progressiveStep);

	if(progressiveEnabled)
	{
		percent_done = ((double)count / numberOfLines) * 0.75 / (progressiveStep * progressiveStep) + progressiveDone;
	}
	else
	{
		percent_done = (double)count / numberOfLines;
	}

	return percent_done;
}

bool cScheduler::ProgresiveNextStep()
{
	progressiveStep /= 2;
	progressivePass ++;

	QTextStream out(stdout);
	out << "step " << progressiveStep << endl;

	if(progressiveStep == 0)
	{
		return false;
	}
	else
	{
		memset(linePendingThreadId, 0, sizeof(int) * numberOfLines);
		memset(lineDone, 0, sizeof(bool) * numberOfLines);
		return true;
	}
}
