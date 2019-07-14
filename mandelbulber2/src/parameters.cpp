/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cParameterContainer class - multipurpose parameter container
 */

#include "parameters.hpp"

#include <algorithm>

#include <QtAlgorithms>

#include "nine_fractals.hpp"
#include "projection_3d.hpp"

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

cParameterContainer &cParameterContainer::operator=(const cParameterContainer &par)
{
	QMutexLocker lock(&m_lock);

	myMap = par.myMap;
	containerName = par.containerName;
	return *this;
}

// defining of params without limits
template <class T>
void cParameterContainer::addParam(QString name, T defaultVal, enumMorphType morphType,
	enumParameterType parType, QStringList enumLookup)
{
	QMutexLocker lock(&m_lock);

	cOneParameter newRecord;
	newRecord.Set(defaultVal, valueDefault);
	newRecord.Set(defaultVal, valueActual);
	newRecord.SetMorphType(morphType);
	newRecord.SetParameterType(parType);
	newRecord.SetOriginalContainerName(containerName);
	newRecord.SetEnumLookup(enumLookup);

	if (myMap.find(name) != myMap.end())
	{
		qWarning() << "addParam(): element '" << name << "' already existed" << endl;
	}
	else
	{
		myMap.insert(name, newRecord);
	}
}
template void cParameterContainer::addParam<double>(QString name, double defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
template void cParameterContainer::addParam<int>(QString name, int defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
template void cParameterContainer::addParam<QString>(QString name, QString defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
template void cParameterContainer::addParam<CVector3>(QString name, CVector3 defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
template void cParameterContainer::addParam<CVector4>(QString name, CVector4 defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
template void cParameterContainer::addParam<sRGB>(QString name, sRGB defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
template void cParameterContainer::addParam<bool>(QString name, bool defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);

// defining of params with limits
template <class T>
void cParameterContainer::addParam(QString name, T defaultVal, T minVal, T maxVal,
	enumMorphType morphType, enumParameterType parType)
{
	QMutexLocker lock(&m_lock);

	cOneParameter newRecord;
	newRecord.Set(defaultVal, valueDefault);
	newRecord.Set(defaultVal, valueActual);
	newRecord.Set(minVal, valueMin);
	newRecord.Set(maxVal, valueMax);
	newRecord.SetMorphType(morphType);
	newRecord.SetParameterType(parType);
	newRecord.SetOriginalContainerName(containerName);

	if (myMap.find(name) != myMap.end())
	{
		qWarning() << "addParam(): element '" << name << "' already existed" << endl;
	}
	else
	{
		myMap.insert(name, newRecord);
	}
}
template void cParameterContainer::addParam<double>(QString name, double defaultVal, double minVal,
	double maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<int>(QString name, int defaultVal, int minVal,
	int maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<CVector3>(QString name, CVector3 defaultVal,
	CVector3 minVal, CVector3 maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<CVector4>(QString name, CVector4 defaultVal,
	CVector4 minVal, CVector4 maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<sRGB>(QString name, sRGB defaultVal, sRGB minVal,
	sRGB maxVal, enumMorphType morphType, enumParameterType parType);

// defining of params without limits and with index
template <class T>
void cParameterContainer::addParam(QString name, int index, T defaultVal, enumMorphType morphType,
	enumParameterType parType, QStringList enumLookup)
{
	QMutexLocker lock(&m_lock);

	if (index >= 0)
	{
		cOneParameter newRecord;
		newRecord.Set(defaultVal, valueDefault);
		newRecord.Set(defaultVal, valueActual);
		newRecord.SetMorphType(morphType);
		newRecord.SetParameterType(parType);
		newRecord.SetOriginalContainerName(containerName);
		newRecord.SetEnumLookup(enumLookup);

		QString indexName = nameWithIndex(&name, index);
		if (myMap.find(indexName) != myMap.end())
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
		qWarning() << "addParam(): element '" << name << "' has negative index (" << index << ")"
							 << endl;
	}
}
template void cParameterContainer::addParam<double>(QString name, int index, double defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
template void cParameterContainer::addParam<int>(QString name, int index, int defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
template void cParameterContainer::addParam<QString>(QString name, int index, QString defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
template void cParameterContainer::addParam<CVector3>(QString name, int index, CVector3 defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
template void cParameterContainer::addParam<CVector4>(QString name, int index, CVector4 defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
template void cParameterContainer::addParam<sRGB>(QString name, int index, sRGB defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
template void cParameterContainer::addParam<bool>(QString name, int index, bool defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);

// defining of params with limits and index
template <class T>
void cParameterContainer::addParam(QString name, int index, T defaultVal, T minVal, T maxVal,
	enumMorphType morphType, enumParameterType parType)
{
	QMutexLocker lock(&m_lock);

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
		if (myMap.find(indexName) != myMap.end())
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
		qWarning() << "addParam(): element '" << name << "' has negative index (" << index << ")"
							 << endl;
	}
}
template void cParameterContainer::addParam<double>(QString name, int index, double defaultVal,
	double minVal, double maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<int>(QString name, int index, int defaultVal,
	int minVal, int maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<CVector3>(QString name, int index, CVector3 defaultVal,
	CVector3 minVal, CVector3 maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<CVector4>(QString name, int index, CVector4 defaultVal,
	CVector4 minVal, CVector4 maxVal, enumMorphType morphType, enumParameterType parType);
template void cParameterContainer::addParam<sRGB>(QString name, int index, sRGB defaultVal,
	sRGB minVal, sRGB maxVal, enumMorphType morphType, enumParameterType parType);

// set parameter value by name
template <class T>
void cParameterContainer::Set(QString name, T val)
{
	QMutexLocker lock(&m_lock);

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
template void cParameterContainer::Set<CVector4>(QString name, CVector4 val);
template void cParameterContainer::Set<sRGB>(QString name, sRGB val);
template void cParameterContainer::Set<bool>(QString name, bool val);

// set parameter value by name and index
template <class T>
void cParameterContainer::Set(QString name, int index, T val)
{
	QMutexLocker lock(&m_lock);

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
template void cParameterContainer::Set<CVector4>(QString name, int index, CVector4 val);
template void cParameterContainer::Set<sRGB>(QString name, int index, sRGB val);
template void cParameterContainer::Set<bool>(QString name, int index, bool val);

// get parameter value by name
template <class T>
T cParameterContainer::Get(QString name) const
{
	QMutexLocker lock(&m_lock);

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
template CVector4 cParameterContainer::Get<CVector4>(QString name) const;
template sRGB cParameterContainer::Get<sRGB>(QString name) const;
template bool cParameterContainer::Get<bool>(QString name) const;

template <>
float cParameterContainer::Get<float>(QString name) const
{
	return float(Get<double>(name));
}

// get parameter value by name and index
template <class T>
T cParameterContainer::Get(QString name, int index) const
{
	QMutexLocker lock(&m_lock);

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
template CVector4 cParameterContainer::Get<CVector4>(QString name, int index) const;
template sRGB cParameterContainer::Get<sRGB>(QString name, int index) const;
template bool cParameterContainer::Get<bool>(QString name, int index) const;

template <>
float cParameterContainer::Get<float>(QString name, int index) const
{
	return float(Get<double>(name, index));
}

// get parameter default value by name
template <class T>
T cParameterContainer::GetDefault(QString name) const
{
	QMutexLocker lock(&m_lock);

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
template CVector4 cParameterContainer::GetDefault<CVector4>(QString name) const;
template sRGB cParameterContainer::GetDefault<sRGB>(QString name) const;
template bool cParameterContainer::GetDefault<bool>(QString name) const;

// get parameter value by name and index
template <class T>
T cParameterContainer::GetDefault(QString name, int index) const
{
	QMutexLocker lock(&m_lock);

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
		qWarning() << "GetDefault(): element '" << name << "' has negative index (" << index << ")"
							 << endl;
	}
	return val;
}
template double cParameterContainer::GetDefault<double>(QString name, int index) const;
template int cParameterContainer::GetDefault<int>(QString name, int index) const;
template QString cParameterContainer::GetDefault<QString>(QString name, int index) const;
template CVector3 cParameterContainer::GetDefault<CVector3>(QString name, int index) const;
template CVector4 cParameterContainer::GetDefault<CVector4>(QString name, int index) const;
template sRGB cParameterContainer::GetDefault<sRGB>(QString name, int index) const;
template bool cParameterContainer::GetDefault<bool>(QString name, int index) const;

QString cParameterContainer::nameWithIndex(QString *str, int index)
{
	QString name = *str + "_" + QString::number(index);
	return name;
}

void cParameterContainer::Copy(QString name, const cParameterContainer *sourceContainer)
{
	QMutexLocker lock(&m_lock);

	QMap<QString, cOneParameter>::const_iterator itSource;
	QMap<QString, cOneParameter>::iterator itDest;
	itDest = myMap.find(name);
	if (itDest != myMap.end())
	{
		itSource = sourceContainer->myMap.find(name);
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

QList<QString> cParameterContainer::GetListOfParameters() const
{
	QMutexLocker lock(&m_lock);

	QList<QString> list = myMap.keys();
	std::sort(list.begin(), list.end(), compareStrings);
	return list;
}

void cParameterContainer::PrintListOfParameters() const
{
	QString parametersOutput = "Non-Default Parameters for Rendered Example Settings\n";
	QList<QString> listOfParameters = GetListOfParameters();
	for (auto parameterName : listOfParameters)
	{
		const QString parameterValue = Get<QString>(parameterName);
		const QString defaultValue = GetDefault<QString>(parameterName);
		if (parameterValue != defaultValue)
			parametersOutput += parameterName + "=" + parameterValue + "\n";
	}
	WriteLog(parametersOutput, 1);
}

enumVarType cParameterContainer::GetVarType(QString name) const
{
	QMutexLocker lock(&m_lock);

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
	QMutexLocker lock(&m_lock);

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
	QMutexLocker lock(&m_lock);

	bool isDefault = true;

	QMap<QString, cOneParameter>::const_iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		isDefault = it.value().isDefaultValue();
	}
	else
	{
		qWarning() << "cParameterContainer::isDefaultValue(QString name): element '" << name
							 << "' doesn't exists" << endl;
	}
	return isDefault;
}

void cParameterContainer::ResetAllToDefault()
{
	QMutexLocker lock(&m_lock);

	QMap<QString, cOneParameter>::iterator it = myMap.begin();
	while (it != myMap.end())
	{
		cOneParameter record = it.value();
		if (record.GetParameterType() != paramApp)
			it.value().SetMultiVal(record.GetMultiVal(valueDefault), valueActual);
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
	QMutexLocker lock(&m_lock);

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
	QMutexLocker lock(&m_lock);

	QMap<QString, cOneParameter>::const_iterator it;
	it = myMap.find(name);
	cOneParameter val;
	if (it != myMap.end())
	{
		val = it.value();
	}
	else
	{
		qWarning() << "cParameterContainer::GetAsOneParameter(QString name): element '" << name
							 << "' doesn't exists" << endl;
	}
	return val;
}

void cParameterContainer::SetFromOneParameter(QString name, const cOneParameter &parameter)
{
	QMutexLocker lock(&m_lock);

	QMap<QString, cOneParameter>::iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		it.value() = parameter;
	}
	else
	{
		qWarning() << "cParameterContainer::SetFromOneParameter(QString name, const cOneParameter "
									"&parameter): element '"
							 << name << "' doesn't exists" << endl;
	}
}

void cParameterContainer::AddParamFromOneParameter(QString name, const cOneParameter &parameter)
{
	QMutexLocker lock(&m_lock);

	if (myMap.find(name) != myMap.end())
	{
		qWarning() << "cParameterContainer::AddParamFromOneParameter(QString name, const cOneParameter "
									"&parameter): element '"
							 << name << "' already existed" << endl;
	}
	else
	{
		myMap.insert(name, parameter);
	}
}

QMap<QString, QString> cParameterContainer::getImageMeta()
{
	QMap<QString, QString> map;
	CVector3 camera = Get<CVector3>("camera");
	map.insert(QString("camera.x"), QString::number(camera.x));
	map.insert(QString("camera.y"), QString::number(camera.y));
	map.insert(QString("camera.z"), QString::number(camera.z));

	CVector3 target = Get<CVector3>("target");
	map.insert(QString("target.x"), QString::number(target.x));
	map.insert(QString("target.y"), QString::number(target.y));
	map.insert(QString("target.z"), QString::number(target.z));

	CVector3 rotation = Get<CVector3>("camera_rotation");
	map.insert(QString("camera_rotation.x"), QString::number(rotation.x));
	map.insert(QString("camera_rotation.y"), QString::number(rotation.y));
	map.insert(QString("camera_rotation.z"), QString::number(rotation.z));

	map.insert(QString("fov"), QString::number(Get<double>("fov")));

	QString perspectiveType = "";
	switch (Get<int>("perspective_type"))
	{
		case params::perspThreePoint: perspectiveType = "persp_three_point"; break;
		case params::perspFishEye: perspectiveType = "persp_fish_eye"; break;
		case params::perspEquirectangular: perspectiveType = "persp_equirectangular"; break;
		case params::perspFishEyeCut: perspectiveType = "persp_fish_eye_cut"; break;
	}
	map.insert(QString("perspective_type"), perspectiveType);
	map.insert(QString("stereo_enabled"), Get<bool>("stereo_enabled") ? "yes" : "no");
	map.insert(QString("stereo_eye_distance"), QString::number(Get<double>("stereo_eye_distance")));
	map.insert(QString("stereo_infinite_correction"),
		QString::number(Get<double>("stereo_infinite_correction")));

	return map;
}

void cParameterContainer::SetAsGradient(QString name)
{
	QMutexLocker lock(&m_lock);

	QMap<QString, cOneParameter>::iterator it;
	it = myMap.find(name);
	if (it != myMap.end())
	{
		it->SetAsGradient();
	}
	else
	{
		qWarning() << "cParameterContainer::SetAsGradient(QString name): element '" << name
							 << "' doesn't exists" << endl;
	}
}
