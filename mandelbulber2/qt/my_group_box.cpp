/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * MyGroupBox class - promoted MyGroupBox widget with context menu
 */

#include "my_group_box.h"
#include "../src/animation_flight.hpp"
#include "../src/animation_keyframes.hpp"
#include <QLineEdit>

MyGroupBox::MyGroupBox(QWidget *parent) : QGroupBox(parent), CommonMyWidgetWrapper(this)
{
	defaultValue = false;
	firstDisplay = true;
	connect(this, SIGNAL(toggled(bool)), this, SLOT(slotToggled(bool)));
}

void MyGroupBox::resetToDefault()
{
	setChecked(defaultValue);
	emit toggled(defaultValue);
}

QString MyGroupBox::getDefaultAsString()
{
	return defaultValue ? "true" : "false";
}

QString MyGroupBox::getFullParameterName()
{
	return parameterName;
}

void MyGroupBox::paintEvent(QPaintEvent *event)
{
	if (firstDisplay)
	{
		originalText = title();
		firstDisplay = false;
	}
	QString displayTitle = originalText;
	if (!isChecked())
	{
		displayTitle += " ... ";
	}

	if (isChecked() != GetDefault())
	{
		displayTitle += " *";
	}
	setTitle(displayTitle);
	QGroupBox::paintEvent(event);
}

bool MyGroupBox::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		int val = parameterContainer->GetDefault<int>(parameterName);
		defaultValue = val;
		gotDefault = true;
		setToolTipText();
	}
	return defaultValue;
}

void MyGroupBox::slotToggled(bool on) const
{
	QList<QWidget *> list = this->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
	for (int i = 0; i < list.size(); ++i)
	{
		if (on)
		{
			list[i]->show();
		}
		else
		{
			list[i]->hide();
		}
	}
}

void MyGroupBox::contextMenuEvent(QContextMenuEvent *event)
{
	CommonMyWidgetWrapper::contextMenuEvent(event);
}
