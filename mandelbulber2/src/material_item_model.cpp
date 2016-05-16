/*
 * material_item_model.cpp
 *
 *  Created on: 8 maj 2016
 *      Author: krzysztof
 */

#include "material_item_model.h"
#include "fractal_container.hpp"
#include "material.h"
#include "settings.hpp"
#include "initparameters.hpp"

cMaterialItemModel::cMaterialItemModel(QObject *parent) : QAbstractListModel(parent)
{
	container = NULL;


}

cMaterialItemModel::~cMaterialItemModel()
{
	// TODO Auto-generated destructor stub
}

void cMaterialItemModel::AssignContainer(cParameterContainer *_parameterContainer)
{
	if(_parameterContainer)
	{
		container = _parameterContainer;
	}
	else
	{
		qCritical() << "Parameter container is NULL";
	}
}

int cMaterialItemModel::rowCount(const QModelIndex &parent) const
{
	// TODO
	return 3;
}

QVariant cMaterialItemModel::data(const QModelIndex &index, int role) const
{
	// TODO Material data
	Qt::ItemDataRole itemRole = (Qt::ItemDataRole)role;

	if(itemRole == Qt::DisplayRole)
	{
		int matIndex = index.row() + 1;

		cParameterContainer params;
		cFractalContainer fractal;

		params.SetContainerName("material");
		InitParams(&params);

		for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
		{
			fractal.at(i).SetContainerName(QString("fractal") + QString::number(i));
			InitFractalParams(&fractal.at(i));
		}
		InitMaterialParams(1, &params);

		//copy parameters from main parameter container to temporary container for material
		for(int i=0; i < cMaterial::paramsList.size(); i++)
		{
			cOneParameter parameter = container->GetAsOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), matIndex));
			params.SetFromOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), 1), parameter);
		}

		cSettings tempSettings(cSettings::formatCondensedText);
		tempSettings.CreateText(&params, &fractal);
		return tempSettings.GetSettingsText();
	}

	return QVariant();
}

bool cMaterialItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	return true;
}

QVariant cMaterialItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	// TODO Material name
	return "TODO Material name";
}

bool cMaterialItemModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	beginInsertRows(QModelIndex(), position, position+rows-1);
	endInsertRows();
	return true;
}
