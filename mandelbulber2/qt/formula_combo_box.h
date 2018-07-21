/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * cFormulaComboBox - Formula Combobox for the selection of the used formula
 * This is a combobox with icons and autocomplete for fast formula access
 * The item list gets populated with the populateItemsFromFractalList()
 *
 * autocomplete adopted from here:
 * http://stackoverflow.com/questions/4827207/how-do-i-filter-the-pyqt-qcombobox-items-based-on-the-text-input
 */

#ifndef MANDELBULBER2_QT_FORMULA_COMBO_BOX_H_
#define MANDELBULBER2_QT_FORMULA_COMBO_BOX_H_

#include <QComboBox>
#include <QCompleter>
#include <QSortFilterProxyModel>

#include "src/fractal_list.hpp"

class cFormulaComboBox : public QComboBox
{
	Q_OBJECT

public:
	explicit cFormulaComboBox(QWidget *parent = nullptr);
	~cFormulaComboBox() override;
	void setModel(QAbstractItemModel *model);
	void setModelColumn(int visibleColumn);
	void populateItemsFromFractalList(
		QList<sFractalDescription> fractalList, QList<QPair<int, QString> /* */> insertHeader);

private slots:
	void onCompleterActivated(QString text);

private:
	QIcon GetIconFromCache(const QString &filename);

	QSortFilterProxyModel *pFilterModel;
	QCompleter *completer;

	static QMap<QString, QIcon> iconCache;
};

#endif /* MANDELBULBER2_QT_FORMULA_COMBO_BOX_H_ */
