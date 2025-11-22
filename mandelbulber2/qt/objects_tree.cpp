/*
 * objects_tree.cpp
 *
 *  Created on: 22 lis 2025
 *      Author: krzysztof
 */

#include "objects_tree.h"
#include "ui_objects_tree_widget.h"
#include "src/initparameters.hpp"
#include "src/fractal_container.hpp"

cObjectsTree::cObjectsTree(QWidget *parent) : QWidget(parent), ui(new Ui::cObjectsTree)
{
	ui->setupUi(this);

	connect(ui->pushButton_refresh, &QPushButton::clicked, this, &cObjectsTree::pressedRefreshButton);
}

cObjectsTree::~cObjectsTree()
{
	delete ui;
}

void cObjectsTree::UpdateTree(
	std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractalParams)
{
	ui->treeWidget_objects->clear();

	// OBJECT TREE TEST PARAMETERS

	// Each "nodeXXXX" parameter is a QString with comma-separated values representing:
	// name, id, type, parent_id, object_id
	// Example: "hybrid group 1,1,0,0,-1"
	// - name: Node display name (QString)
	// - id: Node unique integer ID
	// - type: Node type (int, from enumNodeType)
	// - parent_id: Parent node ID (int)
	// - object_id: Associated object ID (int, or -1 if not applicable)

	// First pass: create all items and store their data
	struct NodeData
	{
		QString name;
		int id;
		int type;
		int parentId;
		int objectId;
	};

	QMap<int, QTreeWidgetItem *> nodeItems;
	QMap<int, NodeData> nodeDataMap;

	QStringList allParams = params->GetListOfParameters();
	for (const QString &paramName : allParams)
	{
		if (paramName.startsWith("node"))
		{
			QString paramValue = params->Get<QString>(paramName);
			QStringList values = paramValue.split(',');
			if (values.size() == 5)
			{
				NodeData data = {
					values[0], values[1].toInt(), values[2].toInt(), values[3].toInt(), values[4].toInt()};
				nodeDataMap[data.id] = data;
				QTreeWidgetItem *item = new QTreeWidgetItem();
				item->setText(0, data.name);
				item->setData(0, Qt::UserRole, data.id);
				item->setData(1, Qt::UserRole, data.type);
				item->setData(2, Qt::UserRole, data.objectId);
				nodeItems[data.id] = item;
			}
		}
	}

	// Second pass: build the tree structure
	for (auto it = nodeDataMap.begin(); it != nodeDataMap.end(); ++it)
	{
		int nodeId = it.key();
		int parentId = it.value().parentId;
		QTreeWidgetItem *item = nodeItems[nodeId];
		if (parentId == 0 || !nodeItems.contains(parentId))
		{
			ui->treeWidget_objects->addTopLevelItem(item);
		}
		else
		{
			nodeItems[parentId]->addChild(item);
		}
	}

	ui->treeWidget_objects->expandAll();
}

void cObjectsTree::pressedRefreshButton()
{
	UpdateTree(gPar, gParFractal);
}
