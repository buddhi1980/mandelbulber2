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

QString cObjectsTreeWidget::nodeTypeToString(enumNodeType type)
{
	switch (type)
	{
		case enumNodeType::fractal: return "fractal";
		case enumNodeType::primitive: return "primitive";
		case enumNodeType::hybrid: return "hybrid";
		case enumNodeType::booleanAdd: return "boolean Add";
		case enumNodeType::booleanMul: return "boolean Mul";
		case enumNodeType::booleanSub: return "boolean Sub";
		default: return "unknown";
	}
}

void cObjectsTreeWidget::UpdateTree(
	std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractalParams)
{
	ui->treeWidget_objects->clear();

	// Set up columns: Name, Type, Object ID
	ui->treeWidget_objects->setColumnCount(3);
	QStringList headers;
	headers << "Name" << "Type" << "Object ID";
	ui->treeWidget_objects->setHeaderLabels(headers);

	QMap<int, QTreeWidgetItem *> nodeItems;

	cObjectsTree objectsTree = cObjectsTree();
	objectsTree.CreateNodeDataFromParameters(params, fractalParams);
	std::vector<cObjectsTree::sNodeData> sortedNodeDataList = objectsTree.GetSortedNodeDataList();

	for (const cObjectsTree::sNodeData &nodeData : sortedNodeDataList)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, nodeData.name);
		item->setText(1, nodeTypeToString(nodeData.type));
		item->setText(2, QString::number(nodeData.objectId));
		item->setData(0, Qt::UserRole, nodeData.id);
		item->setData(1, Qt::UserRole, int(nodeData.type));
		item->setData(2, Qt::UserRole, nodeData.objectId);
		nodeItems[nodeData.id] = item;
	}

	for (const cObjectsTree::sNodeData &node : sortedNodeDataList)
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
