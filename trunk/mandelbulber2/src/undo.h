/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cUndo class - undo / redo functionality
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
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
