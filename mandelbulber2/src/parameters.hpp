/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#ifndef MANDELBULBER2_SRC_PARAMETERS_HPP_
#define MANDELBULBER2_SRC_PARAMETERS_HPP_

#include <memory>

#include <QMap>
#include <QMutex>

#include "one_parameter.hpp"

using namespace parameterContainer;
class cParameterContainer
{
public:
	cParameterContainer();

	cParameterContainer(const cParameterContainer &par)
			: myMap(par.myMap), containerName(par.containerName)
	{
	}

	cParameterContainer &operator=(const cParameterContainer &par);

	~cParameterContainer();

	template <class T>
	void addParam(const QString &name, T defaultVal, enumMorphType morphType,
		enumParameterType parType, QStringList enumLookup = {});

	template <class T>
	void addParam(const QString &name, T defaultVal, T minVal, T maxVal, enumMorphType morphType,
		enumParameterType parType);
	template <class T>
	void addParam(const QString &name, int index, T defaultVal, enumMorphType morphType,
		enumParameterType parType, QStringList enumLookup = {});
	template <class T>
	void addParam(const QString &name, int index, T defaultVal, T minVal, T maxVal,
		enumMorphType morphType, enumParameterType parType);

	template <class T>
	void Set(const QString &name, const T &val);
	template <class T>
	void Set(const QString &name, int index, const T &val);

	void SetScript(const QString &name, const QString &script);

	template <class T>
	T Get(const QString &name) const;

	template <class T>
	T Get(const QString &name, int index) const;

	template <class T>
	T GetDefault(const QString &name) const;
	template <class T>
	T GetDefault(const QString &name, int index) const;

	cOneParameter GetAsOneParameter(const QString &name) const;
	void SetFromOneParameter(const QString &name, const cOneParameter &parameter);
	void AddParamFromOneParameter(const QString &name, const cOneParameter &parameter);

	enumVarType GetVarType(const QString &name) const;
	enumParameterType GetParameterType(const QString &name) const;
	bool isDefaultValue(const QString &name) const;
	void Copy(const QString &name, std::shared_ptr<const cParameterContainer> sourceContainer);
	QList<QString> GetListOfParameters() const;
	int GetCount() const { return myMap.count(); }
	void PrintListOfParameters() const;
	void ResetAllToDefault(const QStringList &exclude = QStringList());
	void SetContainerName(const QString &name) { containerName = name; }
	QString GetContainerName() const { return containerName; }
	bool IfExists(const QString &name) const;
	void DeleteParameter(const QString &name);
	QMap<QString, QString> getImageMeta();
	void SetAsGradient(const QString &name);

private:
	static QString nameWithIndex(const QString &str, int index);

	static bool compareStrings(const QString &p1, const QString &p2)
	{
		return QString::compare(p1, p2, Qt::CaseInsensitive) < 0;
	}

	// std::map container
	QMap<QString, cOneParameter> myMap;
	QString containerName;

	mutable QMutex m_lock;
};

extern template void cParameterContainer::addParam<double>(const QString &name, double defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
extern template void cParameterContainer::addParam<int>(const QString &name, int defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
extern template void cParameterContainer::addParam<QString>(const QString &name, QString defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
extern template void cParameterContainer::addParam<CVector3>(const QString &name,
	CVector3 defaultVal, enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
extern template void cParameterContainer::addParam<CVector4>(const QString &name,
	CVector4 defaultVal, enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
extern template void cParameterContainer::addParam<sRGB>(const QString &name, sRGB defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
extern template void cParameterContainer::addParam<bool>(const QString &name, bool defaultVal,
	enumMorphType morphType, enumParameterType parType, QStringList enumLookup);

extern template void cParameterContainer::addParam<double>(const QString &name, double defaultVal,
	double minVal, double maxVal, enumMorphType morphType, enumParameterType parType);
extern template void cParameterContainer::addParam<int>(const QString &name, int defaultVal,
	int minVal, int maxVal, enumMorphType morphType, enumParameterType parType);
extern template void cParameterContainer::addParam<CVector3>(const QString &name,
	CVector3 defaultVal, CVector3 minVal, CVector3 maxVal, enumMorphType morphType,
	enumParameterType parType);
extern template void cParameterContainer::addParam<CVector4>(const QString &name,
	CVector4 defaultVal, CVector4 minVal, CVector4 maxVal, enumMorphType morphType,
	enumParameterType parType);
extern template void cParameterContainer::addParam<sRGB>(const QString &name, sRGB defaultVal,
	sRGB minVal, sRGB maxVal, enumMorphType morphType, enumParameterType parType);

extern template void cParameterContainer::addParam<double>(const QString &name, int index,
	double defaultVal, enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
extern template void cParameterContainer::addParam<int>(const QString &name, int index,
	int defaultVal, enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
extern template void cParameterContainer::addParam<QString>(const QString &name, int index,
	QString defaultVal, enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
extern template void cParameterContainer::addParam<CVector3>(const QString &name, int index,
	CVector3 defaultVal, enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
extern template void cParameterContainer::addParam<CVector4>(const QString &name, int index,
	CVector4 defaultVal, enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
extern template void cParameterContainer::addParam<sRGB>(const QString &name, int index,
	sRGB defaultVal, enumMorphType morphType, enumParameterType parType, QStringList enumLookup);
extern template void cParameterContainer::addParam<bool>(const QString &name, int index,
	bool defaultVal, enumMorphType morphType, enumParameterType parType, QStringList enumLookup);

extern template void cParameterContainer::addParam<double>(const QString &name, int index,
	double defaultVal, double minVal, double maxVal, enumMorphType morphType,
	enumParameterType parType);
extern template void cParameterContainer::addParam<int>(const QString &name, int index,
	int defaultVal, int minVal, int maxVal, enumMorphType morphType, enumParameterType parType);
extern template void cParameterContainer::addParam<CVector3>(const QString &name, int index,
	CVector3 defaultVal, CVector3 minVal, CVector3 maxVal, enumMorphType morphType,
	enumParameterType parType);
extern template void cParameterContainer::addParam<CVector4>(const QString &name, int index,
	CVector4 defaultVal, CVector4 minVal, CVector4 maxVal, enumMorphType morphType,
	enumParameterType parType);
extern template void cParameterContainer::addParam<sRGB>(const QString &name, int index,
	sRGB defaultVal, sRGB minVal, sRGB maxVal, enumMorphType morphType, enumParameterType parType);

extern template void cParameterContainer::Set<double>(const QString &name, const double &val);
extern template void cParameterContainer::Set<int>(const QString &name, const int &val);
extern template void cParameterContainer::Set<QString>(const QString &name, const QString &val);
extern template void cParameterContainer::Set<CVector3>(const QString &name, const CVector3 &val);
extern template void cParameterContainer::Set<CVector4>(const QString &name, const CVector4 &val);
extern template void cParameterContainer::Set<sRGB>(const QString &name, const sRGB &val);
extern template void cParameterContainer::Set<bool>(const QString &name, const bool &val);

extern template void cParameterContainer::Set<double>(
	const QString &name, int index, const double &val);
extern template void cParameterContainer::Set<int>(const QString &name, int index, const int &val);
extern template void cParameterContainer::Set<QString>(
	const QString &name, int index, const QString &val);
extern template void cParameterContainer::Set<CVector3>(
	const QString &name, int index, const CVector3 &val);
extern template void cParameterContainer::Set<CVector4>(
	const QString &name, int index, const CVector4 &val);
extern template void cParameterContainer::Set<sRGB>(
	const QString &name, int index, const sRGB &val);
extern template void cParameterContainer::Set<bool>(
	const QString &name, int index, const bool &val);

extern template double cParameterContainer::Get<double>(const QString &name) const;
extern template int cParameterContainer::Get<int>(const QString &name) const;
extern template QString cParameterContainer::Get<QString>(const QString &name) const;
extern template CVector3 cParameterContainer::Get<CVector3>(const QString &name) const;
extern template CVector4 cParameterContainer::Get<CVector4>(const QString &name) const;
extern template sRGB cParameterContainer::Get<sRGB>(const QString &name) const;
extern template bool cParameterContainer::Get<bool>(const QString &name) const;

extern template double cParameterContainer::Get<double>(const QString &name, int index) const;
extern template int cParameterContainer::Get<int>(const QString &name, int index) const;
extern template QString cParameterContainer::Get<QString>(const QString &name, int index) const;
extern template CVector3 cParameterContainer::Get<CVector3>(const QString &name, int index) const;
extern template CVector4 cParameterContainer::Get<CVector4>(const QString &name, int index) const;
extern template sRGB cParameterContainer::Get<sRGB>(const QString &name, int index) const;
extern template bool cParameterContainer::Get<bool>(const QString &name, int index) const;

extern template double cParameterContainer::GetDefault<double>(const QString &name) const;
extern template int cParameterContainer::GetDefault<int>(const QString &name) const;
extern template QString cParameterContainer::GetDefault<QString>(const QString &name) const;
extern template CVector3 cParameterContainer::GetDefault<CVector3>(const QString &name) const;
extern template CVector4 cParameterContainer::GetDefault<CVector4>(const QString &name) const;
extern template sRGB cParameterContainer::GetDefault<sRGB>(const QString &name) const;
extern template bool cParameterContainer::GetDefault<bool>(const QString &name) const;

extern template double cParameterContainer::GetDefault<double>(
	const QString &name, int index) const;
extern template int cParameterContainer::GetDefault<int>(const QString &name, int index) const;
extern template QString cParameterContainer::GetDefault<QString>(
	const QString &name, int index) const;
extern template CVector3 cParameterContainer::GetDefault<CVector3>(
	const QString &name, int index) const;
extern template CVector4 cParameterContainer::GetDefault<CVector4>(
	const QString &name, int index) const;
extern template sRGB cParameterContainer::GetDefault<sRGB>(const QString &name, int index) const;
extern template bool cParameterContainer::GetDefault<bool>(const QString &name, int index) const;

#endif /* MANDELBULBER2_SRC_PARAMETERS_HPP_ */
