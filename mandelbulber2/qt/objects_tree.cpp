/*
 * objects_tree.cpp
 *
 *  Created on: 22 lis 2025
 *      Author: krzysztof
 */

#include "objects_tree.h"
#include "ui_objects_tree_widget.h"

cObjectsTree::cObjectsTree(QWidget *parent) : QWidget(parent), ui(new Ui::cObjectsTree)
{
	ui->setupUi(this);
}

cObjectsTree::~cObjectsTree()
{
	delete ui;
}
