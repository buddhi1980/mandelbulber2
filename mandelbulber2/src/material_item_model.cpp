/*
 * material_item_model.cpp
 *
 *  Created on: 8 maj 2016
 *      Author: krzysztof
 */

#include "material_item_model.h"

cMaterialItemModel::cMaterialItemModel(QObject *parent) : QAbstractListModel(parent)
{
	container = NULL;
}

cMaterialItemModel::~cMaterialItemModel()
{
	// TODO Auto-generated destructor stub
}

int cMaterialItemModel::rowCount(const QModelIndex &parent) const
{
	// TODO
	return -1;
}

QVariant cMaterialItemModel::data(const QModelIndex &index, int role) const
{
	// TODO Material data
	return "TODO Material data";
}

QVariant cMaterialItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	// TODO Material name
	return "TODO Material name";
}
