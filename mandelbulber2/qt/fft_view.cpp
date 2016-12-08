/*
 * fft_view.cpp
 *
 *  Created on: 6 gru 2016
 *      Author: krzysztof
 */

#include "fft_view.h"

#include <QPainter>
#include <QtGui>

#include "../src/audio_track.h"
#include "../src/audio_fft_data.h"
#include "../src/system.hpp"

cFFTView::cFFTView(QWidget *parent) : QWidget(parent)
{
	numberOfFrames = 0;
	framesPerSecond = 30.0;
}

cFFTView::~cFFTView()
{
	// TODO Auto-generated destructor stub
}

void cFFTView::SetParameters(double _framesPerSecond)
{
	framesPerSecond = _framesPerSecond;
}

void cFFTView::AssignAudioTrack(const cAudioTrack *audiotrack)
{

	if (audiotrack)
	{
		int numberOfSampels = audiotrack->getLength();
		int sampleRate = audiotrack->getSampleRate();
		int numberOfFrames = numberOfSampels * framesPerSecond / sampleRate;

		this->setFixedWidth(numberOfFrames);

		const int height = 256;

		fftImage = QImage(QSize(numberOfFrames, height), QImage::Format_RGB32);

		for (int x = 0; x < numberOfFrames; x++)
		{
			cAudioFFTdata fftFrame = audiotrack->getFFTSample(x);
			for (int y = 0; y < height; y++)
			{
				int y2 = height - y - 1;
				double value = fftFrame.data[y] * 0.1;

				QRgb pixel;
				if (value < 0.5)
				{
					pixel = qRgba(0, value * 510, 0, 255);
				}
				else
				{
					if (value > 1.0) value = 1.0;
					pixel = qRgba((value - 0.5) * 510, 255, 0, 255);
				}

				QRgb *line = (QRgb *)fftImage.scanLine(y2);
				line[x] = pixel;
			}
		}

		QPainter painter(&fftImage);
		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		scaledFftImage =
			fftImage.scaled(this->width(), height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		update();
		WriteLog("FFTView created", 2);
	}
}

void cFFTView::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	painter.drawImage(0, 0, scaledFftImage);
}
