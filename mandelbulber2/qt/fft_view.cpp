/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <cmath>
#include <memory>

#include <QApplication>
#include <QMouseEvent>
#include <QPainter>

#include "src/audio_fft_data.h"
#include "src/audio_track.h"

// QMouseEvent::y() is deprecated since Qt 6.0; position() is the replacement.
// The fallback keeps this file compilable with Qt < 6.0.
static inline int mouseLocalY(const QMouseEvent *event)
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
	return int(event->position().y());
#else
	return event->y();
#endif
}
#include "src/write_log.hpp"

cFFTView::cFFTView(QWidget *parent) : QWidget(parent)
{
	numberOfFrames = 0;
	framesPerSecond = 30.0;
	lowFreq = 0.0;
	highFreq = 0.0;
	sampleRate = 0;
	firstClickedFreqValue = 0.0;
}

cFFTView::~cFFTView()
{
	// nothing needed here
}

void cFFTView::AssignAudioTrack(const std::shared_ptr<cAudioTrack> audiotrack)
{

	if (audiotrack && audiotrack->isLoaded())
	{
		numberOfFrames = audiotrack->getNumberOfFrames();
		framesPerSecond = audiotrack->getFramesPerSecond();
		sampleRate = audiotrack->getSampleRate();

		setFixedWidth(numberOfFrames);

		const int height = cAudioFFTData::fftSize / 2;

		fftImage = QImage(QSize(numberOfFrames, height), QImage::Format_RGB32);

		for (int x = 0; x < numberOfFrames; x++)
		{
			cAudioFFTData fftFrame = audiotrack->getFFTSample(x);
			for (int y = 0; y < height; y++)
			{
				int y2 = height - y - 1;
				float value = 10.0f * fftFrame.data[y] / audiotrack->getMaxFft();

				QRgb pixel;
				if (value < 0.5f)
				{
					pixel = qRgba(0, int(value * 510), 0, 255);
				}
				else
				{
					if (value > 1.0f) value = 1.0f;
					pixel = qRgba(int((value - 0.5f) * 510), 255, 0, 255);
				}

				QRgb *line = reinterpret_cast<QRgb *>(fftImage.scanLine(y2));
				line[x] = pixel;
			}
		}

		UpdateScaledImage();
		update();
		WriteLog("FFTView created", 2);
	}
	else
	{
		fftImage = QImage();
		scaledFftImage = QImage();
	}
}

// the whole spectrum (0 Hz to the Nyquist frequency) is scaled to the height of the widget
double cFFTView::MaxFrequency() const
{
	return double(sampleRate) * 0.5;
}

int cFFTView::FrequencyToY(double frequency) const
{
	if (sampleRate <= 0) return height();
	return height() - int(frequency / MaxFrequency() * height());
}

double cFFTView::YToFrequency(int y) const
{
	if (height() <= 0) return 0.0;
	return double(height() - y) / double(height()) * MaxFrequency();
}

void cFFTView::UpdateScaledImage()
{
	if (!fftImage.isNull() && width() > 0 && height() > 0)
	{
		scaledFftImage =
			fftImage.scaled(width(), height(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
	}
	else
	{
		scaledFftImage = QImage();
	}
}

void cFFTView::slotFreqChanged(double midFreq, double bandwidth)
{
	lowFreq = midFreq - bandwidth * 0.5;
	highFreq = midFreq + bandwidth * 0.5;
	update();
}

void cFFTView::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
	UpdateScaledImage();
}

void cFFTView::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);
	QPainter painter(this);
	painter.drawImage(0, 0, scaledFftImage);

	QBrush brush(QColor(255, 255, 255, 128));
	painter.setBrush(brush);
	painter.setPen(Qt::NoPen);
	painter.drawRect(
		QRect(QPoint(0, FrequencyToY(highFreq) - 1), QPoint(width(), FrequencyToY(lowFreq) - 1)));

	painter.setPen(Qt::red);
	QFont font = QApplication::font();
	font.setPixelSize(10);
	painter.setFont(font);
	painter.drawText(3, 13, QString("%1 Hz").arg(int(MaxFrequency())));
	painter.drawText(3, height() / 2, QString("%1 Hz").arg(int(MaxFrequency() * 0.5)));
	painter.drawText(3, height() - 3, QString("%1 Hz").arg(0));
}

void cFFTView::mousePressEvent(QMouseEvent *event)
{
	firstClickedFreqValue = YToFrequency(mouseLocalY(event));
}

void cFFTView::mouseReleaseEvent(QMouseEvent *event)
{
	double secondClickedFreqValue = YToFrequency(mouseLocalY(event));

	emitNewFrequencyBand(firstClickedFreqValue, secondClickedFreqValue);
}

void cFFTView::mouseMoveEvent(QMouseEvent *event)
{
	double secondClickedFreqValue = YToFrequency(mouseLocalY(event));

	emitNewFrequencyBand(firstClickedFreqValue, secondClickedFreqValue);
}

void cFFTView::emitNewFrequencyBand(double freq1, double freq2)
{
	freq1 = qBound(0.0, freq1, double(sampleRate));
	freq2 = qBound(0.0, freq2, double(sampleRate));

	double bandWidth = fabs(freq2 - freq1);
	double midFreq = (freq1 + freq2) * 0.5;

	emit newFrequencySelected(midFreq, bandWidth);
}
