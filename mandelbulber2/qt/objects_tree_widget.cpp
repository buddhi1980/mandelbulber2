
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
#include <QCheckBox>
#include <QInputDialog>
#include <QList>
#include <QDialog>
#include <QToolButton>
#include <QGridLayout>
#include <QMessageBox>
#include <QMenu>
#include <QFontMetrics>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QDrag>
#include <QMimeData>
#include <QApplication>

#include "src/interface.hpp"
#include "src/primitives.h"
#include "src/my_ui_loader.h"
#include "src/system_directories.hpp"
#include "src/system_data.hpp"
#include "ui_objects_tree_widget.h"
#include "src/initparameters.hpp"
#include "src/fractal_container.hpp"
#include "src/objects_tree.h"
#include "formula/definition/all_fractal_list.hpp"
#include "fractal_object.h"
#include "general_object_parameters.h"
#include "fractal_calculation_parameters.h"
#include "material_widget.h"
#include "drag_drop_tree_widget.h"

cObjectsTreeWidget::cObjectsTreeWidget(QWidget *parent)
		: QWidget(parent), cMyWidgetWithParams(), ui(new Ui::cObjectsTreeWidget)
{
	ui->setupUi(this);

	// Configure drag-and-drop on the tree widget (promoted to cDragDropTreeWidget)
	ui->treeWidget_objects->setDragDropMode(QTreeWidget::DragDrop);
	ui->treeWidget_objects->setDefaultDropAction(Qt::MoveAction);
	ui->treeWidget_objects->setDropIndicatorShown(true);
	ui->treeWidget_objects->setDragDropOverwriteMode(false);
	ui->treeWidget_objects->header()->setSectionsMovable(false);
	ui->treeWidget_objects->setContextMenuPolicy(Qt::CustomContextMenu);

	// Default 50/50 split between tree widget and editor area
	ui->splitter->setSizes({1, 1});

	// Connect the refresh button so the user can force a reload of the tree from gPar
	connect(
		ui->pushButton_refresh, &QPushButton::clicked, this, &cObjectsTreeWidget::pressedRefreshButton);

	// Keep gPar in sync whenever the user edits a cell directly in the tree
	connect(
		ui->treeWidget_objects, &QTreeWidget::itemChanged, this, &cObjectsTreeWidget::onItemChanged);

	// Connect add/delete buttons for managing scene objects
	connect(ui->pushButton_add_group, &QPushButton::clicked, this, &cObjectsTreeWidget::slotAddGroup);
	connect(
		ui->pushButton_add_fractal, &QPushButton::clicked, this, &cObjectsTreeWidget::slotAddFractal);
	connect(ui->pushButton_add_primitive, &QPushButton::clicked, this,
		&cObjectsTreeWidget::slotAddPrimitive);
	connect(ui->pushButton_delete_object, &QPushButton::clicked, this,
		&cObjectsTreeWidget::slotDeleteObject);
	connect(ui->pushButton_duplicate, &QPushButton::clicked, this,
		&cObjectsTreeWidget::slotDuplicateObject);

	// When the user clicks a different tree item, rebuild the editor panel below the tree
	connect(ui->treeWidget_objects, &QTreeWidget::itemSelectionChanged, this,
		&cObjectsTreeWidget::slotItemSelectionChanged);

	// Initialize the editor area that appears below the tree widget
	editorLayout = new QVBoxLayout();
	editorLayout->setContentsMargins(0, 0, 0, 0);
	ui->widget_editor_area->setLayout(editorLayout);

	// Connect drag-and-drop signals from cDragDropTreeWidget to slots
	cDragDropTreeWidget *treeWidget = qobject_cast<cDragDropTreeWidget *>(ui->treeWidget_objects);
	if (treeWidget)
	{
		connect(treeWidget, &cDragDropTreeWidget::dragStartRequested, this,
			&cObjectsTreeWidget::onDragStartRequested);
		connect(treeWidget, &cDragDropTreeWidget::dragMoveOverItem, this,
			&cObjectsTreeWidget::onDragMoveOverItem);
		connect(
			treeWidget, &cDragDropTreeWidget::dropCompleted, this, &cObjectsTreeWidget::onDropCompleted);
		connect(treeWidget, &cDragDropTreeWidget::treeStructureChanged, this,
			&cObjectsTreeWidget::onTreeStructureChanged);
	}
	connect(ui->treeWidget_objects, &QWidget::customContextMenuRequested, this,
		&cObjectsTreeWidget::onCustomContextMenu);
}

cObjectsTreeWidget::~cObjectsTreeWidget()
{
	delete ui;
}

void cObjectsTreeWidget::AssignParameterContainers(
	std::shared_ptr<cParameterContainer> _params, std::shared_ptr<cFractalContainer> _fractalParams)
{
	cMyWidgetWithParams::AssignParameterContainers(_params, _fractalParams);
}

void cObjectsTreeWidget::AssignSpecialWidgets(
	RenderedImage *_renderedImage, QComboBox *_mouseFunctionCombo)
{
	cMyWidgetWithParams::AssignSpecialWidgets(_renderedImage, _mouseFunctionCombo);
}

// Widget names from cGeneralObjectParameters that represent the common object parameters
// (position, rotation, scale, repeat, material).  These are the canonical sources of truth
// when cGeneralObjectParameters is embedded in an editor; the same-named widgets found in
// primitive .ui files are neutralised so they do not conflict with the sync engine.
static const QSet<QString> s_commonGeneralObjectWidgetNames = {"vect3_position_x",
	"vect3_position_y", "vect3_position_z", "spinboxd3_rotation_x", "spinboxd3_rotation_y",
	"spinboxd3_rotation_z", "vect3_repeat_x", "vect3_repeat_y", "vect3_repeat_z",
	"materialselector_material_id", "logedit_scale", "groupBox_smooth_de_combine",
	"checkBox_smooth_de_combine_enable", "label_smooth_de_combine_distance",
	"spinbox_smooth_de_combine_distance"};

struct sPrimitiveSelectorItem
{
	QString typeName;
	QString iconPath;
};

static const QList<sPrimitiveSelectorItem> s_primitiveSelectorItems = {
	{"rectangle", ":/primitives/icons/rectangle.png"}, {"circle", ":/primitives/icons/circle.png"},
	{"box", ":/primitives/icons/box.png"}, {"cylinder", ":/primitives/icons/cylinder.png"},
	{"cone", ":/primitives/icons/cone.png"}, {"sphere", ":/primitives/icons/sphere.png"},
	{"torus", ":/primitives/icons/torus.png"}, {"plane", ":/primitives/icons/plane.png"},
	{"water", ":/primitives/icons/water.png"}, {"prism", ":/primitives/icons/prism.png"},
	{"ellipsoid", ":/primitives/icons/ellipsoid.png"}};

// --- Private helpers ---

// Returns a flat list of every item in the tree, regardless of nesting depth.
// Uses an explicit stack to avoid recursion on deep trees.
QList<QTreeWidgetItem *> cObjectsTreeWidget::collectAllTreeItems() const
{
	QList<QTreeWidgetItem *> allItems;
	QList<QTreeWidgetItem *> queue;
	for (int i = 0; i < ui->treeWidget_objects->topLevelItemCount(); ++i)
		queue.append(ui->treeWidget_objects->topLevelItem(i));
	while (!queue.isEmpty())
	{
		QTreeWidgetItem *item = queue.takeFirst();
		allItems.append(item);
		for (int i = 0; i < item->childCount(); ++i)
			queue.append(item->child(i));
	}
	return allItems;
}

// Collects the selected node and all its descendants as (nodeId, treeItem) pairs.
// Returns nodes in pre-order (root first) so parent references are always available.
QList<QPair<int, QTreeWidgetItem *>> cObjectsTreeWidget::collectSubtree(QTreeWidgetItem *root) const
{
	QList<QPair<int, QTreeWidgetItem *>> result;
	if (!root || root == worldItem) return result;

	std::function<void(QTreeWidgetItem *)> dfs;
	dfs = [&](QTreeWidgetItem *node)
	{
		int nodeId = node->data(treeData::nodeId, Qt::UserRole).toInt();
		result.append({nodeId, node});
		for (int i = 0; i < node->childCount(); ++i)
			dfs(node->child(i));
	};
	dfs(root);
	return result;
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
// These nodes use the hybrid parent's shared calculation and transform parameters.
bool cObjectsTreeWidget::isFractalInHybridGroup(QTreeWidgetItem *item) const
{
	if (enumNodeType(getNodeType(item)) != enumNodeType::fractal) return false;
	if (!item->parent()) return false;
	return enumNodeType(getNodeType(item->parent())) == enumNodeType::hybrid;
}

// Creates a QLabel displaying the node type as text.
QWidget *cObjectsTreeWidget::buildTypeLabel(int currentType)
{
	QLabel *label = new QLabel(nodeTypeToString(enumNodeType(currentType)));
	label->setAlignment(Qt::AlignCenter);
	return label;
}

// Returns the appropriate QIcon for a node based on its type and, for primitives,
// the primitive type name. Returns an empty QIcon if the icon cannot be loaded.
QIcon cObjectsTreeWidget::getIconForNode(enumNodeType type, const QString &primTypeName)
{
	if (type == enumNodeType::primitive && !primTypeName.isEmpty())
	{
		for (const auto &prim : s_primitiveSelectorItems)
		{
			if (prim.typeName == primTypeName)
			{
				QIcon icon(prim.iconPath);
				return icon.pixmap(QSize(1, 1)).isNull() ? QIcon() : icon;
			}
		}
	}
	else if (type == enumNodeType::hybrid)
	{
		QIcon icon(":/navigation/icons/group_hybrid.svg");
		return icon.pixmap(QSize(1, 1)).isNull() ? QIcon() : icon;
	}
	else if (type == enumNodeType::booleanAdd)
	{
		QIcon icon(":/navigation/icons/group_add.svg");
		return icon.pixmap(QSize(1, 1)).isNull() ? QIcon() : icon;
	}
	else if (type == enumNodeType::booleanMul)
	{
		QIcon icon(":/navigation/icons/group_mul.svg");
		return icon.pixmap(QSize(1, 1)).isNull() ? QIcon() : icon;
	}
	else if (type == enumNodeType::booleanSub)
	{
		QIcon icon(":/navigation/icons/group_sub.svg");
		return icon.pixmap(QSize(1, 1)).isNull() ? QIcon() : icon;
	}
	return QIcon();
}
void cObjectsTreeWidget::updateFractalIcon(QTreeWidgetItem *item, int objectId, int formulaEnum)
{
	if (objectId <= 0 || objectId > fractalParams->size()) return;

	int miniSize = systemData.GetPreferredThumbnailSize() / 4;
	QPixmap scaledPixmap;

	for (cAbstractFractal *fractal : newFractalList)
	{
		if (int(fractal->getInternalId()) == formulaEnum)
		{
			QString iconName = fractal->getIconName();
			QIcon icon(iconName);
			QPixmap pixmap = icon.pixmap(QSize(128, 128));
			if (!pixmap.isNull())
			{
				scaledPixmap =
					pixmap.scaled(miniSize, miniSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
			}
			break;
		}
	}

	if (!scaledPixmap.isNull())
	{
		item->setIcon(treeCol::icon, QIcon(scaledPixmap));
	}
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

	// Find the smallest positive integer not yet used as a fractal object ID.
	// Fractal containers grow dynamically; cap at a generous safety limit.
	const int maxFractals = 10000;
	for (int objectId = 1; objectId <= maxFractals; ++objectId)
	{
		if (!usedFractalIds.contains(objectId)) return objectId;
	}
	qCritical() << "findNextAvailableFractalObjectId: exceeded safety limit of" << maxFractals;
	return maxFractals + 1;
}

int cObjectsTreeWidget::findNextAvailableGroupObjectId() const
{
	QSet<int> usedObjectIds;
	for (QTreeWidgetItem *item : collectAllTreeItems())
	{
		int objectId = item->data(treeData::objectId, Qt::UserRole).toInt();
		if (objectId > 0) usedObjectIds.insert(objectId);
	}

	// Group IDs start at 100, safely above fractal IDs and below primitive IDs (1000+).
	int newObjectId = 100;
	while (usedObjectIds.contains(newObjectId))
		++newObjectId;
	return newObjectId;
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

enumNodeType cObjectsTreeWidget::showGroupSelectionDialog(bool *ok)
{
	if (ok) *ok = false;

	QDialog dialog(this);
	dialog.setWindowTitle(tr("Select group type"));

	QGridLayout *layout = new QGridLayout(&dialog);

	enumNodeType selectedType = enumNodeType::hybrid;
	const QList<QPair<QString, enumNodeType>> options = {{tr("Hybrid fractal"), enumNodeType::hybrid},
		{tr("Boolean ADD"), enumNodeType::booleanAdd}, {tr("Boolean MUL"), enumNodeType::booleanMul},
		{tr("Boolean SUB"), enumNodeType::booleanSub}};

	for (int i = 0; i < options.size(); ++i)
	{
		QToolButton *button = new QToolButton(&dialog);
		button->setText(options[i].first);
		button->setToolButtonStyle(Qt::ToolButtonTextOnly);
		button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
		layout->addWidget(button, i / 2, i % 2);
		connect(button, &QToolButton::clicked, &dialog,
			[&dialog, &selectedType, options, i]()
			{
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

QString cObjectsTreeWidget::showPrimitiveSelectionDialog(bool *ok)
{
	if (ok) *ok = false;

	QDialog dialog(this);
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
		connect(button, &QToolButton::clicked, &dialog,
			[&dialog, &selectedPrimitiveType, item]()
			{
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
	newItem->setText(treeCol::nodeId, QString::number(nodeId));
	newItem->setText(treeCol::name, name);
	newItem->setText(treeCol::objectId, objectId >= 0 ? QString::number(objectId) : QString());

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
	else if (worldItem)
		worldItem->addChild(newItem);
	else
		ui->treeWidget_objects->addTopLevelItem(newItem);
}

void cObjectsTreeWidget::ensureNodeParamsExist(int nodeId)
{
	QString prefix = QString("node_%1_").arg(nodeId, 4, 10, QChar('0'));
	if (!params->IfExists(prefix + "definition")) InitNodeParams(nodeId, params);
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
		lastSelectedNodeId =
			selectedBeforeRefresh.first()->data(treeData::nodeId, Qt::UserRole).toInt();

	ui->treeWidget_objects->clear();

	worldItem = new QTreeWidgetItem();
	worldItem->setText(treeCol::name, tr("World"));
	worldItem->setData(treeData::nodeId, Qt::UserRole, 0); // nodeId 0 = World sentinel
	worldItem->setIcon(treeCol::icon, QIcon(":/navigation/icons/world.png"));
	ui->treeWidget_objects->addTopLevelItem(worldItem);

	ui->treeWidget_objects->setColumnCount(7);
	ui->treeWidget_objects->setHeaderLabels(
		{"Icon", "Name", "Type", "Material", "Enable", "ID", "Object ID"});

	cObjectsTree objectsTree;
	objectsTree.CreateNodeDataFromParameters(params);
	const auto sortedList = objectsTree.GetSortedNodeDataList();

	QMap<int, QTreeWidgetItem *> nodeItems;

	// Retrieve the full list of primitives so we can attach the primitive type name
	// to each primitive tree item via Qt::UserRole on column 3
	QList<sPrimitiveItem> primitiveList = cPrimitives::GetListOfPrimitives(params);

	// First pass: create all items without yet wiring up the parent-child relationships
	for (const auto &nodeData : sortedList)
	{
		QTreeWidgetItem *item = new QTreeWidgetItem();
		item->setText(treeCol::nodeId, QString::number(nodeData.id));
		item->setText(treeCol::name, nodeData.name);
		item->setText(
			treeCol::objectId, nodeData.objectId >= 0 ? QString::number(nodeData.objectId) : QString());

		item->setData(treeData::nodeId, Qt::UserRole, nodeData.id);
		item->setData(treeData::objectId, Qt::UserRole, nodeData.objectId);
		item->setData(treeData::nodeType, Qt::UserRole, int(nodeData.type));
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
		// Load fractal icon for fractal nodes
		{
			int miniSize = systemData.GetPreferredThumbnailSize() / 4;
			QPixmap scaledPixmap;
			if (nodeData.type == enumNodeType::fractal)
			{
				int objectId = nodeData.objectId;
				if (objectId > 0 && objectId <= fractalParams->size())
				{
					int formulaEnum = fractalParams->at(objectId - 1)->Get<int>("formula");
					for (cAbstractFractal *fractal : newFractalList)
					{
						if (int(fractal->getInternalId()) == formulaEnum)
						{
							QString iconName = fractal->getIconName();
							QIcon icon(iconName);
							QPixmap pixmap = icon.pixmap(QSize(128, 128));
							if (!pixmap.isNull())
							{
								scaledPixmap =
									pixmap.scaled(miniSize, miniSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
							}
							break;
						}
					}
				}
			}
			else if (isGroupType(nodeData.type))
			{
				QIcon icon = getIconForNode(nodeData.type);
				QPixmap pixmap = icon.pixmap(QSize(128, 128));
				if (!pixmap.isNull())
				{
					scaledPixmap =
						pixmap.scaled(miniSize, miniSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
				}
			}
			else
			{
				QString primTypeName = item->data(treeData::primTypeName, Qt::UserRole).toString();
				QIcon icon = getIconForNode(nodeData.type, primTypeName);
				QPixmap pixmap = icon.pixmap(QSize(128, 128));
				if (!pixmap.isNull())
				{
					scaledPixmap =
						pixmap.scaled(miniSize, miniSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
				}
			}
			if (!scaledPixmap.isNull())
			{
				item->setIcon(treeCol::icon, QIcon(scaledPixmap));
			}
		}

		nodeItems[nodeData.id] = item;
	}

	// Second pass: wire up parent-child relationships in the tree widget
	for (const auto &node : sortedList)
	{
		QTreeWidgetItem *item = nodeItems[node.id];
		if (node.parentId == 0 || !nodeItems.contains(node.parentId))
			worldItem->addChild(item);
		else
			nodeItems[node.parentId]->addChild(item);
	}

	ui->treeWidget_objects->expandAll();

	// Attach the type combo boxes and miniature material widgets after the tree is fully
	// built so that setItemWidget() can find the correct persistent index for each item.
	int miniSize = systemData.GetPreferredThumbnailSize() / 4;
	int rowHeight = miniSize;
	ui->treeWidget_objects->header()->setMinimumSectionSize(miniSize);
	ui->treeWidget_objects->setIndentation(miniSize);

	// Calculate icon column width: miniSize + max indentation depth * indent pixel width + extra
	// padding
	int maxDepth = 0;
	for (QTreeWidgetItem *item : nodeItems)
	{
		int depth = 0;
		QTreeWidgetItem *parent = item->parent();
		while (parent)
		{
			++depth;
			parent = parent->parent();
		}
		maxDepth = qMax(maxDepth, depth);
	}
	int iconColWidth =
		miniSize + maxDepth * ui->treeWidget_objects->indentation() + miniSize + miniSize / 2;

	ui->treeWidget_objects->header()->setSectionResizeMode(treeCol::icon, QHeaderView::Fixed);
	ui->treeWidget_objects->header()->setSectionResizeMode(treeCol::name, QHeaderView::Interactive);
	ui->treeWidget_objects->header()->setSectionResizeMode(treeCol::type, QHeaderView::Fixed);
	ui->treeWidget_objects->header()->setSectionResizeMode(treeCol::material, QHeaderView::Fixed);
	ui->treeWidget_objects->header()->setSectionResizeMode(treeCol::enable, QHeaderView::Fixed);
	ui->treeWidget_objects->header()->setSectionResizeMode(treeCol::nodeId, QHeaderView::Fixed);
	ui->treeWidget_objects->header()->setSectionResizeMode(treeCol::objectId, QHeaderView::Fixed);
	ui->treeWidget_objects->header()->resizeSection(treeCol::icon, iconColWidth);
	ui->treeWidget_objects->header()->resizeSection(treeCol::name, miniSize * 4);
	ui->treeWidget_objects->header()->resizeSection(treeCol::type, miniSize * 3);
	ui->treeWidget_objects->header()->resizeSection(treeCol::material, miniSize);
	ui->treeWidget_objects->header()->resizeSection(treeCol::enable, miniSize);
	ui->treeWidget_objects->header()->resizeSection(treeCol::nodeId, miniSize);
	ui->treeWidget_objects->header()->resizeSection(treeCol::objectId, miniSize);
	QString style = QString("QTreeWidget::item { height: %1px; }").arg(rowHeight);
	ui->treeWidget_objects->setStyleSheet(style);
	ui->treeWidget_objects->setIconSize(QSize(miniSize, miniSize));
	for (QTreeWidgetItem *item : nodeItems)
	{
		int nodeId = item->data(treeData::nodeId, Qt::UserRole).toInt();
		int currentType = item->data(treeData::nodeType, Qt::UserRole).toInt();
		ui->treeWidget_objects->setItemWidget(item, treeCol::type, buildTypeLabel(currentType));
		attachMaterialWidget(item, nodeId, params);
		QString enabledParam = QString("node_%1_enabled").arg(nodeId, 4, 10, QChar('0'));
		bool isChecked = params->Get<bool>(enabledParam);
		attachEnableCheckbox(item, nodeId, isChecked);
	}

	if (lastSelectedNodeId > 0 && nodeItems.contains(lastSelectedNodeId))
		ui->treeWidget_objects->setCurrentItem(nodeItems.value(lastSelectedNodeId));
}

void cObjectsTreeWidget::onItemChanged(QTreeWidgetItem *item, int column)
{
	Q_UNUSED(item)
	Q_UNUSED(column)
	// Column edits are no longer handled here; position/rotation/scale/repeat have
	// been replaced by the material thumbnail column which is not user-editable.
	// Enable checkbox changes are handled by the lambda in attachEnableCheckbox.
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
	// Items are in DFS order, so we embed displayOrder as the 6th field in the definition string
	int displayOrder = 0;
	for (QTreeWidgetItem *item : allItems)
	{
		int nodeId = item->data(treeData::nodeId, Qt::UserRole).toInt();
		if (nodeId <= 0) continue; // skip World sentinel
		int nodeType = getNodeType(item);
		int objectId = item->data(treeData::objectId, Qt::UserRole).toInt();
		QString name = item->text(treeCol::name);
		int parentId =
			item->parent() ? item->parent()->data(treeData::nodeId, Qt::UserRole).toInt() : 0;

		QString prefix = QString("node_%1").arg(nodeId, 4, 10, QChar('0'));
		QString defParam = prefix + "_definition";

		addOrSetParam(params, defParam,
			QString("%1,%2,%3,%4,%5,%6")
				.arg(name)
				.arg(nodeId)
				.arg(nodeType)
				.arg(parentId)
				.arg(objectId)
				.arg(displayOrder));

		QWidget *enableWidget = ui->treeWidget_objects->itemWidget(item, treeCol::enable);
		if (enableWidget)
		{
			QCheckBox *check = qobject_cast<QCheckBox *>(enableWidget);
			if (check)
			{
				addOrSetParam(params, prefix + "_enabled", check->isChecked());
			}
		}
		displayOrder++;
	}
}

void cObjectsTreeWidget::pressedRefreshButton()
{
	UpdateTree(params, fractalParams);
}
void cObjectsTreeWidget::attachMaterialWidget(
	QTreeWidgetItem *item, int nodeId, std::shared_ptr<cParameterContainer> params)
{
	if (item->parent())
	{
		int parentType = getNodeType(item->parent());
		if (enumNodeType(parentType) == enumNodeType::hybrid) return;
	}
	int materialId = params->Get<int>(QString("node_%1_material").arg(nodeId, 4, 10, QChar('0')));
	if (materialId < 0) return;
	const int miniSize = systemData.GetPreferredThumbnailSize() / 4;
	cMaterialWidget *matWidget = new cMaterialWidget(miniSize, miniSize, 1, this);
	matWidget->AssignMaterial(params, materialId, nullptr);
	ui->treeWidget_objects->setItemWidget(item, treeCol::material, matWidget);
}

void cObjectsTreeWidget::attachEnableCheckbox(QTreeWidgetItem *item, int nodeId, bool checked)
{
	QWidget *centerWidget = new QWidget();
	QHBoxLayout *layout = new QHBoxLayout(centerWidget);
	layout->setContentsMargins(0, 0, 0, 0);
	layout->setAlignment(Qt::AlignCenter);

	QCheckBox *check = new QCheckBox();
	check->setChecked(checked);
	check->setObjectName(QString("enable_checkbox_%1").arg(nodeId));
	check->setStyleSheet(
		"QCheckBox { spacing: 0px; } QCheckBox::indicator { width: 16px; height: 16px; }");
	layout->addWidget(check);

	ui->treeWidget_objects->setItemWidget(item, treeCol::enable, centerWidget);
	checkboxToNodeId[check] = nodeId;
	connect(check, &QCheckBox::stateChanged, this,
		[this, nodeId](int state)
		{
			QString paramName = QString("node_%1_enabled").arg(nodeId, 4, 10, QChar('0'));
			params->Set(paramName, state == Qt::Checked);
		});
}

void cObjectsTreeWidget::slotAddGroup()
{
	bool ok = false;
	enumNodeType groupType = showGroupSelectionDialog(&ok);
	if (!ok) return;

	int newNodeId = findNextAvailableNodeId();
	int groupObjectId = findNextAvailableGroupObjectId();
	ensureNodeParamsExist(newNodeId);

	// Write the correct definition (with the unique objectId) to params immediately,
	// overriding the generic placeholder created by ensureNodeParamsExist.
	QString prefix = QString("node_%1_").arg(newNodeId, 4, 10, QChar('0'));
	QString groupName = nodeTypeToString(groupType);
	params->Set(prefix + "definition",
		QString("%1 %2,%2,%3,%4,%5")
			.arg(groupName)
			.arg(newNodeId)
			.arg(int(groupType))
			.arg(0) // parentId: updated via StoreTreeToParams once added to tree
			.arg(groupObjectId));

	QTreeWidgetItem *newItem =
		createNodeItem(newNodeId, groupType, groupObjectId, groupName, QString());
	addNodeToSelectedGroup(newItem);

	ui->treeWidget_objects->setItemWidget(newItem, treeCol::type, buildTypeLabel(int(groupType)));
	attachMaterialWidget(newItem, newNodeId, params);
	QIcon icon = getIconForNode(groupType);
	QPixmap pixmap = icon.pixmap(QSize(128, 128));
	if (!pixmap.isNull())
	{
		int miniSize = systemData.GetPreferredThumbnailSize() / 4;
		QPixmap scaled =
			pixmap.scaled(miniSize, miniSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		newItem->setIcon(treeCol::icon, QIcon(scaled));
	}
	ui->treeWidget_objects->expandAll();
	ui->treeWidget_objects->setCurrentItem(newItem);
	lastSelectedNodeId = newNodeId;
}

void cObjectsTreeWidget::slotAddFractal()
{
	int fractalObjectId = findNextAvailableFractalObjectId();

	// Grow the fractal container if this objectId needs a new slot.
	// objectId is 1-indexed; container index is objectId-1.
	const int fractalIndex = fractalObjectId - 1;
	if (fractalIndex >= fractalParams->size())
	{
		fractalParams->ensureCapacity(fractalIndex);
		InitFractalParams(fractalParams->at(fractalIndex));
		fractalParams->at(fractalIndex)
			->SetContainerName(QString("fractal") + QString::number(fractalIndex));
	}

	int newNodeId = findNextAvailableNodeId();
	ensureNodeParamsExist(newNodeId);

	QString fractalName = tr("fractal %1").arg(fractalObjectId);
	QTreeWidgetItem *newItem =
		createNodeItem(newNodeId, enumNodeType::fractal, fractalObjectId, fractalName, QString());
	addNodeToSelectedGroup(newItem);

	ui->treeWidget_objects->setItemWidget(
		newItem, treeCol::type, buildTypeLabel(int(enumNodeType::fractal)));
	attachMaterialWidget(newItem, newNodeId, params);
	int defaultFormula = fractalParams->at(fractalObjectId - 1)->Get<int>("formula");
	updateFractalIcon(newItem, fractalObjectId, defaultFormula);
	ui->treeWidget_objects->expandAll();
	ui->treeWidget_objects->setCurrentItem(newItem);
	lastSelectedNodeId = newNodeId;
}

void cObjectsTreeWidget::slotAddPrimitive()
{
	bool ok = false;
	QString primitiveType = showPrimitiveSelectionDialog(&ok);
	if (!ok || primitiveType.isEmpty()) return;

	// Check if the selected target group is a hybrid group
	QTreeWidgetItem *targetGroup = selectedGroupTarget();
	if (targetGroup)
	{
		enumNodeType targetType = enumNodeType(getNodeType(targetGroup));
		if (targetType == enumNodeType::hybrid)
		{
			return;
		}
	}

	int newNodeId = findNextAvailableNodeId();
	ensureNodeParamsExist(newNodeId);

	QList<sPrimitiveItem> primitiveList = cPrimitives::GetListOfPrimitives(params);
	int newPrimitiveIndex = cPrimitives::NewPrimitiveIndex(primitiveType, primitiveList);
	QString primitiveFullName = QString("primitive_%1_%2").arg(primitiveType).arg(newPrimitiveIndex);
	sPrimitiveItem newPrimitive(cPrimitives::PrimitiveNameToEnum(primitiveType), newPrimitiveIndex,
		primitiveFullName, primitiveType);

	InitPrimitiveParams(newPrimitive, params);
	params->Set(newPrimitive.Name("enabled"), true);

	int primitiveObjectId = findNextAvailablePrimitiveObjectId();
	params->Set(newPrimitive.Name("object_id"), primitiveObjectId);

	QString primitiveName = params->Get<QString>(newPrimitive.Name("name"));
	QTreeWidgetItem *newItem = createNodeItem(
		newNodeId, enumNodeType::primitive, primitiveObjectId, primitiveName, primitiveType);
	addNodeToSelectedGroup(newItem);

	ui->treeWidget_objects->setItemWidget(
		newItem, treeCol::type, buildTypeLabel(int(enumNodeType::primitive)));
	attachMaterialWidget(newItem, newNodeId, params);
	QIcon icon = getIconForNode(enumNodeType::primitive, primitiveType);
	QPixmap pixmap = icon.pixmap(QSize(128, 128));
	if (!pixmap.isNull())
	{
		int miniSize = systemData.GetPreferredThumbnailSize() / 4;
		QPixmap scaled =
			pixmap.scaled(miniSize, miniSize, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		newItem->setIcon(treeCol::icon, QIcon(scaled));
	}
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
	if (item == worldItem) return; // World cannot be deleted

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
	{
		QTreeWidgetItem *removed = parent->takeChild(parent->indexOfChild(item));
		delete removed;
	}
	else
		delete ui->treeWidget_objects->takeTopLevelItem(
			ui->treeWidget_objects->indexOfTopLevelItem(item));

	if (deletedType == enumNodeType::primitive)
	{
		const QList<sPrimitiveItem> primitiveList = cPrimitives::GetListOfPrimitives(params);
		for (const sPrimitiveItem &primitive : primitiveList)
		{
			if (params->Get<int>(primitive.Name("object_id")) != deletedObjectId) continue;
			const QList<QString> paramsToDelete =
				cPrimitives::GetListOfPrimitiveParams(primitive, params);
			for (const QString &parameterName : paramsToDelete)
			{
				params->DeleteParameter(parameterName);
			}
			return;
		}
	}
}

// Parses gPar to find all nodes in the subtree rooted at 'rootNodeId'.
// Returns nodes in pre-order (root first) so parent references are always available.
QList<cObjectsTreeWidget::sDuplicateNodeInfo> cObjectsTreeWidget::CollectSubtreeFromParams(
	int rootNodeId, std::shared_ptr<const cParameterContainer> params)
{
	QList<sDuplicateNodeInfo> result;

	// First, collect all node definitions into a map for traversal
	QMap<int, sDuplicateNodeInfo> allNodes;
	QList<QString> allParams = params->GetListOfParameters();
	for (const QString &paramName : allParams)
	{
		if (!paramName.startsWith("node_") || !paramName.endsWith("_definition")) continue;

		bool ok = false;
		int nodeId = paramName.mid(5, 4).toInt(&ok);
		if (!ok || nodeId <= 0) continue;

		QString def = params->Get<QString>(paramName);
		QStringList parts = def.split(',');
		if (parts.size() < 5) continue;

		sDuplicateNodeInfo info;
		info.nodeId = nodeId;
		info.name = parts[0];
		info.nodeType = int(enumNodeType(parts[2].toInt()));
		info.objectId = parts[4].toInt();
		info.primTypeName.clear();
		info.parentId = parts[3].toInt();

		// Extract primTypeName from name for primitives
		if (info.nodeType == int(enumNodeType::primitive))
		{
			// name format: "box 1" or "sphere 1" — first word is the type
			QStringList nameParts = info.name.split(' ');
			if (!nameParts.isEmpty()) info.primTypeName = nameParts.first();
		}

		allNodes[nodeId] = info;
	}

	// DFS from rootNodeId to collect subtree in pre-order
	std::function<void(int)> dfs;
	dfs = [&](int currentId)
	{
		if (!allNodes.contains(currentId)) return;

		sDuplicateNodeInfo info = allNodes[currentId];
		result.append(info);

		// Find and visit children
		for (const auto &node : allNodes)
		{
			if (node.parentId == currentId) dfs(node.nodeId);
		}
	};
	dfs(rootNodeId);

	return result;
}

// Static helper: duplicates nodes in parameters.
// nodes must be in pre-order (root first) so parent references are always available.
// Returns the new root node ID, or -1 on failure.
int cObjectsTreeWidget::DuplicateNodesInParams(const QList<sDuplicateNodeInfo> &nodes,
	std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractalParams)
{
	if (nodes.isEmpty()) return -1;

	// Collect all currently used IDs to avoid collisions
	QSet<int> usedNodeIds;
	QSet<int> usedFractalObjectIds;
	QSet<int> usedGroupObjectIds;
	QSet<int> usedPrimitiveObjectIds;
	// Also collect primitive object IDs that belong to primitives IN the subtree
	// (separate from the next available primitive ID counter)
	QSet<int> subtreePrimitiveObjectIds;
	for (const sDuplicateNodeInfo &info : nodes)
	{
		usedNodeIds.insert(info.nodeId);
		if (info.nodeType == int(enumNodeType::fractal) && info.objectId > 0)
			usedFractalObjectIds.insert(info.objectId);
		if (isGroupType(enumNodeType(info.nodeType)) && info.objectId > 0)
			usedGroupObjectIds.insert(info.objectId);
		if (info.nodeType == int(enumNodeType::primitive) && info.objectId > 0)
		{
			usedPrimitiveObjectIds.insert(info.objectId);
			subtreePrimitiveObjectIds.insert(info.objectId);
		}
	}

	// Also scan existing params for node IDs and object IDs
	QList<QString> allParams = params->GetListOfParameters();
	for (const QString &paramName : allParams)
	{
		if (paramName.startsWith("node_") && paramName.endsWith("_definition"))
		{
			bool ok = false;
			int nodeId = paramName.mid(5, 4).toInt(&ok);
			if (ok && nodeId > 0) usedNodeIds.insert(nodeId);

			// Extract object_id from definition string
			QString def = params->Get<QString>(paramName);
			QStringList parts = def.split(',');
			if (parts.size() >= 5)
			{
				int objId = parts[4].toInt();
				if (objId > 0)
				{
					int type = parts[2].toInt();
					if (type == int(enumNodeType::fractal))
						usedFractalObjectIds.insert(objId);
					else if (isGroupType(enumNodeType(type)))
						usedGroupObjectIds.insert(objId);
					else if (type == int(enumNodeType::primitive))
						usedPrimitiveObjectIds.insert(objId);
				}
			}
		}
	}

	// Build ID mappings: oldId -> newId
	QMap<int, int> nodeIdMap;
	QMap<int, int> fractalObjectIdMap;
	QMap<int, int> groupObjectIdMap;
	QMap<int, int> primitiveObjectIdMap;

	// Find next available node ID that doesn't conflict with ANY existing node
	int nextNodeId = 1;
	while (usedNodeIds.contains(nextNodeId))
		++nextNodeId;

	// Find next available fractal object ID
	int nextFractalId = 1;
	while (usedFractalObjectIds.contains(nextFractalId))
		++nextFractalId;

	// Find next available group object ID
	int nextGroupId = 100;
	while (usedGroupObjectIds.contains(nextGroupId))
		++nextGroupId;

	// Find next available primitive object ID
	int nextPrimitiveId = 1000;
	while (usedPrimitiveObjectIds.contains(nextPrimitiveId))
		++nextPrimitiveId;

	for (const sDuplicateNodeInfo &info : nodes)
	{
		// Ensure new node ID doesn't conflict with any existing node
		int newId = nextNodeId;
		while (usedNodeIds.contains(newId))
			++newId;
		nodeIdMap[info.nodeId] = newId;
		usedNodeIds.insert(newId); // Mark as used to prevent future conflicts
		++nextNodeId;

		if (info.nodeType == int(enumNodeType::fractal) && info.objectId > 0)
		{
			fractalObjectIdMap[info.objectId] = nextFractalId++;
		}
		else if (isGroupType(enumNodeType(info.nodeType)) && info.objectId > 0)
		{
			groupObjectIdMap[info.objectId] = nextGroupId++;
		}
		else if (info.nodeType == int(enumNodeType::primitive) && info.objectId > 0)
		{
			primitiveObjectIdMap[info.objectId] = nextPrimitiveId++;
		}
	}

	// Duplicate parameters at the parameter container level
	for (const sDuplicateNodeInfo &info : nodes)
	{
		int newId = nodeIdMap[info.nodeId];
		int type = info.nodeType;
		QString oldPrefix = QString("node_%1_").arg(info.nodeId, 4, 10, QChar('0'));
		QString newPrefix = QString("node_%1_").arg(newId, 4, 10, QChar('0'));

		// Ensure all default parameters exist for the new node before overwriting values
		if (!params->IfExists(newPrefix + "definition")) InitNodeParams(newId, params);

		// Read old definition string and update id/objectId/parent_id fields
		QString oldDef = params->Get<QString>(oldPrefix + "definition");
		QStringList defParts = oldDef.split(',');
		if (defParts.size() >= 5)
		{
			int oldObjId = defParts[4].toInt();
			int oldParentId = defParts[3].toInt();
			QString newObjIdStr = QString::number(oldObjId);

			// Use the correct object ID map based on this node's type
			if (type == int(enumNodeType::fractal) && fractalObjectIdMap.contains(oldObjId))
			{
				newObjIdStr = QString::number(fractalObjectIdMap[oldObjId]);
			}
			else if (isGroupType(enumNodeType(type)) && groupObjectIdMap.contains(oldObjId))
			{
				newObjIdStr = QString::number(groupObjectIdMap[oldObjId]);
			}
			else if (type == int(enumNodeType::primitive) && primitiveObjectIdMap.contains(oldObjId))
			{
				newObjIdStr = QString::number(primitiveObjectIdMap[oldObjId]);
			}

			QString newParentIdStr = nodeIdMap.contains(oldParentId)
																 ? QString::number(nodeIdMap[oldParentId])
																 : QString::number(oldParentId);
			defParts[1] = QString::number(newId);
			defParts[3] = newParentIdStr;
			defParts[4] = newObjIdStr;
			params->Set(newPrefix + "definition", defParts.join(','));
		}

		// Copy all node parameters
		QStringList nodeParams = {"position", "rotation", "repeat", "scale", "material", "enabled",
			"detail_level_multiplier", "julia_mode", "julia_c", "fractal_constant_factor",
			"initial_waxis", "smooth_de_combine_enable", "smooth_de_combine_distance", "formula_maxiter",
			"formula_stop_iteration"};

		for (const QString &paramName : nodeParams)
		{
			QString oldParam = oldPrefix + paramName;
			QString newParam = newPrefix + paramName;
			if (params->IfExists(oldParam))
			{
				params->SetFromOneParameter(newParam, params->GetAsOneParameter(oldParam));
			}
		}

		// Duplicate fractal parameters for fractal nodes
		if (type == int(enumNodeType::fractal))
		{
			int oldObjId = info.objectId;
			if (!fractalObjectIdMap.contains(oldObjId)) continue;

			int newObjId = fractalObjectIdMap[oldObjId];
			int oldFractalIdx = oldObjId - 1;
			int newFractalIdx = newObjId - 1;

			if (newFractalIdx >= fractalParams->size())
			{
				fractalParams->ensureCapacity(newFractalIdx);
				InitFractalParams(fractalParams->at(newFractalIdx));
				fractalParams->at(newFractalIdx)
					->SetContainerName(QString("fractal") + QString::number(newFractalIdx));
			}

			// Copy all parameters from old fractal to new fractal
			QList<QString> fractalParamsList = fractalParams->at(oldFractalIdx)->GetListOfParameters();
			for (const QString &paramName : fractalParamsList)
			{
				fractalParams->at(newFractalIdx)
					->SetFromOneParameter(
						paramName, fractalParams->at(oldFractalIdx)->GetAsOneParameter(paramName));
			}
		}

		// Duplicate primitive parameters for primitive nodes
		if (type == int(enumNodeType::primitive))
		{
			int oldObjId = info.objectId;
			// Only remap if this node is a primitive IN the subtree (not just any node
			// that happens to have the same objectId value)
			if (!subtreePrimitiveObjectIds.contains(oldObjId)) continue;
			if (!primitiveObjectIdMap.contains(oldObjId)) continue;

			int newObjId = primitiveObjectIdMap[oldObjId];

			// Find the primitive type and old index
			QString primTypeName = info.primTypeName;
			if (primTypeName.isEmpty()) continue;

			// Use the params parameter for primitives manager
			const QList<sPrimitiveItem> primitiveList = cPrimitives::GetListOfPrimitives(params);
			const sPrimitiveItem *oldPrimitive = nullptr;
			for (const sPrimitiveItem &primitive : primitiveList)
			{
				if (primitive.typeName != primTypeName) continue;
				// Check if this primitive's object_id matches the node's objectId
				int primObjId = params->Get<int>(primitive.Name("object_id"));
				if (primObjId == oldObjId)
				{
					oldPrimitive = &primitive;
					break;
				}
			}
			if (!oldPrimitive) continue;

			// Find next available primitive index for this type
			int newPrimIndex = cPrimitives::NewPrimitiveIndex(primTypeName, primitiveList);
			QString newPrimFullName = QString("primitive_%1_%2").arg(primTypeName).arg(newPrimIndex);
			sPrimitiveItem newPrimitive(cPrimitives::PrimitiveNameToEnum(primTypeName), newPrimIndex,
				newPrimFullName, primTypeName);

			// Initialize default parameters for the new primitive
			InitPrimitiveParams(newPrimitive, params);

			// Copy all parameters from old primitive, replacing index in param names
			QString oldPrimFullName = oldPrimitive->fullName;
			QList<QString> primParams = cPrimitives::GetListOfPrimitiveParams(*oldPrimitive, params);
			for (const QString &paramName : primParams)
			{
				if (!params->IfExists(paramName)) continue;
				QString newParamName = paramName;
				// Replace the primitive index part (after "primitive_<type>_")
				QStringList parts = paramName.split('_');
				for (int i = 2; i < parts.size(); ++i)
				{
					bool ok = false;
					parts[i].toInt(&ok);
					if (ok)
					{
						parts[i] = QString::number(newPrimIndex);
						break;
					}
				}
				newParamName = parts.join('_');
				params->SetFromOneParameter(newParamName, params->GetAsOneParameter(paramName));
			}

			// Update object_id
			params->Set(newPrimitive.Name("object_id"), newObjId);
		}
	}

	// Return the new root node ID
	return nodeIdMap.value(nodes.first().nodeId, -1);
}

// Duplicates the selected node (or its enclosing group) with all children.
void cObjectsTreeWidget::slotDuplicateObject()
{
	QList<QTreeWidgetItem *> selected = ui->treeWidget_objects->selectedItems();
	if (selected.isEmpty()) return;

	QTreeWidgetItem *selectedItem = selected.first();
	if (selectedItem == worldItem) return;

	// Synchronize current editor state before reading from gPar/gParFractal
	if (currentEditorWidget)
	{
		SynchronizeEditorWidget(currentEditorWidget, qInterface::read);
	}

	// Determine the actual root node to duplicate.
	// Always duplicate exactly what is selected (whether group or leaf).
	QTreeWidgetItem *rootNode = selectedItem;

	// Collect the subtree from parameters (not from the tree widget)
	auto nodeInfos =
		CollectSubtreeFromParams(rootNode->data(treeData::nodeId, Qt::UserRole).toInt(), params);
	if (nodeInfos.isEmpty()) return;

	// Call the static duplicate method
	int newRootNodeId = DuplicateNodesInParams(nodeInfos, params, fractalParams);
	if (newRootNodeId < 0)
	{
		qWarning() << "slotDuplicateObject: duplication failed";
		return;
	}

	// Rebuild the tree widget from the updated parameters
	lastSelectedNodeId = newRootNodeId;
	ui->treeWidget_objects->blockSignals(true);
	UpdateTree(params, fractalParams);
	ui->treeWidget_objects->expandAll();
	ui->treeWidget_objects->blockSignals(false);
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

// Creates and initialises a cFractalObject editor for the fractal identified by 'objectId'.
// Standalone fractals also get cGeneralObjectParameters and cFractalCalculationParameters;
// fractals inside a hybrid group rely on the parent hybrid node for those shared parameters.
// The fractal index is clamped to the valid range so an out-of-range objectId cannot crash.
QWidget *cObjectsTreeWidget::buildFractalEditor(int objectId, QTreeWidgetItem *item)
{
	// objectId is 1-based; fractal indices stored in fractalParams are 0-based.
	// Ensure the container has enough capacity for this index.
	const int fractalIndex = qMax(0, objectId - 1);
	if (fractalIndex >= fractalParams->size())
	{
		fractalParams->ensureCapacity(fractalIndex);
		InitFractalParams(fractalParams->at(fractalIndex));
		fractalParams->at(fractalIndex)
			->SetContainerName(QString("fractal") + QString::number(fractalIndex));
	}

	QWidget *container = new QWidget();
	QVBoxLayout *layout = new QVBoxLayout(container);
	layout->setContentsMargins(0, 0, 0, 0);

	// Formula-specific parameter editor (formula selector + formula tab)
	cFractalObject *fractalTab = new cFractalObject();
	fractalTab->AssignParameterContainers(params, fractalParams);
	fractalTab->Init(true, fractalIndex);
	fractalTab->AssignParentDockFractal(nullptr);

	connect(fractalTab,
		static_cast<void (cFractalObject::*)(int, int)>(&cFractalObject::formulaChanged), this,
		&cObjectsTreeWidget::onFormulaChanged);

	// Populate the tab with the current parameter values from gPar and gParFractal
	layout->addWidget(fractalTab);

	editorSyncTargets.clear();
	editorSyncTargets.append({fractalTab, fractalParams->at(fractalIndex)});

	if (isFractalInHybridGroup(item))
	{
		fractalTab->FrameIterationFormulaSetWidgetsVisibility(true);
	}
	else
	{
		QWidget *generalParams = buildGeneralObjectParametersEditor(item);
		layout->addWidget(generalParams);

		// Fractal calculation parameters (maxiter, julia mode, constant factor, initial w-axis).
		// For standalone fractals and fractals inside boolean groups, sync to gPar with node
		// prefix instead of per-fractal container.
		cFractalCalculationParameters *calcParams = new cFractalCalculationParameters();

		// Rename widget names to use node prefix so they bind to the correct gPar keys
		QString nodePrefix =
			QString("node_%1").arg(item->data(treeData::nodeId, Qt::UserRole).toInt(), 4, 10, QChar('0'));
		for (QWidget *widget : calcParams->findChildren<QWidget *>())
		{
			QString widgetName = widget->objectName();
			int firstUnderscore = widgetName.indexOf('_');
			if (firstUnderscore >= 0)
			{
				widgetName.insert(firstUnderscore + 1, nodePrefix + "_");
				widget->setObjectName(widgetName);
			}
		}

		editorSyncTargets.append({generalParams, params});
		editorSyncTargets.append({calcParams, params});

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
	const QList<sPrimitiveItem> primitiveList = cPrimitives::GetListOfPrimitives(params);
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
	editorSyncTargets.append({primWidget, params});
	editorSyncTargets.append({generalParams, params});

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

	hideRepeatFieldsForBooleanGroups(generalParams, getNodeType(item));

	return generalParams;
}

void cObjectsTreeWidget::hideRepeatFieldsForBooleanGroups(QWidget *widget, int nodeType)
{
	enumNodeType type = enumNodeType(nodeType);
	if (type != enumNodeType::booleanAdd && type != enumNodeType::booleanMul
			&& type != enumNodeType::booleanSub)
		return;

	for (QWidget *child : widget->findChildren<QWidget *>())
	{
		QString name = child->objectName();
		if (name.contains("repeat", Qt::CaseInsensitive)) child->setVisible(false);
	}
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

	// World sentinel item — allow selection but show no editor
	if (item == worldItem) return;

	lastSelectedNodeId = item->data(treeData::nodeId, Qt::UserRole).toInt();
	enumNodeType type = enumNodeType(item->data(treeData::nodeType, Qt::UserRole).toInt());
	int objectId = item->data(treeData::objectId, Qt::UserRole).toInt();
	int nodeId = item->data(treeData::nodeId, Qt::UserRole).toInt();

	// Wrapper widget so the whole editor area can be removed/replaced as a single unit
	QWidget *editorContainer = new QWidget();
	QVBoxLayout *containerLayout = new QVBoxLayout(editorContainer);
	containerLayout->setContentsMargins(0, 0, 0, 0);

	// Header label: tells the user which object and type they are currently editing
	containerLayout->addWidget(buildInfoLabel(item, type));

	// Build the type-specific parameter editor
	if (type == enumNodeType::fractal)
	{
		// Track the fractal index so we can save to gParFractal on the next selection change.
		// objectId is 1-based; fractal index is objectId-1 (no upper limit).
		currentFractalIndex = qMax(0, objectId - 1);

		QWidget *fractalEditor = buildFractalEditor(objectId, item);
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
		editorSyncTargets.append({generalParams, params});

		containerLayout->addWidget(generalParams);

		// Add calculation params widget for hybrid AND boolean groups
		// Boolean groups need shared julia_mode, julia_c, etc. for child fractals
		if (type == enumNodeType::hybrid || type == enumNodeType::booleanAdd
				|| type == enumNodeType::booleanMul || type == enumNodeType::booleanSub)
		{
			cFractalCalculationParameters *calcParams = new cFractalCalculationParameters();
			// Rename widget names to use node prefix so they bind to the correct gPar keys
			QString nodePrefix = QString("node_%1").arg(
				item->data(treeData::nodeId, Qt::UserRole).toInt(), 4, 10, QChar('0'));
			for (QWidget *widget : calcParams->findChildren<QWidget *>())
			{
				QString widgetName = widget->objectName();
				int firstUnderscore = widgetName.indexOf('_');
				if (firstUnderscore >= 0)
				{
					widgetName.insert(firstUnderscore + 1, nodePrefix + "_");
					widget->setObjectName(widgetName);
				}
			}
			editorSyncTargets.append({calcParams, params});
			containerLayout->addWidget(calcParams);
		}
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
	if (params->Get<bool>("ui_colorize"))
		cInterface::ColorizeGroupBoxes(this, params->Get<int>("ui_colorize_random_seed") + nodeId);
	cInterface::AdjustLayoutSpacing(this, params->Get<int>("ui_layout_spacing"));
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
	if (!treeSyncBlocked)
	{
		SynchronizeEditorWidget(currentEditorWidget, mode);
	}

	if (mode == qInterface::write && !treeSyncBlocked)
	{
		// Block itemSelectionChanged to prevent the editor from being destroyed and
		// recreated during tree reconstruction, which causes blinking and focus loss.
		ui->treeWidget_objects->blockSignals(true);
		UpdateTree(params, fractalParams);
		ui->treeWidget_objects->blockSignals(false);

		// Re-populate the existing editor widgets with the updated parameter values
		// instead of rebuilding the whole editor from scratch.
		SynchronizeEditorWidget(currentEditorWidget, qInterface::write);
	}
	else
	{
		StoreTreeToParams(params, fractalParams);
	}
}

// --- Drag-and-drop slots ---

void cObjectsTreeWidget::onDragStartRequested(int /*nodeId*/, QTreeWidgetItem *item)
{
	if (!item || item == worldItem) return;

	int nodeId = item->data(treeData::nodeId, Qt::UserRole).toInt();
	if (nodeId <= 0) return;

	cDragDropTreeWidget *treeWidget = qobject_cast<cDragDropTreeWidget *>(ui->treeWidget_objects);
	if (treeWidget)
	{
		treeWidget->setSourceNodeId(nodeId);
	}
}

void cObjectsTreeWidget::onDragMoveOverItem(
	QTreeWidgetItem *targetItem, int dropPosition, int sourceNodeId)
{
	if (!targetItem || targetItem == worldItem)
	{
		qobject_cast<cDragDropTreeWidget *>(ui->treeWidget_objects)->setDropValid(false);
		return;
	}

	// Find source item
	QTreeWidgetItem *sourceItem = nullptr;
	auto allItems = collectAllTreeItems();
	for (QTreeWidgetItem *item : allItems)
	{
		if (item->data(treeData::nodeId, Qt::UserRole).toInt() == sourceNodeId)
		{
			sourceItem = item;
			break;
		}
	}
	if (!sourceItem || sourceItem == targetItem)
	{
		qobject_cast<cDragDropTreeWidget *>(ui->treeWidget_objects)->setDropValid(false);
		return;
	}

	if (dropPosition == static_cast<int>(DropPosition::DropOnItem))
	{
		// DropOnItem: target becomes the new parent of source
		enumNodeType targetType = enumNodeType(getNodeType(targetItem));
		if (!isGroupType(targetType))
		{
			qobject_cast<cDragDropTreeWidget *>(ui->treeWidget_objects)->setDropValid(false);
			return;
		}

		// Check for cycles: source must not be an ancestor of target
		QTreeWidgetItem *parent = sourceItem->parent();
		while (parent)
		{
			if (parent == targetItem)
			{
				qobject_cast<cDragDropTreeWidget *>(ui->treeWidget_objects)->setDropValid(false);
				return;
			}
			parent = parent->parent();
		}

		// Check if dropping a primitive into a hybrid group (not allowed)
		enumNodeType sourceType = enumNodeType(getNodeType(sourceItem));
		if (sourceType == enumNodeType::primitive && targetType == enumNodeType::hybrid)
		{
			qobject_cast<cDragDropTreeWidget *>(ui->treeWidget_objects)->setDropValid(false);
			return;
		}
	}
	else
	{
		// DropAboveItem / DropBelowItem: source becomes a sibling of target
		// under the same parent (targetItem->parent()).
		// Only cycle check is needed: target must not be an ancestor of source.
		QTreeWidgetItem *parent = targetItem->parent();
		while (parent)
		{
			if (parent == sourceItem)
			{
				qobject_cast<cDragDropTreeWidget *>(ui->treeWidget_objects)->setDropValid(false);
				return;
			}
			parent = parent->parent();
		}

		// Check if the effective parent (targetItem->parent or worldItem) is a hybrid
		// and source is a primitive — in that case the primitive would end up as a child
		// of the hybrid group, which is not allowed.
		QTreeWidgetItem *effectiveParent = targetItem->parent();
		if (!effectiveParent)
		{
			effectiveParent = worldItem;
		}
		enumNodeType sourceType = enumNodeType(getNodeType(sourceItem));
		if (sourceType == enumNodeType::primitive)
		{
			enumNodeType parentType = enumNodeType(getNodeType(effectiveParent));
			if (parentType == enumNodeType::hybrid)
			{
				qobject_cast<cDragDropTreeWidget *>(ui->treeWidget_objects)->setDropValid(false);
				return;
			}
		}
	}

	qobject_cast<cDragDropTreeWidget *>(ui->treeWidget_objects)->setDropValid(true);
}

void cObjectsTreeWidget::onDropCompleted(
	int sourceNodeId, QTreeWidgetItem *targetItem, int dropPosition)
{
	if (!targetItem || targetItem == worldItem) return;

	// Find source item
	QTreeWidgetItem *sourceItem = nullptr;
	QTreeWidgetItem *oldParent = nullptr;
	int oldSiblingIndex = -1;

	auto allItems = collectAllTreeItems();
	for (QTreeWidgetItem *item : allItems)
	{
		if (item->data(treeData::nodeId, Qt::UserRole).toInt() == sourceNodeId)
		{
			sourceItem = item;
			oldParent = item->parent();
			if (oldParent)
			{
				oldSiblingIndex = oldParent->indexOfChild(item);
			}
			else
			{
				oldSiblingIndex = ui->treeWidget_objects->indexOfTopLevelItem(item);
			}
			break;
		}
	}

	if (!sourceItem) return;

	// Check for cycles
	QTreeWidgetItem *checkParent = sourceItem->parent();
	while (checkParent)
	{
		if (checkParent == targetItem) return;
		checkParent = checkParent->parent();
	}

	// Check if drop on leaf node (not allowed)
	if (dropPosition == static_cast<int>(DropPosition::DropOnItem))
	{
		enumNodeType targetType = enumNodeType(getNodeType(targetItem));
		if (!isGroupType(targetType)) return;
	}

	// Remove source from old location
	if (oldParent)
	{
		oldParent->takeChild(oldSiblingIndex);
	}
	else
	{
		ui->treeWidget_objects->takeTopLevelItem(oldSiblingIndex);
	}

	// Determine insert location based on drop position
	if (dropPosition == static_cast<int>(DropPosition::DropOnItem))
	{
		targetItem->insertChild(targetItem->childCount(), sourceItem);
	}
	else if (dropPosition == static_cast<int>(DropPosition::DropAboveItem))
	{
		QTreeWidgetItem *parent = targetItem->parent();
		if (parent)
		{
			parent->insertChild(parent->indexOfChild(targetItem), sourceItem);
		}
		else
		{
			ui->treeWidget_objects->insertTopLevelItem(
				ui->treeWidget_objects->indexOfTopLevelItem(targetItem), sourceItem);
		}
	}
	else if (dropPosition == static_cast<int>(DropPosition::DropBelowItem))
	{
		QTreeWidgetItem *parent = targetItem->parent();
		if (parent)
		{
			int insertIndex = parent->indexOfChild(targetItem) + 1;
			parent->insertChild(insertIndex, sourceItem);
		}
		else
		{
			int insertIndex = ui->treeWidget_objects->indexOfTopLevelItem(targetItem) + 1;
			ui->treeWidget_objects->insertTopLevelItem(insertIndex, sourceItem);
		}
	}
	else
	{
		ui->treeWidget_objects->addTopLevelItem(sourceItem);
	}

	// Update parentId in params
	QTreeWidgetItem *parentForUpdate = sourceItem->parent();
	int newParentId =
		parentForUpdate ? parentForUpdate->data(treeData::nodeId, Qt::UserRole).toInt() : 0;

	QString prefix = QString("node_%1_").arg(sourceNodeId, 4, 10, QChar('0'));
	QString defParam = prefix + "definition";

	if (params->IfExists(defParam))
	{
		QString currentDef = params->Get<QString>(defParam);
		QStringList parts = currentDef.split(',');
		if (parts.size() >= 5)
		{
			QString namePart = parts[0];
			QString nodeIdPart = parts[1];
			QString typePart = parts[2];
			QString objectIdPart = parts[4];

			QString newDef = QString("%1,%2,%3,%4,%5")
												 .arg(namePart)
												 .arg(nodeIdPart)
												 .arg(typePart)
												 .arg(newParentId)
												 .arg(objectIdPart);
			params->Set(defParam, newDef);
		}
	}

	ui->treeWidget_objects->expandAll();
	ui->treeWidget_objects->setCurrentItem(sourceItem);
}

void cObjectsTreeWidget::onTreeStructureChanged()
{
	// First store the current tree structure (with updated parentId and displayOrder)
	StoreTreeToParams(params, fractalParams);
	// Then rebuild the tree from params
	UpdateTree(params, fractalParams);
}
void cObjectsTreeWidget::onFormulaChanged(int fractalIndex, int formulaEnum)
{
	if (fractalIndex < 0 || fractalIndex >= fractalParams->size()) return;

	int matchedObjectId = -1;
	QString formulaName;
	for (cAbstractFractal *fractal : newFractalList)
	{
		if (int(fractal->getInternalId()) == formulaEnum)
		{
			matchedObjectId = fractalIndex + 1;
			formulaName = fractal->getNameInComboBox();
			break;
		}
	}

	if (matchedObjectId < 0) return;

	for (QTreeWidgetItem *item : collectAllTreeItems())
	{
		int objectId = item->data(treeData::objectId, Qt::UserRole).toInt();
		if (enumNodeType(getNodeType(item)) == enumNodeType::fractal && objectId == matchedObjectId)
		{
			updateFractalIcon(item, objectId, formulaEnum);
			QString currentName = item->text(treeCol::name);
			qDebug() << "onFormulaChanged currentName" << currentName << "formulaName" << formulaName;
			item->setText(treeCol::name, formulaName);
			qDebug() << "  -> name changed to" << formulaName;
			ui->treeWidget_objects->viewport()->update();
			break;
		}
	}
}

void cObjectsTreeWidget::onCustomContextMenu(const QPoint &pos)
{
	QTreeWidgetItem *item = ui->treeWidget_objects->itemAt(pos);
	if (!item || item == worldItem) return;

	QMenu menu;
	QAction *renameAction = menu.addAction(tr("Rename"));
	QAction *selected = menu.exec(ui->treeWidget_objects->viewport()->mapToGlobal(pos));
	if (selected != renameAction) return;

	QString currentName = item->text(treeCol::name);
	bool ok = false;
	QString newName =
		QInputDialog::getText(this, tr("Rename"), tr("New name:"), QLineEdit::Normal, currentName, &ok);
	if (ok && !newName.isEmpty() && newName != currentName)
	{
		int nodeId = item->data(treeData::nodeId, Qt::UserRole).toInt();
		QString prefix = QString("node_%1").arg(nodeId, 4, 10, QChar('0'));
		QString def = params->Get<QString>(prefix + "definition");
		QStringList parts = def.split(',');
		if (parts.size() >= 6)
		{
			parts[0] = newName;
			params->Set<QString>(prefix + "definition", parts.join(','));
		}
		item->setText(treeCol::name, newName);

		QWidget *editor = ui->treeWidget_objects->itemWidget(item, treeCol::type);
		if (editor)
		{
			ui->treeWidget_objects->removeItemWidget(item, treeCol::type);
			delete editor;
		}
		QWidget *matEditor = ui->treeWidget_objects->itemWidget(item, treeCol::material);
		if (matEditor)
		{
			ui->treeWidget_objects->removeItemWidget(item, treeCol::material);
			delete matEditor;
		}
		int nodeType = getNodeType(item);
		ui->treeWidget_objects->setItemWidget(item, treeCol::type, buildTypeLabel(nodeType));
		attachMaterialWidget(item, nodeId, params);
	}
}
