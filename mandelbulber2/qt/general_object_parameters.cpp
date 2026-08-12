/*
 * general_object_parameters.cpp
 *
 *  Created on: 19 kwi 2026
 *      Author: krzysztof
 */

#include "general_object_parameters.h"
#include "ui_general_object_parameters.h"

cGeneralObjectParameters::cGeneralObjectParameters(QWidget *parent)
		: QWidget(parent), cMyWidgetWithParams(), ui(new Ui::cGeneralObjectParameters)
{
	ui->setupUi(this);
}

cGeneralObjectParameters::~cGeneralObjectParameters()
{
	delete ui;
}
