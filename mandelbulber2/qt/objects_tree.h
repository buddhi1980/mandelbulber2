/*
 * objects_tree.h
 *
 *  Created on: 22 lis 2025
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_OBJECTS_TREE_H_
#define MANDELBULBER2_QT_OBJECTS_TREE_H_

#include <QWidget>

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

private:
	Ui::cObjectsTree *ui;
};

#endif /* MANDELBULBER2_QT_OBJECTS_TREE_H_ */
