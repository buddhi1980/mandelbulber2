/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyDoubleSpinBox class - promoted QDoubleSpinBox widget with context menu
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

#ifndef MYDOUBLESPINBOX_HPP_
#define MYDOUBLESPINBOX_HPP_

#include <QtGui>
#include <QtCore>
#include <QDoubleSpinBox>
#include <QMenu>
#include "../src/parameters.hpp"

class MyDoubleSpinBox : public QDoubleSpinBox
{
	Q_OBJECT

public:
	MyDoubleSpinBox(QWidget *parent = 0)  : QDoubleSpinBox(parent)
	{
		actionResetToDefault = NULL;
		actionAddToFlightAnimation = NULL;
		actionAddToKeyframeAnimation = NULL;
		parameterContainer = NULL;
		gotDefault = false;
		defaultValue = 0;
	};

	void AssignParameterContainer(cParameterContainer *container) {parameterContainer = container;}
	void AssingParameterName(QString name) {parameterName = name;}

private:
	QAction *actionResetToDefault;
	QAction *actionAddToFlightAnimation;
	QAction *actionAddToKeyframeAnimation;
	QString GetType(const QString &name);
	cParameterContainer *parameterContainer;
	QString parameterName;

	double GetDefault();
	double defaultValue;
	bool gotDefault;

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	void paintEvent(QPaintEvent *event);
};



#endif /* MYDOUBLESPINBOX_HPP_ */
