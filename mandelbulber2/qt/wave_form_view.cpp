/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016 Krzysztof Marczak        §R-==%w["'~5]m%=L.=~5N
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
 * cWaveFormView - promoted QWidget to display audio track
 */

#include "wave_form_view.h"

#include <QPainter>
#include <algorithm>
#include "../src/audio_track.h"
#include "../src/system.hpp"

cWaveFormView::cWaveFormView(QWidget *parent) : QWidget(parent)
{
	numberOfFrames = 0;
	framesPerSecond = 30.0;
}

cWaveFormView::~cWaveFormView()
{
}

void cWaveFormView::AssignAudioTrack(const cAudioTrack *audiotrack)
{
	if (audiotrack)
	{
		WriteLog("WaveFormView calculation started", 2);
		int numberOfSampels = audiotrack->getLength();
		int sampleRate = audiotrack->getSampleRate();
		numberOfFrames = audiotrack->getNumberOfFrames();
		framesPerSecond = audiotrack->getFramesPerSecond();
		audioFrame *audioBuffer = new audioFrame[numberOfFrames + 1];

		this->setFixedWidth(numberOfFrames);

		for (int i = 0; i < numberOfSampels; i++)
		{
			int frameNo = i * framesPerSecond / sampleRate;
			float sample = audiotrack->getSample(i) / audiotrack->getMaxVolume();
			audioBuffer[frameNo].min = qMin(sample, audioBuffer[frameNo].min);
			audioBuffer[frameNo].max = qMax(sample, audioBuffer[frameNo].max);
		}

		WriteLog("audioBuffer created", 2);

		int height = this->height();

		waveImage = QImage(QSize(numberOfFrames, height), QImage::Format_RGB32);
		waveImage.fill(Qt::black);
		QPainter painter(&waveImage);
		double hscale = 0.5 * height;
		int center = height / 2;

		painter.setPen(Qt::green);
		for (int x = 0; x < numberOfFrames; x++)
		{
			// painter.drawLine(
			//	x, center + hscale * audioBuffer[x].min, x, center + hscale * audioBuffer[x].max);
			int yStart = center + hscale * audioBuffer[x].min;
			if (yStart < 0) yStart = 0;
			int yStop = center + hscale * audioBuffer[x].max;
			if (yStop > height - 1) yStop = height - 1;
			QRgb pixel = qRgba(0, 255, 0, 255);
			for (int y = yStart; y < yStop; y++)
			{
				QRgb *line = (QRgb *)waveImage.scanLine(y);
				line[x] = pixel;
			}
		}

		painter.setRenderHint(QPainter::SmoothPixmapTransform);
		scaledWaveImage =
			waveImage.scaled(this->width(), height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
		update();

		delete[] audioBuffer;

		WriteLog("WaveFormView created", 2);
	}
}

void cWaveFormView::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	painter.drawImage(0, 0, scaledWaveImage);
}
