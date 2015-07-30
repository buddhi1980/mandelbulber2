/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyHistogramLabel class - promoted QLabel widget for displaying histogram data
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 */

#include "myhistogramlabel.h"
#include "../src/parameters.hpp"
#include <QLabel>
#include "../src/global_data.hpp"
#include <algorithm>

MyHistogramLabel::MyHistogramLabel(QWidget *parent) : QLabel(parent)
{
	barColor = QColor(255, 0, 0);
	backgroundColor = QColor(0, 0, 0);
	legendColor = QColor(255, 255, 255);
	maxColor = QColor(255, 255, 255, 200);
	legendX = 0;
	isUpdating = false;
	legendWidth = 15;
	legendHeight = 15;
	pixmapWidth = width();
	pixmapHeight = height();
}

MyHistogramLabel::~MyHistogramLabel(void)
{
	//delete pix;
}

void MyHistogramLabel::UpdateHistogram(const cHistogram &_histData)
{
	histData = _histData;
	update();
}

void MyHistogramLabel::RedrawHistogram(QPainter &painter)
{
	// get max Element
	long maxH = 1; //1 to prevent division by zero
	int extrIndex = 0;
	int minIndex = 0;
	int maxIndex = 0;

  int size = histData.GetSize();

  long long sum = 0;
	for (int i = 0; i <= size; i++)
	{
		if (histData.GetHist(i) > maxH) {
			maxH = histData.GetHist(i);
			extrIndex = i;
		}
		sum += histData.GetHist(i);
		double prob = (double)sum / histData.GetCount();
		if (prob < 0.0062) minIndex = i + 1;
		if (prob < 0.9938) maxIndex = i;
	}

	if(histData.GetCount() > 0)
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
			int height = (double)drawHeight * max(0L, histData.GetHist(i)) / maxH;

			painter.drawRect(
				QRect(legendWidthP1 + i * drawWidth / size,
					drawHeight - height,
					floor(1.0 * drawWidth / size),
					height)
			);
		}

		// draw max description
		painter.setPen(QPen(maxColor));
		painter.setBrush(QBrush(maxColor));

		painter.drawText(
					fmin(legendWidthP1 + (extrIndex * drawWidth / size) + 20, width() - 100), 20,
					QString("min: ")	+ GetShortNumberDisplay(minIndex)
					+ QString(", extr: ")	+ GetShortNumberDisplay(extrIndex)
					+ QString(", max: ")	+ GetShortNumberDisplay(maxIndex)
					+ QString(", avg: ")	+ QString::number((double)histData.GetSum() / histData.GetCount()));

	}
}

QString MyHistogramLabel::GetShortNumberDisplay(int val)
{
	if(val < 10000){
		return QString::number(val);
	}
	else if(val < 10000000){
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

	for(int i = 0; i < legendX; i++){
		if(i % (5 * mul) == 0){
			int xPos = legendWidth + (width() - legendWidth) * i / legendX;
			int nuntiusLength = (i % (10 * mul) == 0 ? legendHeight / 2 : legendHeight / 4);
			painter.drawLine(xPos, height() - legendHeight, xPos, height() - legendHeight + nuntiusLength);
		}
	}

	painter.drawText(3, height() - 1, "0");

	painter.drawText(width() - 26, height() - 1, QString::number(legendX));
}

void MyHistogramLabel::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
	//delete pix;
	//pix = new QPixmap(event->size().width(), event->size().height());
}

void MyHistogramLabel::paintEvent(QPaintEvent *event)
{
	QWidget::paintEvent(event);
	QPainter painter(this);
	DrawLegend(painter);

	if(histData.GetCount() > 0)
	{
		RedrawHistogram(painter);
	}
}
