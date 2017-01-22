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
 * cMaterialItemModel class - list data model to hold materials
 * See also more information on QT's model-view scheme here:
 * http://doc.qt.io/qt-5/model-view-programming.html
 */

#ifndef MANDELBULBER2_SRC_MATERIAL_ITEM_MODEL_H_
#define MANDELBULBER2_SRC_MATERIAL_ITEM_MODEL_H_

#include <QtCore/qabstractitemmodel.h>

// forward declarations
class cParameterContainer;

class cMaterialItemModel : public QAbstractListModel
{
	Q_OBJECT

public:
	cMaterialItemModel(QObject *parent);
	~cMaterialItemModel();

	// will be used to synchronize the data
	void AssignContainer(cParameterContainer *_parameterContainer);
	cParameterContainer *GetContainer() { return container; };

	// regenerate model from parametr container data
	void Regenerate();

	// will return number of material items
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;

	// will return QString with settings string (from cSettings) for selected index
	// will be also used to update material data after editing
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) override;

	// will provide material name
	QVariant headerData(
		int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

	bool insertRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;
	bool removeRows(int position, int rows, const QModelIndex &parent = QModelIndex()) override;

	int materialIndex(const QModelIndex &index) const;
	QModelIndex getModelIndexByMaterialId(int materialId) const;

	void insertRowWithParameters(const cParameterContainer *params1);

public slots:
	void slotMaterialChanged(int matIndex);

private:
	int FindFreeIndex();

	cParameterContainer *container;
	QList<int> materialIndexes;
};

#endif /* MANDELBULBER2_SRC_MATERIAL_ITEM_MODEL_H_ */
