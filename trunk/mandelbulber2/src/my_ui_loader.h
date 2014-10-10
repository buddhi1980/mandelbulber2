/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyUiLoader class - extension for QUiLoader class
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

#ifndef MYUILOADER_H_
#define MYUILOADER_H_

#include <QtUiTools/QtUiTools>

class MyUiLoader: public QUiLoader
{
	 Q_OBJECT
public:
	MyUiLoader(QObject *parent = 0) : QUiLoader(parent) {}
	~MyUiLoader();

	virtual QWidget* createWidget(const QString &className, QWidget *parent = 0, const QString &name = QString());
};

#endif /* MYUILOADER_H_ */
