
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
		ui->pushButton_add_object, &QPushButton::clicked, this, &cObjectsTreeWidget::slotAddObject);
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
	QComboBox *combo = qobject_cast<QComboBox *>(ui->treeWidget_objects->itemWidget(item, 1));
	return combo ? combo->currentData().toInt() : item->data(1, Qt::UserRole).toInt();
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
	return QString("%1 %2 %3").arg(pos.x, 0, 'f', 6).arg(pos.y, 0, 'f', 6).arg(pos.z, 0, 'f', 6);
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
	ui->treeWidget_objects->clear();
	ui->treeWidget_objects->setColumnCount(4);
	ui->treeWidget_objects->setHeaderLabels({"Name", "Type", "Object ID", "Position"});

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
		item->setText(0, nodeData.name);
		item->setText(1, nodeTypeToString(nodeData.type));
		item->setText(2, QString::number(nodeData.objectId));

		// Fractal nodes inside a hybrid group inherit the group's position; skip the column
		bool inHybrid = nodeData.type == enumNodeType::fractal
										&& nodeTypeMap.value(nodeData.parentId) == enumNodeType::hybrid;
		if (!inHybrid) item->setText(3, formatPosition(nodeData.position));

		item->setData(0, Qt::UserRole, nodeData.id);
		item->setData(1, Qt::UserRole, int(nodeData.type));
		item->setData(2, Qt::UserRole, nodeData.objectId);
		item->setFlags(item->flags() | Qt::ItemIsEditable);

		// Store the primitive type name (e.g. "box", "sphere") so the editor can later
		// load the matching .ui file without having to search through gPar again
		if (nodeData.type == enumNodeType::primitive)
		{
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
		int currentType = item->data(1, Qt::UserRole).toInt();
		ui->treeWidget_objects->setItemWidget(item, 1, buildTypeComboBox(currentType));
	}
}

// Validates and reformats the position cell whenever the user edits it directly.
// Any text that does not parse to exactly three numbers is reset to the origin.
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
		treeNodeIds.insert(item->data(0, Qt::UserRole).toInt());

	// Remove parameters that belong to nodes that have been deleted from the tree
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

	// Write the current state of each tree item back into params
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

		// Hybrid-child fractals share the parent position; do not store a separate position for them
		if (!isFractalInHybridGroup(item))
			addOrSetParam(params, posParam, parsePosition(item->text(3)));
	}
}

void cObjectsTreeWidget::pressedRefreshButton()
{
	UpdateTree(gPar, gParFractal);
}

// Asks the user for a type and name, then inserts a new node into the tree.
// The new item is added as a child of the currently selected item, or at the
// top level when nothing is selected.
void cObjectsTreeWidget::slotAddObject()
{
	// Prompt the user to choose the node type
	QStringList types = {
		"fractal", "primitive", "hybrid", "boolean Add", "boolean Mul", "boolean Sub"};
	bool ok = false;
	QString typeName =
		QInputDialog::getItem(this, tr("Add Object"), tr("Select object type:"), types, 0, false, &ok);
	if (!ok) return;

	// Prompt the user for a display name for the new node
	QString name = QInputDialog::getText(
		this, tr("Add Object"), tr("Enter object name:"), QLineEdit::Normal, typeName, &ok);
	if (!ok || name.isEmpty()) return;

	// Assign the next available node ID (one more than the current maximum)
	int newId = 1;
	const auto allItems = collectAllTreeItems();
	for (QTreeWidgetItem *item : allItems)
		newId = qMax(newId, item->data(0, Qt::UserRole).toInt() + 1);

	QTreeWidgetItem *newItem = new QTreeWidgetItem();
	newItem->setText(0, name);
	newItem->setText(2, QString::number(newId)); // objectId = nodeId for new objects

	// Map the selected type string back to the enum value
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

	// Add as child of selected item, or at top level if nothing is selected
	QList<QTreeWidgetItem *> selected = ui->treeWidget_objects->selectedItems();
	if (!selected.isEmpty())
		selected.first()->addChild(newItem);
	else
		ui->treeWidget_objects->addTopLevelItem(newItem);

	ui->treeWidget_objects->setItemWidget(newItem, 1, buildTypeComboBox(int(type)));
	ui->treeWidget_objects->expandAll();
	ui->treeWidget_objects->setCurrentItem(newItem);
}

// Removes the currently selected node from the tree.
// Children of the deleted node are re-parented to the deleted node's parent
// so that grandchildren are not lost along with their parent.
void cObjectsTreeWidget::slotDeleteObject()
{
	QList<QTreeWidgetItem *> selected = ui->treeWidget_objects->selectedItems();
	if (selected.isEmpty()) return;

	QTreeWidgetItem *item = selected.first();

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
		parent->removeChild(item);
	else
		delete ui->treeWidget_objects->takeTopLevelItem(
			ui->treeWidget_objects->indexOfTopLevelItem(item));
}

// Builds the info label that is shown at the top of the editor panel.
// It tells the user which object they are currently editing and what its type is.
// For primitive nodes the specific primitive subtype (e.g. "box", "sphere") is
// appended in parentheses so the user has full context without looking at the tree.
QLabel *cObjectsTreeWidget::buildInfoLabel(QTreeWidgetItem *item, enumNodeType type)
{
	QString objectName = item->text(0);
	QString objectTypeName = nodeTypeToString(type);
	QString objectTypeDisplay = objectTypeName;

	if (type == enumNodeType::primitive)
	{
		// Append the primitive subtype stored by UpdateTree() so the label reads e.g.
		// "Type: primitive (box)" instead of just "Type: primitive"
		QString primTypeName = item->data(3, Qt::UserRole).toString();
		if (primTypeName.isEmpty()) primTypeName = item->text(0);
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

// Builds a group box containing X/Y/Z spin boxes pre-filled with the item's current position.
// Each spin box is connected so that any change is immediately written back into column 3
// of the tree item, keeping the tree and the editor in sync without requiring an explicit Apply.
QGroupBox *cObjectsTreeWidget::buildPositionGroup(QTreeWidgetItem *item)
{
	QGroupBox *posGroup = new QGroupBox(tr("Position"));
	QHBoxLayout *posLayout = new QHBoxLayout(posGroup);

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

	// Lambda captures the spin boxes by value; any change to X, Y, or Z immediately
	// reformats and stores the full position string back into the tree item
	auto updatePos = [=]()
	{ item->setText(3, formatPosition({spinX->value(), spinY->value(), spinZ->value()})); };

	connect(spinX, QOverload<double>::of(&QDoubleSpinBox::valueChanged), spinX, updatePos);
	connect(spinY, QOverload<double>::of(&QDoubleSpinBox::valueChanged), spinY, updatePos);
	connect(spinZ, QOverload<double>::of(&QDoubleSpinBox::valueChanged), spinZ, updatePos);

	return posGroup;
}

// Creates and initialises a cTabFractal editor for the fractal identified by 'objectId'.
// The fractal index is clamped to the valid range so an out-of-range objectId cannot crash.
QWidget *cObjectsTreeWidget::buildFractalEditor(int objectId)
{
	// objectId is 1-based; fractal indices stored in gParFractal are 0-based
	int fractalIndex = qBound(0, objectId - 1, NUMBER_OF_FRACTALS - 1);

	cFractalObject *fractalTab = new cFractalObject();
	fractalTab->AssignParameterContainers(gPar, gParFractal);
	fractalTab->Init(true, fractalIndex);
	fractalTab->AssignParentDockFractal(nullptr);

	// Populate the tab with the current parameter values from gPar and gParFractal
	fractalTab->SynchronizeInterface(gPar, qInterface::write);
	fractalTab->SynchronizeFractal(gParFractal->at(fractalIndex), qInterface::write);

	return fractalTab;
}

// Loads the .ui file for the given primitive type, renames all child widgets so that
// their object names include the primitive's full parameter name prefix, and then
// populates the widgets from gPar via SynchronizeInterfaceWindow.
// Returns nullptr when no matching .ui file exists or the file fails to load.
QWidget *cObjectsTreeWidget::buildPrimitiveEditor(QTreeWidgetItem *item, int objectId)
{
	// Retrieve the primitive type name stored by UpdateTree() (e.g. "box", "sphere")
	QString primTypeName = item->data(3, Qt::UserRole).toString();
	if (primTypeName.isEmpty()) primTypeName = item->text(0);

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
		// Insert the full primitive name into each child widget's object name immediately
		// after the first underscore so that SynchronizeInterfaceWindow can match widget
		// names to parameter keys (e.g. "spinbox_x" becomes "spinbox_primitive_box_001_x")
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

	// Populate the primitive editor widgets with values from gPar
	SynchronizeInterfaceWindow(primWidget, gPar, qInterface::write);
	return primWidget;
}

// Responds to the user selecting a different item in the object tree.
// Before tearing down the old editor, the current widget values are read back
// into gPar so that any changes the user made are not silently discarded.
// A fresh editor container is then built for the newly selected item.
void cObjectsTreeWidget::slotItemSelectionChanged()
{
	// Save any user-modified parameter values from the current editor back to gPar
	// before tearing it down, so changes are not lost when switching selection
	if (currentEditorWidget)
	{
		SynchronizeInterfaceWindow(currentEditorWidget, gPar, qInterface::read);
		editorLayout->removeWidget(currentEditorWidget);
		delete currentEditorWidget;
		currentEditorWidget = nullptr;
	}

	QList<QTreeWidgetItem *> selected = ui->treeWidget_objects->selectedItems();
	if (selected.isEmpty()) return;

	QTreeWidgetItem *item = selected.first();
	enumNodeType type = enumNodeType(item->data(1, Qt::UserRole).toInt());
	int objectId = item->data(2, Qt::UserRole).toInt();

	// Wrapper widget so the whole editor area can be removed/replaced as a single unit
	QWidget *editorContainer = new QWidget();
	QVBoxLayout *containerLayout = new QVBoxLayout(editorContainer);

	// Header label: tells the user which object and type they are currently editing
	containerLayout->addWidget(buildInfoLabel(item, type));

	// Position group is shown for all nodes except fractals nested inside a hybrid group
	if (!isFractalInHybridGroup(item)) containerLayout->addWidget(buildPositionGroup(item));

	// Build the type-specific parameter editor
	if (type == enumNodeType::fractal)
	{
		QWidget *fractalEditor = buildFractalEditor(objectId);
		if (fractalEditor) containerLayout->addWidget(fractalEditor);
	}
	else if (type == enumNodeType::primitive)
	{
		QWidget *primEditor = buildPrimitiveEditor(item, objectId);
		if (primEditor)
			containerLayout->addWidget(primEditor);
		else
			return; // No matching .ui file – leave the editor area empty
	}

	containerLayout->addStretch();
	currentEditorWidget = editorContainer;
	editorLayout->addWidget(currentEditorWidget);

	// Apply the global UI style settings (colour coding, layout spacing) to the new editor
	if (gPar->Get<bool>("ui_colorize"))
		cInterface::ColorizeGroupBoxes(this, gPar->Get<int>("ui_colorize_random_seed") + objectId);
	cInterface::AdjustLayoutSpacing(this, gPar->Get<int>("ui_layout_spacing"));
}
