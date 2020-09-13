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

#ifndef MANDELBULBER2_SRC_UNDO_H_
#define MANDELBULBER2_SRC_UNDO_H_

#include <QObject>

#include "animation_frames.hpp"
#include "fractal_container.hpp"
#include "keyframes.hpp"
#include "parameters.hpp"

class cUndo : QObject
{
	Q_OBJECT
public:
	cUndo(QObject *parent);
	~cUndo();
	void Store(std::shared_ptr<cParameterContainer> par,
		std::shared_ptr<cFractalContainer> parFractal,
		std::shared_ptr<cAnimationFrames> frames = nullptr,
		std::shared_ptr<cKeyframes> keyframes = nullptr);
	bool Undo(std::shared_ptr<cParameterContainer> par, std::shared_ptr<cFractalContainer> parFractal,
		std::shared_ptr<cAnimationFrames> frames, std::shared_ptr<cKeyframes> keyframes,
		bool *refreshFrames, bool *refreshKeyframes);
	bool Redo(std::shared_ptr<cParameterContainer> par, std::shared_ptr<cFractalContainer> parFractal,
		std::shared_ptr<cAnimationFrames> frames, std::shared_ptr<cKeyframes> keyframes,
		bool *refreshFrames, bool *refreshKeyframes);

private:
	struct sUndoRecord
	{
		sUndoRecord()
		{
			mainParams.reset(new cParameterContainer);
			fractParams.reset(new cFractalContainer);
			animationFrames.reset(new cAnimationFrames);
			animationKeyframes.reset(new cKeyframes);
		}

		std::shared_ptr<cParameterContainer> mainParams;
		std::shared_ptr<cFractalContainer> fractParams;
		std::shared_ptr<cAnimationFrames> animationFrames;
		std::shared_ptr<cKeyframes> animationKeyframes;
		bool hasFrames = false;
		bool hasKeyframes = false;
		bool isLoaded = false;
	};

	QList<sUndoRecord> undoBuffer;
	int level;
	int fileIndex;
};

extern cUndo *gUndo;

#endif /* MANDELBULBER2_SRC_UNDO_H_ */
