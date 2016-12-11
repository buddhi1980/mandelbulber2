/*
 * anim_audio_view.h
 *
 *  Created on: 10 gru 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_ANIM_AUDIO_VIEW_H_
#define MANDELBULBER2_QT_ANIM_AUDIO_VIEW_H_

#include <QWidget>

// forward declarations
class cAudioTrack;

class cAnimAudioView : public QWidget
{
	Q_OBJECT

public:
	cAnimAudioView(QWidget *parent);
	~cAnimAudioView();
	void UpdateChart(const cAudioTrack *audiotrack);

private:
	void paintEvent(QPaintEvent *event);

	QImage animAudioImage;
};

#endif /* MANDELBULBER2_QT_ANIM_AUDIO_VIEW_H_ */
