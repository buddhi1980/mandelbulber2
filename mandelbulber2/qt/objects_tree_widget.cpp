
/*
 * objects_tree_widget.cpp
 *
 *  Created on: 22 lis 2025
 *      Author: krzysztof
 */

#include "objects_tree_widget.h"

#include <memory>
#include <QStack>
#include <QComboBox>
#include <QInputDialog>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QList>

#include "../src/interface.hpp"
#include "../src/primitives.h"
#include "src/my_ui_loader.h"
#include "src/system_directories.hpp"
#include "ui_objects_tree_widget.h"
#include "src/initparameters.hpp"
#include "src/fractal_container.hpp"
#include "src/objects_tree.h"
#include "tab_fractal.h"
#include "formula/definition/all_fractal_list.hpp"

cObjectsTreeWidget::cObjectsTreeWidget(QWidget *parent)
		: QWidget(parent), ui(new Ui::cObjectsTreeWidget)
{
	ui->setupUi(this);
	connect(
		ui->pushButton_refresh, &QPushButton::clicked, this, &cObjectsTreeWidget::pressedRefreshButton);
	connect(
		ui->treeWidget_objects, &QTreeWidget::itemChanged, this, &cObjectsTreeWidget::onItemChanged);

	// In constructor, connect new signals:
	connect(
		ui->pushButton_add_object, &QPushButton::clicked, this, &cObjectsTreeWidget::slotAddObject);
	connect(ui->pushButton_delete_object, &QPushButton::clicked, this,
		&cObjectsTreeWidget::slotDeleteObject);
	connect(ui->treeWidget_objects, &QTreeWidget::itemSelectionChanged, this,
		&cObjectsTreeWidget::slotItemSelectionChanged);

	// Initialize editor area below tree
	editorLayout = new QVBoxLayout();
	ui->widget_editor_area->setLayout(editorLayout);
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

	// Get all primitives from params
	QList<sPrimitiveItem> primitiveList = cPrimitives::GetListOfPrimitives(params);

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

		// When building tree items for primitives:
		if (nodeData.type == enumNodeType::primitive)
		{
			// Find matching primitive by objectId
			for (const sPrimitiveItem &primitive : primitiveList)
			{
				if (primitive.objectID == nodeData.objectId)
				{
					item->setData(3, Qt::UserRole, primitive.typeName);
					break;
				}
			}
		}

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

void cObjectsTreeWidget::slotAddObject()
{
	// Show dialog to pick object type
	QStringList types = {
		"fractal", "primitive", "hybrid", "boolean Add", "boolean Mul", "boolean Sub"};
	bool ok = false;
	QString typeName =
		QInputDialog::getItem(this, tr("Add Object"), tr("Select object type:"), types, 0, false, &ok);
	if (!ok) return;

	QString name = QInputDialog::getText(
		this, tr("Add Object"), tr("Enter object name:"), QLineEdit::Normal, typeName, &ok);
	if (!ok || name.isEmpty()) return;

	// Find max existing node ID
	int newId = 1;
	const auto allItems = collectAllTreeItems();
	for (QTreeWidgetItem *item : allItems)
		newId = qMax(newId, item->data(0, Qt::UserRole).toInt() + 1);

	QTreeWidgetItem *newItem = new QTreeWidgetItem();
	newItem->setText(0, name);
	newItem->setText(2, QString::number(newId)); // objectId = nodeId for new objects

	enumNodeType type = enumNodeType::fractal;
	if (typeName == "primitive")
		type = enumNodeType::primitive;
	else if (typeName == "hybrid")
		type = enumNodeType::hybrid;
	else if (typeName == "boolean Add")
		type = enumNodeType::booleanAdd;
	else if (typeName == "boolean Mul")
		type = enumNodeType::booleanMul;
	else if (typeName == "boolean Sub")
		type = enumNodeType::booleanSub;

	newItem->setData(0, Qt::UserRole, newId);
	newItem->setData(1, Qt::UserRole, int(type));
	newItem->setData(2, Qt::UserRole, newId);
	newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);
	newItem->setText(3, formatPosition({0.0, 0.0, 0.0}));

	// Add as child of selected item or top-level
	QList<QTreeWidgetItem *> selected = ui->treeWidget_objects->selectedItems();
	if (!selected.isEmpty())
		selected.first()->addChild(newItem);
	else
		ui->treeWidget_objects->addTopLevelItem(newItem);

	ui->treeWidget_objects->setItemWidget(newItem, 1, buildTypeComboBox(int(type)));
	ui->treeWidget_objects->expandAll();
	ui->treeWidget_objects->setCurrentItem(newItem);
}

void cObjectsTreeWidget::slotDeleteObject()
{
	QList<QTreeWidgetItem *> selected = ui->treeWidget_objects->selectedItems();
	if (selected.isEmpty()) return;

	QTreeWidgetItem *item = selected.first();

	// Clear editor area before deleting
	if (currentEditorWidget)
	{
		editorLayout->removeWidget(currentEditorWidget);
		delete currentEditorWidget;
		currentEditorWidget = nullptr;
	}

	// Re-parent children to deleted item's parent
	QTreeWidgetItem *parent = item->parent();
	while (item->childCount() > 0)
	{
		QTreeWidgetItem *child = item->takeChild(0);
		if (parent)
			parent->addChild(child);
		else
			ui->treeWidget_objects->addTopLevelItem(child);
	}

	if (parent)
		parent->removeChild(item);
	else
		delete ui->treeWidget_objects->takeTopLevelItem(
			ui->treeWidget_objects->indexOfTopLevelItem(item));
}

void cObjectsTreeWidget::slotItemSelectionChanged()
{
	// Remove any previously loaded editor widget to avoid stacking multiple editors
	if (currentEditorWidget)
	{
		editorLayout->removeWidget(currentEditorWidget);
		delete currentEditorWidget;
		currentEditorWidget = nullptr;
	}

	QList<QTreeWidgetItem *> selected = ui->treeWidget_objects->selectedItems();
	if (selected.isEmpty()) return;

	QTreeWidgetItem *item = selected.first();
	// Cast stored integer back to enum for type-safe branching
	enumNodeType type = enumNodeType(item->data(1, Qt::UserRole).toInt());
	// objectId links this tree node to a fractal slot or primitive definition in gPar
	int objectId = item->data(2, Qt::UserRole).toInt();

	// Wrapper widget so the whole editor area can be removed/replaced as a single unit
	QWidget *editorContainer = new QWidget();
	QVBoxLayout *containerLayout = new QVBoxLayout(editorContainer);

	// Position group is irrelevant for fractals inside a hybrid node because
	// their position is controlled by the hybrid parent, not individually
	if (!isFractalInHybridGroup(item))
	{
		QGroupBox *posGroup = new QGroupBox(tr("Position"));
		QHBoxLayout *posLayout = new QHBoxLayout(posGroup);

		// Parse the position stored as "x y z" text in column 3 of the tree item
		CVector3 pos = parsePosition(item->text(3));

		QDoubleSpinBox *spinX = new QDoubleSpinBox();
		QDoubleSpinBox *spinY = new QDoubleSpinBox();
		QDoubleSpinBox *spinZ = new QDoubleSpinBox();

		for (QDoubleSpinBox *spin : {spinX, spinY, spinZ})
		{
			spin->setRange(-1e9, 1e9);
			spin->setDecimals(6);
			spin->setSingleStep(0.1);
		}
		spinX->setValue(pos.x);
		spinY->setValue(pos.y);
		spinZ->setValue(pos.z);

		posLayout->addWidget(new QLabel("X:"));
		posLayout->addWidget(spinX);
		posLayout->addWidget(new QLabel("Y:"));
		posLayout->addWidget(spinY);
		posLayout->addWidget(new QLabel("Z:"));
		posLayout->addWidget(spinZ);

		// Lambda captures spinboxes and item by value; any spin change
		// writes back the formatted position string into the tree item's column 3,
		// which is later read by StoreTreeToParams()
		auto updatePos = [=]()
		{ item->setText(3, formatPosition({spinX->value(), spinY->value(), spinZ->value()})); };

		// 5-argument connect: signal, context object, functor — keeps lambda lifetime-safe
		connect(spinX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), spinX, updatePos);
		connect(spinY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), spinY, updatePos);
		connect(spinZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), spinZ, updatePos);

		containerLayout->addWidget(posGroup);
	}

	if (type == enumNodeType::fractal)
	{
		// objectId is 1-based; convert to 0-based index for the fractal container array
		int fractalIndex = qBound(0, objectId - 1, NUMBER_OF_FRACTALS - 1);

		cTabFractal *fractalTab = new cTabFractal();

		// Must assign containers before Init() so the tab can read current parameter values
		fractalTab->AssignParameterContainers(gPar, gParFractal);

		// firstTab=true enables the iteration count / weight controls on the first fractal slot
		fractalTab->Init(true, fractalIndex);

		// No parent dock fractal in this context (embedded standalone, not inside cDockFractal)
		fractalTab->AssignParentDockFractal(nullptr);

		// Push current parameter values into the UI controls (write = params → UI)
		fractalTab->SynchronizeInterface(gPar, qInterface::write);
		fractalTab->SynchronizeFractal(gParFractal->at(fractalIndex), qInterface::write);

		containerLayout->addWidget(fractalTab);
	}
	else if (type == enumNodeType::primitive)
	{
		// typeName (e.g. "box", "sphere") was stored in UserRole on column 3 during UpdateTree()
		QString primTypeName = item->data(3, Qt::UserRole).toString();
		if (primTypeName.isEmpty()) primTypeName = item->text(0);

		// Each primitive type has its own .ui file under formula/ui/primitive_<type>.ui
		QString uiFileName = systemDirectories.sharedDir + "formula" + QDir::separator() + "ui"
												 + QDir::separator() + "primitive_" + primTypeName + ".ui";
		QFile uiFile(uiFileName);
		if (!uiFile.exists()) return;

		uiFile.open(QFile::ReadOnly);
		MyUiLoader loader;
		QWidget *primWidget = loader.load(&uiFile);
		uiFile.close();

		if (primWidget)
		{
			// Resolve the full primitive name (e.g. "primitiveBox_0001") from the live
			// parameter list so widget names can be prefixed for SynchronizeInterfaceWindow()
			QString primFullName;
			const QList<sPrimitiveItem> primitiveList = cPrimitives::GetListOfPrimitives(gPar);
			for (const sPrimitiveItem &prim : primitiveList)
			{
				if (prim.objectID == objectId)
				{
					primFullName = prim.fullName;
					break;
				}
			}

			if (gPar->Get<bool>("ui_colorize"))
				cInterface::ColorizeGroupBoxes(
					primWidget, gPar->Get<int>("ui_colorize_random_seed") + objectId);
			cInterface::AdjustLayoutSpacing(primWidget, gPar->Get<int>("ui_layout_spacing"));

			// Widget names in the .ui file are generic (e.g. "spinbox_size").
			// SynchronizeInterfaceWindow() matches widget names to parameter names, so each
			// widget must be renamed to include the primitive's full name as a prefix segment,
			// e.g. "spinbox_size" → "spinbox_primitiveBox_0001_size"
			if (!primFullName.isEmpty())
			{
				for (QWidget *widget : primWidget->findChildren<QWidget *>())
				{
					QString widgetName = widget->objectName();
					int firstUnderscore = widgetName.indexOf('_');
					if (firstUnderscore >= 0)
					{
						widgetName.insert(firstUnderscore + 1, primFullName + "_");
						widget->setObjectName(widgetName);
					}
				}
			}

			// Populate the renamed widgets with current parameter values from gPar
			SynchronizeInterfaceWindow(primWidget, gPar, qInterface::write);
			containerLayout->addWidget(primWidget);
		}
	}

	// Stretch pushes content to the top of the editor area
	containerLayout->addStretch();
	currentEditorWidget = editorContainer;
	editorLayout->addWidget(currentEditorWidget);
}
