/*
 * scheduler.cpp
 *
 *  Created on: May 7, 2014
 *      Author: krzysztof
 */

#include "scheduler.hpp"

#include <string.h>
#include <QtCore>

cScheduler::cScheduler(int _numberOfLines)
{
	numberOfLines = _numberOfLines;
	linePendingThreadId = new int[numberOfLines];
	lineDone = new bool[numberOfLines];
	lastLinesDone = new bool[numberOfLines];

	memset(linePendingThreadId, 0, sizeof(int) * numberOfLines);
	memset(lineDone, 0, sizeof(bool) * numberOfLines);
	memset(lastLinesDone, 0, sizeof(bool) * numberOfLines);
}

cScheduler::~cScheduler()
{
	delete[] lineDone;
	delete[] linePendingThreadId;
	delete[] lastLinesDone;
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
	return result;
}

bool cScheduler::ShouldIBreak(int threadId, int actualLine)
{
	return threadId != linePendingThreadId[actualLine];
}

int cScheduler::NextLine(int threadId, int actualLine)
{
	QTextStream out(stdout);

	int nextLine = -1;
	int firstFree = -1;
	int lastFree = -1;
	int maxHole = 0;
	bool firstFreeFound = false;

	lineDone[actualLine] = true;
	lastLinesDone[actualLine] = true;

	//next line is not occupied by any thread
	if(actualLine < numberOfLines - 1 && linePendingThreadId[actualLine + 1] == 0)
	{
		nextLine = actualLine + 1;
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

QVector<int> cScheduler::GetLastRenderedLines(void)
{
	QVector<int> list;
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

	return (double)count / numberOfLines;
}
