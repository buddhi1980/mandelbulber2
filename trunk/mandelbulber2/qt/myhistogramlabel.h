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

#ifndef MYHISTOGRAMLABEL_HPP_
#define MYHISTOGRAMLABEL_HPP_

#include <QtGui>
#include <QtCore>
#include <QLabel>
#include "../src/parameters.hpp"

class MyHistogramLabel : public QLabel
{
	Q_OBJECT

public:
	MyHistogramLabel(QWidget *parent = 0);
	~MyHistogramLabel();

	void UpdateHistogram(long histogram[], int size, int count);
	void SetBarcolor(QColor c){ barColor = &c; }
	void SetBackgroundcolor(QColor c){ backgroundColor = &c; }
	void SetLegendcolor(QColor c){ legendColor = &c; }
	void SetMaxcolor(QColor c){ maxColor = &c; }

protected:
	void resizeEvent(QResizeEvent *event);

private:
	int getLegendY(int y);
	void DrawLegend();
	QString GetShortNumberDisplay(int val);

	QPixmap *pix;
	QPainter *painter;
	QColor *barColor;
	QColor *backgroundColor;
	QColor *legendColor;
	QColor *maxColor;
	int legendX;
	bool isUpdating;
	int legendWidth;
	int legendHeight;
};

#endif /* MYHISTOGRAMLABEL_HPP_ */
