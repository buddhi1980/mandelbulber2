/*
 * script_dialog.cpp
 *
 *  Created on: 25 lis 2023
 *      Author: krzysztof
 */

#include "script_dialog.h"
#include "ui_script_dialog.h"

#include "src/container_selector.hpp"
#include "src/initparameters.hpp"
#include "src/fractal_container.hpp"
#include "src/one_parameter.hpp"
#include <QDebug>

cScriptDialog::cScriptDialog(QWidget *parent) : QDialog(parent), ui(new Ui::cScriptDialog)
{
	ui->setupUi(this);
	setModal(false);
	connect(this, &cScriptDialog::accepted, this, &cScriptDialog::slotAccepted);
}

cScriptDialog::~cScriptDialog()
{
	delete ui;
}

void cScriptDialog::closeEvent(QCloseEvent *event) {}

void cScriptDialog::AssignParameterName(
	const QString &_parameterName, const QString &_containerName)
{
	parameterName = _parameterName;
	containerName = _containerName;

	const std::shared_ptr<cParameterContainer> container =
		parameterContainer::ContainerSelector(containerName + "_" + parameterName, gPar, gParFractal);
	cOneParameter parameter = container->GetAsOneParameter(parameterName);
	QString script = parameter.GetScript();
	ui->lineEdit_script->setText(script);
}

void cScriptDialog::slotAccepted()
{

	const std::shared_ptr<cParameterContainer> container =
		parameterContainer::ContainerSelector(containerName + "_" + parameterName, gPar, gParFractal);

	cOneParameter parameter = container->GetAsOneParameter(parameterName);
	QString script = ui->lineEdit_script->text();
	parameter.SetScript(script);

	container->SetFromOneParameter(parameterName, parameter);
}
