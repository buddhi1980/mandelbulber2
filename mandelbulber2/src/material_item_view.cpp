/*
 * material_item_view.cpp
 *
 *  Created on: 20 maj 2016
 *      Author: krzysztof
 */

#include "material_item_view.h"
#include "../qt/material_widget.h"
#include "system.hpp"
#include <qpushbutton.h>
#include <qscrollbar.h>

int cMaterialItemView::iconMargin = 10;

cMaterialItemView::cMaterialItemView(QWidget *parent) : QAbstractItemView(parent)
{
//cMaterialWidget *widget = new cMaterialWidget(128, 128, 2, this);
//	widget->setGeometry(10, 10, 128, 128);
//	setIndexWidget(QModelIndex(), widget);
	setMinimumHeight(150);
	setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	horizontalScrollBar()->setVisible(true);
	setAutoScroll(true);
}

cMaterialItemView::~cMaterialItemView()
{
	// TODO Auto-generated destructor stub
}

QModelIndex cMaterialItemView::indexAt(const QPoint& point) const
{
	return QModelIndex(model()->index(point.x() / (cMaterialWidget::previewWidth + iconMargin), 0, QModelIndex()));
}

QRegion cMaterialItemView::visualRegionForSelection(const QItemSelection& selection) const
{
	return QRegion();
}

void cMaterialItemView::scrollTo(const QModelIndex& index, ScrollHint hint)
{
}

int cMaterialItemView::verticalOffset() const
{
	return verticalScrollBar()->value();
}

int cMaterialItemView::horizontalOffset() const
{
	return horizontalScrollBar()->value();
}

void cMaterialItemView::setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags flags)
{
}

QModelIndex cMaterialItemView::moveCursor(CursorAction cursorAction,
		Qt::KeyboardModifiers modifiers)
{
	return QModelIndex();
}

QRect cMaterialItemView::visualRect(const QModelIndex& index) const
{
	return QRect(index.row() * (cMaterialWidget::previewWidth + iconMargin), 0, cMaterialWidget::previewWidth, cMaterialWidget::previewHeight);
}

bool cMaterialItemView::isIndexHidden(const QModelIndex& index) const
{
	return false;
}

void cMaterialItemView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
	qDebug() << "dataChanged:" << topLeft << bottomRight << roles;
	QString settings = model()->data(topLeft).toString();
	cMaterialWidget *widget = (cMaterialWidget*)indexWidget(topLeft);
	if(widget)
	{
		widget->AssignMaterial(settings);
	}
	qDebug() << settings;
	qDebug() << indexWidget(topLeft);
	qDebug() << indexWidget(topLeft)->sizeHint();
	qDebug() << visualRect(topLeft);
}

void	cMaterialItemView::rowsInserted(const QModelIndex &parent, int start, int end)
{

	qDebug() << "rowsInserted:" << parent << start << end;
	for(int r = start; r <= end; r++)
	{
		cMaterialWidget *widget = new cMaterialWidget(128, 128, 2, this);
		//QPushButton *widget = new QPushButton(this);
		qDebug() << widget;
		widget->setGeometry(10 + r * 138, 10, 128, 128);
		widget->setAutoFillBackground(true);
		setIndexWidget(model()->index(r, 0, parent), widget);
	}
	QAbstractItemView::rowsInserted(parent, start, end);
	updateGeometries();

  viewport()->update();
}

void cMaterialItemView::resizeEvent(QResizeEvent *event)
{
	QAbstractItemView::resizeEvent(event);
	if(model())
	{
    horizontalScrollBar()->setRange(0, model()->rowCount() * (cMaterialWidget::previewWidth + iconMargin) - width());
	}
}

void cMaterialItemView::scrollContentsBy(int dx, int dy)
{
    scrollDirtyRegion(dx, dy);
    viewport()->scroll(dx, dy);
}
