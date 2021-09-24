/*
 * navigator_window.cpp
 *
 *  Created on: 24 wrz 2021
 *      Author: krzysztof
 */

#include "navigator_window.h"

#include "ui_navigator_window.h"

cNavigatorWindow::cNavigatorWindow(QWidget *parent) : QDialog(parent), ui(new Ui::cNavigatorWindow)
{
	ui->setupUi(this);

	setModal(false);
}

cNavigatorWindow::~cNavigatorWindow()
{
	delete ui;
}
