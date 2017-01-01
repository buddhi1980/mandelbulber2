/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016 Krzysztof Marczak        §R-==%w["'~5]m%=L.=~5N
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
 * Created on: 16 gru 2016
 * Author: krzysztof
 */

#include "pushbutton_anim_sound.h"
#include "audio_selector.h"
#include "../src/initparameters.hpp"
#include "../src/animation_frames.hpp"
#include "../src/audio_track.h"

cPushButtonAnimSound::cPushButtonAnimSound(QWidget *parent) : QPushButton(parent)
{
	connect(this, SIGNAL(clicked()), this, SLOT(slotLoadAudio()));
	animationFrames = NULL;
	setIcon(QIcon::fromTheme("audio-x-generic", QIcon(":system/icons/audio-x-generic.svg")));
}

cPushButtonAnimSound::~cPushButtonAnimSound()
{
	// nothing needed here
}

void cPushButtonAnimSound::AssignParameterName(const QString &_parameterName)
{
	parameterName = _parameterName;
}

void cPushButtonAnimSound::slotUpdateButton()
{
	QFont f = font();
	if (animationFrames)
	{
		cAudioTrack *audio = animationFrames->GetAudioPtr(parameterName);
		if (audio && audio->isLoaded())
		{
			setText("Audio loaded");
			f.setBold(true);
			return;
		}
	}
	f.setBold(false);
	setText("Anim By Sound");
}

void cPushButtonAnimSound::slotLoadAudio()
{
	cAudioSelector *audioSelectorDialog = new cAudioSelector(this);
	audioSelectorDialog->setWindowFlags(Qt::Dialog);
	audioSelectorDialog->AssignParameter(parameterName);
	audioSelectorDialog->AssignAnimation(animationFrames);
	audioSelectorDialog->setWindowModality(Qt::ApplicationModal);
	connect(audioSelectorDialog, SIGNAL(audioLoaded()), this, SLOT(slotUpdateButton()));
	audioSelectorDialog->show();
}
