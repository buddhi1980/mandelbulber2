/*
 * objects_tree.h
 *
 *  Created on: 22 lis 2025
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_OBJECTS_TREE_WIDGET_H_
#define MANDELBULBER2_QT_OBJECTS_TREE_WIDGET_H_

#include <QWidget>
#include <QComboBox>
#include <QLabel>
#include <QGroupBox>
#include <QTreeWidgetItem>
#include <QVBoxLayout>
#include <memory>

#include "src/algebra.hpp"
#include "src/synchronize_interface.hpp"
#include "src/object_node_type.h"

// Forward declaration – full definition is only needed in the .cpp
class cMaterialWidget;

class cParameterContainer;
class cFractalContainer;

namespace Ui
{
class cObjectsTreeWidget;
}

class cObjectsTreeWidget : public QWidget
{
	Q_OBJECT

public:
	explicit cObjectsTreeWidget(QWidget *parent = nullptr);
	~cObjectsTreeWidget() override;

	void UpdateTree(
		std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractalParams);
	void StoreTreeToParams(
		std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractalParams);

	static QString nodeTypeToString(enumNodeType type);

	void SynchronizeInterface(std::shared_ptr<cParameterContainer> params,
		std::shared_ptr<cFractalContainer> fractalParams, qInterface::enumReadWrite mode);

private:
	// Column indices for setText() / text() / itemWidget()
	struct treeCol
	{
		static constexpr int name = 0;		 // display name string
		static constexpr int type = 1;		 // type string + combo box
		static constexpr int objectId = 2; // object ID string
		static constexpr int material = 3; // miniature cMaterialWidget thumbnail
	};

	// Column indices for setData() / data(col, Qt::UserRole)
	struct treeData
	{
		static constexpr int nodeId = 0;			 // int: node ID
		static constexpr int nodeType = 1;		 // int: enumNodeType
		static constexpr int objectId = 2;		 // int: object ID
		static constexpr int primTypeName = 3; // QString: e.g. "box", "sphere"
	};

	void pressedRefreshButton();

	QList<QTreeWidgetItem *> collectAllTreeItems() const;
	int getNodeType(QTreeWidgetItem *item) const;
	QComboBox *buildTypeComboBox(int currentType);
	void attachMaterialWidget(
		QTreeWidgetItem *item, int nodeId, std::shared_ptr<cParameterContainer> params);
	int findNextAvailableNodeId() const;
	int findNextAvailableFractalObjectId() const;
	int findNextAvailablePrimitiveObjectId() const;
	int findNextAvailableGroupObjectId() const;
	QTreeWidgetItem *selectedGroupTarget() const;
	static bool isGroupType(enumNodeType type);
	enumNodeType showGroupSelectionDialog(bool *ok);
	QString showPrimitiveSelectionDialog(bool *ok);
	QTreeWidgetItem *createNodeItem(int nodeId, enumNodeType nodeType, int objectId,
		const QString &name, const QString &primTypeName);
	void addNodeToSelectedGroup(QTreeWidgetItem *newItem);
	void ensureNodeParamsExist(int nodeId);
	void addOrSetParam(
		std::shared_ptr<cParameterContainer> params, const QString &name, const QString &value);
	void addOrSetParam(
		std::shared_ptr<cParameterContainer> params, const QString &name, const CVector3 &value);
	void addOrSetParam(
		std::shared_ptr<cParameterContainer> params, const QString &name, double value);

	QLabel *buildInfoLabel(QTreeWidgetItem *item, enumNodeType type);
	QWidget *buildFractalEditor(int objectId, QTreeWidgetItem *item);
	QWidget *buildPrimitiveEditor(QTreeWidgetItem *item, int objectId);
	QWidget *buildGeneralObjectParametersEditor(QTreeWidgetItem *item);

	// Inserts 'prefix' into every immediate and nested child widget name of 'parent'
	// immediately after the first '_', so that SynchronizeInterfaceWindow can map the
	// widget to the correctly namespaced parameter (e.g. "vect3_position_x" with prefix
	// "formula_" becomes "vect3_formula_position_x").
	static void renameWidgetsWithPrefix(QWidget *parent, const QString &prefix);

	void SynchronizeEditorWidget(QWidget *widget, qInterface::enumReadWrite mode);

private slots:
	void onItemChanged(QTreeWidgetItem *item, int column);
	void slotAddGroup();
	void slotAddFractal();
	void slotAddPrimitive();
	void slotDeleteObject();
	void slotItemSelectionChanged();

private:
	Ui::cObjectsTreeWidget *ui;

	QTreeWidgetItem *worldItem = nullptr;

	QWidget *currentEditorWidget = nullptr;
	QVBoxLayout *editorLayout = nullptr; // layout below the tree for dynamic editors
	int lastSelectedNodeId = -1;
	int currentFractalIndex =
		-1; // index into gParFractal for the currently edited fractal (-1 if none)

	QList<QPair<QWidget *, std::shared_ptr<cParameterContainer>>> editorSyncTargets;
};

#endif /* MANDELBULBER2_QT_OBJECTS_TREE_WIDGET_H_ */
