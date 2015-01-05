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

#include "my_table_widget_anim.hpp"
#include <QtCore>
#include <QMenu>
#include <QContextMenuEvent>
#include <QHeaderView>
#include "../src/global_data.hpp"

MyTableWidgetAnim::MyTableWidgetAnim(QWidget *parent) : QTableWidget(parent)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(tableContextMenuRequest(QPoint)));

	horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this->horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)), SLOT(columnContextMenuRequest(QPoint)));

	verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this->verticalHeader(), SIGNAL(customContextMenuRequested(QPoint)), SLOT(rowContextMenuRequest(QPoint)));
}

MyTableWidgetAnim::~MyTableWidgetAnim()
{
}

void MyTableWidgetAnim::tableContextMenuRequest(QPoint point)
{
	/*
	QMenu *menu = new QMenu;
	QAction *action;

	action = menu->addAction(tr("Render this frame"));

	QModelIndex index = indexAt(point);
	qDebug() << index;
	int row = index.row();
	int column = index.column();

	QAction *selectedItem = menu->exec(viewport()->mapToGlobal(point));
	*/
}

void MyTableWidgetAnim::columnContextMenuRequest(QPoint point)
{
	QMenu *menu = new QMenu;

	QAction *actionRender;
	QAction *actionDeleteTo;
	QAction *actionDeleteFrom;

	actionRender = menu->addAction(tr("Render this frame"));
	actionDeleteTo = menu->addAction(tr("Delete all frames to here"));
	actionDeleteFrom = menu->addAction(tr("Delete all frames from here"));

	int column = horizontalHeader()->logicalIndexAt(point);

	QAction *selectedItem = menu->exec(horizontalHeader()->viewport()->mapToGlobal(point));

	if (selectedItem)
	{
		if (selectedItem == actionRender)
		{
			gFlightAnimation->RenderFrame(column);
		}
		else if(selectedItem == actionDeleteFrom)
		{
			gFlightAnimation->DeleteFramesFrom(column);
		}
		else if(selectedItem == actionDeleteTo)
		{
			gFlightAnimation->DeleteFramesTo(column);
		}
	}
}

void MyTableWidgetAnim::rowContextMenuRequest(QPoint point)
{
	int row = verticalHeader()->logicalIndexAt(point);

	if(row > 0)
	{
		QMenu *menu = new QMenu;
		QAction *actionDeleteParameter;

		QString name = gFlightAnimation->GetParameterName(row);
		actionDeleteParameter = menu->addAction(tr("Remove '") + name + tr("' from animation"));

		QAction *selectedItem = menu->exec(verticalHeader()->viewport()->mapToGlobal(point));

		if (selectedItem)
		{
			if (selectedItem == actionDeleteParameter)
			{
				gAnimFrames->RemoveAnimagedParameter(name);
				gFlightAnimation->RefreshTable();
			}
		}
	}
}
