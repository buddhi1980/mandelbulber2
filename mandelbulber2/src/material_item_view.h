/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016 Krzysztof Marczak        §R-==%w["'~5]m%=L.=~5N
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

#ifndef MANDELBULBER2_SRC_MATERIAL_ITEM_VIEW_H_
#define MANDELBULBER2_SRC_MATERIAL_ITEM_VIEW_H_

#include <qabstractitemview.h>

class cMaterialItemView : public QAbstractItemView
{
	Q_OBJECT

public:
	cMaterialItemView(QWidget *parent);
	~cMaterialItemView();
	virtual QModelIndex indexAt(const QPoint &point) const;
	virtual QRegion visualRegionForSelection(const QItemSelection &selection) const;
	virtual void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
	virtual int verticalOffset() const;
	virtual int horizontalOffset() const;
	virtual void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags flags);
	virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
	virtual QRect visualRect(const QModelIndex &index) const;
	virtual bool isIndexHidden(const QModelIndex &index) const;
	virtual void setModel(QAbstractItemModel *model);

private:
	void resizeEvent(QResizeEvent *event);
	void scrollContentsBy(int dx, int dy);
	void paintEvent(QPaintEvent *event);
	void updateScrollBar();
	void updateNameHeight();

	int viewHeight;
	int iconMargin;
	int maxNameHeight;

protected slots:
	virtual void dataChanged(const QModelIndex &topLeft, const QModelIndex &bottomRight,
		const QVector<int> &roles = QVector<int>());
	virtual void rowsInserted(const QModelIndex &parent, int start, int end);
	virtual void currentChanged(const QModelIndex &current, const QModelIndex &previous);
};

#endif /* MANDELBULBER2_SRC_MATERIAL_ITEM_VIEW_H_ */
