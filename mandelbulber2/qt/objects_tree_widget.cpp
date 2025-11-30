/*
 * objects_tree.cpp
 *
 *  Created on: 22 lis 2025
 *      Author: krzysztof
 */

#include "objects_tree_widget.h"

#include "ui_objects_tree_widget.h"
#include "src/initparameters.hpp"
#include "src/fractal_container.hpp"
#include "src/objects_tree.h"

cObjectsTreeWidget::cObjectsTreeWidget(QWidget *parent)
		: QWidget(parent), ui(new Ui::cObjectsTreeWidget)
{
	ui->setupUi(this);

	connect(
		ui->pushButton_refresh, &QPushButton::clicked, this, &cObjectsTreeWidget::pressedRefreshButton);
}

cObjectsTreeWidget::~cObjectsTreeWidget()
{
	delete ui;
}

void cObjectsTreeWidget::UpdateTree(
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

	cObjectsTree objectsTree = cObjectsTree();
	objectsTree.CreateNodeDataFromParameters(params, fractalParams);
	cObjectsTree::nodeData_t nodeDataMap = objectsTree.GetNodeDataMap();

	for (auto it = nodeDataMap.begin(); it != nodeDataMap.end(); ++it)
	{
		int nodeId = it.key();
		const cObjectsTree::NodeData &nodeData = it.value();
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, nodeData.name);
		item->setData(0, Qt::UserRole, nodeId);
		item->setData(1, Qt::UserRole, nodeData.type);
		item->setData(2, Qt::UserRole, nodeData.objectId);
		nodeItems[nodeId] = item;
	}

	// get sorted list of nodes (for testing)
	QList<cObjectsTree::NodeData> sortedNodeDataList = objectsTree.GetSortedNodeDataList();

	// Second pass: build the tree structure
	for (cObjectsTree::NodeData node : sortedNodeDataList)
	{
		int nodeId = node.id;
		int parentId = node.parentId;
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

void cObjectsTreeWidget::pressedRefreshButton()
{
	UpdateTree(gPar, gParFractal);
}
