/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * cScheduler class - class to schedule rendering job between CPU cores
 *
 * The image to render is divided into [height] horizontal lines of size [width] x 1.
 * Each line will be managed by the scheduler and given to the asking threads,
 * while the image renders.
 */

#include "scheduler.hpp"
#include <QtCore>

#include "system.hpp"
#define LINE_DONE_BY_SERVER 9999

cScheduler::cScheduler(cRegion<int> screenRegion, int progressive)
{
	startLine = screenRegion.y1;
	endLine = screenRegion.y2;
	numberOfLines = screenRegion.height;
	linePendingThreadId = new int[endLine];
	lineDone = new bool[endLine];
	lastLinesDone = new bool[endLine];
	stopRequest = false;
	progressiveStep = progressive;
	progressivePass = 1;
	progressiveEnabled = progressive > 1;
	Reset();
}

cScheduler::~cScheduler()
{
	delete[] lineDone;
	delete[] linePendingThreadId;
	delete[] lastLinesDone;
}

void cScheduler::Reset() const
{
	memset(linePendingThreadId, 0, sizeof(int) * endLine);
	memset(lineDone, 0, sizeof(bool) * endLine);
	memset(lastLinesDone, 0, sizeof(bool) * endLine);
}

bool cScheduler::ThereIsStillSomethingToDo(int threadId) const
{
	bool result = false;
	for (int i = startLine; i < endLine; i++)
	{
		// qDebug() << "ThereIsStillSomethingToDo:" << i << lineDone[i];
		if (!lineDone[i] && (linePendingThreadId[i] == threadId || linePendingThreadId[i] == 0))
		{
			result = true;
			break;
		}
	}

	if (stopRequest) result = false;
	if (systemData.globalStopRequest) result = false;

	return result;
}

bool cScheduler::AllLinesDone() const
{
	bool result = true;
	for (int i = startLine; i < endLine; i++)
	{
		// qDebug() << "AllLinesDone:" << i << lineDone[i];
		if (!lineDone[i])
		{
			result = false;
			break;
		}
	}

	if (stopRequest) result = true;

	return result;
}

bool cScheduler::ShouldIBreak(int threadId, int actualLine) const
{
	if (actualLine >= 0)
	{
		return threadId != linePendingThreadId[actualLine] || stopRequest;
	}
	else
	{
		qCritical()
			<< "cScheduler::ShouldIBreak(int threadId, int actualLine): actualLine lower than zero";
		return true;
	}
}

int cScheduler::NextLine(int threadId, int actualLine, bool lastLineWasBroken)
{
	mutex.lock();
	// qDebug() << "threadID:" << threadId << " Actual line:" << actualLine;

	int nextLine;

	if (!lastLineWasBroken)
	{
		for (int i = 0; i < progressiveStep; i++)
		{
			if (actualLine + i < endLine)
			{
				lineDone[actualLine + i] = true;
				lastLinesDone[actualLine + i] = true;
			}
		}
	}
	else
	{
		// qDebug() << "threadID:" << threadId << " lastLineWasBroken, line:" << actualLine;
	}

	// next line is not occupied by any thread
	if (actualLine < endLine - progressiveStep
			&& linePendingThreadId[actualLine + progressiveStep] == 0 && !lastLineWasBroken)
	{
		nextLine = actualLine + progressiveStep;
		// qDebug() << "threadID:" << threadId << " one after:" << nextLine;
	}
	else
	// next line is occupied or it's last line. There is needed to find new optimal line for rendering
	{
		nextLine = FindBiggestGap();
		// qDebug() << "threadID:" << threadId << " gap:" << nextLine;
	}

	if (nextLine >= 0)
	{
		if (linePendingThreadId[nextLine] == 0)
		{
			for (int i = 0; i < progressiveStep; i++)
			{
				if (nextLine + i < endLine)
				{
					linePendingThreadId[nextLine + i] = threadId;
				}
			}
		}
	}
	mutex.unlock();

	if (nextLine < 0)
	{
		// qCritical() << "cScheduler::NextLine(int threadId, int actualLine): not possible to find new
		// line";
		return -1;
	}

	// qDebug() << "threadID:" << threadId << " Next line:" << nextLine;
	return nextLine;
}

int cScheduler::FindBiggestGap() const
{
	bool firstFreeFound = false;
	int firstFree = -1;
	int lastFree;
	int maxHole = 0;
	int theBest = -1;

	for (int i = startLine; i < endLine; i++)
	{
		if (!firstFreeFound && linePendingThreadId[i] == 0)
		{
			firstFreeFound = true;
			firstFree = i;
			if (i == endLine - 1)
			{
				theBest = i;
				theBest /= progressiveStep;
				theBest *= progressiveStep;
				break;
			}
			continue;
		}

		if (firstFreeFound && (linePendingThreadId[i] > 0 || i == endLine - 1))
		{
			lastFree = i;
			int holeSize = lastFree - firstFree;
			firstFreeFound = false;

			if (holeSize > maxHole)
			{
				maxHole = holeSize;
				// next line should be in the middle of the biggest gap
				theBest = (lastFree + firstFree) / 2;
				theBest /= progressiveStep;
				theBest *= progressiveStep;
				// out << "Jump Id: " << threadId  << " first: " << firstFree << " last: " << lastFree <<
				// endl;
			}
		}
	}
	return theBest;
}

void cScheduler::InitFirstLine(int threadId, int firstLine) const
{
	linePendingThreadId[firstLine] = threadId;
}

QList<int> cScheduler::GetLastRenderedLines() const
{
	QList<int> list;
	for (int i = startLine; i < endLine; i++)
	{
		if (lastLinesDone[i])
		{
			list.append(i);
			lastLinesDone[i] = false;
		}
	}
	return list;
}

double cScheduler::PercentDone() const
{
	int count = 0;
	for (int i = startLine; i < endLine; i++)
	{
		if (lineDone[i]) count++;
	}

	double progressiveDone, percent_done;
	if (progressivePass == 1)
		progressiveDone = 0;
	else
		progressiveDone = 0.25 / (progressiveStep * progressiveStep);

	if (progressiveEnabled)
	{
		percent_done =
			double(count) / numberOfLines * 0.75 / (progressiveStep * progressiveStep) + progressiveDone;
	}
	else
	{
		percent_done = double(count) / numberOfLines;
	}

	return percent_done;
}

bool cScheduler::ProgressiveNextStep()
{
	progressiveStep /= 2;
	progressivePass++;

	if (progressiveStep == 0)
	{
		return false;
	}
	else
	{
		memset(linePendingThreadId, 0, sizeof(int) * endLine);
		memset(lineDone, 0, sizeof(bool) * endLine);
		return true;
	}
}

void cScheduler::MarkReceivedLines(const QList<int> &lineNumbers) const
{
	for (int i = 0; i < lineNumbers.size(); i++)
	{
		int line = lineNumbers.at(i);
		lineDone[line] = true;
		lastLinesDone[line] = true;
		linePendingThreadId[line] = LINE_DONE_BY_SERVER;
	}
}

QList<int> cScheduler::CreateDoneList() const
{
	QList<int> list;
	for (int i = startLine; i < endLine; i++)
	{
		if (lineDone[i])
		{
			list.append(i);
		}
	}

	return list;
}

void cScheduler::UpdateDoneLines(const QList<int> &done)
{
	mutex.lock();
	for (int i = 0; i < done.size(); i++)
	{
		int line = done.at(i);
		lastLinesDone[line] = false;
		lineDone[line] = true;
		linePendingThreadId[line] = LINE_DONE_BY_SERVER;
	}
	mutex.unlock();
}

bool cScheduler::IsLineDoneByServer(int line) const
{
	if (line >= startLine && line < endLine)
	{
		return linePendingThreadId[line] == LINE_DONE_BY_SERVER;
	}
	return false;
}
