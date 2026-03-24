
/*
 * objects_tree.cpp
 *
 *  Created on: 22 lis 2025
 *      Author: krzysztof
 */

#include "objects_tree_widget.h"

#include <QStack>

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

	connect(
		ui->treeWidget_objects, &QTreeWidget::itemChanged, this, &cObjectsTreeWidget::onItemChanged);
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

	ui->treeWidget_objects->setColumnCount(4);
	QStringList headers;
	headers << "Name" << "Type" << "Object ID" << "Position";
	ui->treeWidget_objects->setHeaderLabels(headers);

	QMap<int, QTreeWidgetItem *> nodeItems;

	cObjectsTree objectsTree = cObjectsTree();
	objectsTree.CreateNodeDataFromParameters(params);
	std::vector<cObjectsTree::sNodeData> sortedNodeDataList = objectsTree.GetSortedNodeDataList();

	// Build a map from node ID to node type for parent lookup
	QMap<int, enumNodeType> nodeTypeMap;
	for (const cObjectsTree::sNodeData &nodeData : sortedNodeDataList)
		nodeTypeMap[nodeData.id] = nodeData.type;

	for (const cObjectsTree::sNodeData &nodeData : sortedNodeDataList)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, nodeData.name);
		item->setText(1, nodeTypeToString(nodeData.type));
		item->setText(2, QString::number(nodeData.objectId));

		bool isFractalInHybrid = nodeData.type == enumNodeType::fractal
														 && nodeTypeMap.value(nodeData.parentId) == enumNodeType::hybrid;

		if (!isFractalInHybrid)
		{
			item->setText(3, QString("%1 %2 %3")
												 .arg(nodeData.position.x, 0, 'f', 6)
												 .arg(nodeData.position.y, 0, 'f', 6)
												 .arg(nodeData.position.z, 0, 'f', 6));
		}

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
			ui->treeWidget_objects->addTopLevelItem(item);
		else
			nodeItems[parentId]->addChild(item);
	}

	// Make name (col 0) and position (col 3) editable
	for (QTreeWidgetItem *item : nodeItems)
	{
		item->setFlags(item->flags() | Qt::ItemIsEditable);
	}

	ui->treeWidget_objects->expandAll();
}

void cObjectsTreeWidget::onItemChanged(QTreeWidgetItem *item, int column)
{
	// Temporarily block signals to avoid recursive calls
	QSignalBlocker blocker(ui->treeWidget_objects);

	if (column == 3)
	{
		QString text = item->text(3);
		QStringList parts = text.split(' ', Qt::SkipEmptyParts);
		if (parts.size() == 3)
		{
			bool okX, okY, okZ;
			double x = parts[0].toDouble(&okX);
			double y = parts[1].toDouble(&okY);
			double z = parts[2].toDouble(&okZ);
			if (okX && okY && okZ)
			{
				// Reformat to consistent display
				item->setText(3, QString("%1 %2 %3").arg(x, 0, 'f', 6).arg(y, 0, 'f', 6).arg(z, 0, 'f', 6));
			}
			else
			{
				// Reset to zero on invalid input
				item->setText(3, "0.000000 0.000000 0.000000");
			}
		}
		else
		{
			item->setText(3, "0.000000 0.000000 0.000000");
		}
	}
}

void cObjectsTreeWidget::StoreTreeToParams(
	std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractalParams)
{
	// Collect all tree items using a stack-based traversal
	QList<QTreeWidgetItem *> allItems;
	QStack<QTreeWidgetItem *> stack;
	for (int i = 0; i < ui->treeWidget_objects->topLevelItemCount(); ++i)
		stack.push(ui->treeWidget_objects->topLevelItem(i));
	while (!stack.isEmpty())
	{
		QTreeWidgetItem *item = stack.pop();
		allItems.append(item);
		for (int i = 0; i < item->childCount(); ++i)
			stack.push(item->child(i));
	}

	// Build set of node IDs currently in the tree
	QSet<int> treeNodeIds;
	for (QTreeWidgetItem *item : allItems)
		treeNodeIds.insert(item->data(0, Qt::UserRole).toInt());

	// Remove parameters for nodes no longer in the tree
	QStringList allParams = params->GetListOfParameters();
	for (const QString &paramName : allParams)
	{
		if (paramName.startsWith("node_definition_"))
		{
			QString suffix = paramName.mid(QString("node_definition").length()); // e.g. "_0001"
			int nodeId = suffix.mid(1).toInt(); // remove leading '_', parse int
			if (!treeNodeIds.contains(nodeId))
			{
				params->DeleteParameter(paramName);
				QString positionParamName = "node_position" + suffix;
				if (params->IfExists(positionParamName)) params->DeleteParameter(positionParamName);
			}
		}
	}

	for (QTreeWidgetItem *item : allItems)
	{
		int nodeId = item->data(0, Qt::UserRole).toInt();
		int nodeType = item->data(1, Qt::UserRole).toInt();
		int objectId = item->data(2, Qt::UserRole).toInt();
		QString name = item->text(0);

		// Determine parent ID from parent widget item
		int parentId = 0;
		if (item->parent()) parentId = item->parent()->data(0, Qt::UserRole).toInt();

		QString suffix = QString("_%1").arg(nodeId, 4, 10, QChar('0'));
		QString definitionParamName = "node_definition" + suffix;
		QString positionParamName = "node_position" + suffix;

		// Format: name,id,type,parent_id,object_id
		QString definitionValue =
			QString("%1,%2,%3,%4,%5").arg(name).arg(nodeId).arg(nodeType).arg(parentId).arg(objectId);

		if (params->IfExists(definitionParamName))
			params->Set(definitionParamName, definitionValue);
		else
			params->addParam(definitionParamName, definitionValue, morphNone, paramStandard);

		// Store position only for nodes that are not fractals inside a hybrid group
		bool isFractalInHybrid =
			(enumNodeType(nodeType) == enumNodeType::fractal) && item->parent()
			&& (enumNodeType(item->parent()->data(1, Qt::UserRole).toInt()) == enumNodeType::hybrid);

		if (!isFractalInHybrid)
		{
			// Parse position from space-separated text in column 3
			CVector3 position(0.0, 0.0, 0.0);
			QString posText = item->text(3);
			QStringList parts = posText.split(' ', Qt::SkipEmptyParts);
			if (parts.size() == 3)
			{
				position.x = parts[0].toDouble();
				position.y = parts[1].toDouble();
				position.z = parts[2].toDouble();
			}

			if (params->IfExists(positionParamName))
				params->Set(positionParamName, position);
			else
				params->addParam(positionParamName, position, morphNone, paramStandard);
		}
	}
}

void cObjectsTreeWidget::pressedRefreshButton()
{
	UpdateTree(gPar, gParFractal);
}
