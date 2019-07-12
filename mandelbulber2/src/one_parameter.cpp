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
 * cOneParameter class - stores all information of one parameter
 */

#include "one_parameter.hpp"

#include "nine_fractals.hpp"

using namespace parameterContainer;

cOneParameter::cOneParameter()
{
	morphType = morphNone;
	parType = paramStandard;
	limitsDefined = false;
	isEmpty = true;
	isGradientString = false;
}

cOneParameter::~cOneParameter() = default;

void cOneParameter::copy(const cOneParameter &other)
{
	morphType = other.morphType;
	parType = other.parType;
	limitsDefined = other.limitsDefined;
	isEmpty = other.isEmpty;
	isGradientString = other.isGradientString;
}

void cOneParameter::copyNotMovable(const cOneParameter &other)
{
	actualVal = other.actualVal;
	defaultVal = other.defaultVal;
	minVal = other.minVal;
	maxVal = other.maxVal;
	enumLookup = other.enumLookup;
	originalContainer = other.originalContainer;
}

cOneParameter::cOneParameter(const cOneParameter &other)
{
	copy(other);
	copyNotMovable(other);
}

cOneParameter::cOneParameter(cOneParameter &&other)
{
	copy(other);
	actualVal = std::move(other.actualVal);
	defaultVal = std::move(other.defaultVal);
	minVal = std::move(other.minVal);
	maxVal = std::move(other.maxVal);
	originalContainer = std::move(other.originalContainer);
}

cOneParameter &cOneParameter::operator=(const cOneParameter &other)
{
	copy(other);
	copyNotMovable(other);
	return *this;
}

cOneParameter &cOneParameter::operator=(cOneParameter &&other)
{
	copy(other);
	actualVal = std::move(other.actualVal);
	defaultVal = std::move(other.defaultVal);
	minVal = std::move(other.minVal);
	maxVal = std::move(other.maxVal);
	originalContainer = std::move(other.originalContainer);
	return *this;
}

// set parameter value
template <class T>
void cOneParameter::Set(T val, enumValueSelection selection)
{
	switch (selection)
	{
		case valueActual:
			actualVal.Store(val);
			LimitValue(actualVal);
			break;
		case valueDefault: defaultVal.Store(val); break;
		case valueMin:
			minVal.Store(val);
			limitsDefined = true;
			break;
		case valueMax:
			maxVal.Store(val);
			limitsDefined = true;
			break;
	}
	isEmpty = false;
}
template void cOneParameter::Set<double>(double val, enumValueSelection selection);
template void cOneParameter::Set<int>(int val, enumValueSelection selection);
template void cOneParameter::Set<QString>(QString val, enumValueSelection selection);
template void cOneParameter::Set<CVector3>(CVector3 val, enumValueSelection selection);
template void cOneParameter::Set<CVector4>(CVector4 val, enumValueSelection selection);
template void cOneParameter::Set<sRGB>(sRGB val, enumValueSelection selection);
template void cOneParameter::Set<bool>(bool val, enumValueSelection selection);

// get parameter value
template <class T>
T cOneParameter::Get(enumValueSelection selection) const
{
	T val = T();
	actualVal.Get(val);

	switch (selection)
	{
		case valueActual: actualVal.Get(val); break;
		case valueDefault: defaultVal.Get(val); break;
		case valueMin: minVal.Get(val); break;
		case valueMax: maxVal.Get(val); break;
	}
	return val;
}
template double cOneParameter::Get<double>(enumValueSelection selection) const;
template int cOneParameter::Get<int>(enumValueSelection selection) const;
template QString cOneParameter::Get<QString>(enumValueSelection selection) const;
template CVector3 cOneParameter::Get<CVector3>(enumValueSelection selection) const;
template CVector4 cOneParameter::Get<CVector4>(enumValueSelection selection) const;
template sRGB cOneParameter::Get<sRGB>(enumValueSelection selection) const;
template bool cOneParameter::Get<bool>(enumValueSelection selection) const;

bool cOneParameter::isDefaultValue() const
{
	return (actualVal == defaultVal);
}

cMultiVal cOneParameter::GetMultiVal(enumValueSelection selection) const
{
	switch (selection)
	{
		case valueActual: return actualVal;
		case valueDefault: return defaultVal;
		case valueMin: return minVal;
		case valueMax: return maxVal;
	}
	return actualVal;
}

void cOneParameter::SetMultiVal(cMultiVal multi, enumValueSelection selection)
{
	switch (selection)
	{
		case valueActual: actualVal = multi; break;
		case valueDefault: defaultVal = multi; break;
		case valueMin: minVal = multi; break;
		case valueMax: maxVal = multi; break;
	}
	isEmpty = false;
}

void cOneParameter::LimitValue(cMultiVal &multi) const
{
	enumVarType varType = multi.GetDefaultType();
	switch (varType)
	{
		case typeInt:
		{
			if (limitsDefined)
			{
				int min = Get<int>(valueMin);
				int max = Get<int>(valueMax);
				int act = Get<int>(valueActual);
				if (act < min) multi.Store(min);
				if (act > max) multi.Store(max);
			}
			break;
		}
		case typeDouble:
		{
			if (limitsDefined)
			{
				double min = Get<double>(valueMin);
				double max = Get<double>(valueMax);
				double act = Get<double>(valueActual);
				if (act < min) multi.Store(min);
				if (act > max) multi.Store(max);
			}
			break;
		}
		case typeRgb:
		{
			sRGB actLimited = Get<sRGB>(valueActual);
			if (actLimited.R < 0) actLimited.R = 0;
			if (actLimited.R > 65535) actLimited.R = 65535;
			if (actLimited.G < 0) actLimited.G = 0;
			if (actLimited.G > 65535) actLimited.G = 65535;
			if (actLimited.B < 0) actLimited.B = 0;
			if (actLimited.B > 65535) actLimited.B = 65535;
			multi.Store(actLimited);
			break;
		}
		default: break;
	}
}

QString cOneParameter::GetValueByEnumeration() const
{
	int index = Get<int>(valueActual);
	if (index >= 0 && index < enumLookup.size())
		return enumLookup.at(index);
	else
	{
		qWarning() << "Enumeration not found, cannot find index: " << index << " in QStringList "
							 << enumLookup;
		return Get<QString>(valueActual);
	}
}

int cOneParameter::GetIndexByEnumeration(QString value) const
{
	int index = enumLookup.indexOf(value);
	if (index != -1)
		return index;
	else
	{
		bool checkParse;
		const int index = value.toInt(&checkParse);
		if (!checkParse)
		{
			WriteLogCout("Enumeration not found, cannot find value: '" + value + "' in QStringList "
										 + enumLookup.join(","),
				1);
		}
		return index;
	}
}
