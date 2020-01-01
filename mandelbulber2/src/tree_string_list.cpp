/*
 * tree_string_list.cpp
 *
 *  Created on: 22 gru 2019
 *      Author: krzysztof
 */

#include "tree_string_list.h"

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

int cTreeStringList::AddItem(const QString &string, bool enabled)
{
	if (CheckIfExists(string)) return -1;

	int newId = items.size();
	sItem newItem;
	newItem.string = string;
	newItem.itemId = newId;
	newItem.parentItemId = actualParentId;
	newItem.enabled = enabled;

	if (newItem.parentItemId >= 0)
	{
		items[actualParentId].chirdrenNodesId.append(newId);
	}

	items.append(newItem);
	actualItemId = newId;
	return newId;
}

int cTreeStringList::AddChildItem(const QString &string, bool enabled, int parentId)
{
	if (CheckIfExists(string)) return -1;

	int newId = items.size();
	sItem newItem;
	newItem.string = string;
	newItem.itemId = newId;
	newItem.enabled = enabled;

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
