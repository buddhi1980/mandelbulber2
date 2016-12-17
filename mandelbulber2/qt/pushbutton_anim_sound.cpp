/*
 * pushbutton_anim_sound.cpp
 *
 *  Created on: 16 gru 2016
 *      Author: krzysztof
 */

#include "pushbutton_anim_sound.h"
#include "audio_selector.h"

cPushButtonAnimSound::cPushButtonAnimSound(QWidget *parent) : QPushButton(parent)
{
	setText("Anim By Sound");
	connect(this, SIGNAL(clicked()), this, SLOT(slotLoadAudio()));
	animationFrames = NULL;
}

cPushButtonAnimSound::~cPushButtonAnimSound()
{
	// TODO Auto-generated destructor stub
}

void cPushButtonAnimSound::AssignParameterName(const QString &_parameterName)
{
	parameterName = _parameterName;
}

void cPushButtonAnimSound::slotLoadAudio()
{
	cAudioSelector *audioSelectorDialog = new cAudioSelector(this);
	audioSelectorDialog->setWindowFlags(Qt::Dialog);
	audioSelectorDialog->AssignParameter(parameterName);
	audioSelectorDialog->AssignAnimation(animationFrames);
	audioSelectorDialog->show();
}
