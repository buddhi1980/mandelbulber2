/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyCheckBox class - promoted QCheckBox widget with context menu
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

#ifndef MYCHECKBOX_HPP_
#define MYCHECKBOX_HPP_

#include <QtGui>
#include <QtCore>
#include <QCheckBox>
#include <QMenu>
#include "../src/parameters.hpp"

class MyCheckBox : public QCheckBox
{
	Q_OBJECT

public:
	MyCheckBox(QWidget *parent = 0)  : QCheckBox(parent)
	{
		actionResetToDefault = NULL;
		parameterContainer = NULL;
		gotDefault = false;
		defaultValue = 0;
	};

	void AssignParameterContainer(cParameterContainer *container) {parameterContainer = container;}
	void AssingParameterName(QString name) {parameterName = name;}

private:
	QAction *actionResetToDefault;
	QString GetType(const QString &name);
	cParameterContainer *parameterContainer;
	QString parameterName;

	bool GetDefault();
	bool defaultValue;
	bool gotDefault;

protected:
	void contextMenuEvent(QContextMenuEvent *event);
	void paintEvent(QPaintEvent *event);
};



#endif /* MYCHECKBOX_HPP_ */
