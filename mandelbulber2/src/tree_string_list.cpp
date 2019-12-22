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
	actualItemId = 0;
	actualParentId = 0;
	items.append(newItem);
}

int cTreeStringList::AddItem(const QString &string)
{
	int newId = items.size();
	sItem newItem;
	newItem.string = string;
	newItem.itemId = newId;
	newItem.parentItemId = actualParentId;

	if (newItem.parentItemId >= 0)
	{
		items[actualParentId].chirdrenNodesId.append(newId);
	}

	items.append(newItem);
	actualItemId = newId;
	return newId;
}

int cTreeStringList::AddChilderItem(const QString &string, int parentId)
{
	int newId = items.size();
	sItem newItem;
	newItem.string = string;
	newItem.itemId = newId;

	if (parentId >= 0)
	{
		newItem.parentItemId = parentId;
	}
	else
	{
		newItem.parentItemId = actualItemId;
	}

	items[actualItemId].chirdrenNodesId.append(newId);

	items.append(newItem);

	actualItemId = newId;
	actualParentId = newId;
	return newId;
}
