/*
 * wave_form_view.cpp
 *
 *  Created on: 26 lis 2016
 *      Author: krzysztof
 */

#include "wave_form_view.h"

#include <QPainter>
#include <algorithm>
#include "../src/audio_track.h"

cWaveFormView::cWaveFormView(QWidget *parent) : QWidget(parent)
{
	numberOfFrames = 0;
	framesPerSecond = 30.0;
}

cWaveFormView::~cWaveFormView()
{
	// TODO Auto-generated destructor stub
}

void cWaveFormView::SetParameters(double _framesPerSecond)
{
	framesPerSecond = _framesPerSecond;
}

void cWaveFormView::AssignAudioTrack(const cAudioTrack *audiotrack)
{
	QVector<audioFrame> audioBuffer;
	if(audiotrack)
	{
		int numberOfSampels = audiotrack->getLength();
		int sampleRate = audiotrack->getSampleRate();

		int numberOfFrames = numberOfSampels * framesPerSecond / sampleRate;
		audioBuffer.reserve(numberOfFrames + 1);
		audioBuffer.resize(numberOfFrames + 1);

		for(int i = 0; i < numberOfSampels; i++)
		{
			int frameNo = i * framesPerSecond / sampleRate;
			float sample = audiotrack->getSample(i);

			audioBuffer[frameNo].min = qMin(sample, audioBuffer[frameNo].min);
			audioBuffer[frameNo].max = qMax(sample, audioBuffer[frameNo].max);
		}

		int height = this->height();

		waveImage = QImage(QSize(numberOfFrames, height), QImage::Format_RGB32);
		waveImage.fill(Qt::black);
		QPainter painter(&waveImage);
		double hscale = 0.5 * height;
		int center = height / 2;

		painter.setPen(Qt::green);
		for(int x = 0; x < numberOfFrames; x++)
		{
			painter.drawLine(x, center + hscale * audioBuffer[x].min, x, center + hscale * audioBuffer[x].max);
		}

		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		scaledWaveImage = waveImage.scaled(this->width(), height);
		update();
	}
}

void cWaveFormView::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	painter.drawImage(0,0,scaledWaveImage);
}
