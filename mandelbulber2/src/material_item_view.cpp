/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cMaterialItemView class - ui presentation element for one material item
 * This UI will get used for each material in the material list.
 * See also more information on QT's model-view scheme here:
 * http://doc.qt.io/qt-5/model-view-programming.html
 */

#include "material_item_view.h"

#include <qpainter.h>

#include "../qt/material_widget.h"
#include "system.hpp"
#include <qpushbutton.h>
#include <qscrollbar.h>

cMaterialItemView::cMaterialItemView(QWidget *parent) : QAbstractItemView(parent)
{
	updateNameHeight();
	iconMargin = 10;
	if (horizontalScrollBar()->isVisible())
		viewHeight = cMaterialWidget::previewHeight + horizontalScrollBar()->height() + iconMargin * 2
								 + maxNameHeight;
	else
		viewHeight = cMaterialWidget::previewHeight + iconMargin * 2 + maxNameHeight;

	setMinimumHeight(viewHeight);
	setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
	setAutoScroll(true);
}

cMaterialItemView::~cMaterialItemView()
{
}

QModelIndex cMaterialItemView::indexAt(const QPoint &point) const
{
	int r =
		(point.x() + horizontalOffset() - iconMargin) / (cMaterialWidget::previewWidth + iconMargin);
	return QModelIndex(model()->index(r, 0, QModelIndex()));
}

QRegion cMaterialItemView::visualRegionForSelection(const QItemSelection &selection) const
{
	Q_UNUSED(selection);
	return QRegion();
}

void cMaterialItemView::scrollTo(const QModelIndex &index, ScrollHint hint)
{
	Q_UNUSED(hint);
	horizontalScrollBar()->setValue(
		iconMargin + index.row() * (cMaterialWidget::previewWidth + iconMargin));
}

int cMaterialItemView::verticalOffset() const
{
	return verticalScrollBar()->value();
}

int cMaterialItemView::horizontalOffset() const
{
	return horizontalScrollBar()->value();
}

void cMaterialItemView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags flags)
{
	Q_UNUSED(flags);
	Q_UNUSED(rect);
	viewport()->update();
}

QModelIndex cMaterialItemView::moveCursor(
	CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
	Q_UNUSED(modifiers);
	QModelIndex newIndex;
	if (cursorAction == MoveRight)
	{
		newIndex = model()->index(currentIndex().row() + 1, 0);
	}
	else if (cursorAction == MoveLeft)
	{
		newIndex = model()->index(currentIndex().row() - 1, 0);
	}
	else if (cursorAction == MoveHome)
	{
		newIndex = model()->index(0, 0);
	}
	else if (cursorAction == MoveEnd)
	{
		newIndex = model()->index(model()->rowCount() - 1, 0);
	}
	return newIndex;
}

QRect cMaterialItemView::visualRect(const QModelIndex &index) const
{
	return QRect(
		iconMargin + index.row() * (cMaterialWidget::previewWidth + iconMargin) - horizontalOffset(),
		iconMargin, cMaterialWidget::previewWidth, cMaterialWidget::previewHeight + maxNameHeight);
}

bool cMaterialItemView::isIndexHidden(const QModelIndex &index) const
{
	Q_UNUSED(index);
	return false;
}

void cMaterialItemView::dataChanged(
	const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles)
{
	Q_UNUSED(bottomRight);
	Q_UNUSED(roles);
	QString settings = model()->data(topLeft).toString();
	cMaterialWidget *widget = static_cast<cMaterialWidget *>(indexWidget(topLeft));
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
	// updateGeometries();

	updateScrollBar();
	updateGeometries();

	setCurrentIndex(model()->index(start, 0));
	scrollTo(model()->index(start, 0));

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
	if (horizontalScrollBar()->isVisible())
		viewHeight = cMaterialWidget::previewHeight + horizontalScrollBar()->height() + iconMargin * 2
								 + maxNameHeight;
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

	// paint selection box
	QModelIndex current = currentIndex();
	QRect rectCurrent = visualRect(current);
	rectCurrent.translate(QPoint(0, 0));
	rectCurrent.adjust(-3, -3, 3, 3);
	painter.fillRect(rectCurrent, QApplication::palette().highlight());

	if (model())
	{
		// paint labels
		for (int r = 0; r < model()->rowCount(); r++)
		{
			QString name = model()->headerData(r, Qt::Horizontal).toString();
			int x = r * (cMaterialWidget::previewWidth + iconMargin) + iconMargin - horizontalOffset();

			painter.drawText(QRect(x, cMaterialWidget::previewHeight + iconMargin,
												 cMaterialWidget::previewWidth, maxNameHeight),
				Qt::AlignHCenter | Qt::TextWordWrap, name);
		}
	}
}

void cMaterialItemView::updateScrollBar() const
{
	horizontalScrollBar()->setRange(
		0, model()->rowCount() * (cMaterialWidget::previewWidth + iconMargin) - width());
}

void cMaterialItemView::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
	Q_UNUSED(previous);
	if (current.isValid() && model())
	{
		emit activated(current);
	}
	viewport()->update();
}

void cMaterialItemView::setModel(QAbstractItemModel *model)
{
	QAbstractItemView::setModel(model);
	rowsInserted(QModelIndex(), 0, model->rowCount() - 1);
}

void cMaterialItemView::updateNameHeight()
{
	QFont f = font();
	maxNameHeight = max(f.pixelSize(), int(f.pointSizeF())) * 3;
}
