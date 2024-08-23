/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2024 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * TODO: description
 */

#include "script_dialog.h"

#include <QAbstractItemView>
#include <QCompleter>
#include <QDebug>

#include "ui_script_dialog.h"

#include "src/container_selector.hpp"
#include "src/fractal_container.hpp"
#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/one_parameter.hpp"
#include "src/scripts.h"
#include "src/synchronize_interface.hpp"

cScriptDialog::cScriptDialog(QWidget *parent) : QDialog(parent), ui(new Ui::cScriptDialog)
{
	ui->setupUi(this);
	setModal(false);

	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::read);

	connect(this, &cScriptDialog::accepted, this, &cScriptDialog::slotAccepted);
	connect(ui->pushButton_Test, &QPushButton::clicked, this, &cScriptDialog::slotTest);
	connect(ui->pushButton_insertParameter, &QPushButton::clicked, this,
		&cScriptDialog::slotInsertParameter);
	connect(ui->pushButton_delete, &QPushButton::clicked, this, &cScriptDialog::slotDeleteScript);
	connect(ui->comboBox_container, qOverload<int>(&QComboBox::currentIndexChanged), this,
		&cScriptDialog::slotPopulateComboWithParameters);
	connect(ui->lineEdit_script, &QLineEdit::cursorPositionChanged, this,
		&cScriptDialog::slotCursorPositionChanged);

	ui->comboBox_container->addItem("Last used", QVariant(QString("last")));
	ui->comboBox_container->addItem("Modified", QVariant(QString("modified")));
	ui->comboBox_container->addItem("Main parameters", QVariant(QString("main")));
	for (int i = 1; i <= NUMBER_OF_FRACTALS; i++)
	{
		ui->comboBox_container->addItem(
			QString("Fractal #%1").arg(i), QVariant(QString("fractal%1").arg(i - 1)));
	}
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
		parameterContainer::ContainerSelectorByContainerName(containerName, gPar, gParFractal);
	cOneParameter parameter = container->GetAsOneParameter(parameterName);
	QString script = parameter.GetScript();
	ui->lineEdit_script->setText(script);
	setWindowTitle(tr("Edit script for parameter '%1_%2'").arg(containerName).arg(parameterName));

	if (script.isEmpty())
	{
		if (parameter.GetValueType() == typeVector3)
		{
			ui->lineEdit_script->setText("x: y: z:");
		}
		else if (parameter.GetValueType() == typeVector4)
		{
			ui->lineEdit_script->setText("x: y: z: w:");
		}
	}
}

void cScriptDialog::slotAccepted()
{
	const std::shared_ptr<cParameterContainer> container =
		parameterContainer::ContainerSelectorByContainerName(containerName, gPar, gParFractal);

	cOneParameter parameter = container->GetAsOneParameter(parameterName);
	QString script = ui->lineEdit_script->text();
	parameter.SetScript(script);
	container->SetFromOneParameter(parameterName, parameter);

	cOneParameter oneParameter = container->GetAsOneParameter(parameterName);
	QString error;
	QString evaluation;
	cScripts::EvaluateParameter(gPar, gParFractal, parameterName, oneParameter, error, evaluation);
	container->SetFromOneParameter(parameterName, oneParameter);
	if (parentWidget) SynchronizeInterfaceWindow(parentWidget, container, qInterface::write);
}

void cScriptDialog::slotTest()
{
	std::shared_ptr<cParameterContainer> paramsCopy;
	std::shared_ptr<cFractalContainer> fractalParamsCopy;
	paramsCopy.reset(new cParameterContainer());
	fractalParamsCopy.reset(new cFractalContainer());

	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::read);

	*paramsCopy = *gPar;
	*fractalParamsCopy = *gParFractal;

	const std::shared_ptr<cParameterContainer> container =
		parameterContainer::ContainerSelectorByContainerName(
			containerName, paramsCopy, fractalParamsCopy);

	container->SetScript(parameterName, ui->lineEdit_script->text());
	cOneParameter oneParameter = container->GetAsOneParameter(parameterName);

	QString error;
	QString evaluation;
	cScripts::EvaluateParameter(
		paramsCopy, fractalParamsCopy, parameterName, oneParameter, error, evaluation);

	if (error.isEmpty())
	{
		QString value = container->Get<QString>(parameterName);
		ui->textBrowser_result->setText(
			QString("evaluated value: %1\n\nEvaluation steps:\n%2").arg(value).arg(evaluation));
	}
	else
	{
		ui->textBrowser_result->setText(
			QString("Error: %1\n\nEvaluation steps: %2").arg(error).arg(evaluation));
	}
}

void cScriptDialog::slotInsertParameter()
{
	QString parameter;

	if (ui->comboBox_parameter->currentIndex() >= 0)
	{
		parameter = ui->comboBox_parameter->currentText();
	}
	if (parameter.startsWith("main_"))
	{
		parameter = parameter.mid(5);
	}

	qDebug() << parameter << lastCursorPosition;

	QString script = ui->lineEdit_script->text();
	script.insert(lastCursorPosition, QString("'%1'").arg(parameter));
	ui->lineEdit_script->setText(script);
	ui->lineEdit_script->setCursorPosition(lastCursorPosition + parameter.length() + 2);
	ui->lineEdit_script->setFocus();

	QStringList listOfLastUsed = gPar->Get<QString>("script_last_used_parameters").split(' ');
	if (!listOfLastUsed.contains(parameter))
	{
		listOfLastUsed.insert(0, parameter);
		if (listOfLastUsed.length() > 20)
		{
			listOfLastUsed.removeLast();
		}

		QString newList = listOfLastUsed.join(' ');
		gPar->Set("script_last_used_parameters", newList);

		QString selectedContainer = ui->comboBox_container->currentData().toString();
		if (selectedContainer == "last")
		{
			ui->comboBox_parameter->clear();
			for (const QString &parName : listOfLastUsed)
			{
				ui->comboBox_parameter->addItem(parName);
			}
		}
	}
}

void cScriptDialog::slotPopulateComboWithParameters()
{
	ui->comboBox_parameter->view()->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
	ui->comboBox_parameter->clear();

	QString selectedContainer = ui->comboBox_container->currentData().toString();
	if (selectedContainer == "last")
	{
		QStringList split = gPar->Get<QString>("script_last_used_parameters").split(' ');
		for (const QString &parName : split)
		{
			ui->comboBox_parameter->addItem(parName);
		}
	}
	else if (selectedContainer == "modified")
	{
		QStringList listOfAllModifiedParameters;

		QList<QString> listOfMainParameters = gPar->GetListOfParameters();
		for (const QString &parName : listOfMainParameters)
		{
			if (!gPar->isDefaultValue(parName) && gPar->GetParameterType(parName) != paramApp)
			{
				listOfAllModifiedParameters.append(gPar->GetContainerName() + "_" + parName);
			}
		}
		for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
		{
			QList<QString> listOfFractalParameters = gParFractal->at(i)->GetListOfParameters();
			for (const QString &parName : listOfFractalParameters)
			{
				if (!gParFractal->at(i)->isDefaultValue(parName)
						&& gParFractal->at(i)->GetParameterType(parName) != paramApp)
				{
					listOfAllModifiedParameters.append(
						gParFractal->at(i)->GetContainerName() + "_" + parName);
				}
			}
		}

		for (const QString &parName : listOfAllModifiedParameters)
		{
			ui->comboBox_parameter->addItem(parName);
		}
	}
	else
	{
		const std::shared_ptr<cParameterContainer> container =
			parameterContainer::ContainerSelectorByContainerName(selectedContainer, gPar, gParFractal);
		QList<QString> listOfMainParameters = container->GetListOfParameters();
		for (const QString &parName : listOfMainParameters)
		{
			if (container->GetParameterType(parName) != paramApp && !parName.startsWith("animsound"))
			{
				ui->comboBox_parameter->addItem(selectedContainer + "_" + parName);
			}
		}
	}
}

void cScriptDialog::slotCursorPositionChanged(int oldPos, int newPos)
{
	Q_UNUSED(oldPos)
	lastCursorPosition = newPos;
}

void cScriptDialog::slotDeleteScript()
{
	const std::shared_ptr<cParameterContainer> container =
		parameterContainer::ContainerSelectorByContainerName(containerName, gPar, gParFractal);
	container->SetScript(parameterName, QString());

	if (parentWidget) SynchronizeInterfaceWindow(parentWidget, container, qInterface::write);
	close();
}
