/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyColorButton class - promoted QPushButton widget with context menu
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

#ifndef MYCOLORBUTTON_HPP_
#define MYCOLORBUTTON_HPP_

#include <QtGui>
#include <QtCore>
#include <QPushButton>
#include <QMenu>
#include "../src/parameters.hpp"

class MyColorButton : public QPushButton
{
	Q_OBJECT

public:
	MyColorButton(QWidget *parent = 0);
	~MyColorButton();

	sRGB GetColor();
	void SetColor(sRGB newColor);
	void SetupColor();

	void AssignParameterContainer(cParameterContainer *container) {parameterContainer = container;}
	void AssingParameterName(QString name) {parameterName = name;}

private:
	QAction *actionResetToDefault;
	QAction *actionAddToFlightAnimation;
	QAction *actionAddToKeyframeAnimation;
	QString GetType(const QString &name);
	cParameterContainer *parameterContainer;
	QString parameterName;

	void mousePressEvent(QMouseEvent *event);
	void SetDimension();
	sRGB GetDefault();

	sRGB defaultValue;
	sRGB currentValue;
	bool gotDefault;

	int w;
	int h;
	QPixmap *pix;
	QPainter *painter;

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	void UpdateColor();
};



#endif /* MYCOLORBUTTON_HPP_ */
