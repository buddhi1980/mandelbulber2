/*
 * material_item_model.h
 *
 *  Created on: 8 maj 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_MATERIAL_ITEM_MODEL_H_
#define MANDELBULBER2_SRC_MATERIAL_ITEM_MODEL_H_

#include <QtCore/qabstractitemmodel.h>
#include "parameters.hpp"

class cMaterialItemModel: public QAbstractListModel
{
	Q_OBJECT;
public:
	cMaterialItemModel(QObject *parent);
	~cMaterialItemModel();

	//will be used to synchronize the data
	void AssignContainer(cParameterContainer *_parameterContainer);

	//will return number of material items
	int rowCount(const QModelIndex &parent = QModelIndex()) const;

	//will return QString with settings string (from cSettings) for selected index
	//will be also used to update material data after editing
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
	bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

	//will provide material name
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	bool insertRows(int position, int rows, const QModelIndex &parent);

private:

	int FindFreeIndex();

	cParameterContainer *container;
	QList<int> materialIndexes;
};

#endif /* MANDELBULBER2_SRC_MATERIAL_ITEM_MODEL_H_ */
