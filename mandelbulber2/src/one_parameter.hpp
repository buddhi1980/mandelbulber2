/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-16 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#ifndef MANDELBULBER2_SRC_ONE_PARAMETER_HPP_
#define MANDELBULBER2_SRC_ONE_PARAMETER_HPP_

#include "multi_val.hpp"

namespace parameterContainer
{
enum enumParameterType
{
	paramStandard,
	paramApp,
	paramNoSave,
	paramOnlyForNet
};

enum enumMorphType
{
	morphNone,
	morphLinear,
	morphLinearAngle,
	morphCatMullRom,
	morphCatMullRomAngle,
	morphAkima,
	morphAkimaAngle
};

enum enumValueSelection
{
	valueDefault,
	valueActual,
	valueMin,
	valueMax
};
}
using namespace parameterContainer;

class cOneParameter
{
public:
	cOneParameter();
	~cOneParameter();
	enumVarType GetValueType() const { return defaultVal.GetDefaultype(); }
	enumParameterType GetParameterType() const { return parType; }
	enumMorphType GetMorphType() const { return morphType; }
	QString GetOriginalContainerName() const { return originalContainer; }
	void SetParameterType(enumParameterType _parType) { parType = _parType; }
	void SetMorphType(enumMorphType _morphType) { morphType = _morphType; }
	void SetOriginalContainerName(const QString &containerName) { originalContainer = containerName; }
	bool isDefaultValue() const;
	cMultiVal GetMultival(enumValueSelection selection) const;
	void SetMultival(cMultiVal multi, enumValueSelection selection);
	bool IsEmpty() const { return isEmpty; }

	template <class T>
	void Set(T val, enumValueSelection selection);
	template <class T>
	T Get(enumValueSelection selection) const;
	void LimitValue(cMultiVal &multi) const;

private:
	// parameter data
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

#endif /* MANDELBULBER2_SRC_ONE_PARAMETER_HPP_ */
