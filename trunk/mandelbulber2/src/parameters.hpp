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

#ifndef SHADERS_HPP_
#define SHADERS_HPP_

#include <QtCore>
#include "one_parameter.hpp"

using namespace parameterContainer;
class cParameterContainer
{
public:
	cParameterContainer();
	~cParameterContainer();

	template <class T> void addParam(QString name, T defaultVal, enumMorphType morphType, enumParameterType parType);
	template <class T> void addParam(QString name, T defaultVal, T minVal, T maxVal, enumMorphType morphType, enumParameterType parType);
	template <class T> void addParam(QString name, int index, T defaultVal, enumMorphType morphType, enumParameterType parType);
	template <class T> void addParam(QString name, int index, T defaultVal, T minVal, T maxVal, enumMorphType morphType, enumParameterType parType);

	template <class T> void Set(QString name, T val);
	template <class T> void Set(QString name, int index, T val);

	template <class T> T Get(QString name) const;
	template <class T> T Get(QString name, int index) const;
	template <class T> T GetDefault(QString name) const;
	template <class T> T GetDefault(QString name, int index) const;

	cOneParameter GetAsOneParameter(QString name) const;
	void SetFromOneParameter(QString name, const cOneParameter &parameter);
	void AddParamFromOneParameter(QString name, const cOneParameter &parameter);

	enumVarType GetVarType(QString name) const;
	enumParameterType GetParameterType(QString name) const;
	bool isDefaultValue(QString name) const;
	void Copy(QString name, const cParameterContainer &sourceContainer);
	QList<QString> GetListOfParameters(void) const;
	void ResetAllToDefault(void);
	void SetContainerName(QString name) {containerName = name;}
	QString GetContainerName(void) const {return containerName;}
	bool IfExists(const QString &name) const;
	void DeleteParameter(const QString &name);

private:

	QString nameWithIndex(QString *str, int index) const;

	static bool compareStrings(const QString &p1, const QString &p2)
	{
	    if(p1.toLower() < p2.toLower())
	    		return true;
	    else
	        return false;
	}

	//std::map contairer
	QMap<QString, cOneParameter> myMap;
	QString containerName;
};



#endif /*SHADERS_HPP_*/
