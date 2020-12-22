/*
 * light_editor.cpp
 *
 *  Created on: 22 gru 2020
 *      Author: krzysztof
 */

#include "light_editor.h"
#include "ui_light_editor.h"
#include "src/automated_widgets.hpp"

cLightEditor::cLightEditor(QWidget *parent) : QWidget(parent), ui(new Ui::cLightEditor)
{
	ui->setupUi(this);

	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
}

cLightEditor::~cLightEditor()
{
	delete ui;
}
