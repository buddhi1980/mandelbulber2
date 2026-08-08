/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
	void SetEmpty(const QString &name);
	bool IsEmpty(const QString &name) const;

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
	bool HasScript(const QString &name) const;
	QString GetScript(const QString &name) const;

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

#endif /* MANDELBULBER2_SRC_PARAMETERS_HPP_ */
