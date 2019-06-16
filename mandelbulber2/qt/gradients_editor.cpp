/*
 * gradients_editor.cpp
 *
 *  Created on: 16 cze 2019
 *      Author: krzysztof
 */

#include "ui_gradients_editor.h"

#include "gradients_editor.h"

cGradientsEditor::cGradientsEditor(QWidget *parent) : QWidget(parent), ui(new Ui::cGradientsEditor)
{
	ui->setupUi(this);
}

cGradientsEditor::~cGradientsEditor()
{
	delete ui;
}
