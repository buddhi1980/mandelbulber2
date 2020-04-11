/*
 * custom_formula_editor.cpp
 *
 *  Created on: 19 mar 2020
 *      Author: krzysztof
 */

#include "custom_formula_editor.h"
#include "ui_custom_formula_editor.h"
#include "src/system.hpp"

#include <QFileDialog>
#include <QTextStream>
#include <QDebug>

cCustomFormulaEditor::cCustomFormulaEditor(QWidget *parent)
		: QWidget(parent), ui(new Ui::cCustomFormulaEditor)
{
	ui->setupUi(this);

	connect(ui->pushButton_new, &QPushButton::pressed, this, &cCustomFormulaEditor::slotNewFormula);
	connect(ui->textEdit_formula_code, &QTextEdit::textChanged, this,
		&cCustomFormulaEditor::slotTextChanged);
	connect(ui->pushButton_load_builtin, &QPushButton::pressed, this,
		&cCustomFormulaEditor::slotLoadBuiltIn);
	connect(ui->pushButton_check_syntax, &QPushButton::pressed, this,
		&cCustomFormulaEditor::slotCheckSyntax);
}

cCustomFormulaEditor::~cCustomFormulaEditor()
{
	delete ui;
}

void cCustomFormulaEditor::slotNewFormula()
{
	QString code =
		"REAL4 CustomIteration(REAL4 z, __constant sFractalCl *fractal, sExtendedAuxCl *aux)\n"
		"{\n"
		"\treturn z;\n"
		"}\n";

	ui->textEdit_formula_code->setText(code);
}

void cCustomFormulaEditor::slotTextChanged()
{
	ui->textEdit_formula_code->setMinimumHeight(
		ui->textEdit_formula_code->document()->size().height());
}

void cCustomFormulaEditor::slotLoadBuiltIn()
{
	QFileDialog dialog(this);
	dialog.setOption(QFileDialog::DontUseNativeDialog);
	dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setNameFilter(tr("OpenCL programs (*.cl)"));
	dialog.setDirectory(
		QDir::toNativeSeparators(systemData.sharedDir + "formula" + QDir::separator() + "opencl"));
	dialog.setAcceptMode(QFileDialog::AcceptOpen);
	dialog.setWindowTitle(tr("Load existing fractal formula code..."));
	QStringList filenames;
	if (dialog.exec())
	{
		filenames = dialog.selectedFiles();
		QString filename = QDir::toNativeSeparators(filenames.first());
		QFile file(filename);
		if (file.open(QFile::ReadOnly | QFile::Text))
		{
			QTextStream stream(&file);
			QString code = stream.readAll();
			code.replace(" * D O    N O T    E D I T    T H I S    F I L E !",
				" * F E E L   F R E E    T O    E D I T    T H I S    F I L E !");
			QRegularExpression regex("REAL4\\ (.*?)\\(REAL4\\ z");
			code.replace(regex, "REAL4 CustomIteration(REAL4 z");
			ui->textEdit_formula_code->setText(code);
			file.close();
		}
	}
}

void cCustomFormulaEditor::slotCheckSyntax()
{
	// create list of parameters used in the code
	QStringList parametersInCode = CreateListOfParametersInCode();

	QList<sParameterDesctiption> list = ConvertListOfParameters(parametersInCode);
}

QStringList cCustomFormulaEditor::CreateListOfParametersInCode()
{
	QString code = ui->textEdit_formula_code->toPlainText();
	QRegularExpression regex("fractal->(.*?)[^a-zA-Z0-9_.]");

	QStringList listOfFoundParameters;

	bool found = false;
	int lastPosition = 0;
	do
	{
		QRegularExpressionMatch match = regex.match(code, lastPosition);
		found = match.hasMatch();
		if (found)
		{
			lastPosition = match.capturedEnd();
			listOfFoundParameters.append(match.captured(1));
		}
	} while (found);

	listOfFoundParameters.removeDuplicates();
	return listOfFoundParameters;
}

QList<cCustomFormulaEditor::sParameterDesctiption> cCustomFormulaEditor::ConvertListOfParameters(
	const QStringList &inputList)
{
	QString fileName(systemData.sharedDir + "data/parameterNames.txt");
	QFile file(fileName);
	QStringList conversionList;
	if (file.open(QIODevice::ReadOnly | QIODevice::Text))
	{

		QTextStream textStream(&file);

		while (!textStream.atEnd())
			conversionList << textStream.readLine();

		file.close();
	}
	else
	{
		qCritical()
			<< "cCustomFormulaEditor::ConvertListOfParameters(): cannot open file with parameter names!";
	}

	QMap<QString, QString> conversionTable;
	{
		for (int i = 0; i < conversionList.size(); i++)
		{
			if (conversionList[i].length() > 0)
			{
				QStringList split = conversionList[i].split(" ");
				if (split.length() == 3)
				{
					conversionTable.insert(split[0], split[2]);
				}
				else
				{
					qCritical()
						<< "cCustomFormulaEditor::ConvertListOfParameters(): error in conversion list: "
						<< conversionList[i];
				}
			}
		}
	}

	QList<sParameterDesctiption> list;

	for (int i = 0; i < inputList.size(); i++)
	{
		QString sourceName = inputList[i];
		if (sourceName.endsWith(".x") || sourceName.endsWith(".y") || sourceName.endsWith(".z")
				|| sourceName.endsWith(".w"))
		{
			sourceName = sourceName.left(sourceName.length() - 2);
		}

		if (conversionTable.contains(sourceName))
		{
			QString parameterName = conversionTable[sourceName];
			qDebug() << inputList[i] << parameterName;
		}
		else
		{
			qWarning() << "cCustomFormulaEditor::ConvertListOfParameters(): Unknown parameter: "
								 << sourceName;
		}
	}

	return list;
}
