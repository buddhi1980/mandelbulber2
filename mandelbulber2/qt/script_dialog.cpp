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
#include "src/scripts.h"
#include <QDebug>

cScriptDialog::cScriptDialog(QWidget *parent) : QDialog(parent), ui(new Ui::cScriptDialog)
{
	ui->setupUi(this);
	setModal(false);
	connect(this, &cScriptDialog::accepted, this, &cScriptDialog::slotAccepted);
	connect(ui->pushButton_Test, &QPushButton::clicked, this, &cScriptDialog::slotTest);
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
	setWindowTitle(tr("Edti script for parameter '%1_%2'").arg(containerName).arg(parameterName));
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

void cScriptDialog::slotTest()
{
	std::shared_ptr<cParameterContainer> paramsCopy;
	std::shared_ptr<cFractalContainer> fractalParamsCopy;
	paramsCopy.reset(new cParameterContainer());
	fractalParamsCopy.reset(new cFractalContainer());

	*paramsCopy = *gPar;
	*fractalParamsCopy = *gParFractal;

	const std::shared_ptr<cParameterContainer> container = parameterContainer::ContainerSelector(
		containerName + "_" + parameterName, paramsCopy, fractalParamsCopy);

	container->SetScript(parameterName, ui->lineEdit_script->text());

	QString error = cScripts::EvaluateAll(paramsCopy, fractalParamsCopy);

	if (error.isEmpty())
	{
		QString value = container->Get<QString>(parameterName);
		ui->textBrowser_result->setText(value);
	}
	else
	{
		ui->textBrowser_result->setText(error);
	}
}
