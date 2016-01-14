/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyTabBar class - promoted MyTabBar widget with drag and drop functionality
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

#include "mytabbar.h"
#include "../src/parameters.hpp"
#include <QLineEdit>
#include "../src/global_data.hpp"

MyTabBar::MyTabBar(QWidget *parent) :	QTabBar(parent)
{
	setMovable (true);
}

void MyTabBar::mouseReleaseEvent(QMouseEvent *event)
{
	emit dragDropChange();
}

