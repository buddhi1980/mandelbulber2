/*
 * randomizer_dialog.cpp
 *
 *  Created on: 29 gru 2019
 *      Author: krzysztof
 */

#include "randomizer_dialog.h"
#include "ui_randomizer_dialog.h"

cRandomizerDialog::cRandomizerDialog(QWidget *parent)
		: QDialog(parent), ui(new Ui::cRandomizerDialog)
{
	ui->setupUi(this);
}

cRandomizerDialog::~cRandomizerDialog()
{
	delete ui;
}
