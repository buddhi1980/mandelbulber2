/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyTableWidgetAnim class -  to display animation frames
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


#ifndef QT_MY_TABLE_WIDGET_ANIM_HPP_
#define QT_MY_TABLE_WIDGET_ANIM_HPP_

#include <QTableWidget>

class MyTableWidgetAnim : public QTableWidget
{
	Q_OBJECT

public:
	MyTableWidgetAnim(QWidget *parent = 0);
	~MyTableWidgetAnim();

private:


public slots:
	void tableContextMenuRequest(QPoint point);
	void columnContextMenuRequest(QPoint point);
	void rowContextMenuRequest(QPoint point);
};

#endif /* QT_MY_TABLE_WIDGET_ANIM_HPP_ */
