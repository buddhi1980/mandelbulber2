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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * MyHistogramLabel class - promoted QLabel widget for displaying histogram data
 */

#ifndef MANDELBULBER2_QT_MY_HISTOGRAM_LABEL_H_
#define MANDELBULBER2_QT_MY_HISTOGRAM_LABEL_H_

#include "../src/histogram.hpp"
#include <QLabel>
#include <QtCore>
#include <QtGui>

class MyHistogramLabel : public QLabel
{
	Q_OBJECT

public:
	MyHistogramLabel(QWidget *parent = 0);
	~MyHistogramLabel();

	void SetBarcolor(const QColor &c) { barColor = c; }
	QColor GetBarcolor() { return barColor; }

	void SetBackgroundcolor(const QColor &c) { backgroundColor = c; }
	QColor GetBackgroundcolor() { return backgroundColor; }

	void SetLegendcolor(const QColor &c) { legendColor = c; }
	QColor GetLegendcolor() { return legendColor; }

	void SetMaxcolor(const QColor &c) { maxColor = c; }
	QColor GetMaxcolor() { return maxColor; }

	void UpdateHistogram(const cHistogram &histData);

protected:
	void resizeEvent(QResizeEvent *event);
	void paintEvent(QPaintEvent *event);

private:
	void DrawLegend(QPainter &painter);
	void RedrawHistogram(QPainter &painter);
	QString GetShortNumberDisplay(int val);

	QColor barColor;
	QColor backgroundColor;
	QColor legendColor;
	QColor maxColor;
	int legendX;
	int legendWidth;
	int legendHeight;

	cHistogram histData;
};

#endif /* MANDELBULBER2_QT_MY_HISTOGRAM_LABEL_H_ */
