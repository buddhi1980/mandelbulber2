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
}

cOpenClScheduler::~cOpenClScheduler()
{
	// TODO Auto-generated destructor stub
}

void cOpenClScheduler::EnableAllTiles() {}

void cOpenClScheduler::DisableTile(int tileIndex) {}

int cOpenClScheduler::GetNextTileToRender(int lastTile) {}

bool cOpenClScheduler::AllDone() {}
