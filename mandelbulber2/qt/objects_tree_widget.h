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

#include "../src/algebra.hpp"
#include "src/object_node_type.h"

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

private:
	void pressedRefreshButton();

	QList<QTreeWidgetItem *> collectAllTreeItems() const;
	int getNodeType(QTreeWidgetItem *item) const;
	bool isFractalInHybridGroup(QTreeWidgetItem *item) const;
	static CVector3 parsePosition(const QString &text);
	static QString formatPosition(const CVector3 &pos);
	QComboBox *buildTypeComboBox(int currentType);
	void addOrSetParam(
		std::shared_ptr<cParameterContainer> params, const QString &name, const QString &value);
	void addOrSetParam(
		std::shared_ptr<cParameterContainer> params, const QString &name, const CVector3 &value);

	QLabel *buildInfoLabel(QTreeWidgetItem *item, enumNodeType type);
	QWidget *buildFractalEditor(int objectId, bool isHybrid);
	QWidget *buildPrimitiveEditor(QTreeWidgetItem *item, int objectId);

	// Inserts 'prefix' into every immediate and nested child widget name of 'parent'
	// immediately after the first '_', so that SynchronizeInterfaceWindow can map the
	// widget to the correctly namespaced parameter (e.g. "vect3_position_x" with prefix
	// "formula_" becomes "vect3_formula_position_x").
	static void renameWidgetsWithPrefix(QWidget *parent, const QString &prefix);

private slots:
	void onItemChanged(QTreeWidgetItem *item, int column);
	void slotAddObject();
	void slotDeleteObject();
	void slotItemSelectionChanged();

private:
	Ui::cObjectsTreeWidget *ui;

	QWidget *currentEditorWidget = nullptr;
	QVBoxLayout *editorLayout = nullptr; // layout below the tree for dynamic editors
	int currentFractalIndex =
		-1; // index into gParFractal for the currently edited fractal (-1 if none)
};

#endif /* MANDELBULBER2_QT_OBJECTS_TREE_WIDGET_H_ */
