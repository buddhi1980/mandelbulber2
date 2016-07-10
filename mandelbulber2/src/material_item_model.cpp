/*
 * material_item_model.cpp
 *
 *  Created on: 8 maj 2016
 *      Author: krzysztof
 */

#include "material_item_model.h"
#include "material.h"
#include "settings.hpp"
#include "initparameters.hpp"

cMaterialItemModel::cMaterialItemModel(QObject *parent) : QAbstractListModel(parent)
{
	container = NULL;
}

cMaterialItemModel::~cMaterialItemModel()
{
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
	Q_UNUSED(parent);
	return materialIndexes.size();
}

QVariant cMaterialItemModel::data(const QModelIndex &index, int role) const
{
	Qt::ItemDataRole itemRole = (Qt::ItemDataRole)role;

	if(itemRole == Qt::DisplayRole)
	{
		int matIndex = materialIndexes.at(index.row());

		cParameterContainer params;

		params.SetContainerName("material");
		InitMaterialParams(matIndex, &params);

		//copy parameters from main parameter container to temporary container for material
		for(int i=0; i < cMaterial::paramsList.size(); i++)
		{
			cOneParameter parameter = container->GetAsOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), matIndex));
			params.SetFromOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), matIndex), parameter);
		}

		cSettings tempSettings(cSettings::formatCondensedText);
		tempSettings.CreateText(&params, NULL);
		return tempSettings.GetSettingsText();
	}

	if(itemRole == Qt::UserRole)
	{
		return materialIndexes.at(index.row());
	}

	return QVariant();
}

bool cMaterialItemModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
	Qt::ItemDataRole itemRole = (Qt::ItemDataRole)role;

	if(itemRole == Qt::EditRole)
	{
		//look for first free material index
		int matIndex = materialIndexes.at(index.row());

		cParameterContainer params;
		params.SetContainerName("material");
		InitMaterialParams(matIndex, &params);

		cSettings tempSettings(cSettings::formatCondensedText);
		tempSettings.LoadFromString(value.toString());
		tempSettings.Decode(&params, NULL);

		//copy parameters from temporary container for material to main parameter container
		for(int i=0; i < cMaterial::paramsList.size(); i++)
		{
			cOneParameter parameter = params.GetAsOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), matIndex));
			container->SetFromOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), matIndex), parameter);
		}
		emit dataChanged(index, index);
	}

	return true;
}

QVariant cMaterialItemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	Qt::ItemDataRole itemRole = (Qt::ItemDataRole)role;

	if(itemRole == Qt::DisplayRole && orientation == Qt::Horizontal)
	{
		int matIndex = materialIndexes.at(section);
		QString materialName = container->Get<QString>(cMaterial::Name("name", matIndex)) + QString(" [mat%1]").arg(matIndex);
		return materialName;
	}
	return QString();
}

bool cMaterialItemModel::insertRows(int position, int rows, const QModelIndex &parent)
{
	Q_UNUSED(parent);

	beginInsertRows(QModelIndex(), position, position+rows-1);

	for(int r = 0; r < rows; r++)
	{
		//look for first free material indes
		int matIndex = FindFreeIndex();
		materialIndexes.insert(position + r, matIndex);
		InitMaterialParams(matIndex, container);
		container->Set(cMaterial::Name("is_defined", matIndex), true);
	}
	endInsertRows();
	return true;
}

bool cMaterialItemModel::removeRows(int position, int rows, const QModelIndex &parent)
{
	Q_UNUSED(parent);
	beginRemoveRows(QModelIndex(), position, position+rows-1);

	for(int r = position + rows - 1; r >= position; r--)
	{
		int matIndex = materialIndexes.at(r);
		for(int i = 0; i < cMaterial::paramsList.length(); i++)
		{
			container->DeleteParameter(cMaterial::Name(cMaterial::paramsList.at(i), matIndex));
		}
		materialIndexes.removeAt(r);
	}
	endRemoveRows();
	return true;
}

void cMaterialItemModel::Regenerate()
{
	beginRemoveRows(QModelIndex(), 0, rowCount() - 1);
	materialIndexes.clear();
	endRemoveRows();

	QList<QString> list = container->GetListOfParameters();
	for (int i = 0; i < list.size(); i++)
	{
		QString parameterName = list.at(i);
		if (parameterName.left(3) == "mat")
		{
			int positionOfDash = parameterName.indexOf('_');
			int matIndex = parameterName.mid(3, positionOfDash - 3).toInt();
			if(materialIndexes.indexOf(matIndex) < 0)
			{
				materialIndexes.append(matIndex);
			}
		}
	}

	beginInsertRows(QModelIndex(), 0, materialIndexes.length() - 1);
	endInsertRows();
}

int cMaterialItemModel::FindFreeIndex()
{
	bool occupied = false;
	int materialIndex = 1;
	do
	{
		occupied = false;
		for(int i = 0; i < materialIndexes.size(); i++)
		{
			if(materialIndex == materialIndexes[i])
			{
				occupied = true;
				materialIndex++;
				break;
			}
		}
	}
	while(occupied);

	return materialIndex;
}

void cMaterialItemModel::slotMaterialChanged(int matIndex)
{
	int row = 0;
	for(int i = 0; i <materialIndexes.size(); i++)
	{
		if(matIndex == materialIndexes[i])
		{
			row = i;
			break;
		}
	}
	emit dataChanged(index(row, 0, QModelIndex()), index(row, 0, QModelIndex()));
}

int cMaterialItemModel::materialIndex(const QModelIndex& index)
{
	if(index.row() < materialIndexes.count())
	{
		return materialIndexes.at(index.row());
	}
	else
	{
		qCritical() << "cMaterialItemModel::materialIndex(const QModelIndex& index): index out of range";
		return 0;
	}
}

QModelIndex cMaterialItemModel::getModelIndexByMaterialId(int materialId)
{
	int row = materialIndexes.indexOf(materialId);
	if(row >= 0)
	{
		return index(row, 0);
	}
	else
	{
		qCritical() << "cMaterialItemModel::getModelIndexByMaterialId(int materialId): wrong material id!";
		return QModelIndex();
	}
}

void cMaterialItemModel::insertRowWithParameters(const cParameterContainer *params1)
{
	insertRows(rowCount(), 1);
	int matIndex = materialIndexes[rowCount()-1];
	QModelIndex newIndex = index(rowCount()-1, 0);

	cParameterContainer params;
	params.SetContainerName("material");
	InitMaterialParams(matIndex, &params);

	//copy parameters from temporary container for material to main parameter container
	for(int i=0; i < cMaterial::paramsList.size(); i++)
	{
		cOneParameter parameter = params1->GetAsOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), 1));
		container->SetFromOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), matIndex), parameter);
	}
	emit dataChanged(newIndex, newIndex);
}
