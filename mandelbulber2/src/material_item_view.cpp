/*
 * material_item_view.cpp
 *
 *  Created on: 20 maj 2016
 *      Author: krzysztof
 */

#include "material_item_view.h"

#include <qpainter.h>

#include "../qt/material_widget.h"
#include "system.hpp"
#include <qpushbutton.h>
#include <qscrollbar.h>

cMaterialItemView::cMaterialItemView(QWidget *parent) :
		QAbstractItemView(parent)
{
	updateNameHeight();
	iconMargin = 10;
	if(horizontalScrollBar()->isVisible())
		viewHeight = cMaterialWidget::previewHeight + horizontalScrollBar()->height() + iconMargin * 2 + maxNameHeight;
	else
		viewHeight = cMaterialWidget::previewHeight + iconMargin * 2 + maxNameHeight;

	setMinimumHeight(viewHeight);
	setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	setAutoScroll(true);
}

cMaterialItemView::~cMaterialItemView()
{
}

QModelIndex cMaterialItemView::indexAt(const QPoint& point) const
{
	int r = (point.x() + horizontalOffset() - iconMargin)
			/ (cMaterialWidget::previewWidth + iconMargin);
	return QModelIndex(model()->index(r, 0, QModelIndex()));
}

QRegion cMaterialItemView::visualRegionForSelection(const QItemSelection& selection) const
{
	Q_UNUSED(selection);
	return QRegion();
}

void cMaterialItemView::scrollTo(const QModelIndex& index, ScrollHint hint)
{
	Q_UNUSED(hint);
	horizontalScrollBar()->setValue(iconMargin + index.row() * (cMaterialWidget::previewWidth + iconMargin));
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
	Q_UNUSED(flags);
	Q_UNUSED(rect);
	viewport()->update();
}

QModelIndex cMaterialItemView::moveCursor(CursorAction cursorAction,
		Qt::KeyboardModifiers modifiers)
{
	Q_UNUSED(modifiers);
	QModelIndex newIndex;
	if(cursorAction == MoveRight)
	{
		newIndex = model()->index(currentIndex().row() + 1, 0);
	}
	else if(cursorAction == MoveLeft)
	{
		newIndex = model()->index(currentIndex().row() - 1, 0);
	}
	else if(cursorAction == MoveHome)
	{
		newIndex = model()->index(0, 0);
	}
	else if(cursorAction == MoveEnd)
	{
		newIndex = model()->index(model()->rowCount() - 1, 0);
	}
	return newIndex;
}

QRect cMaterialItemView::visualRect(const QModelIndex& index) const
{
	return QRect(	iconMargin + index.row() * (cMaterialWidget::previewWidth + iconMargin) - horizontalOffset(),
								iconMargin,
								cMaterialWidget::previewWidth,
								cMaterialWidget::previewHeight + maxNameHeight);
}

bool cMaterialItemView::isIndexHidden(const QModelIndex& index) const
{
	Q_UNUSED(index);
	return false;
}

void cMaterialItemView::dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
		const QVector<int> &roles)
{
	Q_UNUSED(bottomRight);
	Q_UNUSED(roles);
	QString settings = model()->data(topLeft).toString();
	cMaterialWidget *widget = (cMaterialWidget*) indexWidget(topLeft);
	if (widget)
	{
		int materialId = model()->data(topLeft, Qt::UserRole).toInt();
		widget->AssignMaterial(settings, materialId);
	}

	viewport()->update();
}

void cMaterialItemView::rowsInserted(const QModelIndex &parent, int start, int end)
{
	for (int r = start; r <= end; r++)
	{
		cMaterialWidget *widget = new cMaterialWidget(this);
		widget->setAutoFillBackground(true);
		setIndexWidget(model()->index(r, 0, parent), widget);
	}
	QAbstractItemView::rowsInserted(parent, start, end);
	//updateGeometries();

	updateScrollBar();
	updateGeometries();

	setCurrentIndex(model()->index(start,0));
	scrollTo(model()->index(start,0));

	model()->fetchMore(QModelIndex());
	viewport()->update();
}

void cMaterialItemView::resizeEvent(QResizeEvent *event)
{
	QAbstractItemView::resizeEvent(event);
	if (model())
	{
		updateScrollBar();
	}

	updateNameHeight();
	if(horizontalScrollBar()->isVisible())
		viewHeight = cMaterialWidget::previewHeight + horizontalScrollBar()->height() + iconMargin * 2 + maxNameHeight;
	else
		viewHeight = cMaterialWidget::previewHeight + iconMargin * 2 + maxNameHeight;
	setMinimumHeight(viewHeight);
}

void cMaterialItemView::scrollContentsBy(int dx, int dy)
{
	scrollDirtyRegion(dx, dy);
	viewport()->scroll(dx, dy);
	viewport()->update();
}

void cMaterialItemView::paintEvent(QPaintEvent *event)
{
	QAbstractItemView::paintEvent(event);
	QPainter painter(viewport());
	painter.setRenderHint(QPainter::Antialiasing);

	//paint selection box
	QModelIndex current = currentIndex();
	QRect rectCurrent = visualRect(current);
	rectCurrent.translate(QPoint(0, 0));
	rectCurrent.adjust(-3, -3, 3, 3);
	painter.fillRect(rectCurrent, QApplication::palette().highlight());

	//paint labels
	for (int r = 0; r < model()->rowCount(); r++)
	{
		QString name = model()->headerData(r, Qt::Horizontal).toString();
		int x = r * (cMaterialWidget::previewWidth + iconMargin) + iconMargin - horizontalOffset();

		painter.drawText(	QRect(x,
														cMaterialWidget::previewHeight + iconMargin,
														cMaterialWidget::previewWidth,
														maxNameHeight),
											Qt::AlignHCenter | Qt::TextWordWrap,
											name);
	}
}

void cMaterialItemView::updateScrollBar()
{
	horizontalScrollBar()->setRange(0,
																	model()->rowCount() * (cMaterialWidget::previewWidth + iconMargin)
																			- width());
}

void cMaterialItemView::currentChanged(const QModelIndex& current, const QModelIndex& previous)
{
	Q_UNUSED(previous);
	if(current.isValid() && model())
	{
		emit activated(current);
	}
	viewport()->update();
}

void cMaterialItemView::setModel(QAbstractItemModel* model)
{
	QAbstractItemView::setModel(model);
	rowsInserted(QModelIndex(), 0, model->rowCount() -1);
}

void cMaterialItemView::updateNameHeight()
{
	QFont f = font();
	maxNameHeight = max(f.pixelSize(), (int)f.pointSizeF()) * 3;
}
