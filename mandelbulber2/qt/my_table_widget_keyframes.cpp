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

#include "my_table_widget_keyframes.hpp"
#include <QtCore>
#include <QMenu>
#include <QContextMenuEvent>
#include <QHeaderView>
#include "../src/global_data.hpp"

MyTableWidgetKeyframes::MyTableWidgetKeyframes(QWidget *parent) : QTableWidget(parent)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(tableContextMenuRequest(QPoint)));

	horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this->horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)), SLOT(columnContextMenuRequest(QPoint)));

	verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this->verticalHeader(), SIGNAL(customContextMenuRequested(QPoint)), SLOT(rowContextMenuRequest(QPoint)));
}

MyTableWidgetKeyframes::~MyTableWidgetKeyframes()
{
}

void MyTableWidgetKeyframes::tableContextMenuRequest(QPoint point)
{
	QModelIndex index = indexAt(point);
	int row = index.row();
	int column = index.column();

	QMenu *menu = new QMenu;

	QAction *actionRender = NULL;
	QAction *interpolateForward = NULL;

	if(row == 0)
	{
		actionRender = menu->addAction(tr("Render this keyframe"));
	}
	else
	{
		if(column < columnCount() - 1)
		{
			interpolateForward = menu->addAction(tr("Interpolate next frames"));
		}
	}

	QAction *selectedItem = menu->exec(viewport()->mapToGlobal(point));

	if (selectedItem)
	{
		if (selectedItem == actionRender)
		{
			gKeyframeAnimation->RenderFrame(column);
		}
		else if (selectedItem == interpolateForward)
		{
			gKeyframeAnimation->InterpolateForward(row, column);
		}
	}

	delete menu;
}

void MyTableWidgetKeyframes::columnContextMenuRequest(QPoint point)
{
	QMenu *menu = new QMenu;

	QAction *actionRender;
	QAction *actionDeleteTo;
	QAction *actionDeleteFrom;

	actionRender = menu->addAction(tr("Render this keyframe"));
	actionDeleteTo = menu->addAction(tr("Delete all keyframes to here"));
	actionDeleteFrom = menu->addAction(tr("Delete all keyframes from here"));

	int column = horizontalHeader()->logicalIndexAt(point);

	QAction *selectedItem = menu->exec(horizontalHeader()->viewport()->mapToGlobal(point));

	if (selectedItem)
	{
		if (selectedItem == actionRender)
		{
			gKeyframeAnimation->RenderFrame(column);
		}
		else if(selectedItem == actionDeleteFrom)
		{
			gKeyframeAnimation->DeleteFramesFrom(column);
		}
		else if(selectedItem == actionDeleteTo)
		{
			gKeyframeAnimation->DeleteFramesTo(column);
		}
	}

	delete menu;
}

void MyTableWidgetKeyframes::rowContextMenuRequest(QPoint point)
{
	int row = verticalHeader()->logicalIndexAt(point);

	QMenu *menu = new QMenu;

	if(row > 0)
	{
		QAction *actionDeleteParameter;

		QString name = gKeyframeAnimation->GetParameterName(row);
		actionDeleteParameter = menu->addAction(tr("Remove '%1' from animation").arg(name));

		QAction *selectedItem = menu->exec(verticalHeader()->viewport()->mapToGlobal(point));

		if (selectedItem)
		{
			if (selectedItem == actionDeleteParameter)
			{
				gKeyframes->RemoveAnimatedParameter(name);
				gKeyframeAnimation->RefreshTable();
			}
		}
	}
	else
	{
		QAction *actionRefresThumbnails = menu->addAction(tr("Refresh all thumbnails"));

		QAction *selectedItem = menu->exec(verticalHeader()->viewport()->mapToGlobal(point));

		if (selectedItem)
		{
			if (selectedItem == actionRefresThumbnails)
			{
				gKeyframeAnimation->RefreshTable();
			}
		}
	}

	delete menu;
}
