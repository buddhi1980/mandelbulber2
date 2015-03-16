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
	pix = new QPixmap(width(), height());
	painter = new QPainter(pix);
	barColor = new QColor(255, 0, 0);
	backgroundColor = new QColor(0, 0, 0);
	legendColor = new QColor(255, 255, 255);
	maxColor = new QColor(255, 255, 255, 200);
	legendX = 0;
	isUpdating = false;
	legendWidth = 15;
	legendHeight = 15;
}

MyHistogramLabel::~MyHistogramLabel(void)
{
	delete painter;
	delete pix;
	delete barColor;
	delete backgroundColor;
	delete legendColor;
	delete maxColor;
}

void MyHistogramLabel::UpdateHistogram(long histogram[], int size, int count)
{
	if(isUpdating) return;
	isUpdating = true;

	// get max Element
	long max = 0;
	int maxIndex = 0;

	for (int i = 0; i < size; i++)
	{
		if (histogram[i] > max) {
			max = histogram[i];
			maxIndex = i;
		}
	}
	int maxAVG = max / count;

	// redraw legend, if necessary
	if(legendX != size)
	{
		legendX = size;
		DrawLegend();
	}

	int legendWidthP1 = legendWidth + 1;
	int legendHeightP1 = legendHeight + 1;

	int drawWidth = pix->width() - legendWidthP1;
	int drawHeight = pix->height() - legendHeightP1;

	// draw background
	painter->setPen(QPen(*backgroundColor));
	painter->setBrush(QBrush(*backgroundColor));
	painter->drawRect(QRect(legendWidthP1, 0, drawWidth, drawHeight));

	painter->setPen(QPen(*barColor));
	painter->setBrush(QBrush(*barColor));

	// draw each column bar
	for (int i = 0; i < size; i++)
	{
		int height = drawHeight * fmax(0, histogram[i]) / max;

		painter->drawRect(
			QRect(legendWidthP1 + i * drawWidth / size,
				drawHeight - height,
				ceil(1.0 * drawWidth / size),
				height)
		);
	}

	// draw max description
	painter->setPen(QPen(*maxColor));
	painter->setBrush(QBrush(*maxColor));

	painter->drawText(
				fmin(legendWidthP1 + (maxIndex * drawWidth / size) + 20, pix->width() - 100), 20,
				"max: " + GetShortNumberDisplay(maxAVG));

	setPixmap(*pix);
	isUpdating = false;
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

void MyHistogramLabel::DrawLegend()
{
	// draw background
	painter->setPen(QPen(*backgroundColor));
	painter->setBrush(QBrush(*backgroundColor));
	painter->drawRect(QRect(0, 0, pix->width(), pix->height()));

	painter->setPen(QPen(*legendColor));
	painter->setBrush(QBrush(*legendColor));

	// X-Axis
	painter->drawLine(legendWidth / 2, pix->height() - legendHeight, pix->width() - 1, pix->height() - legendHeight);

	// Y-Axis
	painter->drawLine(legendWidth, 0, legendWidth, pix->height() - legendWidth / 2);

	// Nuntius X-Axis
	for(int i = 0; i < legendX; i++){
		if(i % 5 == 0){
			int xPos = legendWidth + (pix->width() - legendWidth) * i / legendX;
			int nuntiusLength = (i % 10 == 0 ? legendHeight / 2 : legendHeight / 4);
			painter->drawLine(xPos, pix->height() - legendHeight, xPos, pix->height() - legendHeight + nuntiusLength);
		}
	}

	painter->drawText(3, pix->height() - 1, "0");

	painter->drawText(pix->width() - 26, pix->height() - 1, QString::number(legendX));
}

void MyHistogramLabel::resizeEvent(QResizeEvent *event)
{
	pix = new QPixmap(event->size().width(), event->size().height());
	painter = new QPainter(pix);
	DrawLegend();
}
