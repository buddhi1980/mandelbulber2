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

