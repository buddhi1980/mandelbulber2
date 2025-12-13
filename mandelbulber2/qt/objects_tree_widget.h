/*
 * objects_tree.h
 *
 *  Created on: 22 lis 2025
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_OBJECTS_TREE_WIDGET_H_
#define MANDELBULBER2_QT_OBJECTS_TREE_WIDGET_H_

#include <QWidget>
#include <memory>

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
	static QString nodeTypeToString(enumNodeType type);

private:
	void pressedRefreshButton();

	Ui::cObjectsTreeWidget *ui;
};

#endif /* MANDELBULBER2_QT_OBJECTS_TREE_WIDGET_H_ */
