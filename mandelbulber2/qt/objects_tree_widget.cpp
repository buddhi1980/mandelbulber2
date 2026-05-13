
/*
 * objects_tree_widget.cpp
 *
 *  Created on: 22 lis 2025
 *      Author: krzysztof
 */

#include "objects_tree_widget.h"

#include <memory>
#include <QSet>
#include <QStack>
#include <QComboBox>
#include <QInputDialog>
#include <QList>
#include <QDialog>
#include <QToolButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QFontMetrics>
#include <QVBoxLayout>

#include "src/interface.hpp"
#include "src/primitives.h"
#include "src/my_ui_loader.h"
#include "src/system_directories.hpp"
#include "ui_objects_tree_widget.h"
#include "src/initparameters.hpp"
#include "src/fractal_container.hpp"
#include "src/objects_tree.h"
#include "formula/definition/all_fractal_list.hpp"
#include "fractal_object.h"
#include "general_object_parameters.h"
#include "fractal_calculation_parameters.h"

cObjectsTreeWidget::cObjectsTreeWidget(QWidget *parent)
		: QWidget(parent), ui(new Ui::cObjectsTreeWidget)
{
	ui->setupUi(this);

	// Connect the refresh button so the user can force a reload of the tree from gPar
	connect(
		ui->pushButton_refresh, &QPushButton::clicked, this, &cObjectsTreeWidget::pressedRefreshButton);

	// Keep gPar in sync whenever the user edits a cell directly in the tree
	connect(
		ui->treeWidget_objects, &QTreeWidget::itemChanged, this, &cObjectsTreeWidget::onItemChanged);

	// Connect add/delete buttons for managing scene objects
	connect(
		ui->pushButton_add_group, &QPushButton::clicked, this, &cObjectsTreeWidget::slotAddGroup);
	connect(
		ui->pushButton_add_fractal, &QPushButton::clicked, this, &cObjectsTreeWidget::slotAddFractal);
	connect(ui->pushButton_add_primitive, &QPushButton::clicked, this,
		&cObjectsTreeWidget::slotAddPrimitive);
	connect(ui->pushButton_delete_object, &QPushButton::clicked, this,
		&cObjectsTreeWidget::slotDeleteObject);

	// When the user clicks a different tree item, rebuild the editor panel below the tree
	connect(ui->treeWidget_objects, &QTreeWidget::itemSelectionChanged, this,
		&cObjectsTreeWidget::slotItemSelectionChanged);

	// Initialize the editor area that appears below the tree widget
	editorLayout = new QVBoxLayout();
	ui->widget_editor_area->setLayout(editorLayout);
}

cObjectsTreeWidget::~cObjectsTreeWidget()
{
	delete ui;
}

// Widget names from cGeneralObjectParameters that represent the common object parameters
// (position, rotation, scale, repeat, material).  These are the canonical sources of truth
// when cGeneralObjectParameters is embedded in an editor; the same-named widgets found in
// primitive .ui files are neutralised so they do not conflict with the sync engine.
static const QSet<QString> s_commonGeneralObjectWidgetNames = {"vect3_position_x",
	"vect3_position_y", "vect3_position_z", "spinboxd3_rotation_x", "spinboxd3_rotation_y",
	"spinboxd3_rotation_z", "vect3_repeat_x", "vect3_repeat_y", "vect3_repeat_z",
	"materialselector_material_id", "logedit_scale"};

struct sPrimitiveSelectorItem
{
	QString typeName;
	QString iconPath;
};

static const QList<sPrimitiveSelectorItem> s_primitiveSelectorItems = {
	{"rectangle", ":/primitives/icons/rectangle.png"}, {"circle", ":/primitives/icons/circle.png"},
	{"box", ":/primitives/icons/box.png"},						 {"cylinder", ":/primitives/icons/cylinder.png"},
	{"cone", ":/primitives/icons/cone.png"},					 {"sphere", ":/primitives/icons/sphere.png"},
	{"torus", ":/primitives/icons/torus.png"},					 {"plane", ":/primitives/icons/plane.png"},
	{"water", ":/primitives/icons/water.png"},					 {"prism", ":/primitives/icons/prism.png"},
	{"ellipsoid", ":/primitives/icons/ellipsoid.png"}};

// --- Private helpers ---

// Returns a flat list of every item in the tree, regardless of nesting depth.
// Uses an explicit stack to avoid recursion on deep trees.
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

// Returns the node type for an item, preferring the live combo-box value if one
// has been attached to the tree widget, otherwise falling back to the stored UserRole data.
int cObjectsTreeWidget::getNodeType(QTreeWidgetItem *item) const
{
	QComboBox *combo =
		qobject_cast<QComboBox *>(ui->treeWidget_objects->itemWidget(item, treeCol::type));
	return combo ? combo->currentData().toInt()
							 : item->data(treeData::nodeType, Qt::UserRole).toInt();
}

// Returns true when 'item' is a fractal node that sits directly inside a hybrid group.
// These nodes intentionally have no position of their own – the hybrid parent owns it.
bool cObjectsTreeWidget::isFractalInHybridGroup(QTreeWidgetItem *item) const
{
	if (enumNodeType(getNodeType(item)) != enumNodeType::fractal) return false;
	if (!item->parent()) return false;
	return enumNodeType(getNodeType(item->parent())) == enumNodeType::hybrid;
}

// Parses a space-separated "x y z" string into a CVector3.
// Returns the zero vector when the string does not contain exactly three numeric tokens.
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

// Formats a CVector3 as a space-separated string with six decimal places,
// matching the format expected by parsePosition().
QString cObjectsTreeWidget::formatPosition(const CVector3 &pos)
{
	return QString("%1 %2 %3").arg(pos.x, 0, 'f', 15).arg(pos.y, 0, 'f', 15).arg(pos.z, 0, 'f', 15);
}

// Creates a combo box pre-populated with all supported node types and
// with the entry matching 'currentType' already selected.
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

int cObjectsTreeWidget::findNextAvailableNodeId() const
{
	QSet<int> usedIds;
	for (QTreeWidgetItem *item : collectAllTreeItems())
	{
		int nodeId = item->data(treeData::nodeId, Qt::UserRole).toInt();
		if (nodeId > 0) usedIds.insert(nodeId);
	}

	int newId = 1;
	while (usedIds.contains(newId))
		++newId;
	return newId;
}

int cObjectsTreeWidget::findNextAvailableFractalObjectId() const
{
	QSet<int> usedFractalIds;
	for (QTreeWidgetItem *item : collectAllTreeItems())
	{
		if (enumNodeType(getNodeType(item)) != enumNodeType::fractal) continue;
		int objectId = item->data(treeData::objectId, Qt::UserRole).toInt();
		if (objectId > 0) usedFractalIds.insert(objectId);
	}

	for (int objectId = 1; objectId <= NUMBER_OF_FRACTALS; ++objectId)
	{
		if (!usedFractalIds.contains(objectId)) return objectId;
	}

	return -1;
}

int cObjectsTreeWidget::findNextAvailablePrimitiveObjectId() const
{
	QSet<int> usedObjectIds;
	for (QTreeWidgetItem *item : collectAllTreeItems())
	{
		int objectId = item->data(treeData::objectId, Qt::UserRole).toInt();
		if (objectId > 0) usedObjectIds.insert(objectId);
	}

	int newObjectId = 1000;
	while (usedObjectIds.contains(newObjectId))
		++newObjectId;
	return newObjectId;
}

bool cObjectsTreeWidget::isGroupType(enumNodeType type)
{
	return type == enumNodeType::hybrid || type == enumNodeType::booleanAdd
				 || type == enumNodeType::booleanMul || type == enumNodeType::booleanSub;
}

QTreeWidgetItem *cObjectsTreeWidget::selectedGroupTarget() const
{
	const QList<QTreeWidgetItem *> selected = ui->treeWidget_objects->selectedItems();
	if (selected.isEmpty()) return nullptr;

	QTreeWidgetItem *selectedItem = selected.first();
	enumNodeType selectedType = enumNodeType(getNodeType(selectedItem));
	if (isGroupType(selectedType)) return selectedItem;

	QTreeWidgetItem *parent = selectedItem->parent();
	if (!parent) return nullptr;

	enumNodeType parentType = enumNodeType(getNodeType(parent));
	return isGroupType(parentType) ? parent : nullptr;
}

enumNodeType cObjectsTreeWidget::showGroupSelectionDialog(bool *ok) const
{
	if (ok) *ok = false;

	QDialog dialog(const_cast<cObjectsTreeWidget *>(this));
	dialog.setWindowTitle(tr("Select group type"));

	QGridLayout *layout = new QGridLayout(&dialog);

	enumNodeType selectedType = enumNodeType::hybrid;
	const QList<QPair<QString, enumNodeType>> options = {
		{tr("Hybrid fractal"), enumNodeType::hybrid},
		{tr("Boolean ADD"), enumNodeType::booleanAdd},
		{tr("Boolean MUL"), enumNodeType::booleanMul},
		{tr("Boolean SUB"), enumNodeType::booleanSub}};

	for (int i = 0; i < options.size(); ++i)
	{
		QToolButton *button = new QToolButton(&dialog);
		button->setText(options[i].first);
		button->setToolButtonStyle(Qt::ToolButtonTextOnly);
		button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		layout->addWidget(button, i / 2, i % 2);
		connect(button, &QToolButton::clicked, &dialog, [&dialog, &selectedType, options, i]() {
			selectedType = options[i].second;
			dialog.accept();
		});
	}

	if (dialog.exec() == QDialog::Accepted)
	{
		if (ok) *ok = true;
	}

	return selectedType;
}

QString cObjectsTreeWidget::showPrimitiveSelectionDialog(bool *ok) const
{
	if (ok) *ok = false;

	QDialog dialog(const_cast<cObjectsTreeWidget *>(this));
	dialog.setWindowTitle(tr("Select primitive type"));
	QGridLayout *layout = new QGridLayout(&dialog);

	QString selectedPrimitiveType;
	const int columns = 3;
	QFontMetrics fm(font());
	QSize iconSize(int(fm.height() * 2.5), int(fm.height() * 2.5));

	for (int i = 0; i < s_primitiveSelectorItems.size(); ++i)
	{
		const sPrimitiveSelectorItem &item = s_primitiveSelectorItems[i];
		QToolButton *button = new QToolButton(&dialog);
		button->setText(item.typeName);
		button->setIcon(QIcon(item.iconPath));
		button->setIconSize(iconSize);
		button->setToolButtonStyle(Qt::ToolButtonTextUnderIcon);
		button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		layout->addWidget(button, i / columns, i % columns);
		connect(button, &QToolButton::clicked, &dialog, [&dialog, &selectedPrimitiveType, item]() {
			selectedPrimitiveType = item.typeName;
			dialog.accept();
		});
	}

	if (dialog.exec() == QDialog::Accepted)
	{
		if (ok) *ok = !selectedPrimitiveType.isEmpty();
	}

	return selectedPrimitiveType;
}

QTreeWidgetItem *cObjectsTreeWidget::createNodeItem(
	int nodeId, enumNodeType nodeType, int objectId, const QString &name, const QString &primTypeName)
{
	QTreeWidgetItem *newItem = new QTreeWidgetItem();
	newItem->setText(treeCol::name, name);
	newItem->setText(treeCol::type, nodeTypeToString(nodeType));
	newItem->setText(treeCol::objectId, QString::number(objectId));
	newItem->setText(treeCol::position, formatPosition({0.0, 0.0, 0.0}));
	newItem->setText(treeCol::rotation, formatPosition({0.0, 0.0, 0.0}));
	newItem->setText(treeCol::repeat, formatPosition({0.0, 0.0, 0.0}));
	newItem->setText(treeCol::scale, QString::number(1.0, 'f', 15));

	newItem->setData(treeData::nodeId, Qt::UserRole, nodeId);
	newItem->setData(treeData::nodeType, Qt::UserRole, int(nodeType));
	newItem->setData(treeData::objectId, Qt::UserRole, objectId);
	if (!primTypeName.isEmpty()) newItem->setData(treeData::primTypeName, Qt::UserRole, primTypeName);
	newItem->setFlags(newItem->flags() | Qt::ItemIsEditable);

	return newItem;
}

void cObjectsTreeWidget::addNodeToSelectedGroup(QTreeWidgetItem *newItem)
{
	QTreeWidgetItem *targetGroup = selectedGroupTarget();
	if (targetGroup)
		targetGroup->addChild(newItem);
	else
		ui->treeWidget_objects->addTopLevelItem(newItem);
}

void cObjectsTreeWidget::ensureNodeParamsExist(int nodeId)
{
	QString prefix = QString("node_%1_").arg(nodeId, 4, 10, QChar('0'));
	if (!gPar->IfExists(prefix + "definition")) InitNodeParams(nodeId, gPar);
}

// Adds or updates a string parameter in 'params'.
// Using addParam when the key is missing avoids an assertion in cParameterContainer::Set().
void cObjectsTreeWidget::addOrSetParam(
	std::shared_ptr<cParameterContainer> params, const QString &name, const QString &value)
{
	if (params->IfExists(name))
		params->Set(name, value);
	else
		params->addParam(name, value, morphNone, paramStandard);
}

// Overload for CVector3 values – same add-or-update semantics as the string variant.
void cObjectsTreeWidget::addOrSetParam(
	std::shared_ptr<cParameterContainer> params, const QString &name, const CVector3 &value)
{
	if (params->IfExists(name))
		params->Set(name, value);
	else
		params->addParam(name, value, morphNone, paramStandard);
}

// Overload for double values – same add-or-update semantics as the string variant.
void cObjectsTreeWidget::addOrSetParam(
	std::shared_ptr<cParameterContainer> params, const QString &name, double value)
{
	if (params->IfExists(name))
		params->Set(name, value);
	else
		params->addParam(name, value, morphNone, paramStandard);
}

// Inserts 'prefix' into every child widget name of 'parent' immediately after the first '_',
// so SynchronizeInterfaceWindow can map each widget to its namespaced parameter.
// Example: prefix "formula_" transforms "vect3_position_x" → "vect3_formula_position_x".
void cObjectsTreeWidget::renameWidgetsWithPrefix(QWidget *parent, const QString &prefix)
{
	for (QWidget *widget : parent->findChildren<QWidget *>())
	{
		QString widgetName = widget->objectName();
		int firstUnderscore = widgetName.indexOf('_');
		if (firstUnderscore >= 0)
		{
			widgetName.insert(firstUnderscore + 1, prefix);
			widget->setObjectName(widgetName);
		}
	}
}

// --- Public methods ---

// Converts an enumNodeType value to its human-readable display string.
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

// Rebuilds the tree widget from scratch using the current contents of 'params'.
// Called on startup, after a file load, or when the user presses the Refresh button.
void cObjectsTreeWidget::UpdateTree(
	std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractalParams)
{
	QList<QTreeWidgetItem *> selectedBeforeRefresh = ui->treeWidget_objects->selectedItems();
	if (!selectedBeforeRefresh.isEmpty())
		lastSelectedNodeId = selectedBeforeRefresh.first()->data(treeData::nodeId, Qt::UserRole).toInt();

	ui->treeWidget_objects->clear();
	ui->treeWidget_objects->setColumnCount(4);
	ui->treeWidget_objects->setHeaderLabels(
		{"Name", "Type", "Object ID", "Position", "Rotation", "Repeat", "Scale"});

	cObjectsTree objectsTree;
	objectsTree.CreateNodeDataFromParameters(params);
	const auto sortedList = objectsTree.GetSortedNodeDataList();

	// Build a quick lookup so we can detect hybrid-child fractals without walking the tree
	QMap<int, enumNodeType> nodeTypeMap;
	for (const auto &node : sortedList)
		nodeTypeMap[node.id] = node.type;

	QMap<int, QTreeWidgetItem *> nodeItems;

	// Retrieve the full list of primitives so we can attach the primitive type name
	// to each primitive tree item via Qt::UserRole on column 3
	QList<sPrimitiveItem> primitiveList = cPrimitives::GetListOfPrimitives(params);

	// First pass: create all items without yet wiring up the parent-child relationships
	for (const auto &nodeData : sortedList)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(treeCol::name, nodeData.name);
		item->setText(treeCol::type, nodeTypeToString(nodeData.type));
		item->setText(treeCol::objectId, QString::number(nodeData.objectId));

		// Fractal nodes inside a hybrid group inherit the group's position; skip the column
		bool inHybrid = nodeData.type == enumNodeType::fractal
										&& nodeTypeMap.value(nodeData.parentId) == enumNodeType::hybrid;
		if (!inHybrid)
		{
			item->setText(treeCol::position, formatPosition(nodeData.position));
			item->setText(treeCol::rotation, formatPosition(nodeData.rotation));
			item->setText(treeCol::repeat, formatPosition(nodeData.repeat));
			item->setText(treeCol::scale, QString::number(nodeData.scale, 'f', 15));
		}

		item->setData(treeData::nodeId, Qt::UserRole, nodeData.id);
		item->setData(treeData::nodeType, Qt::UserRole, int(nodeData.type));
		item->setData(treeData::objectId, Qt::UserRole, nodeData.objectId);
		// item->setFlags(item->flags() | Qt::ItemIsEditable);

		// Store the primitive type name (e.g. "box", "sphere") so the editor can later
		// load the matching .ui file without having to search through gPar again
		if (nodeData.type == enumNodeType::primitive)
		{
			for (const sPrimitiveItem &primitive : primitiveList)
			{
				if (primitive.objectID == nodeData.objectId)
				{
					item->setData(treeData::primTypeName, Qt::UserRole, primitive.typeName);
					break;
				}
			}
		}

		nodeItems[nodeData.id] = item;
	}

	// Second pass: wire up parent-child relationships in the tree widget
	for (const auto &node : sortedList)
	{
		QTreeWidgetItem *item = nodeItems[node.id];
		if (node.parentId == 0 || !nodeItems.contains(node.parentId))
			ui->treeWidget_objects->addTopLevelItem(item);
		else
			nodeItems[node.parentId]->addChild(item);
	}

	ui->treeWidget_objects->expandAll();

	// Attach the type combo boxes after the tree is fully built so that
	// setItemWidget() can find the correct persistent index for each item
	for (QTreeWidgetItem *item : nodeItems)
	{
		int currentType = item->data(treeData::nodeType, Qt::UserRole).toInt();
		ui->treeWidget_objects->setItemWidget(item, 1, buildTypeComboBox(currentType));
	}

	if (lastSelectedNodeId > 0 && nodeItems.contains(lastSelectedNodeId))
		ui->treeWidget_objects->setCurrentItem(nodeItems.value(lastSelectedNodeId));
}

// Validates and reformats the position cell whenever the user edits it directly.
// Any text that does not parse to exactly three numbers is reset to the origin.
void cObjectsTreeWidget::onItemChanged(QTreeWidgetItem *item, int column)
{
	QSignalBlocker blocker(ui->treeWidget_objects);
	if (column != treeCol::position) return;

	QString text = item->text(treeCol::position);
	QStringList parts = text.split(' ', Qt::SkipEmptyParts);
	if (parts.size() == 3)
	{
		bool okX, okY, okZ;
		double x = parts[0].toDouble(&okX);
		double y = parts[1].toDouble(&okY);
		double z = parts[2].toDouble(&okZ);
		item->setText(treeCol::position,
			(okX && okY && okZ) ? formatPosition({x, y, z}) : "0.000000 0.000000 0.000000");
	}
	else
	{
		item->setText(treeCol::position, "0.000000 0.000000 0.000000");
	}
}

// Writes the current tree structure back into 'params', removing any node parameters
// that no longer have a corresponding tree item and storing updated definitions for
// all items that do exist.

void cObjectsTreeWidget::StoreTreeToParams(
	std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractalParams)
{
	const auto allItems = collectAllTreeItems();

	// Build the set of node IDs currently visible in the tree
	QSet<int> treeNodeIds;
	for (QTreeWidgetItem *item : allItems)
		treeNodeIds.insert(item->data(treeData::nodeId, Qt::UserRole).toInt());

	// Remove stale node_ parameters
	QList<QString> list = params->GetListOfParameters();
	for (auto &parameterName : list)
	{
		if (parameterName.left(5) == "node_")
		{
			bool ok = false;
			int nodeId = parameterName.section('_', 1, 1).toInt(&ok);
			if (ok && !treeNodeIds.contains(nodeId))
			{
				params->DeleteParameter(parameterName);
			}
		}
	}

	// Write the current state of each tree item back into params
	for (QTreeWidgetItem *item : allItems)
	{
		int nodeId = item->data(treeData::nodeId, Qt::UserRole).toInt();
		int nodeType = getNodeType(item);
		int objectId = item->data(treeData::objectId, Qt::UserRole).toInt();
		QString name = item->text(treeCol::name);
		int parentId =
			item->parent() ? item->parent()->data(treeData::nodeId, Qt::UserRole).toInt() : 0;

		QString prefix = QString("node_%1").arg(nodeId, 4, 10, QChar('0'));
		QString defParam = prefix + "_definition";

		addOrSetParam(params, defParam,
			QString("%1,%2,%3,%4,%5").arg(name).arg(nodeId).arg(nodeType).arg(parentId).arg(objectId));
	}
}

void cObjectsTreeWidget::pressedRefreshButton()
{
	UpdateTree(gPar, gParFractal);
}

void cObjectsTreeWidget::slotAddGroup()
{
	bool ok = false;
	enumNodeType groupType = showGroupSelectionDialog(&ok);
	if (!ok) return;

	int newNodeId = findNextAvailableNodeId();
	ensureNodeParamsExist(newNodeId);

	QString groupName = nodeTypeToString(groupType);
	QTreeWidgetItem *newItem = createNodeItem(newNodeId, groupType, -1, groupName, QString());
	addNodeToSelectedGroup(newItem);

	ui->treeWidget_objects->setItemWidget(newItem, treeCol::type, buildTypeComboBox(int(groupType)));
	ui->treeWidget_objects->expandAll();
	ui->treeWidget_objects->setCurrentItem(newItem);
	lastSelectedNodeId = newNodeId;
}

void cObjectsTreeWidget::slotAddFractal()
{
	int fractalObjectId = findNextAvailableFractalObjectId();
	if (fractalObjectId < 0)
	{
		QMessageBox::warning(this, tr("Add Fractal"),
			tr("No free fractal slot is available. Delete an existing fractal first."));
		return;
	}

	int newNodeId = findNextAvailableNodeId();
	ensureNodeParamsExist(newNodeId);

	QString fractalName = tr("fractal %1").arg(fractalObjectId);
	QTreeWidgetItem *newItem =
		createNodeItem(newNodeId, enumNodeType::fractal, fractalObjectId, fractalName, QString());
	addNodeToSelectedGroup(newItem);

	ui->treeWidget_objects->setItemWidget(
		newItem, treeCol::type, buildTypeComboBox(int(enumNodeType::fractal)));
	ui->treeWidget_objects->expandAll();
	ui->treeWidget_objects->setCurrentItem(newItem);
	lastSelectedNodeId = newNodeId;
}

void cObjectsTreeWidget::slotAddPrimitive()
{
	bool ok = false;
	QString primitiveType = showPrimitiveSelectionDialog(&ok);
	if (!ok || primitiveType.isEmpty()) return;

	int newNodeId = findNextAvailableNodeId();
	ensureNodeParamsExist(newNodeId);

	QList<sPrimitiveItem> primitiveList = cPrimitives::GetListOfPrimitives(gPar);
	int newPrimitiveIndex = cPrimitives::NewPrimitiveIndex(primitiveType, primitiveList);
	QString primitiveFullName = QString("primitive_%1_%2").arg(primitiveType).arg(newPrimitiveIndex);
	sPrimitiveItem newPrimitive(cPrimitives::PrimitiveNameToEnum(primitiveType), newPrimitiveIndex,
		primitiveFullName, primitiveType);

	InitPrimitiveParams(newPrimitive, gPar);
	gPar->Set(newPrimitive.Name("enabled"), true);

	int primitiveObjectId = findNextAvailablePrimitiveObjectId();
	gPar->Set(newPrimitive.Name("object_id"), primitiveObjectId);

	QString primitiveName = gPar->Get<QString>(newPrimitive.Name("name"));
	QTreeWidgetItem *newItem = createNodeItem(
		newNodeId, enumNodeType::primitive, primitiveObjectId, primitiveName, primitiveType);
	addNodeToSelectedGroup(newItem);

	ui->treeWidget_objects->setItemWidget(
		newItem, treeCol::type, buildTypeComboBox(int(enumNodeType::primitive)));
	ui->treeWidget_objects->expandAll();
	ui->treeWidget_objects->setCurrentItem(newItem);
	lastSelectedNodeId = newNodeId;
}

// Removes the currently selected node from the tree.
// Children of the deleted node are re-parented to the deleted node's parent
// so that grandchildren are not lost along with their parent.
void cObjectsTreeWidget::slotDeleteObject()
{
	QList<QTreeWidgetItem *> selected = ui->treeWidget_objects->selectedItems();
	if (selected.isEmpty()) return;

	QTreeWidgetItem *item = selected.first();
	enumNodeType deletedType = enumNodeType(getNodeType(item));
	int deletedObjectId = item->data(treeData::objectId, Qt::UserRole).toInt();
	lastSelectedNodeId = -1;

	// Clear the editor panel before removing the item to avoid a dangling reference
	if (currentEditorWidget)
	{
		editorLayout->removeWidget(currentEditorWidget);
		delete currentEditorWidget;
		currentEditorWidget = nullptr;
	}

	// Re-parent children to the deleted item's parent so they are not orphaned
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
		delete parent->takeChild(parent->indexOfChild(item));
	else
		delete ui->treeWidget_objects->takeTopLevelItem(
			ui->treeWidget_objects->indexOfTopLevelItem(item));

	if (deletedType == enumNodeType::primitive)
	{
		const QList<sPrimitiveItem> primitiveList = cPrimitives::GetListOfPrimitives(gPar);
		for (const sPrimitiveItem &primitive : primitiveList)
		{
			if (gPar->Get<int>(primitive.Name("object_id")) != deletedObjectId) continue;
			const QList<QString> paramsToDelete = cPrimitives::GetListOfPrimitiveParams(primitive, gPar);
			for (const QString &parameterName : paramsToDelete)
			{
				gPar->DeleteParameter(parameterName);
			}
			break;
		}
	}
}

// Builds the info label that is shown at the top of the editor panel.
// It tells the user which object they are currently editing and what its type is.
// For primitive nodes the specific primitive subtype (e.g. "box", "sphere") is
// appended in parentheses so the user has full context without looking at the tree.
QLabel *cObjectsTreeWidget::buildInfoLabel(QTreeWidgetItem *item, enumNodeType type)
{
	QString objectName = item->text(treeCol::name);
	QString objectTypeName = nodeTypeToString(type);
	QString objectTypeDisplay = objectTypeName;

	if (type == enumNodeType::primitive)
	{
		// Append the primitive subtype stored by UpdateTree() so the label reads e.g.
		// "Type: primitive (box)" instead of just "Type: primitive"
		QString primTypeName = item->data(treeData::primTypeName, Qt::UserRole).toString();
		if (primTypeName.isEmpty()) primTypeName = item->text(treeCol::name);
		if (!primTypeName.isEmpty())
			objectTypeDisplay = tr("%1 (%2)").arg(objectTypeName).arg(primTypeName);
	}

	QLabel *infoLabel = new QLabel(tr("<b>Editing:</b> %1 &nbsp;&nbsp; <b>Type:</b> %2")
			.arg(objectName.toHtmlEscaped())
			.arg(objectTypeDisplay.toHtmlEscaped()));
	infoLabel->setFrameShape(QFrame::StyledPanel);
	infoLabel->setContentsMargins(6, 4, 6, 4);
	infoLabel->setWordWrap(true);
	return infoLabel;
}

// Creates and initialises a cFractalObject editor for the fractal identified by 'objectId',
// and adds cGeneralObjectParameters and cFractalCalculationParameters widgets to expose all
// common fractal parameters defined in InitFractalParams.
// The fractal index is clamped to the valid range so an out-of-range objectId cannot crash.
QWidget *cObjectsTreeWidget::buildFractalEditor(int objectId, bool isHybrid, QTreeWidgetItem *item)
{
	// objectId is 1-based; fractal indices stored in gParFractal are 0-based
	int fractalIndex = qBound(0, objectId - 1, NUMBER_OF_FRACTALS - 1);

	QWidget *container = new QWidget();
	QVBoxLayout *layout = new QVBoxLayout(container);
	layout->setContentsMargins(0, 0, 0, 0);

	// Formula-specific parameter editor (formula selector + formula tab)
	cFractalObject *fractalTab = new cFractalObject();
	fractalTab->AssignParameterContainers(gPar, gParFractal);
	fractalTab->Init(true, fractalIndex);
	fractalTab->AssignParentDockFractal(nullptr);

	// Populate the tab with the current parameter values from gPar and gParFractal
	layout->addWidget(fractalTab);

	editorSyncTargets.clear();
	editorSyncTargets.append({fractalTab, gParFractal->at(fractalIndex)});

	if (!isHybrid)
	{
		QWidget *generalParams = buildGeneralObjectParametersEditor(item);
		layout->addWidget(generalParams);

		// Fractal calculation parameters (maxiter, julia mode, constant factor, initial w-axis).
		// Widget names already include the parameter name prefix so no renaming is needed.
		cFractalCalculationParameters *calcParams = new cFractalCalculationParameters();

		editorSyncTargets.append({generalParams, gPar});
		editorSyncTargets.append({calcParams, gParFractal->at(fractalIndex)});

		layout->addWidget(calcParams);
	}

	return container;
}

// Loads the .ui file for the given primitive type, renames all child widgets so that
// their object names include the primitive's full parameter name prefix, and then
// populates the widgets from gPar via SynchronizeInterfaceWindow.
// Returns nullptr when no matching .ui file exists or the file fails to load.
QWidget *cObjectsTreeWidget::buildPrimitiveEditor(QTreeWidgetItem *item, int objectId)
{
	// Retrieve the primitive type name stored by UpdateTree() (e.g. "box", "sphere")
	QString primTypeName = item->data(treeData::primTypeName, Qt::UserRole).toString();

	QString uiFileName = systemDirectories.sharedDir + "formula" + QDir::separator() + "ui"
											 + QDir::separator() + "primitive_" + primTypeName + ".ui";
	QFile uiFile(uiFileName);
	if (!uiFile.exists()) return nullptr;

	uiFile.open(QFile::ReadOnly);
	MyUiLoader loader;
	QWidget *primWidget = loader.load(&uiFile);
	uiFile.close();

	if (!primWidget) return nullptr;

	// Find the full parameter name prefix for this primitive (e.g. "primitive_box_001")
	// so we can rewrite the widget object names to match the parameter keys in gPar
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

	if (!primFullName.isEmpty())
	{
		// Rename child widgets: common ones are prefixed with "_" so the sync engine skips them;
		// primitive-specific ones get the full primitive name inserted after the first underscore
		// (e.g. "spinbox_x" → "spinbox_primitive_box_001_x") so they bind to the correct gPar key.
		for (QWidget *widget : primWidget->findChildren<QWidget *>())
		{
			QString widgetName = widget->objectName();
			if (s_commonGeneralObjectWidgetNames.contains(widgetName))
			{
				// Neutralise: typeName becomes "" so no sync branch matches this widget
				widget->setObjectName("_" + widgetName);
				continue;
			}
			int firstUnderscore = widgetName.indexOf('_');
			if (firstUnderscore >= 0)
			{
				widgetName.insert(firstUnderscore + 1, primFullName + "_");
				widget->setObjectName(widgetName);
			}
		}
	}

	// General object parameters (position, rotation, repeat, material) for this primitive.
	// Widget names are prefixed with the full primitive name so they bind to the correct gPar keys
	// (e.g. vect3_position_x → vect3_primitive_box_001_position_x → param
	// primitive_box_001_position.x).
	QWidget *generalParams = buildGeneralObjectParametersEditor(item);

	editorSyncTargets.clear();
	editorSyncTargets.append({primWidget, gPar});
	editorSyncTargets.append({generalParams, gPar});

	QWidget *container = new QWidget();
	QVBoxLayout *layout = new QVBoxLayout(container);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->addWidget(primWidget);
	layout->addWidget(generalParams);

	return container;
}

QWidget *cObjectsTreeWidget::buildGeneralObjectParametersEditor(QTreeWidgetItem *item)
{
	cGeneralObjectParameters *generalParams = new cGeneralObjectParameters();
	// renaming edit fields to have node_ prefix so they bind to the correct gPar keys (e.g.
	// vect3_position_x → vect3_node_001_position_x)

	QString nodePrefix =
		QString("node_%1").arg(item->data(treeData::nodeId, Qt::UserRole).toInt(), 4, 10, QChar('0'));
	for (QWidget *widget : generalParams->findChildren<QWidget *>())
	{
		QString widgetName = widget->objectName();
		int firstUnderscore = widgetName.indexOf('_');
		if (firstUnderscore >= 0)
		{
			widgetName.insert(firstUnderscore + 1, nodePrefix + "_");
			widget->setObjectName(widgetName);
		}
	}

	return generalParams;
}

// Responds to the user selecting a different item in the object tree.
// Before tearing down the old editor, the current widget values are read back
// into gPar (and into gParFractal for fractal nodes) so that any changes the
// user made are not silently discarded.
// A fresh editor container is then built for the newly selected item.
void cObjectsTreeWidget::slotItemSelectionChanged()
{
	// Save any user-modified parameter values from the current editor back to gPar
	// (and to gParFractal if the previous selection was a fractal node) before tearing
	// it down, so changes are not lost when switching selection
	// Save current editor state before switching selection
	if (currentEditorWidget)
	{
		SynchronizeEditorWidget(currentEditorWidget, qInterface::read);
		editorLayout->removeWidget(currentEditorWidget);
		delete currentEditorWidget;
		currentEditorWidget = nullptr;
	}

	QList<QTreeWidgetItem *> selected = ui->treeWidget_objects->selectedItems();
	if (selected.isEmpty()) return;

	QTreeWidgetItem *item = selected.first();
	lastSelectedNodeId = item->data(treeData::nodeId, Qt::UserRole).toInt();
	enumNodeType type = enumNodeType(item->data(treeData::nodeType, Qt::UserRole).toInt());
	int objectId = item->data(treeData::objectId, Qt::UserRole).toInt();
	int nodeId = item->data(treeData::nodeId, Qt::UserRole).toInt();

	// Wrapper widget so the whole editor area can be removed/replaced as a single unit
	QWidget *editorContainer = new QWidget();
	QVBoxLayout *containerLayout = new QVBoxLayout(editorContainer);

	// Header label: tells the user which object and type they are currently editing
	containerLayout->addWidget(buildInfoLabel(item, type));

	// Build the type-specific parameter editor
	if (type == enumNodeType::fractal)
	{
		// Track the fractal index so we can save to gParFractal on the next selection change
		currentFractalIndex = qBound(0, objectId - 1, NUMBER_OF_FRACTALS - 1);

		bool isHybrid = isFractalInHybridGroup(item);
		QWidget *fractalEditor = buildFractalEditor(objectId, isHybrid, item);
		if (fractalEditor) containerLayout->addWidget(fractalEditor);
	}
	else if (type == enumNodeType::primitive)
	{
		currentFractalIndex = -1;
		QWidget *primEditor = buildPrimitiveEditor(item, objectId);
		if (primEditor)
			containerLayout->addWidget(primEditor);
		else
			return; // No matching .ui file – leave the editor area empty
	}
	else if (type == enumNodeType::hybrid || type == enumNodeType::booleanAdd
					 || type == enumNodeType::booleanMul || type == enumNodeType::booleanSub)
	{
		QWidget *generalParams = buildGeneralObjectParametersEditor(item);
		editorSyncTargets.clear();
		editorSyncTargets.append({generalParams, gPar});

		containerLayout->addWidget(generalParams);
	}
	else
	{
		currentFractalIndex = -1;
	}

	containerLayout->addStretch();
	currentEditorWidget = editorContainer;

	// Populate the new editor — dispatches to gPar and/or gParFractal as needed
	SynchronizeEditorWidget(currentEditorWidget, qInterface::write);

	editorLayout->addWidget(currentEditorWidget);

	// Apply the global UI style settings (colour coding, layout spacing) to the new editor
	if (gPar->Get<bool>("ui_colorize"))
		cInterface::ColorizeGroupBoxes(this, gPar->Get<int>("ui_colorize_random_seed") + nodeId);
	cInterface::AdjustLayoutSpacing(this, gPar->Get<int>("ui_layout_spacing"));
}

void cObjectsTreeWidget::SynchronizeEditorWidget(QWidget *widget, qInterface::enumReadWrite mode)
{
	if (!widget) return;
	for (auto &[subWidget, params] : editorSyncTargets)
	{
		if (subWidget) SynchronizeInterfaceWindow(subWidget, params, mode);
	}
}

void cObjectsTreeWidget::SynchronizeInterface(std::shared_ptr<cParameterContainer> params,
	std::shared_ptr<cFractalContainer> fractalParams, qInterface::enumReadWrite mode)
{
	// FIXME to use local copy of parameters instead of global gPar and gParFractal
	// to be able to use this wighet in the NAvogator window with different parameter containers

	SynchronizeEditorWidget(currentEditorWidget, mode);

	if (mode == qInterface::write)
	{
		UpdateTree(params, fractalParams);
	}
	else
	{
		StoreTreeToParams(params, fractalParams);
	}
}
