/*
 * fractal_calculation_parameters.h
 *
 *  Created on: 19 kwi 2026
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_FRACTAL_CALCULATION_PARAMETERS_H_
#define MANDELBULBER2_QT_FRACTAL_CALCULATION_PARAMETERS_H_

#include <QWidget>
#include "my_widget_with_params.h"

namespace Ui
{
class cFractalCalculationParameters;
}

class cFractalCalculationParameters : public QWidget, public cMyWidgetWithParams
{
	Q_OBJECT
public:
	cFractalCalculationParameters(QWidget *parent = nullptr);
	~cFractalCalculationParameters() override;

private:
	Ui::cFractalCalculationParameters *ui;
};

#endif /* MANDELBULBER2_QT_FRACTAL_CALCULATION_PARAMETERS_H_ */
