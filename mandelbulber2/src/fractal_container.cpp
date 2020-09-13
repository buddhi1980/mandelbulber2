/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cFractalContainer - class to store fractal parameters
 */

#include "fractal_container.hpp"

#include <QDebug>

std::shared_ptr<cFractalContainer> gParFractal;

cFractalContainer::cFractalContainer()
{
	fractals.resize(NUMBER_OF_FRACTALS);
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		fractals[i].reset(new cParameterContainer());
	}
}

cFractalContainer::cFractalContainer(const cFractalContainer &other)
{
	fractals.resize(NUMBER_OF_FRACTALS);
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		fractals[i].reset(new cParameterContainer());
		*fractals[i] = *other.fractals[i];
	}
}

cFractalContainer &cFractalContainer::operator=(const cFractalContainer &other)
{
	if (&other == this) return *this;

	fractals.resize(NUMBER_OF_FRACTALS);
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		fractals[i].reset(new cParameterContainer());
		*fractals[i] = *other.fractals[i];
	}
	return *this;
}

std::shared_ptr<cParameterContainer> cFractalContainer::operator[](int index)
{
	if (index >= 0 && index < NUMBER_OF_FRACTALS)
	{
		return fractals[index];
	}
	else
	{
		qCritical() << "cParameterContainer &cFractalContainer::operator[](int index): wrong index:"
								<< index;
		return fractals[0];
	}
}

const std::shared_ptr<cParameterContainer> cFractalContainer::operator[](int index) const
{
	if (index >= 0 && index < NUMBER_OF_FRACTALS)
	{
		return fractals[index];
	}
	else
	{
		qCritical()
			<< "const cParameterContainer &cFractalContainer::operator[](int index) const: wrong index:"
			<< index;
		return fractals[0];
	}
}

std::shared_ptr<cParameterContainer> cFractalContainer::at(int index)
{
	if (index >= 0 && index < NUMBER_OF_FRACTALS)
	{
		return fractals[index];
	}
	else
	{
		qCritical() << "cParameterContainer &cFractalContainer::at(int index): wrong index:" << index;
		return fractals[0];
	}
}

const std::shared_ptr<cParameterContainer> cFractalContainer::at(int index) const
{
	if (index >= 0 && index < NUMBER_OF_FRACTALS)
	{
		return fractals[index];
	}
	else
	{
		qCritical() << "const cParameterContainer &cFractalContainer::at(int index) const: wrong index:"
								<< index;
		return fractals[0];
	}
}

bool cFractalContainer::isUsedCustomFormula()
{
	bool used = false;
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		if (!fractals[i]->isDefaultValue("formula_code"))
		{
			used = true;
			break;
		}
	}
	return used;
}
