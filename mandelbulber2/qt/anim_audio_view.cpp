/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cAnimAudioView - promoted QWidget to display the animation curve extracted from
 * the audio data and the audio settings
 */

#include "anim_audio_view.h"

#include <QPainter>

#include "src/audio_track.h"

cAnimAudioView::cAnimAudioView(QWidget *parent) : QWidget(parent)
{
	playbackPositionX = 0;
	framesPerSecond = 30.0;
}

cAnimAudioView::~cAnimAudioView() = default;

void cAnimAudioView::UpdateChart(const QSharedPointer<cAudioTrack> audioTrack)
{
	if (audioTrack && audioTrack->isLoaded())
	{
		const int numberOfFrames = audioTrack->getNumberOfFrames();
		framesPerSecond = audioTrack->getFramesPerSecond();
		setFixedWidth(numberOfFrames);

		animAudioImage = QImage(QSize(numberOfFrames, height()), QImage::Format_RGB32);
		animAudioImage.fill(Qt::black);

		QPainter painter(&animAudioImage);

		const int maxY = height() - 1;
		QPoint prevPoint(0, maxY);

		painter.setPen(Qt::green);
		painter.setRenderHint(QPainter::Antialiasing, true);

		for (int frame = 0; frame < numberOfFrames; frame++)
		{
			const QPoint point(frame, int(maxY - audioTrack->getAnimation(frame) * maxY));
			painter.drawLine(prevPoint, point);
			prevPoint = point;
		}
		update();
	}
	else
	{
		animAudioImage = QImage();
		update();
	}
}

void cAnimAudioView::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	painter.drawImage(0, 0, animAudioImage);
	const QPen pen(QColor(255, 255, 255, 128));

	painter.setPen(pen);
	painter.drawLine(playbackPositionX, 0, playbackPositionX, height() - 1);
}

void cAnimAudioView::positionChanged(qint64 position)
{
	const double time = position * 0.001;
	playbackPositionX = int(time * framesPerSecond);
	update();
}
