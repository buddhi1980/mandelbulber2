/*
 * pushbutton_anim_sound.h
 *
 *  Created on: 16 gru 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_PUSHBUTTON_ANIM_SOUND_H_
#define MANDELBULBER2_QT_PUSHBUTTON_ANIM_SOUND_H_

#include <qpushbutton.h>

// forward declarations
class cAnimationFrames;

class cPushButtonAnimSound : public QPushButton
{
	Q_OBJECT
public:
	cPushButtonAnimSound(QWidget *parent = 0);
	~cPushButtonAnimSound();
	void AssignParameterName(const QString &_parameterName);
	void AssignAnimation(cAnimationFrames *_animationFrames) { animationFrames = _animationFrames; }

private slots:
	void slotLoadAudio();

private:
	QString parameterName;
	cAnimationFrames *animationFrames;
};

#endif /* MANDELBULBER2_QT_PUSHBUTTON_ANIM_SOUND_H_ */
