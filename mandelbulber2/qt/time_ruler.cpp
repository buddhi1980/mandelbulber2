/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * cTimeRuler - class to display a ruler with time axis and marked keyframes used
 * in the animation chart for an audio file.
 */

#include "time_ruler.h"

#include <memory>

#include <QApplication>
#include <QDebug>
#include <QPaintEvent>
#include <QPainter>

#include "src/audio_track.h"
#include "src/keyframes.hpp"

cTimeRuler::cTimeRuler(QWidget *parent) : QWidget(parent)
{
	frames = 0;
	framesPerSecond = 30.0;
	soundLength = 0.0;
}

cTimeRuler::~cTimeRuler() = default;

void cTimeRuler::SetParameters(
	std::shared_ptr<cAudioTrack> audioTrack, std::shared_ptr<cKeyframes> _keyframes)
{
	keyframes = _keyframes;
	frames = audioTrack->getNumberOfFrames();
	framesPerSecond = audioTrack->getFramesPerSecond();
	framesIndexesTable = keyframes->getFramesIndexesTable();
	soundLength = frames / framesPerSecond;
	soundDelay = audioTrack->getSoundDelay();
	update();
}

void cTimeRuler::paintEvent(QPaintEvent *event)
{
	if (frames > 0)
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

		for (int second = 0; second < soundLength; second++)
		{
			int x = int(second * framesPerSecond) - soundDelay;
			if (x >= xStart - 50 && x <= xEnd + 50)
			{
				if (second % 60 == 0)
				{
					painter.drawLine(x, 0, x, 40);
					painter.drawText(x + 3, 27, QString("%1 m").arg(second / 60));
				}

				if (second % 10 == 0)
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

		pen.setColor(Qt::red);
		painter.setPen(pen);

		for (size_t key = 0; key < framesIndexesTable.size(); key++)
		{
			int x = framesIndexesTable.at(key) - soundDelay;
			if (x >= xStart - 50 && x <= xEnd + 50)
			{
				painter.drawLine(x, 0, x, 60);
				painter.drawText(x + 3, 42, QString("key"));
				painter.drawText(x + 3, 52, QString::number(key));
			}
		}
	}
}

int cTimeRuler::FindKeyAtPosition(int mouseX)
{
	for (size_t key = 0; key < framesIndexesTable.size(); key++)
	{
		if (mouseX > framesIndexesTable[key] - 2 && mouseX < framesIndexesTable[key] + 2)
		{
			return key;
		}
	}
	return -1;
}

void cTimeRuler::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{

		int mouseX = event->x();

		int index = FindKeyAtPosition(mouseX);

		dragStartX = mouseX;
		pressedKeyIndex = index;
	}
}

void cTimeRuler::mouseMoveEvent(QMouseEvent *event)
{
	Q_UNUSED(event);

	if (pressedKeyIndex > 0)
	{
		if (event->x() != dragStartX)
		{
			mouseDragStarted = true;
		}

		if (mouseDragStarted)
		{
			ModifyKeyframePosition(pressedKeyIndex, event->x());
			emit update();
		}
	}
}

void cTimeRuler::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		pressedKeyIndex = -1;
		mouseDragStarted = false;
	}
}

void cTimeRuler::ModifyKeyframePosition(int key, int position)
{
	if (key > 0)
	{
		int framesPerKeyframe = position - framesIndexesTable.at(key - 1);
		if (framesPerKeyframe < 1) framesPerKeyframe = 1;

		cAnimationFrames::sAnimationFrame frame = keyframes->GetFrame(key - 1);
		frame.numberOfSubFrames = framesPerKeyframe;
		keyframes->ModifyFrame(key - 1, frame);

		keyframes->UpdateFramesIndexesTable();

		framesIndexesTable = keyframes->getFramesIndexesTable();

		update();
	}
}
