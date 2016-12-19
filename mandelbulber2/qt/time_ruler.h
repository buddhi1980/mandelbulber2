/*
 * time_ruller.h
 *
 *  Created on: 19 gru 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_TIME_RULER_H_
#define MANDELBULBER2_QT_TIME_RULER_H_

#include <QWidget>

//forward declarations
class cAudioTrack;

class cTimeRuler : public QWidget
{
	Q_OBJECT

public:
	cTimeRuler(QWidget *parent);
	~cTimeRuler();

	void SetParameters(cAudioTrack *audioTrack, int _framesPerKeyframe);

private:
	void paintEvent(QPaintEvent *event);

	int frames;
	double framesPerSecond;
	int framesPerKeyframe;
	double soundLength;
};

#endif /* MANDELBULBER2_QT_TIME_RULER_H_ */
