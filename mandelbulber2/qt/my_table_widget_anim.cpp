/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * MyTableWidgetAnim class -  to display flight animation frames
 */

#include "my_table_widget_anim.hpp"
#include "../src/animation_flight.hpp"
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QMenu>
#include <QtCore>

MyTableWidgetAnim::MyTableWidgetAnim(QWidget *parent) : QTableWidget(parent)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this, SIGNAL(customContextMenuRequested(QPoint)), SLOT(tableContextMenuRequest(QPoint)));

	horizontalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this->horizontalHeader(), SIGNAL(customContextMenuRequested(QPoint)),
		SLOT(columnContextMenuRequest(QPoint)));

	verticalHeader()->setContextMenuPolicy(Qt::CustomContextMenu);
	connect(this->verticalHeader(), SIGNAL(customContextMenuRequested(QPoint)),
		SLOT(rowContextMenuRequest(QPoint)));
}

MyTableWidgetAnim::~MyTableWidgetAnim()
{
}

void MyTableWidgetAnim::tableContextMenuRequest(QPoint point) const
{
	QModelIndex index = indexAt(point);
	int row = index.row();
	int column = index.column();

	QMenu *menu = new QMenu;

	QAction *actionRender = nullptr;
	QAction *interpolateForward = nullptr;

	if (row == 0)
	{
		actionRender = menu->addAction(tr("Render this frame"));
	}
	else
	{
		if (column < columnCount() - 1 && column >= 0)
		{
			interpolateForward = menu->addAction(tr("Interpolate next frames"));
		}
	}

	QAction *selectedItem = menu->exec(viewport()->mapToGlobal(point));

	if (selectedItem)
	{
		if (selectedItem == actionRender)
		{
			gFlightAnimation->RenderFrame(column);
		}
		else if (selectedItem == interpolateForward)
		{
			gFlightAnimation->InterpolateForward(row, column);
		}
	}

	delete menu;
}

void MyTableWidgetAnim::columnContextMenuRequest(QPoint point) const
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
		else if (selectedItem == actionDeleteFrom)
		{
			gFlightAnimation->DeleteFramesFrom(column);
		}
		else if (selectedItem == actionDeleteTo)
		{
			gFlightAnimation->DeleteFramesTo(column);
		}
	}

	delete menu;
}

void MyTableWidgetAnim::rowContextMenuRequest(QPoint point) const
{
	int row = verticalHeader()->logicalIndexAt(point);

	QMenu *menu = new QMenu;

	if (row > 0)
	{
		QAction *actionDeleteParameter;

		QString name = gFlightAnimation->GetParameterName(row);
		actionDeleteParameter = menu->addAction(tr("Remove '%1' from animation").arg(name));

		QAction *selectedItem = menu->exec(verticalHeader()->viewport()->mapToGlobal(point));

		if (selectedItem)
		{
			if (selectedItem == actionDeleteParameter)
			{
				gAnimFrames->RemoveAnimatedParameter(name);
				gFlightAnimation->RefreshTable();
			}
		}
	}
	else
	{
		QAction *actionRefreshThumbnails = menu->addAction(tr("Refresh all thumbnails"));

		QAction *selectedItem = menu->exec(verticalHeader()->viewport()->mapToGlobal(point));

		if (selectedItem)
		{
			if (selectedItem == actionRefreshThumbnails)
			{
				gFlightAnimation->RefreshTable();
			}
		}
	}

	delete menu;
}
