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
 * list of fractal formulas
 */

#ifndef MANDELBULBER2_FORMULA_DEFINITION_ALL_FRACTAL_LIST_HPP_
#define MANDELBULBER2_FORMULA_DEFINITION_ALL_FRACTAL_LIST_HPP_

#include <utility>

#include "formula/definition/all_fractal_definitions.h"
#include "formula/definition/abstract_fractal.h"
#include "all_fractal_list_enums.hpp"
#include "src/algebra.hpp"
#include "src/fractal_enums.h"
#include "src/system_directories.hpp"

struct sFractal;
struct sExtendedAux;

using fractalFormulaFcn = void (*)(CVector4 &, const sFractal *, sExtendedAux &);

struct sFractalDescription
{
	QString nameInComboBox;
	QString internalName;
	fractal::enumFractalFormula internalID;
	fractalFormulaFcn fractalFormulaFunction;
	fractal::enumDEType DEType;
	fractal::enumDEFunctionType DEFunctionType;
	fractal::enumCPixelAddition cpixelAddition;
	double defaultBailout;
	fractal::enumDEAnalyticFunction DEAnalyticFunction;
	fractal::enumColoringFunction coloringFunction;

	sFractalDescription(QString _nameInComboBox, QString _internalName,
		fractal::enumFractalFormula _internalID, fractalFormulaFcn _fractalFormulaFunction,
		fractal::enumDEType _DEType, fractal::enumDEFunctionType _DEFunctionType,
		fractal::enumCPixelAddition _cpixelAddition, double _defaultBailout,
		fractal::enumDEAnalyticFunction _DEAnalyticFunction,
		fractal::enumColoringFunction _coloringFunction)
			: nameInComboBox(std::move(_nameInComboBox)),
				internalName(std::move(_internalName)),
				internalID(_internalID),
				fractalFormulaFunction(_fractalFormulaFunction),
				DEType(_DEType),
				DEFunctionType(_DEFunctionType),
				cpixelAddition(_cpixelAddition),
				defaultBailout(_defaultBailout),
				DEAnalyticFunction(_DEAnalyticFunction),
				coloringFunction(_coloringFunction)
	{
	}
	QString getIconName() const
	{
		if (internalID == fractal::none)
		{
			return QString(":system/icons/list-remove.svg");
		}
		return systemDirectories.sharedDir + "formula" + QDir::separator() + "img" + QDir::separator()
					 + internalName + ".png";
	}
	QString getUiFilename() const
	{
		return systemDirectories.sharedDir + "formula" + QDir::separator() + "ui" + QDir::separator()
					 + internalName + ".ui";
	}
	QString getOpenCLFilename() const
	{
		return systemDirectories.sharedDir + "formula" + QDir::separator() + "opencl"
					 + QDir::separator() + internalName + ".cl";
	}
};

extern QList<sFractalDescription> fractalList;

extern QList<cAbstractFractal *> newFractalList;

void DefineFractalList(QList<cAbstractFractal *> *fractalList);

#endif /* MANDELBULBER2_FORMULA_DEFINITION_ALL_FRACTAL_LIST_HPP_ */
