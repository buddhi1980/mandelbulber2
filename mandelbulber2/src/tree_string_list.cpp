/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2019-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * TODO: description
 */

#include "tree_string_list.h"

#include <QDebug>

cTreeStringList::cTreeStringList()
{
	sItem newItem;
	newItem.string = "root";
	newItem.itemId = 0;
	newItem.parentItemId = -1;
	newItem.enabled = true;
	actualItemId = 0;
	actualParentId = 0;
	items.append(newItem);
}

int cTreeStringList::AddItem(const QString &string, bool enabled, bool isFloat)
{
	if (CheckIfExists(string)) return -1;

	int newId = items.size();
	sItem newItem;
	newItem.string = string;
	newItem.itemId = newId;
	newItem.parentItemId = actualParentId;
	newItem.enabled = enabled;
	newItem.isFloat = isFloat;

	if (newItem.parentItemId >= 0)
	{
		items[actualParentId].chirdrenNodesId.append(newId);
	}

	items.append(newItem);
	actualItemId = newId;
	return newId;
}

int cTreeStringList::AddChildItem(const QString &string, bool enabled, bool isFloat, int parentId)
{
	if (CheckIfExists(string)) return -1;

	int newId = items.size();
	sItem newItem;
	newItem.string = string;
	newItem.itemId = newId;
	newItem.enabled = enabled;
	newItem.isFloat = isFloat;

	if (parentId >= 0)
	{
		newItem.parentItemId = parentId;
	}
	else
	{
		newItem.parentItemId = actualItemId;
	}

	items[newItem.parentItemId].chirdrenNodesId.append(newId);

	items.append(newItem);

	actualItemId = newId;
	actualParentId = newId;
	return newId;
}

QString cTreeStringList::GetString(int index)
{
	if (index >= 0 && index < items.size())
	{
		return items.at(index).string;
	}
	else
	{
		qCritical() << "cTreeStringList::GetString(int index): Wrong index: " << index;
		return QString();
	}
}

int cTreeStringList::GetParentIndex(int index)
{
	if (index >= 0 && index < items.size())
	{
		return items.at(index).parentItemId;
	}
	else
	{
		qCritical() << "cTreeStringList::GetParentIndex(int index): Wrong index: " << index;
		return -1;
	}
}

bool cTreeStringList::IsEnabled(int index)
{
	if (index >= 0 && index < items.size())
	{
		return items.at(index).enabled;
	}
	else
	{
		qCritical() << "cTreeStringList::IsEnabled(int index): Wrong index: " << index;
		return false;
	}
}

bool cTreeStringList::IsGroup(int index)
{
	if (index >= 0 && index < items.size())
	{
		return items.at(index).chirdrenNodesId.size() > 0;
	}
	else
	{
		qCritical() << "cTreeStringList::IsGroup(int index): Wrong index: " << index;
		return false;
	}
}

bool cTreeStringList::IsFloat(int index)
{
	if (index >= 0 && index < items.size())
	{
		return items.at(index).isFloat;
	}
	else
	{
		qCritical() << "cTreeStringList::IsFloat(int index): Wrong index: " << index;
		return false;
	}
}

QList<int> cTreeStringList::GetChildrens(int index)
{
	if (index >= 0 && index < items.size())
	{
		return items.at(index).chirdrenNodesId;
	}
	else
	{
		qCritical() << "cTreeStringList::GetChildrens(int index): Wrong index: " << index;
		return QList<int>();
	}
}

bool cTreeStringList::CheckIfExists(QString string)
{
	bool exists = false;

	for (int i = 0; i < items.size(); i++)
	{
		if (items.at(i).string == string) return true;
	}

	return false;
}
