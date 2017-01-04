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
 * cUndo class - undo / redo functionality
 *
 * This class holds an undoBuffer to store changes to the settings.
 * The methods Undo and Redo are used to navigate inside the undo timeline.
 * The buffer is a simple LIFO buffer which holds the parameter entries.
 * (A Store() invocation while Undo-ed in the list will truncate to the current level
 * and append the new entry. The Redo entries will be lost.)
 */

#include "undo.h"
#include "error_message.hpp"
#include "initparameters.hpp"
#include "settings.hpp"
#include "system.hpp"

cUndo gUndo;

cUndo::cUndo()
{
	level = 0;
}

cUndo::~cUndo()
{
}

void cUndo::Store(cParameterContainer *par, cFractalContainer *parFractal, cAnimationFrames *frames,
	cKeyframes *keyframes)
{
	sUndoRecord record;

	// autosave
	WriteLog("Autosave started", 2);
	cSettings parSettings(cSettings::formatCondensedText);
	parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
	parSettings.SaveToFile(systemData.GetAutosaveFile());
	WriteLog("Autosave finished", 2);

	WriteLog("cUndo::Store() started", 2);
	record.mainParams = *par;
	record.fractParams = *parFractal;
	if (frames)
	{
		record.animationFrames = *frames;
		record.hasFrames = true;
	}
	else
	{
		record.hasFrames = false;
		record.animationFrames = cAnimationFrames();
	}

	if (keyframes)
	{
		record.animationKeyframes = *keyframes;
		record.hasKeyframes = true;
	}
	else
	{
		record.hasKeyframes = false;
		record.animationKeyframes = cKeyframes();
	}

	if (undoBuffer.size() > level)
	{
		for (int i = undoBuffer.size() - 1; i >= level; i--)
		{
			undoBuffer.removeAt(i);
		}
	}

	undoBuffer.append(record);
	level++;
	WriteLog("cUndo::Store() finished", 2);
}

bool cUndo::Undo(cParameterContainer *par, cFractalContainer *parFractal, cAnimationFrames *frames,
	cKeyframes *keyframes, bool *refreshFrames, bool *refreshKeyframes)
{
	if (level > 1)
	{
		sUndoRecord record;
		if (undoBuffer.length() >= level)
		{
			level--;
			record = undoBuffer.at(level - 1);
			*par = record.mainParams;
			*parFractal = record.fractParams;
			if (frames && record.hasFrames)
			{
				*frames = record.animationFrames;
				*refreshFrames = true;
			}
			if (keyframes && record.hasKeyframes)
			{
				*keyframes = record.animationKeyframes;
				keyframes->RegenerateAudioTracks(par);
				*refreshKeyframes = true;
			}
		}
		return true;
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("No more undo"), cErrorMessage::warningMessage);
		return false;
	}
}

bool cUndo::Redo(cParameterContainer *par, cFractalContainer *parFractal, cAnimationFrames *frames,
	cKeyframes *keyframes, bool *refreshFrames, bool *refreshKeyframes)
{
	if (level < undoBuffer.size())
	{
		sUndoRecord record;
		record = undoBuffer.at(level);
		level++;
		*par = record.mainParams;
		*parFractal = record.fractParams;
		if (frames && record.hasFrames)
		{
			*frames = record.animationFrames;
			*refreshFrames = true;
		}
		if (keyframes && record.hasKeyframes)
		{
			*keyframes = record.animationKeyframes;
			keyframes->RegenerateAudioTracks(par);
			*refreshKeyframes = true;
		}
		return true;
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("No more redo"), cErrorMessage::warningMessage);
		return false;
	}
}
