/*
 * material_editor.cpp
 *
 *  Created on: 3 maj 2016
 *      Author: krzysztof
 */

#include "material_editor.h"
#include "ui_material_editor.h"
#include <QtCore>

cMaterialEditor::cMaterialEditor(QWidget *parent) :
		QWidget(parent), ui(new Ui::cMaterialEditor)
{
	ui->setupUi(this);
}

cMaterialEditor::~cMaterialEditor()
{
	delete ui;
}

