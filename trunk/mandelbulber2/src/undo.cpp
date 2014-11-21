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

#include "undo.h"
#include "error_message.hpp"

cUndo gUndo;

cUndo::cUndo()
{
	level = 0;
}

cUndo::~cUndo()
{
}

void cUndo::Store(cParameterContainer *par, cFractalContainer *parFractal)
{
	sUndoRecord record;
	record.mainParams = *par;
	record.fractParams = *parFractal;

	if(undoBuffer.size() > level)
	{
		for(int i = undoBuffer.size() - 1; i >= level; i--)
		{
			undoBuffer.removeAt(i);
		}
	}

	undoBuffer.append(record);
	level++;
}

bool cUndo::Undo(cParameterContainer *par, cFractalContainer *parFractal)
{
	if(level > 1)
	{
		sUndoRecord record;
		if(undoBuffer.length() >= level)
		{
			level--;
			record = undoBuffer.at(level-1);
			*par = record.mainParams;
			*parFractal = record.fractParams;

		}
		return true;
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("No more undo"), cErrorMessage::warningMessage);
		return false;
	}
}

bool cUndo::Redo(cParameterContainer *par, cFractalContainer *parFractal)
{
	if (level < undoBuffer.size())
	{
		sUndoRecord record;
		record = undoBuffer.at(level);
		level++;
		*par = record.mainParams;
		*parFractal = record.fractParams;
		return true;
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("No more redo"), cErrorMessage::warningMessage);
		return false;
	}
}
