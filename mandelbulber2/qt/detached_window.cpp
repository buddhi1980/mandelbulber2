/*
 * detached_window.cpp
 *
 *  Created on: 25 lut 2017
 *      Author: krzysztof
 */

#include "detached_window.h"
#include "ui_detached_window.h"

cDetachedWindow::cDetachedWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::cDetachedWindow)
{
	ui->setupUi(this);
	setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
	statusBar()->hide();
}

cDetachedWindow::~cDetachedWindow()
{
	delete ui;
}

void cDetachedWindow::InstallImageWidget(QWidget *widget)
{
	ui->verticalLayout->addWidget(widget);
}
