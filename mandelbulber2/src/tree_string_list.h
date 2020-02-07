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

	int AddItem(const QString &string, bool enabled, bool isFloat);
	int AddChildItem(const QString &string, bool enabled, bool isFloat, int parentId = -1);
	int GetSize() { return items.size(); }
	QString GetString(int index);
	int GetParentIndex(int index);
	bool IsEnabled(int index);
	bool IsGroup(int index);
	bool IsFloat(int index);
	QList<int> GetChildrens(int index);

private:
	struct sItem
	{
		QString string;
		int itemId = -1;
		int parentItemId = -1;
		bool enabled = true;
		bool isFloat = false;
		QList<int> chirdrenNodesId;
	};

	bool CheckIfExists(QString string);

	QList<sItem> items;
	int actualItemId = -1;
	int actualParentId = -1;
};

#endif /* MANDELBULBER2_SRC_TREE_STRING_LIST_H_ */
