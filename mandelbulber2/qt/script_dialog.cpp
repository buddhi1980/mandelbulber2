/*
 * script_dialog.cpp
 *
 *  Created on: 25 lis 2023
 *      Author: krzysztof
 */

#include "script_dialog.h"
#include "ui_script_dialog.h"
#include "src/initparameters.hpp"

cScriptDialog::cScriptDialog(QWidget *parent) : QDialog(parent), ui(new Ui::cScriptDialog)
{

	ui->setupUi(this);
}

cScriptDialog::~cScriptDialog()
{
	delete ui;
}

void cScriptDialog::closeEvent(QCloseEvent *event) {}

void AssignParameterName(const QString &parameter, const QString containerName) {}
