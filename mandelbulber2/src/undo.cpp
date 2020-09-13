/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <QDebug>
#include <QDir>

#include "error_message.hpp"
#include "initparameters.hpp"
#include "settings.hpp"
#include "system_directories.hpp"
#include "write_log.hpp"

cUndo *gUndo = nullptr;

cUndo::cUndo(QObject *parent) : QObject(parent)
{
	level = 0;
	fileIndex = 0;

	QDir undoDir(systemDirectories.GetUndoFolder());
	QStringList listOfFiles = undoDir.entryList(QStringList() << "*.fract", QDir::Files, QDir::Time);

	if (listOfFiles.size() > 0)
	{
		int lastFoundFileIndex = listOfFiles[0].mid(5, 2).toInt();
		fileIndex = (lastFoundFileIndex + 1 + 100) % 100;

		for (int i = 0; i < listOfFiles.size(); i++)
		{
			sUndoRecord record;
			record.isLoaded = false;
			undoBuffer.append(record);
			level++;
		}
	}
}

cUndo::~cUndo() = default;

void cUndo::Store(std::shared_ptr<cParameterContainer> par,
	std::shared_ptr<cFractalContainer> parFractal, std::shared_ptr<cAnimationFrames> frames,
	std::shared_ptr<cKeyframes> keyframes)
{
	sUndoRecord record;

	// autosave
	WriteLog("Autosave started", 2);
	cSettings parSettings(cSettings::formatCondensedText);
	parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
	parSettings.SaveToFile(systemDirectories.GetAutosaveFile());
	parSettings.SaveToFile(systemDirectories.GetUndoFolder() + QDir::separator()
												 + QString("undo_%1.fract").arg(fileIndex, 2, 10, QChar('0')));
	WriteLog("Autosave finished", 2);

	WriteLog("cUndo::Store() started", 2);
	*record.mainParams = *par;
	*record.fractParams = *parFractal;
	if (frames)
	{
		*record.animationFrames = *frames;
		record.hasFrames = true;
	}
	else
	{
		record.hasFrames = false;
		record.animationFrames.reset(new cAnimationFrames());
	}

	if (keyframes)
	{
		*record.animationKeyframes = *keyframes;
		record.hasKeyframes = true;
	}
	else
	{
		record.hasKeyframes = false;
		record.animationKeyframes.reset(new cKeyframes());
	}

	if (undoBuffer.size() > level)
	{
		for (int i = undoBuffer.size() - 1; i >= level; i--)
		{
			undoBuffer.removeAt(i);
		}
	}
	record.isLoaded = true;
	undoBuffer.append(record);
	level++;
	fileIndex = (fileIndex + 1 + 100) % 100;
	WriteLog("cUndo::Store() finished", 2);
}

bool cUndo::Undo(std::shared_ptr<cParameterContainer> par,
	std::shared_ptr<cFractalContainer> parFractal, std::shared_ptr<cAnimationFrames> frames,
	std::shared_ptr<cKeyframes> keyframes, bool *refreshFrames, bool *refreshKeyframes)
{
	if (level > 1)
	{
		sUndoRecord record;
		if (undoBuffer.length() >= level)
		{
			level--;
			fileIndex = (fileIndex - 1 + 100) % 100;
			record = undoBuffer.at(level - 1);

			if (!record.isLoaded)
			{
				// if record in not in memory then load from settings stored in undo folder
				QString undoFilename = systemDirectories.GetUndoFolder() + QDir::separator()
															 + QString("undo_%1.fract").arg(fileIndex, 2, 10, QChar('0'));

				if (QFile::exists(undoFilename))
				{
					*record.mainParams = *par;
					*record.fractParams = *parFractal;
					cSettings parSettings(cSettings::formatCondensedText);
					parSettings.LoadFromFile(undoFilename);
					if (parSettings.Decode(record.mainParams, record.fractParams, record.animationFrames,
								record.animationKeyframes))
					{
						record.isLoaded = true;
						undoBuffer[level - 1] = record;
					}
				}
				else
				{
					cErrorMessage::showMessage(
						QObject::tr("Missing undo data in disk cache"), cErrorMessage::warningMessage);
					return false;
				}
			}

			if (record.isLoaded)
			{
				*par = *record.mainParams;
				*parFractal = *record.fractParams;
				if (frames && record.hasFrames)
				{
					*frames = *record.animationFrames;
					*refreshFrames = true;
				}
				if (keyframes && record.hasKeyframes)
				{
					*keyframes = *record.animationKeyframes;
					keyframes->RegenerateAudioTracks(par);
					*refreshKeyframes = true;
				}
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

bool cUndo::Redo(std::shared_ptr<cParameterContainer> par,
	std::shared_ptr<cFractalContainer> parFractal, std::shared_ptr<cAnimationFrames> frames,
	std::shared_ptr<cKeyframes> keyframes, bool *refreshFrames, bool *refreshKeyframes)
{
	if (level < undoBuffer.size())
	{
		sUndoRecord record;
		record = undoBuffer.at(level);
		level++;
		fileIndex = (fileIndex + 1 + 100) % 100;
		if (record.isLoaded)
		{
			*par = *record.mainParams;
			*parFractal = *record.fractParams;
			if (frames && record.hasFrames)
			{
				*frames = *record.animationFrames;
				*refreshFrames = true;
			}
			if (keyframes && record.hasKeyframes)
			{
				*keyframes = *record.animationKeyframes;
				keyframes->RegenerateAudioTracks(par);
				*refreshKeyframes = true;
			}
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		cErrorMessage::showMessage(QObject::tr("No more redo"), cErrorMessage::warningMessage);
		return false;
	}
}
