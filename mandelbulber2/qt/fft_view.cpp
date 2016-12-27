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
 * cFFTView - promoted QWidget for displaying of fft audio course
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
	lowFreqY = 0;
	highFreqY = 0;
	sampleRate = 0;
}

cFFTView::~cFFTView()
{
	// nothing needed here
}

void cFFTView::AssignAudioTrack(const cAudioTrack *audiotrack)
{

	if (audiotrack)
	{
		numberOfFrames = audiotrack->getNumberOfFrames();
		framesPerSecond = audiotrack->getFramesPerSecond();
		sampleRate = audiotrack->getSampleRate();

		this->setFixedWidth(numberOfFrames);

		const int height = 256;

		fftImage = QImage(QSize(numberOfFrames, height), QImage::Format_RGB32);

		for (int x = 0; x < numberOfFrames; x++)
		{
			cAudioFFTdata fftFrame = audiotrack->getFFTSample(x);
			for (int y = 0; y < height; y++)
			{
				int y2 = height - y - 1;
				double value = 10.0 * fftFrame.data[y] / audiotrack->getMaxFft();

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

void cFFTView::slotFreqChanged(double midFreq, double bandwidth)
{
	lowFreqY = height() - 1
						 - (double)cAudioFFTdata::fftSize / (double)sampleRate * (midFreq - bandwidth * 0.5);
	highFreqY = height() - 1
							- (double)cAudioFFTdata::fftSize / (double)sampleRate * (midFreq + bandwidth * 0.5);
	update();
}

void cFFTView::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	painter.drawImage(0, 0, scaledFftImage);

	QBrush brush(QColor(255, 255, 255, 128));
	painter.setBrush(brush);
	painter.setPen(Qt::NoPen);
	painter.drawRect(QRect(QPoint(0, highFreqY), QPoint(width(), lowFreqY)));

	painter.setPen(Qt::red);
	QFont font = QApplication::font();
	font.setPixelSize(10);
	painter.setFont(font);
	painter.drawText(
		3, 13, QString("%1 Hz").arg((int)((double)sampleRate / cAudioFFTdata::fftSize * height())));
	painter.drawText(3, height() / 2,
		QString("%1 Hz").arg((int)((double)sampleRate / cAudioFFTdata::fftSize * height() * 0.5)));
	painter.drawText(3, height() - 3, QString("%1 Hz").arg(0));
}
