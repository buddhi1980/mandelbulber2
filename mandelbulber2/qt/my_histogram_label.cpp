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
 * Authors: Sebastian Jennen (jenzebas@gmail.com), Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * MyHistogramLabel class - promoted QLabel widget for displaying histogram data
 */

#include "my_histogram_label.h"

#include <algorithm>

#include <QLabel>
using std::max;

MyHistogramLabel::MyHistogramLabel(QWidget *parent) : QLabel(parent)
{
	barColor = QColor(255, 0, 0);
	backgroundColor = QColor(0, 0, 0);
	legendColor = QColor(255, 255, 255);
	maxColor = QColor(255, 255, 255, 200);
	legendX = 0;
	legendWidth = 15;
	legendHeight = 15;
}

MyHistogramLabel::~MyHistogramLabel()
{
	// delete pix;
}

void MyHistogramLabel::UpdateHistogram(const cHistogram &_histData)
{
	histData = _histData;
	update();
}

void MyHistogramLabel::RedrawHistogram(QPainter &painter) const
{
	// get max Element
	long maxH = 1; // 1 to prevent division by zero
	int extremeIndex = 0;
	int minIndex = 0;
	int maxIndex = 0;

	int size = histData.GetSize();

	// calculate statistics
	long long totalSum = 0;
	for (int i = 0; i <= size; i++)
	{
		totalSum += histData.GetHist(i);
	}

	if (totalSum > 0)
	{
		long long sum = 0;
		for (int i = 0; i <= size; i++)
		{
			if (histData.GetHist(i) > maxH)
			{
				maxH = histData.GetHist(i);
				extremeIndex = i;
			}
			sum += histData.GetHist(i);
			double prob = double(sum) / totalSum;
			if (prob < 0.003) minIndex = i + 1;
			if (prob < 0.997) maxIndex = i + 1;
		}
		double average = double(histData.GetSum()) / totalSum;

		if (histData.GetCount() > 0)
		{
			int legendWidthP1 = legendWidth + 1;
			int legendHeightP1 = legendHeight + 1;

			int drawWidth = width() - legendWidthP1;
			int drawHeight = height() - legendHeightP1;

			// draw background
			painter.setPen(QPen(backgroundColor));
			painter.setBrush(QBrush(backgroundColor));
			painter.drawRect(QRect(legendWidthP1, 0, drawWidth, drawHeight));

			painter.setPen(QPen(barColor));
			painter.setBrush(QBrush(barColor));

			// draw each column bar
			for (int i = 0; i < size; i++)
			{
				int height = int(double(drawHeight) * max(0L, histData.GetHist(i)) / maxH);

				painter.drawRect(QRect(legendWidthP1 + i * drawWidth / size, drawHeight - height,
					int(floor(1.0 * drawWidth / size)), height));
			}

			// draw max description
			painter.setPen(QPen(maxColor));
			painter.setBrush(QBrush(maxColor));

			painter.drawText(10, legendWidthP1 + 5,
				QString("min: ") + GetShortNumberDisplay(minIndex) + QString(", mode: ")
					+ GetShortNumberDisplay(extremeIndex) + QString(", max: ")
					+ GetShortNumberDisplay(maxIndex) + QString(", avg: ") + QString::number(average));
		}
	} // totalSum > 0
}

QString MyHistogramLabel::GetShortNumberDisplay(int val) const
{
	if (val < 10000)
	{
		return QString::number(val);
	}
	else if (val < 10000000)
	{
		return QString::number((val / 1000)) + "K";
	}
	return QString::number((val / 1000000)) + "M";
}

void MyHistogramLabel::DrawLegend(QPainter &painter)
{
	legendX = histData.GetSize();
	painter.setPen(QPen(backgroundColor));
	painter.setBrush(QBrush(backgroundColor));
	painter.drawRect(QRect(0, 0, width(), height()));

	painter.setPen(QPen(legendColor));
	painter.setBrush(QBrush(legendColor));

	// X-Axis
	painter.drawLine(legendWidth / 2, height() - legendHeight, width() - 1, height() - legendHeight);

	// Y-Axis
	painter.drawLine(legendWidth, 0, legendWidth, height() - legendWidth / 2);

	// Nuntius X-Axis

	int mul = (legendX > 250) ? 10 : 1;

	for (int i = 0; i < legendX; i++)
	{
		if (i % (5 * mul) == 0)
		{
			int xPos = legendWidth + (width() - legendWidth) * i / legendX;
			int nuntiusLength = (i % (10 * mul) == 0 ? legendHeight / 2 : legendHeight / 4);
			painter.drawLine(
				xPos, height() - legendHeight, xPos, height() - legendHeight + nuntiusLength);
		}
	}

	painter.drawText(3, height() - 1, "0");

	painter.drawText(width() - 26, height() - 1, QString::number(legendX));
}

void MyHistogramLabel::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
	// delete pix;
	// pix = new QPixmap(event->size().width(), event->size().height());
}

void MyHistogramLabel::paintEvent(QPaintEvent *event)
{
	QWidget::paintEvent(event);
	QPainter painter(this);
	DrawLegend(painter);

	if (histData.GetCount() > 0)
	{
		RedrawHistogram(painter);
	}
}
