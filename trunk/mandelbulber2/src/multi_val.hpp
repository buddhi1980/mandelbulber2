/*
 * multi_val.hpp
 *
 *  Created on: Dec 21, 2014
 *      Author: krzysztof
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
