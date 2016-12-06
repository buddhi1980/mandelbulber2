/*
 * fft_view.h
 *
 *  Created on: 6 gru 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_FFT_VIEW_H_
#define MANDELBULBER2_QT_FFT_VIEW_H_

#include <QWidget>

// forward declarations
class cAudioTrack;

class cFFTView : public QWidget
{
	Q_OBJECT

public:
	cFFTView(QWidget *parent);
	~cFFTView();

	void SetParameters(double _framesPerSecond);
	void AssignAudioTrack(const cAudioTrack *audiotrack);

private:
	void paintEvent(QPaintEvent *event);

	QImage fftImage;
	QImage scaledFftImage;
	int numberOfFrames;
	double framesPerSecond;
};

#endif /* MANDELBULBER2_QT_FFT_VIEW_H_ */
