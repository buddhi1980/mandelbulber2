/*
 * material_item_view.h
 *
 *  Created on: 20 maj 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_MATERIAL_ITEM_VIEW_H_
#define MANDELBULBER2_QT_MATERIAL_ITEM_VIEW_H_

#include <qabstractitemview.h>

class cMaterialItemView: public QAbstractItemView
{
	Q_OBJECT;
public:
	cMaterialItemView(QWidget *parent);
	~cMaterialItemView();
	virtual QModelIndex	indexAt(const QPoint &point) const;
	virtual QRegion	visualRegionForSelection(const QItemSelection &selection) const;
	virtual void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
	virtual int	verticalOffset() const;
	virtual int	horizontalOffset() const;
	virtual void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags flags);
	virtual QModelIndex	moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
	virtual QRect	visualRect(const QModelIndex &index) const;
	virtual bool isIndexHidden(const QModelIndex &index) const;
};

#endif /* MANDELBULBER2_QT_MATERIAL_ITEM_VIEW_H_ */
