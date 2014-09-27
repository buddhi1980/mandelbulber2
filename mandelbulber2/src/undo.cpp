/*
 * undo.cpp
 *
 *  Created on: Sep 22, 2014
 *      Author: krzysztof
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

void cUndo::Store(cParameterContainer *par, cParameterContainer *parFractal)
{
	sUndoRecord record;
	record.mainParams = *par;
	for(int i=0; i<4; i++)
	{
		record.fractParams[i] = parFractal[i];
	}

	if(undoBuffer.size() > level)
	{
		for(int i = undoBuffer.size() - 1; i >= level; i--)
		{
			undoBuffer.removeAt(i);
		}
	}

	undoBuffer.append(record);
	level++;
	qDebug() << "Undo level" << level << "size" << undoBuffer.size();
}

bool cUndo::Undo(cParameterContainer *par, cParameterContainer *parFractal)
{
	qDebug() << "Undo level" << level << "size" << undoBuffer.size();
	if(level > 1)
	{
		sUndoRecord record;
		if(undoBuffer.length() >= level)
		{
			level--;
			record = undoBuffer.at(level-1);
			*par = record.mainParams;
			for(int i=0; i<4; i++)
			{
				parFractal[i] = record.fractParams[i];
			}
		}
		return true;
	}
	else
	{
		cErrorMessage::showMessage(QString("No more undo"), cErrorMessage::warningMessage);
		return false;
	}
}

bool cUndo::Redo(cParameterContainer *par, cParameterContainer *parFractal)
{
	if (level < undoBuffer.size())
	{
		sUndoRecord record;
		record = undoBuffer.at(level);
		level++;
		*par = record.mainParams;
		for (int i = 0; i < 4; i++)
		{
			parFractal[i] = record.fractParams[i];
		}
		return true;
	}
	else
	{
		cErrorMessage::showMessage(QString("No more redo"), cErrorMessage::warningMessage);
		return false;
	}
}
