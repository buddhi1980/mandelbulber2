/*
 * tree_string_list.h
 *
 *  Created on: 22 gru 2019
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_TREE_STRING_LIST_H_
#define MANDELBULBER2_SRC_TREE_STRING_LIST_H_
#include <QtCore>

class cTreeStringList
{
public:
	cTreeStringList();
	~cTreeStringList() = default;

	int AddItem(const QString &string);
	int AddChildItem(const QString &string, int parentId = -1);
	int GetSize() { return items.size(); }
	QString GetString(int index);

private:
	struct sItem
	{
		QString string;
		int itemId = -1;
		int parentItemId = -1;
		bool enabled = true;
		QList<int> chirdrenNodesId;
	};

	QList<sItem> items;
	int actualItemId = -1;
	int actualParentId = -1;
};

#endif /* MANDELBULBER2_SRC_TREE_STRING_LIST_H_ */
