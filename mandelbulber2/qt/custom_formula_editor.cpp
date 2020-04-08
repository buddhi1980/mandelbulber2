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
			QRegularExpressionMatch match = regex.match(code);

			code.replace(regex, "REAL4 CustomIteration(REAL4 z");
			ui->textEdit_formula_code->setText(code);
			file.close();
		}
	}
}
