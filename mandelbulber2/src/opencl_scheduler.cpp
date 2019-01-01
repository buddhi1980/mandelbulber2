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
	for (sTileStatus &status : tiles)
	{
		status.done = false;
		status.reserved = false;
	}
}

void cOpenClScheduler::DisableTile(int tileIndex)
{
	{
		tiles[tileIndex].enabled = false;
	}
}

int cOpenClScheduler::GetNextTileToRender(int lastTile)
{
	lock.lock();

	tiles[lastTile].done = true;

	int nextTile = -1; //-1 if nothing more to do

	for (int i = lastTile + 1; i < tileSequence->size(); i++)
	{
		if (tiles[i].reserved)
		{
			continue;
		}
		else
		{
			nextTile = i;
			tiles[i].reserved = true;
			break;
		}
	}
	lock.unlock();
	return nextTile;
}

bool cOpenClScheduler::AllDone()
{
	for (sTileStatus &status : tiles)
	{
		if (!status.done) return false;
	}
	return true;
}
