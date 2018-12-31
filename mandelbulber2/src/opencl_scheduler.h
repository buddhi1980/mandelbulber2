/*
 * opencl_scheduler.h
 *
 *  Created on: 29 gru 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPENCL_SCHEDULER_H_
#define MANDELBULBER2_SRC_OPENCL_SCHEDULER_H_

#include "opencl_scheduler.h"
#include <QtCore>

class cOpenClScheduler
{
	struct sTileStatus
	{
		bool enabled;
		bool reserved;
		bool done;
	};

public:
	cOpenClScheduler(const QList<QPoint> *tileSequence);
	~cOpenClScheduler();
	void EnableAllTiles();
	void DisableTile(int tileIndex);
	void Clear();
	int GetNextTileToRender(int lastTile);
	bool AllDone();

	const QList<QPoint> *getTileSequence() const { return tileSequence; }

private:
	const QList<QPoint> *tileSequence;
	QList<sTileStatus> tiles;

	QMutex lock;
};

#endif /* MANDELBULBER2_SRC_OPENCL_SCHEDULER_H_ */
