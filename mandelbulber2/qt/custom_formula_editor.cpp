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
}

cCustomFormulaEditor::~cCustomFormulaEditor()
{
	delete ui;
}
