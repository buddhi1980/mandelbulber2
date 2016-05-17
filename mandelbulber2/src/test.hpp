/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * class for testing mandelbulber functionality on the cli
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

#ifndef MANDELBULBER2_TEST_HPP_
#define MANDELBULBER2_TEST_HPP_

#include <QWidget>
#include <QtTest/QtTest>

class Test: public QObject
{
	Q_OBJECT
private slots:
	void renderExamples();
	void netrender();
};

#endif /* MANDELBULBER2_TEST_HPP_ */
