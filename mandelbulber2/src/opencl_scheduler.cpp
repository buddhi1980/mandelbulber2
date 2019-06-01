/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * scheduler for multi-gpu rendering
 */

#include "opencl_scheduler.h"

cOpenClScheduler::cOpenClScheduler(const QList<QPoint> *tileSequence)
{
	this->tileSequence = tileSequence;

	sTileStatus initStatus;
	initStatus.enabled = true;
	initStatus.reserved = false;
	initStatus.done = false;

	for (int i = 0; i < tileSequence->size(); i++)
	{
		tiles.append(initStatus);
	}
}

cOpenClScheduler::~cOpenClScheduler()
{
	// nothing to destroy
}

void cOpenClScheduler::EnableAllTiles()
{
	for (sTileStatus &status : tiles)
	{
		status.enabled = true;
	}
}

void cOpenClScheduler::Clear()
{
	lock.lock();
	for (sTileStatus &status : tiles)
	{
		status.done = 0;
		status.reserved = 0;
	}
	lock.unlock();
}

void cOpenClScheduler::DisableTile(int tileIndex)
{
	tiles[tileIndex].enabled = false;
}

void cOpenClScheduler::ReserveTile(int tileIndex)
{
	tiles[tileIndex].reserved = true;
}

int cOpenClScheduler::GetNextTileToRender(int lastTile, int monteCarloIteration)
{
	lock.lock();

	tiles[lastTile].done = monteCarloIteration; // done at n MC iteration

	int nextTile = -1; //-1 if nothing more to do

	for (int i = lastTile + 1; i < tileSequence->size(); i++)
	{
		if (tiles[i].reserved >= monteCarloIteration || !tiles[i].enabled)
		{
			continue;
		}
		else
		{
			nextTile = i;
			tiles[i].reserved = monteCarloIteration; // reserve for n MC iterartion
			break;
		}
	}
	lock.unlock();
	return nextTile;
}

bool cOpenClScheduler::AllDone(int monteCarloIteration)
{
	for (sTileStatus &status : tiles)
	{
		if (status.done < monteCarloIteration)
			return false; // if tile was only done for lower MC iteration
										// then something is still to to for actual iteration
	}
	return true;
}
