/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <memory>

#include "audio_selector.h"

#include "src/animation_frames.hpp"
#include "src/audio_track.h"
#include "src/global_data.hpp"
#include "src/initparameters.hpp"
#include "src/parameters.hpp"

cPushButtonAnimSound::cPushButtonAnimSound(QWidget *parent) : QPushButton(parent)
{
	connect(this, SIGNAL(clicked()), this, SLOT(slotLoadAudio()));
	animationFrames = nullptr;
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
		std::shared_ptr<cAudioTrack> audio = animationFrames->GetAudioPtr(parameterName);
		if (audio && audio->isLoaded())
		{
			// bold text only when the animation by sound is enabled, so the state of every row can be
			// seen in the table without opening the audio selector
			const QString enableParameter = QString("animsound_enable_%1").arg(parameterName);
			const bool enabled = gPar->IfExists(enableParameter) && gPar->Get<bool>(enableParameter);
			setText(tr("Audio loaded"));
			setToolTip(enabled ? tr("Animation by sound is enabled")
												 : tr("Animation by sound is disabled (audio file is loaded)"));
			f.setBold(enabled);
			setFont(f);
			return;
		}
	}
	f.setBold(false);
	setText(tr("Anim By Sound"));
	setToolTip(QString());
	setFont(f);
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

	// the dialog stores its settings (also the enable checkbox) when it is destroyed
	connect(audioSelectorDialog, &QWidget::destroyed, this, &cPushButtonAnimSound::slotUpdateButton);
	connect(audioSelectorDialog, &cAudioSelector::closed, this,
		&cPushButtonAnimSound::signalAudioSelectorClosed);
}
