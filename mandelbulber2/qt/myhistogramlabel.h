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
#include "../src/histogram.hpp"

class MyHistogramLabel : public QLabel
{
	Q_OBJECT

public:
	MyHistogramLabel(QWidget *parent = 0);
	~MyHistogramLabel();

	void SetBarcolor(const QColor& c){ barColor = c; }
	QColor GetBarcolor(){ return barColor; }

	void SetBackgroundcolor(const QColor& c){ backgroundColor = c; }
	QColor GetBackgroundcolor(){ return backgroundColor; }

	void SetLegendcolor(const QColor& c){ legendColor = c; }
	QColor GetLegendcolor(){ return legendColor; }

	void SetMaxcolor(const QColor& c){ maxColor = c; }
	QColor GetMaxcolor(){ return maxColor; }

	void UpdateHistogram(const cHistogram &histData);

protected:
	void resizeEvent(QResizeEvent *event);
	void paintEvent(QPaintEvent *event);

private:
	int getLegendY(int y);
	void DrawLegend(QPainter &painter);
	void RedrawHistogram(QPainter &painter);
	QString GetShortNumberDisplay(int val);

	QColor barColor;
	QColor backgroundColor;
	QColor legendColor;
	QColor maxColor;
	int legendX;
	bool isUpdating;
	int legendWidth;
	int legendHeight;
	int pixmapWidth;
	int pixmapHeight;

	cHistogram histData;
};

#endif /* MYHISTOGRAMLABEL_HPP_ */
