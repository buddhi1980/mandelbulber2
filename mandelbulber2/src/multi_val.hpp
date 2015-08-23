/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cMultiVal class - union like storage for one parameter
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

#ifndef MULTI_VAL_HPP_
#define MULTI_VAL_HPP_

#include <QtCore>
#include "color_palette.hpp"
#include "algebra.hpp"
#include "color_structures.hpp"

namespace parameterContainer
{
	enum enumVarType
	{
		typeNull, typeInt, typeDouble, typeString, typeVector3, typeRgb, typeBool, typeColorPalette
	};
}

using namespace parameterContainer;

class cMultiVal
{
public:
	cMultiVal();
	~cMultiVal();

	enumVarType Store(double val);
	enumVarType Store(int val);
	enumVarType Store(QString val);
	enumVarType Store(CVector3 val);
	enumVarType Store(sRGB val);
	enumVarType Store(bool val);
	enumVarType Store(cColorPalette val);
	enumVarType Get(double &val) const;
	enumVarType Get(int &val) const;
	enumVarType Get(QString &val) const;
	enumVarType Get(CVector3 &val) const;
	enumVarType Get(sRGB &val) const;
	enumVarType Get(bool &val) const;
	enumVarType Get(cColorPalette &val) const;
	enumVarType GetDefaultype(void) const {return type;}
	bool operator==(const cMultiVal &m) const;

private:
	QString MakePaletteString(cColorPalette &palette);
	cColorPalette GetPaletteFromString(const QString &paletteString) const;
	bool isEqual(const cMultiVal &m) const;

	double dVal[4];
	int iVal[4];
	QString sVal;
	enumVarType type;
	bool typeDefined;
};

#endif /* MULTI_VAL_HPP_ */
