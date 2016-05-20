/*
 * material_item_view.cpp
 *
 *  Created on: 20 maj 2016
 *      Author: krzysztof
 */

#include "material_item_view.h"

cMaterialItemView::cMaterialItemView(QWidget *parent) : QAbstractItemView(parent)
{
	// TODO Auto-generated constructor stub

}

cMaterialItemView::~cMaterialItemView()
{
	// TODO Auto-generated destructor stub
}

QModelIndex cMaterialItemView::indexAt(const QPoint& point) const
{
	return QModelIndex();
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
	return 0;
}

int cMaterialItemView::horizontalOffset() const
{
	return 0;
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
	return QRect();
}

bool cMaterialItemView::isIndexHidden(const QModelIndex& index) const
{
	return false;
}
