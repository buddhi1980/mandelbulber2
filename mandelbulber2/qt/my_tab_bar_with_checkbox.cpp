/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * MyTabBar - promoted QTabBar widget with swap tabs functionality
 */

#include "my_tab_bar_with_checkbox.h"

#include <QtWidgets/QtWidgets>

#include "my_check_box.h"

MyTabBarWithCheckBox::MyTabBarWithCheckBox(QWidget *parent) : QTabBar(parent) {}

void MyTabBarWithCheckBox::AddCheckBox(int index, const QString &checkBoxName)
{
	MyCheckBox *chActive = new MyCheckBox(this);
	chActive->setObjectName(checkBoxName);
	connect(chActive, &QCheckBox::stateChanged, this, &MyTabBarWithCheckBox::slotToggleActive);
	setTabButton(index, QTabBar::LeftSide, chActive);
	listOfCheboxes.insert(checkBoxName, index);
}

void MyTabBarWithCheckBox::slotToggleActive(int state)
{
	QString buttonName = sender()->objectName();
	int index = listOfCheboxes[buttonName];

	QPalette palette = QApplication::palette();
	setTabTextColor(index, state == Qt::Checked ? palette.text().color() : palette.dark().color());
	// setTabEnabled(index, state == Qt::Checked);
	emit toggledEnable(index + 1, state == Qt::Checked);
}

void MyTabBarWithCheckBox::RemoveCheckBox(int i)
{
	QString checkBoxName = listOfCheboxes.key(i);
	QWidget *checkBox = findChild<MyCheckBox *>(checkBoxName);
	delete checkBox;
	listOfCheboxes.remove(checkBoxName);
}
