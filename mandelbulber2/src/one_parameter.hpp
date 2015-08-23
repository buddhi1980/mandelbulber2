/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cOneParameter class - stores all information of one parameter
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

#ifndef SRC_ONE_PARAMETER_HPP_
#define SRC_ONE_PARAMETER_HPP_

#include "multi_val.hpp"

namespace parameterContainer
{
	enum enumParameterType
	{
		paramStandard, paramApp, paramNoSave, paramOnlyForNet
	};

	enum enumMorphType
	{
		morphNone, morphLinear, morphLinearAngle, morphCatMullRom, morphCatMullRomAngle, morphAkima, morphAkimaAngle
	};

	enum enumValueSelection
	{
		valueDefault,	valueActual, valueMin, valueMax
	};
}
using namespace parameterContainer;

class cOneParameter
{
public:
	cOneParameter();
	~cOneParameter();
	enumVarType GetValueType() const {return defaultVal.GetDefaultype();}
	enumParameterType GetParameterType() const {return parType;}
	enumMorphType GetMorphType() const {return morphType;}
	QString GetOriginalContainerName() const {return originalContainer;}
	void SetParameterType(enumParameterType _parType) {parType = _parType;}
	void SetMorphType(enumMorphType _morphType) {morphType = _morphType;}
	void SetOriginalContainerName(const QString &containerName) {originalContainer = containerName;}
	bool isDefaultValue() const;
	cMultiVal GetMultival(enumValueSelection selection);
	void SetMultival(cMultiVal multi, enumValueSelection selection);
	bool IsEmpty() {return isEmpty;}

	template <class T> void Set(T val, enumValueSelection selection);
	template <class T> T Get(enumValueSelection selection) const;
	void LimitValue(cMultiVal &multi);

private:
	//parameter data
	enumMorphType morphType;
	enumParameterType parType;
	cMultiVal actualVal;
	cMultiVal defaultVal;
	cMultiVal minVal;
	cMultiVal maxVal;
	QString originalContainer;
	bool limitsDefined;
	bool isEmpty;
};

#endif /* SRC_ONE_PARAMETER_HPP_ */
