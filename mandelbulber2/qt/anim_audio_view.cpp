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
	// TODO Auto-generated constructor stub
}

cAnimAudioView::~cAnimAudioView()
{
	// TODO Auto-generated destructor stub
}

void cAnimAudioView::UpdateChart(const cAudioTrack *audiotrack)
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
		QPoint point(frame, maxY - audiotrack->getBand(frame, 8000.0, 500.0) * maxY);
		painter.drawLine(prevPoint, point);
		prevPoint = point;
	}
}

void cAnimAudioView::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	painter.drawImage(0, 0, animAudioImage);
}
