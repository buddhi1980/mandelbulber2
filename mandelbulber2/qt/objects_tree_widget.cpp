
/*
 * objects_tree_widget.cpp
 *
 *  Created on: 22 lis 2025
 *      Author: krzysztof
 */

#include "objects_tree_widget.h"

#include <QStack>
#include <QComboBox>

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

// --- Private helpers ---

QList<QTreeWidgetItem *> cObjectsTreeWidget::collectAllTreeItems() const
{
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
	return allItems;
}

int cObjectsTreeWidget::getNodeType(QTreeWidgetItem *item) const
{
	QComboBox *combo = qobject_cast<QComboBox *>(ui->treeWidget_objects->itemWidget(item, 1));
	return combo ? combo->currentData().toInt() : item->data(1, Qt::UserRole).toInt();
}

bool cObjectsTreeWidget::isFractalInHybridGroup(QTreeWidgetItem *item) const
{
	if (enumNodeType(getNodeType(item)) != enumNodeType::fractal) return false;
	if (!item->parent()) return false;
	return enumNodeType(getNodeType(item->parent())) == enumNodeType::hybrid;
}

CVector3 cObjectsTreeWidget::parsePosition(const QString &text)
{
	CVector3 pos(0.0, 0.0, 0.0);
	QStringList parts = text.split(' ', Qt::SkipEmptyParts);
	if (parts.size() == 3)
	{
		pos.x = parts[0].toDouble();
		pos.y = parts[1].toDouble();
		pos.z = parts[2].toDouble();
	}
	return pos;
}

QString cObjectsTreeWidget::formatPosition(const CVector3 &pos)
{
	return QString("%1 %2 %3").arg(pos.x, 0, 'f', 6).arg(pos.y, 0, 'f', 6).arg(pos.z, 0, 'f', 6);
}

QComboBox *cObjectsTreeWidget::buildTypeComboBox(int currentType)
{
	QComboBox *combo = new QComboBox();
	combo->addItem("fractal", int(enumNodeType::fractal));
	combo->addItem("primitive", int(enumNodeType::primitive));
	combo->addItem("hybrid", int(enumNodeType::hybrid));
	combo->addItem("boolean Add", int(enumNodeType::booleanAdd));
	combo->addItem("boolean Mul", int(enumNodeType::booleanMul));
	combo->addItem("boolean Sub", int(enumNodeType::booleanSub));
	int idx = combo->findData(currentType);
	if (idx >= 0) combo->setCurrentIndex(idx);
	return combo;
}

void cObjectsTreeWidget::addOrSetParam(
	std::shared_ptr<cParameterContainer> params, const QString &name, const QString &value)
{
	if (params->IfExists(name))
		params->Set(name, value);
	else
		params->addParam(name, value, morphNone, paramStandard);
}

void cObjectsTreeWidget::addOrSetParam(
	std::shared_ptr<cParameterContainer> params, const QString &name, const CVector3 &value)
{
	if (params->IfExists(name))
		params->Set(name, value);
	else
		params->addParam(name, value, morphNone, paramStandard);
}

// --- Public methods ---

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
	ui->treeWidget_objects->setHeaderLabels({"Name", "Type", "Object ID", "Position"});

	cObjectsTree objectsTree;
	objectsTree.CreateNodeDataFromParameters(params);
	const auto sortedList = objectsTree.GetSortedNodeDataList();

	// Build parent-type lookup
	QMap<int, enumNodeType> nodeTypeMap;
	for (const auto &node : sortedList)
		nodeTypeMap[node.id] = node.type;

	QMap<int, QTreeWidgetItem *> nodeItems;

	// Create items
	for (const auto &nodeData : sortedList)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(0, nodeData.name);
		item->setText(1, nodeTypeToString(nodeData.type));
		item->setText(2, QString::number(nodeData.objectId));

		bool inHybrid = nodeData.type == enumNodeType::fractal
										&& nodeTypeMap.value(nodeData.parentId) == enumNodeType::hybrid;
		if (!inHybrid) item->setText(3, formatPosition(nodeData.position));

		item->setData(0, Qt::UserRole, nodeData.id);
		item->setData(1, Qt::UserRole, int(nodeData.type));
		item->setData(2, Qt::UserRole, nodeData.objectId);
		item->setFlags(item->flags() | Qt::ItemIsEditable);

		nodeItems[nodeData.id] = item;
	}

	// Build tree structure
	for (const auto &node : sortedList)
	{
		QTreeWidgetItem *item = nodeItems[node.id];
		if (node.parentId == 0 || !nodeItems.contains(node.parentId))
			ui->treeWidget_objects->addTopLevelItem(item);
		else
			nodeItems[node.parentId]->addChild(item);
	}

	ui->treeWidget_objects->expandAll();

	// Attach type combo boxes
	for (QTreeWidgetItem *item : nodeItems)
	{
		int currentType = item->data(1, Qt::UserRole).toInt();
		ui->treeWidget_objects->setItemWidget(item, 1, buildTypeComboBox(currentType));
	}
}

void cObjectsTreeWidget::onItemChanged(QTreeWidgetItem *item, int column)
{
	QSignalBlocker blocker(ui->treeWidget_objects);
	if (column != 3) return;

	QString text = item->text(3);
	QStringList parts = text.split(' ', Qt::SkipEmptyParts);
	if (parts.size() == 3)
	{
		bool okX, okY, okZ;
		double x = parts[0].toDouble(&okX);
		double y = parts[1].toDouble(&okY);
		double z = parts[2].toDouble(&okZ);
		item->setText(
			3, (okX && okY && okZ) ? formatPosition({x, y, z}) : "0.000000 0.000000 0.000000");
	}
	else
	{
		item->setText(3, "0.000000 0.000000 0.000000");
	}
}

void cObjectsTreeWidget::StoreTreeToParams(
	std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractalParams)
{
	const auto allItems = collectAllTreeItems();

	// Build set of current node IDs
	QSet<int> treeNodeIds;
	for (QTreeWidgetItem *item : allItems)
		treeNodeIds.insert(item->data(0, Qt::UserRole).toInt());

	// Remove stale parameters
	for (const QString &paramName : params->GetListOfParameters())
	{
		if (!paramName.startsWith("node_definition_")) continue;
		QString suffix = paramName.mid(QString("node_definition").length()); // e.g. "_0001"
		int nodeId = suffix.mid(1).toInt();
		if (!treeNodeIds.contains(nodeId))
		{
			params->DeleteParameter(paramName);
			QString posParam = "node_position" + suffix;
			if (params->IfExists(posParam)) params->DeleteParameter(posParam);
		}
	}

	// Store each node
	for (QTreeWidgetItem *item : allItems)
	{
		int nodeId = item->data(0, Qt::UserRole).toInt();
		int nodeType = getNodeType(item);
		int objectId = item->data(2, Qt::UserRole).toInt();
		QString name = item->text(0);
		int parentId = item->parent() ? item->parent()->data(0, Qt::UserRole).toInt() : 0;

		QString suffix = QString("_%1").arg(nodeId, 4, 10, QChar('0'));
		QString defParam = "node_definition" + suffix;
		QString posParam = "node_position" + suffix;

		addOrSetParam(params, defParam,
			QString("%1,%2,%3,%4,%5").arg(name).arg(nodeId).arg(nodeType).arg(parentId).arg(objectId));

		if (!isFractalInHybridGroup(item))
			addOrSetParam(params, posParam, parsePosition(item->text(3)));
	}
}

void cObjectsTreeWidget::pressedRefreshButton()
{
	UpdateTree(gPar, gParFractal);
}
