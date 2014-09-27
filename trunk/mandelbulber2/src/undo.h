/*
 * undo.h
 *
 *  Created on: Sep 22, 2014
 *      Author: krzysztof
 */

#ifndef UNDO_H_
#define UNDO_H_

#include <QtCore>
#include "parameters.hpp"

#define MAX_UNDO_LEVELS 100

class cUndo
{
public:
	cUndo();
	~cUndo();
	void Store(cParameterContainer *par, cParameterContainer *parFractal);
	bool Undo(cParameterContainer *par, cParameterContainer *parFractal);
	bool Redo(cParameterContainer *par, cParameterContainer *parFractal);

private:
	struct sUndoRecord
	{
		cParameterContainer mainParams;
		cParameterContainer fractParams[4];
	};

	QList<sUndoRecord> undoBuffer;
	int level;
};

#endif /* UNDO_H_ */

extern cUndo gUndo;
