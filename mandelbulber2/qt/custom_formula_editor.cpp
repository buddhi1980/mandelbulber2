/*
 * custom_formula_editor.cpp
 *
 *  Created on: 19 mar 2020
 *      Author: krzysztof
 */

#include "custom_formula_editor.h"
#include "ui_custom_formula_editor.h"

cCustomFormulaEditor::cCustomFormulaEditor(QWidget *parent)
		: QWidget(parent), ui(new Ui::cCustomFormulaEditor)
{
	ui->setupUi(this);

	connect(ui->pushButton_new, &QPushButton::pressed, this, &cCustomFormulaEditor::slotNewFormula);
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
