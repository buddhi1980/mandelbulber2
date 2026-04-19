/*
 * fractal_calculation_parameters.cpp
 *
 *  Created on: 19 kwi 2026
 *      Author: krzysztof
 */

#include "fractal_calculation_parameters.h"
#include "ui_fractal_calculation_parameters.h"

cFractalCalculationParameters::cFractalCalculationParameters(QWidget *parent)
		: QWidget(parent), cMyWidgetWithParams(), ui(new Ui::cFractalCalculationParameters)
{
	ui->setupUi(this);
}
