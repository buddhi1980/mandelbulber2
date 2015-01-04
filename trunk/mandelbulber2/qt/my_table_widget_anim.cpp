/*
 * my_table_widget_anim.cpp
 *
 *  Created on: Jan 1, 2015
 *      Author: krzysztof
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
