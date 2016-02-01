/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyTabWidget class - promoted QTabWidget widget with drag and drop functionality (through mytabbar)
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

#include "mytabwidget.h"
#include "mytabbar.h"
#include <QPushButton>

MyTabWidget::MyTabWidget(QWidget *parent) : QTabWidget(parent)
{
	MyTabBar* bar = new MyTabBar();
	connect(bar, SIGNAL(swapTabs(int, int)), this, SIGNAL(swapTabs(int, int)));
	this->setTabBar(bar);
	bar->setupMoveButtons();
}



/*
void MyTabWidget::slotDragDropChange()
{
	QRegularExpression re("^.*([0-9])+$");
	for(int l = 0; l < count(); l++)
	{
		QRegularExpressionMatch matchL = re.match(widget(l)->objectName());
		if (!matchL.hasMatch()) return;
		int indexInWidgetL = matchL.captured(1).toInt() - 1;
		if(indexInWidgetL != l)
		{
			for(int r = count() - 1; r >= 0; r--)
			{
				QRegularExpressionMatch matchR = re.match(widget(r)->objectName());
				if (!matchR.hasMatch()) return;
				int indexInWidgetR = matchR.captured(1).toInt() - 1;
				if(indexInWidgetR != r)
				{
					emit swapTabs(l, r);
					return;
				}
			}
		}
	}
}
*/
