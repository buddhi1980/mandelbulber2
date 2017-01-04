/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * MyUiLoader class - extension for QUiLoader class
 */

#include "my_ui_loader.h"
#include "../qt/file_select_widget.h"
#include "../qt/material_selector.h"
#include "../qt/my_check_box.h"
#include "../qt/my_color_button.h"
#include "../qt/my_double_spin_box.h"
#include "../qt/my_group_box.h"
#include "../qt/my_line_edit.h"
#include "../qt/my_spin_box.h"

MyUiLoader::~MyUiLoader()
{
}

QWidget *MyUiLoader::createWidget(const QString &className, QWidget *parent, const QString &name)
{
	QWidget *widget;

	if (className == QString("MyLineEdit"))
	{
		widget = new MyLineEdit(parent);
		widget->setObjectName(name);
	}
	else if (className == QString("MySpinBox"))
	{
		widget = new MySpinBox(parent);
		widget->setObjectName(name);
	}
	else if (className == QString("MyDoubleSpinBox"))
	{
		widget = new MyDoubleSpinBox(parent);
		widget->setObjectName(name);
	}
	else if (className == QString("MyCheckBox"))
	{
		widget = new MyCheckBox(parent);
		widget->setObjectName(name);
	}
	else if (className == QString("MyGroupBox"))
	{
		widget = new MyGroupBox(parent);
		widget->setObjectName(name);
	}
	else if (className == QString("MyColorButton"))
	{
		widget = new MyColorButton(parent);
		widget->setObjectName(name);
	}
	else if (className == QString("FileSelectWidget"))
	{
		widget = new FileSelectWidget(parent);
		widget->setObjectName(name);
	}
	else if (className == QString("cMaterialSelector"))
	{
		widget = new cMaterialSelector(parent);
		widget->setObjectName(name);
	}
	else
	{
		widget = QUiLoader::createWidget(className, parent, name);
	}
	return widget;
}
