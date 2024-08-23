/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-23 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <QDebug>

#include "nine_fractals.hpp"
#include "write_log.hpp"

using namespace parameterContainer;

cOneParameter::cOneParameter()
{
	morphType = morphNone;
	parType = paramStandard;
	limitsDefined = false;
	isEmpty = true;
	isGradientString = false;
}

// set parameter value
template <class T>
void cOneParameter::Set(const T &val, enumValueSelection selection)
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
template void cOneParameter::Set<double>(const double &val, enumValueSelection selection);
template void cOneParameter::Set<int>(const int &val, enumValueSelection selection);
template void cOneParameter::Set<long long>(const long long &val, enumValueSelection selection);
template void cOneParameter::Set<QString>(const QString &val, enumValueSelection selection);
template void cOneParameter::Set<CVector3>(const CVector3 &val, enumValueSelection selection);
template void cOneParameter::Set<CVector4>(const CVector4 &val, enumValueSelection selection);
template void cOneParameter::Set<sRGB>(const sRGB &val, enumValueSelection selection);
template void cOneParameter::Set<bool>(const bool &val, enumValueSelection selection);

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
