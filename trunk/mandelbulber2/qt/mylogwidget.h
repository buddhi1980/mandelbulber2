/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * QPlainTextEdit class - promoted QPlainTextEdit widget for Log File view
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
* Authors: Sebastian Jennen
 */

#ifndef MYLOGWIDGET_HPP_
#define MYLOGWIDGET_HPP_

#include <QtGui>
#include <QtCore>
#include <QPlainTextEdit>
#include "../src/system.hpp"

class MyLogWidget : public QPlainTextEdit
{
	Q_OBJECT
public:
	MyLogWidget(QWidget *parent = 0)  : QPlainTextEdit(parent)
	{
		this->setReadOnly(true);
		initializedFromLogFile = false;
	};

public slots:
	void appendMessage(const QString& text);
	void initFromLogFile();
	QString formatLine(const QString& text);

private:
	bool initializedFromLogFile;
};

#endif /* MYLOGWIDGET_HPP_ */
