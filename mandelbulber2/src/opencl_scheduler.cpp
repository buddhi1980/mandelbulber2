/*
 * opencl_scheduler.cpp
 *
 *  Created on: 29 gru 2018
 *      Author: krzysztof
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
	// TODO Auto-generated destructor stub
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

