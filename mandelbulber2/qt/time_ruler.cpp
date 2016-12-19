/*
 * time_ruller.cpp
 *
 *  Created on: 19 gru 2016
 *      Author: krzysztof
 */

#include "time_ruler.h"

#include <QPainter>
#include <QPaintEvent>
#include <QApplication>
#include "../src/audio_track.h"

cTimeRuler::cTimeRuler(QWidget *parent) : QWidget(parent)
{
	frames = 0;
	framesPerKeyframe = 0;
	framesPerSecond = 30.0;
	soundLength = 0.0;
}

cTimeRuler::~cTimeRuler()
{
}

void cTimeRuler::SetParameters(cAudioTrack *audioTrack, int _framesPerKeyframe)
{
	frames = audioTrack->getNumberOfFrames();
	framesPerSecond = audioTrack->getFramesPerSecond();
	framesPerKeyframe = _framesPerKeyframe;
	soundLength = frames / framesPerSecond;
	update();
}

void cTimeRuler::paintEvent(QPaintEvent *event)
{
	if(frames > 0)
	{
		QPainter painter(this);

		QPen pen(QApplication::palette().text().color());
		pen.setWidth(0);
		painter.setPen(pen);

		QFont font = QApplication::font();
		font.setPixelSize(10);
		painter.setFont(font);

		int xStart = event->rect().x();
		int xEnd = xStart + event->rect().width();

		for(int second = 0; second < soundLength; second++)
		{
			int x = second * framesPerSecond;
			if(x >= xStart - 50 && x <= xEnd + 50)
			{
				if(second % 60 == 0)
				{
					painter.drawLine(x, 0, x, 40);
					painter.drawText(x + 3, 27, QString("%1 m").arg(second / 60));
				}

				if(second % 10 == 0)
				{
					painter.drawLine(x, 0, x, 25);
					painter.drawText(x + 3, 12, QString("%1s").arg(second % 60));
				}
				else
				{
					painter.drawLine(x, 0, x, 10);
				}
			}
		}

		int numberOfKeyframes = frames / framesPerKeyframe;
		pen.setColor(Qt::red);
		painter.setPen(pen);

		for(int key = 0; key <= numberOfKeyframes; key++)
		{
			int x = key * framesPerKeyframe;
			if(x >= xStart - 50 && x <= xEnd + 50)
			{
				painter.drawLine(x, 0, x, 60);
				painter.drawText(x + 3, 42, QString("key"));
				painter.drawText(x + 3, 52, QString::number(key));
			}
		}
	}
}
