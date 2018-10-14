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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * MyGroupBox class - promoted MyGroupBox widget with context menu
 */

#include "my_group_box.h"

#include <qaction.h>
#include <qicon.h>
#include <qlist.h>
#include <qmenu.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qstring.h>

#include "src/parameters.hpp"

MyGroupBox::MyGroupBox(QWidget *parent) : QGroupBox(parent), CommonMyWidgetWrapper(this)
{
	defaultValue = false;
	firstDisplay = true;
	actionResetAllToDefault = nullptr;
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
	QList<QWidget *> list = findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
	for (auto &widget : list)
	{
		if (on)
		{
			widget->show();
		}
		else
		{
			widget->hide();
		}
	}
}

void MyGroupBox::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu;
	QIcon iconReset = QIcon(":system/icons/edit-undo.png");

	actionResetAllToDefault = menu->addAction(tr("Reset all to default"));
	actionResetAllToDefault->setIcon(iconReset);

	connect(actionResetAllToDefault, SIGNAL(triggered()), this, SLOT(slotResetAllToDefault()));
	CommonMyWidgetWrapper::contextMenuEvent(event, menu);
}

void MyGroupBox::slotResetAllToDefault()
{
	QList<QWidget *> listOfWidgets = findChildren<QWidget *>();

	foreach (QWidget *widget, listOfWidgets)
	{
		CommonMyWidgetWrapper *myWidget = dynamic_cast<CommonMyWidgetWrapper *>(widget);
		if (myWidget)
		{
			myWidget->resetToDefault();
		}
	}
}
