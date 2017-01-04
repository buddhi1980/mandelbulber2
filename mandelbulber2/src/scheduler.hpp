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

#ifndef MANDELBULBER2_SRC_SCHEDULER_HPP_
#define MANDELBULBER2_SRC_SCHEDULER_HPP_

#include "region.hpp"
#include <QMutex>
#include <qvector.h>

class cScheduler
{
public:
	cScheduler(cRegion<int> screenRegion, int progressive);
	~cScheduler();
	int NextLine(int threadId, int actualLine, bool lastLineWasBroken);
	bool ShouldIBreak(int threadId, int actualLine) const;
	bool ThereIsStillSomethingToDo(int ThreadId) const;
	bool AllLinesDone() const;
	void InitFirstLine(int threadId, int firstLine) const;
	QList<int> GetLastRenderedLines() const;
	double PercentDone() const;
	void Stop() { stopRequest = true; }
	void MarkReceivedLines(const QList<int> &lineNumbers) const;
	void UpdateDoneLines(const QList<int> &done);

	int GetProgressiveStep() const { return progressiveStep; }
	int GetProgressivePass() const { return progressivePass; }
	bool ProgressiveNextStep();
	QList<int> CreateDoneList() const;
	bool IsLineDoneByServer(int line) const;

private:
	void Reset() const;
	int FindBiggestGap() const;

	int *linePendingThreadId;
	bool *lineDone;
	bool *lastLinesDone;
	int numberOfLines;
	int startLine;
	int endLine;
	volatile bool stopRequest;
	int progressiveStep;
	int progressivePass;
	bool progressiveEnabled;
	QMutex mutex;
};

#endif /* MANDELBULBER2_SRC_SCHEDULER_HPP_ */
