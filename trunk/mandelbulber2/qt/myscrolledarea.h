/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyScrolledArea class - promoted QScrollArea widget with custom resize event
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#ifndef MYSCROLLEDAREA_HPP_
#define MYSCROLLEDAREA_HPP_

#include <QtGui>
#include <QScrollBar>
#include <QScrollArea>

class MyScrolledArea : public QScrollArea
{
	Q_OBJECT

public:
	MyScrolledArea(QWidget *parent = 0)  : QScrollArea(parent)
	{
		visibleAreaWidth = width() - verticalScrollBar()->width();
		visibleAreaHeight = height() - horizontalScrollBar()->height();
	};
	int VisibleAreaWidth() {return visibleAreaWidth;}
	int VisibleAreaHeight() {return visibleAreaHeight;}

private:
	int visibleAreaWidth;
	int visibleAreaHeight;

protected:
	void resizeEvent(QResizeEvent *event)
	{
		//qDebug() << "resize event";
		QScrollArea::resizeEvent(event);
		visibleAreaWidth = width() - verticalScrollBar()->width();
		visibleAreaHeight = height() - horizontalScrollBar()->height();
		emit resized(visibleAreaWidth, visibleAreaHeight);
	}

signals:
	void resized(const int &newWidth, const int &newHeight);
};



#endif /* MYSCROLLEDAREA_HPP_ */
