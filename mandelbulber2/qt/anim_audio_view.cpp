/*
 * anim_audio_view.cpp
 *
 *  Created on: 10 gru 2016
 *      Author: krzysztof
 */

#include "anim_audio_view.h"

#include <QPainter>
#include "../src/audio_track.h"

cAnimAudioView::cAnimAudioView(QWidget *parent) : QWidget(parent)
{
}

cAnimAudioView::~cAnimAudioView()
{
	// TODO Auto-generated destructor stub
}

void cAnimAudioView::UpdateChart(const cAudioTrack *audiotrack, double midFreq, double bandwidth)
{
	int numberOfFrames = audiotrack->getNumberOfFrames();
	this->setFixedWidth(numberOfFrames);

	animAudioImage = QImage(QSize(numberOfFrames, height()), QImage::Format_RGB32);
	animAudioImage.fill(Qt::black);

	QPainter painter(&animAudioImage);

	int maxY = height() - 1;
	QPoint prevPoint(0, maxY);

	painter.setPen(Qt::green);
	painter.setRenderHint(QPainter::Antialiasing, true);

	for (int frame = 0; frame < numberOfFrames; frame++)
	{
		QPoint point(frame, maxY - audiotrack->getBand(frame, midFreq, bandwidth) * maxY);
		painter.drawLine(prevPoint, point);
		prevPoint = point;
	}
	update();
}

void cAnimAudioView::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	painter.drawImage(0, 0, animAudioImage);
}
