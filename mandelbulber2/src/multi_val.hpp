/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#ifndef MANDELBULBER2_SRC_MULTI_VAL_HPP_
#define MANDELBULBER2_SRC_MULTI_VAL_HPP_

#include "algebra.hpp"
#include "color_structures.hpp"

namespace parameterContainer
{
enum enumVarType
{
	typeNull,
	typeInt,
	typeDouble,
	typeString,
	typeVector3,
	typeVector4,
	typeRgb,
	typeBool,
};
}

using namespace parameterContainer;

class cMultiVal
{
public:
	cMultiVal();

	enumVarType Store(double val);
	enumVarType Store(int val);
	enumVarType Store(long long val);
	enumVarType Store(const QString &val);
	enumVarType Store(const CVector3 &val);
	enumVarType Store(const CVector4 &val);
	enumVarType Store(const sRGB &val);
	enumVarType Store(bool val);

	inline void Get(double &val) const { val = dVal[0]; }
	inline void Get(int &val) const { val = iVal[0]; }
	inline void Get(long long &val) const { val = iVal[0]; }
	inline void Get(CVector3 &val) const { val = CVector3(dVal[0], dVal[1], dVal[2]); }
	inline void Get(CVector4 &val) const { val = CVector4(dVal[0], dVal[1], dVal[2], dVal[3]); }
	inline void Get(sRGB &val) const { val = sRGB(iVal[0], iVal[1], iVal[2]); }
	inline void Get(bool &val) const { val = iVal[0]; }

	void Get(QString &val) const
	{
		switch (type)
		{
			case typeBool: val = QString::number(iVal[0]);
			case typeInt: val = QString::number(iVal[0]); break;
			case typeDouble: val = QString("%L1").arg(dVal[0], 0, 'g', 15); break;
			case typeString: val = sVal; break;
			case typeVector3:
				val = QString("%L1 %L2 %L3")
								.arg(dVal[0], 0, 'g', 15)
								.arg(dVal[1], 0, 'g', 15)
								.arg(dVal[2], 0, 'g', 15);
				break;
			case typeVector4:
				val = QString("%L1 %L2 %L3 %L4")
								.arg(dVal[0], 0, 'g', 15)
								.arg(dVal[1], 0, 'g', 15)
								.arg(dVal[2], 0, 'g', 15)
								.arg(dVal[3], 0, 'g', 15);
				break;
			case typeRgb:
				val = QString("%1 %2 %3")
								.arg(iVal[0], 4, 16, QChar('0'))
								.arg(iVal[1], 4, 16, QChar('0'))
								.arg(iVal[2], 4, 16, QChar('0'));
				break;
			case typeNull: val = QString(); break;
		}
	}

	enumVarType GetDefaultType() const { return type; }
	bool operator==(const cMultiVal &m) const;

private:
	bool isEqual(const cMultiVal &m) const;

	double dVal[4];
	int iVal[4];
	QString sVal;
	enumVarType type;
	bool typeDefined;
};

#endif /* MANDELBULBER2_SRC_MULTI_VAL_HPP_ */
