/*
 * cWaveFormView.h
 *
 *  Created on: 26 lis 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_WAVE_FORM_VIEW_H_
#define MANDELBULBER2_QT_WAVE_FORM_VIEW_H_

#include <QWidget>

//forward declarations
class cAudioTrack;

class cWaveFormView : public QWidget
{
	Q_OBJECT

	struct audioFrame
	{
		audioFrame()
		{
			min = 0.0;
			max = 0.0;
		}
		float min;
		float max;
	};

public:
	cWaveFormView(QWidget *parent);
	~cWaveFormView();

	void SetParameters(double _framesPerSecond);
	void AssignAudioTrack(const cAudioTrack *audiotrack);

private:

	void paintEvent(QPaintEvent * event);

	QImage waveImage;
	QImage scaledWaveImage;
	int numberOfFrames;
	double framesPerSecond;

};

#endif /* MANDELBULBER2_QT_WAVE_FORM_VIEW_H_ */
