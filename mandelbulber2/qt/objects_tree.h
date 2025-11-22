/*
 * objects_tree.h
 *
 *  Created on: 22 lis 2025
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_OBJECTS_TREE_H_
#define MANDELBULBER2_QT_OBJECTS_TREE_H_

#include <QWidget>
#include <memory>

class cParameterContainer;
class cFractalContainer;

namespace Ui
{
class cObjectsTree;
}

class cObjectsTree : public QWidget
{
	Q_OBJECT

public:
	explicit cObjectsTree(QWidget *parent = nullptr);
	~cObjectsTree() override;

	void UpdateTree(
		std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> fractalParams);

private:
	void pressedRefreshButton();

	Ui::cObjectsTree *ui;
};

#endif /* MANDELBULBER2_QT_OBJECTS_TREE_H_ */
