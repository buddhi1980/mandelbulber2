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
 * cMultiVal class - union like storage for one parameter
 */

#include "multi_val.hpp"

#include <QLocale>
#include <QtCore>

using namespace parameterContainer;

cMultiVal::cMultiVal()
{
	iVal[0] = 0;
	iVal[1] = 0;
	iVal[2] = 0;
	iVal[3] = 0;
	dVal[0] = 0.0;
	dVal[1] = 0.0;
	dVal[2] = 0.0;
	dVal[3] = 0.0;
	sVal.clear();
	typeDefined = false;
	type = typeNull;
}

cMultiVal::~cMultiVal()
{
	// nothing
}

void cMultiVal::copy(const cMultiVal &other)
{
	for (int i = 0; i < 4; i++)
	{
		iVal[i] = other.iVal[i];
		dVal[i] = other.dVal[i];
	}
	type = other.type;
	typeDefined = other.typeDefined;
}

cMultiVal::cMultiVal(const cMultiVal &other)
{
	copy(other);
	sVal = other.sVal;
}

cMultiVal::cMultiVal(cMultiVal &&other)
{
	copy(other);
	sVal = std::move(other.sVal);
}

cMultiVal &cMultiVal::operator=(const cMultiVal &other)
{
	copy(other);
	sVal = other.sVal;
	return *this;
}

cMultiVal &cMultiVal::operator=(cMultiVal &&other)
{
	copy(other);
	sVal = std::move(other.sVal);
	return *this;
}

enumVarType cMultiVal::Store(double val)
{
	dVal[0] = val;
	iVal[0] = int(val);
	sVal = QString("%L1").arg(val, 0, 'g', 16);

	if (!typeDefined) type = typeDouble;
	return typeDouble;
}

enumVarType cMultiVal::Store(int val)
{
	dVal[0] = val;
	iVal[0] = val;
	sVal = QString::number(val);

	if (!typeDefined) type = typeInt;
	return typeInt;
}

enumVarType cMultiVal::Store(QString val)
{
	switch (type)
	{
		case typeNull:
		case typeVector3:
		case typeVector4:
		case typeString:
		{
			QStringList split = val.split(' ');
			int size = split.size();
			if (size >= 4) size = 4;
			for (int i = 0; i < size; i++)
			{
				dVal[i] = QLocale::system().toDouble(split[i]);
				iVal[i] = split[i].toInt();
			}
			break;
		}
		case typeInt:
		case typeDouble:
		case typeBool:
			dVal[0] = QLocale::system().toDouble(val);
			iVal[0] = val.toInt();
			break;

		case typeRgb:
		{
			QStringList split = val.split(' ');
			int size = split.size();
			if (size >= 4) size = 4;
			for (int i = 0; i < size; i++)
			{
				dVal[i] = iVal[i] = split[i].toInt(nullptr, 16);
			}
			break;
		}
	}
	sVal = val;

	if (!typeDefined) type = typeString;
	return typeString;
}

enumVarType cMultiVal::Store(CVector3 val)
{
	dVal[0] = val.x;
	dVal[1] = val.y;
	dVal[2] = val.z;
	iVal[0] = int(val.x);
	iVal[1] = int(val.y);
	iVal[2] = int(val.z);
	sVal =
		QString("%L1 %L2 %L3").arg(val.x, 0, 'g', 16).arg(val.y, 0, 'g', 16).arg(val.z, 0, 'g', 16);

	if (!typeDefined) type = typeVector3;
	return type;
}

enumVarType cMultiVal::Store(CVector4 val)
{
	dVal[0] = val.x;
	dVal[1] = val.y;
	dVal[2] = val.z;
	dVal[3] = val.w;
	iVal[0] = int(val.x);
	iVal[1] = int(val.y);
	iVal[2] = int(val.z);
	iVal[3] = int(val.w);
	sVal = QString("%L1 %L2 %L3 %L4")
					 .arg(val.x, 0, 'g', 16)
					 .arg(val.y, 0, 'g', 16)
					 .arg(val.z, 0, 'g', 16)
					 .arg(val.w, 0, 'g', 16);

	if (!typeDefined) type = typeVector4;
	return type;
}

enumVarType cMultiVal::Store(sRGB val)
{
	dVal[0] = val.R;
	dVal[1] = val.G;
	dVal[2] = val.B;
	iVal[0] = val.R;
	iVal[1] = val.G;
	iVal[2] = val.B;
	sVal = QString("%1 %2 %3")
					 .arg(val.R, 4, 16, QChar('0'))
					 .arg(val.G, 4, 16, QChar('0'))
					 .arg(val.B, 4, 16, QChar('0'));

	if (!typeDefined) type = typeRgb;
	return typeRgb;
}

enumVarType cMultiVal::Store(bool val)
{
	dVal[0] = val;
	iVal[0] = val;
	sVal = QString::number(val);

	if (!typeDefined) type = typeBool;
	return typeBool;
}

enumVarType cMultiVal::Get(double &val) const
{
	val = dVal[0];
	return typeDouble;
}

enumVarType cMultiVal::Get(int &val) const
{
	val = iVal[0];
	return typeInt;
}

enumVarType cMultiVal::Get(CVector3 &val) const
{
	val = CVector3(dVal[0], dVal[1], dVal[2]);
	return typeVector3;
}

enumVarType cMultiVal::Get(CVector4 &val) const
{
	val = CVector4(dVal[0], dVal[1], dVal[2], dVal[3]);
	return typeVector4;
}

enumVarType cMultiVal::Get(QString &val) const
{
	val = sVal;
	return typeString;
}

enumVarType cMultiVal::Get(sRGB &val) const
{
	val = sRGB(iVal[0], iVal[1], iVal[2]);
	return typeRgb;
}

enumVarType cMultiVal::Get(bool &val) const
{
	val = iVal[0];
	return typeBool;
}

bool cMultiVal::isEqual(const cMultiVal &m) const
{
	bool isEqual = true;
	switch (type)
	{
		case typeBool:
		case typeInt:
		case typeRgb:
		{
			for (int i = 0; i < 4; i++)
			{
				if (iVal[i] != m.iVal[i])
				{
					isEqual = false;
					break;
				}
			}
			break;
		}
		case typeDouble:
		case typeVector3:
		case typeVector4:
		{
			for (int i = 0; i < 4; i++)
			{
				if (dVal[i] != m.dVal[i])
				{
					isEqual = false;
					break;
				}
			}
			break;
		}
		case typeString:
		{
			if (sVal != m.sVal)
			{
				isEqual = false;
			}
			break;
		}
		case typeNull:
		{
			qCritical() << "cMultiVal::isEqual(const cMultiVal &m): undefined type of variable";
			break;
		}
	}
	return isEqual;
}

bool cMultiVal::operator==(const cMultiVal &m) const
{
	return isEqual(m);
}
