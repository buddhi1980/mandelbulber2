/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cParameterContainer class - multipurpose parameter container
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "parameters.hpp"
#include "one_parameter.hpp"
#include <QtCore>
#include <QtAlgorithms>
#include <algorithm>

//#define _PARAM_DEBUG

using namespace parameterContainer;

cParameterContainer::cParameterContainer()
{
	myMap.clear();
}

cParameterContainer::~cParameterContainer()
{
	myMap.clear();
}

//defining of params without limits
template<class T>
void cParameterContainer::addParam(QString name, T defaultVal, enumMorphType morphType, enumParameterType parType)
{
	cOneParameter newRecord;
	newRecord.Set(defaultVal, valueDefault);
	newRecord.Set(defaultVal, valueActual);
	newRecord.SetMorphType(morphType);
	newRecord.SetParameterType(parType);
	newRecord.SetOriginalContainerName(containerName);

	if(myMap.find(name) != myMap.end())
	{
		qWarning() << "addParam(): element '" << name << "' already existed" << endl;
	}
	else
	{
		myMap.insert(name, newRecord);
	}
}
template void cParameterContainer::addParam<double>(QString name, double defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<int>(QString name, int defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<QString>(QString name, QString defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<CVector3>(QString name, CVector3 defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<sRGB>(QString name, sRGB defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<bool>(QString name, bool defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<cColorPalette>(QString name, cColorPalette defaultVal, enumMorphType morphType, enumParameterType parType);

//defining of params with limits
template<class T>
void cParameterContainer::addParam(QString name, T defaultVal, T minVal, T maxVal, enumMorphType morphType, enumParameterType parType)
{
	cOneParameter newRecord;
	newRecord.Set(defaultVal, valueDefault);
	newRecord.Set(defaultVal, valueActual);
	newRecord.Set(minVal, valueMin);
	newRecord.Set(maxVal, valueMax);
	newRecord.SetMorphType(morphType);
	newRecord.SetParameterType(parType);
	newRecord.SetOriginalContainerName(containerName);

	if(myMap.find(name) != myMap.end())
	{
		qWarning() << "addParam(): element '" << name << "' already existed" << endl;
	}
	else
	{
		myMap.insert(name, newRecord);
	}
}
template void cParameterContainer::addParam<double>(QString name, double defaultVal, double minVal, double maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<int>(QString name, int defaultVal, int minVal, int maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<CVector3>(QString name, CVector3 defaultVal, CVector3 minVal, CVector3 maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<sRGB>(QString name, sRGB defaultVal, sRGB minVal, sRGB maxVal, enumMorphType morphType, enumParameterType parType);

//defining of params without limits and with index
template<class T>
void cParameterContainer::addParam(QString name, int index, T defaultVal, enumMorphType morphType, enumParameterType parType)
{
	if (index >= 0)
	{
		cOneParameter newRecord;
		newRecord.Set(defaultVal, valueDefault);
		newRecord.Set(defaultVal, valueActual);
		newRecord.SetMorphType(morphType);
		newRecord.SetParameterType(parType);
		newRecord.SetOriginalContainerName(containerName);

		QString indexName = nameWithIndex(&name, index);
		if(myMap.find(indexName) != myMap.end())
		{
			qWarning() << "addParam(): element '" << indexName << "' already existed" << endl;
		}
		else
		{
			myMap.insert(indexName, newRecord);
		}
	}
	else
	{
		qWarning() << "addParam(): element '" << name << "' has negative index (" << index << ")" << endl;
	}
}
template void cParameterContainer::addParam<double>(QString name, int index, double defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<int>(QString name, int index, int defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<QString>(QString name, int index, QString defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<CVector3>(QString name, int index, CVector3 defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<sRGB>(QString name, int index, sRGB defaultVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<bool>(QString name, int index, bool defaultVal, enumMorphType morphType, enumParameterType parType);

//defining of params with limits and index
template<class T>
void cParameterContainer::addParam(QString name, int index, T defaultVal, T minVal, T maxVal, enumMorphType morphType, enumParameterType parType)
{
	if (index >= 0)
	{
		cOneParameter newRecord;
		newRecord.Set(defaultVal, valueDefault);
		newRecord.Set(defaultVal, valueActual);
		newRecord.Set(minVal, valueMin);
		newRecord.Set(maxVal, valueMax);
		newRecord.SetMorphType(morphType);
		newRecord.SetParameterType(parType);
		newRecord.SetOriginalContainerName(containerName);

		QString indexName = nameWithIndex(&name, index);
		if(myMap.find(indexName) != myMap.end())
		{
			qWarning() << "addParam(): element '" << indexName << "' already existed" << endl;
		}
		else
		{
			myMap.insert(indexName, newRecord);
		}
	}
	else
	{
		qWarning() << "addParam(): element '" << name << "' has negative index (" << index << ")" << endl;
	}
}
template void cParameterContainer::addParam<double>(QString name, int index, double defaultVal, double minVal, double maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<int>(QString name, int index, int defaultVal, int minVal, int maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<CVector3>(QString name, int index, CVector3 defaultVal, CVector3 minVal, CVector3 maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<sRGB>(QString name, int index, sRGB defaultVal, sRGB minVal, sRGB maxVal, enumMorphType morphType, enumParameterType parType);

//set parameter value by name
template<class T>
void cParameterContainer::Set(QString name, T val)
{
	QMap<QString, cOneParameter>::iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		it->Set(val, valueActual);
	}
	else
	{
		qWarning() << "Set(): element '" << name << "' doesn't exists" << endl;
	}
}
template void cParameterContainer::Set<double>(QString name, double val);
template void cParameterContainer::Set<int>(QString name, int val);
template void cParameterContainer::Set<QString>(QString name, QString val);
template void cParameterContainer::Set<CVector3>(QString name, CVector3 val);
template void cParameterContainer::Set<sRGB>(QString name, sRGB val);
template void cParameterContainer::Set<bool>(QString name, bool val);
template void cParameterContainer::Set<cColorPalette>(QString name, cColorPalette val);

//set parameter value by name and index
template<class T>
void cParameterContainer::Set(QString name, int index, T val)
{
	if (index >= 0)
	{
		QString indexName = nameWithIndex(&name, index);
		QMap<QString, cOneParameter>::iterator it;
		it = myMap.find(indexName);
		if (it != myMap.end())
		{
			it->Set(val, valueActual);
		}
		else
		{
			qWarning() << "Set(): element '" << indexName << "' doesn't exists" << endl;
		}
	}
	else
	{
		qWarning() << "Set(): element '" << name << "' has negative index (" << index << ")" << endl;
	}
}
template void cParameterContainer::Set<double>(QString name, int index, double val);
template void cParameterContainer::Set<int>(QString name, int index, int val);
template void cParameterContainer::Set<QString>(QString name, int index, QString val);
template void cParameterContainer::Set<CVector3>(QString name, int index, CVector3 val);
template void cParameterContainer::Set<sRGB>(QString name, int index, sRGB val);
template void cParameterContainer::Set<bool>(QString name, int index, bool val);

//get parameter value by name
template<class T>
T cParameterContainer::Get(QString name) const
{
	QMap<QString, cOneParameter>::const_iterator it;
	it = myMap.find(name);
	T val = T();
	if (it != myMap.end())
	{
		val = it.value().Get<T>(valueActual);
	}
	else
	{
		qWarning() << "Get(): element '" << name << "' doesn't exists" << endl;
	}
	return val;
}
template double cParameterContainer::Get<double>(QString name) const;
template int cParameterContainer::Get<int>(QString name) const;
template QString cParameterContainer::Get<QString>(QString name) const;
template CVector3 cParameterContainer::Get<CVector3>(QString name) const;
template sRGB cParameterContainer::Get<sRGB>(QString name) const;
template bool cParameterContainer::Get<bool>(QString name) const;
template cColorPalette cParameterContainer::Get<cColorPalette>(QString name) const;

//get parameter value by name and index
template<class T>
T cParameterContainer::Get(QString name, int index) const
{
	T val = T();
	if (index >= 0)
	{
		QString indexName = nameWithIndex(&name, index);
		QMap<QString, cOneParameter>::const_iterator it;
		it = myMap.find(indexName);
		if (it != myMap.end())
		{
			val = it.value().Get<T>(valueActual);
		}
		else
		{
			qWarning() << "Get(): element '" << indexName << "' doesn't exists" << endl;
		}
	}
	else
	{
		qWarning() << "Get(): element '" << name << "' has negative index (" << index << ")" << endl;
	}
	return val;
}
template double cParameterContainer::Get<double>(QString name, int index) const;
template int cParameterContainer::Get<int>(QString name, int index) const;
template QString cParameterContainer::Get<QString>(QString name, int index) const;
template CVector3 cParameterContainer::Get<CVector3>(QString name, int index) const;
template sRGB cParameterContainer::Get<sRGB>(QString name, int index) const;
template bool cParameterContainer::Get<bool>(QString name, int index) const;

//get parameter default value by name
template<class T>
T cParameterContainer::GetDefault(QString name) const
{
	QMap<QString, cOneParameter>::const_iterator it;
	it = myMap.find(name);
	T val = T();
	if (it != myMap.end())
	{
		val = it.value().Get<T>(valueDefault);
	}
	else
	{
		qWarning() << "GetDefault(): element '" << name << "' doesn't exists" << endl;
	}
	return val;
}
template double cParameterContainer::GetDefault<double>(QString name) const;
template int cParameterContainer::GetDefault<int>(QString name) const;
template QString cParameterContainer::GetDefault<QString>(QString name) const;
template CVector3 cParameterContainer::GetDefault<CVector3>(QString name) const;
template sRGB cParameterContainer::GetDefault<sRGB>(QString name) const;
template bool cParameterContainer::GetDefault<bool>(QString name) const;
template cColorPalette cParameterContainer::GetDefault<cColorPalette>(QString name) const;

//get parameter value by name and index
template<class T>
T cParameterContainer::GetDefault(QString name, int index) const
{
	T val = T();
	if (index >= 0)
	{
		QString indexName = nameWithIndex(&name, index);
		QMap<QString, cOneParameter>::const_iterator it;
		it = myMap.find(indexName);
		if (it != myMap.end())
		{
			val = it.value().Get<T>(valueDefault);
		}
		else
		{
			qWarning() << "GetDefault(): element '" << indexName << "' doesn't exists" << endl;
		}
	}
	else
	{
		qWarning() << "GetDefault(): element '" << name << "' has negative index (" << index << ")" << endl;
	}
	return val;
}
template double cParameterContainer::GetDefault<double>(QString name, int index) const;
template int cParameterContainer::GetDefault<int>(QString name, int index) const;
template QString cParameterContainer::GetDefault<QString>(QString name, int index) const;
template CVector3 cParameterContainer::GetDefault<CVector3>(QString name, int index) const;
template sRGB cParameterContainer::GetDefault<sRGB>(QString name, int index) const;
template bool cParameterContainer::GetDefault<bool>(QString name, int index) const;


QString cParameterContainer::nameWithIndex(QString *str, int index) const
{
	QString name = *str + "_" + QString::number(index);
	return name;
}


void cParameterContainer::Copy(QString name, const cParameterContainer &sourceContainer)
{
	QMap<QString, cOneParameter>::const_iterator itSource;
	QMap<QString, cOneParameter>::iterator itDest;
	itDest = myMap.find(name);
	if (itDest != myMap.end())
	{
		itSource = sourceContainer.myMap.find(name);
		if (itSource != myMap.end())
		{
			itDest.value() = itSource.value();
		}
		else
		{
			qWarning() << "CopyPar(): source element '" << name << "' doesn't exists" << endl;
		}
	}
	else
	{
		qWarning() << "CopyPar(): destination element '" << name << "' doesn't exists" << endl;
	}
}

QList<QString> cParameterContainer::GetListOfParameters(void) const
{
	QList<QString> list = myMap.keys();
	std::sort(list.begin(), list.end(), compareStrings);
	return list;
}

enumVarType cParameterContainer::GetVarType(QString name) const
{
	enumVarType type = typeNull;

	QMap<QString, cOneParameter>::const_iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		type = it.value().GetValueType();
	}
	else
	{
		qWarning() << "GetVarType(): element '" << name << "' doesn't exists" << endl;
	}
	return type;
}

enumParameterType cParameterContainer::GetParameterType(QString name) const
{
	enumParameterType type = paramStandard;

	QMap<QString, cOneParameter>::const_iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		type = it.value().GetParameterType();
	}
	else
	{
		qWarning() << "GetParameterType(): element '" << name << "' doesn't exists" << endl;
	}
	return type;
}

bool cParameterContainer::isDefaultValue(QString name) const
{
	bool isDefault = true;

	QMap<QString, cOneParameter>::const_iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		isDefault = it.value().isDefaultValue();
	}
	else
	{
		qWarning() << "cParameterContainer::isDefaultValue(QString name): element '" << name << "' doesn't exists" << endl;
	}
	return isDefault;
}

void cParameterContainer::ResetAllToDefault(void)
{
	QMap<QString, cOneParameter>::iterator it = myMap.begin();
	while (it != myMap.end())
	{
		cOneParameter record = it.value();
		if(record.GetParameterType() != paramApp)	it.value().SetMultival(record.GetMultival(valueDefault), valueActual);
		++it;
	}
}

bool cParameterContainer::IfExists(const QString &name) const
{
	QMap<QString, cOneParameter>::const_iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		return true;
	}
	else
	{
		return false;
	}
}

void cParameterContainer::DeleteParameter(const QString &name)
{
	QMap<QString, cOneParameter>::iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		myMap.remove(name);
	}
	else
	{
		qWarning() << "DeleteParameter(): element '" << name << "' doesn't exists" << endl;
	}
}

cOneParameter cParameterContainer::GetAsOneParameter(QString name) const
{
	QMap<QString, cOneParameter>::const_iterator it;
	it = myMap.find(name);
	cOneParameter val;
	if (it != myMap.end())
	{
		val = it.value();
	}
	else
	{
		qWarning() << "cParameterContainer::GetAsOneParameter(QString name): element '" << name << "' doesn't exists" << endl;
	}
	return val;
}

void cParameterContainer::SetFromOneParameter(QString name, const cOneParameter &parameter)
{
	QMap<QString, cOneParameter>::iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		it.value() = parameter;
	}
	else
	{
		qWarning() << "cParameterContainer::SetFromOneParameter(QString name, const cOneParameter &parameter): element '" << name << "' doesn't exists" << endl;
	}
}

void cParameterContainer::AddParamFromOneParameter(QString name, const cOneParameter &parameter)
{
	if(myMap.find(name) != myMap.end())
	{
		qWarning() << "cParameterContainer::AddParamFromOneParameter(QString name, const cOneParameter &parameter): element '" << name << "' already existed" << endl;
	}
	else
	{
		myMap.insert(name, parameter);
	}
}
