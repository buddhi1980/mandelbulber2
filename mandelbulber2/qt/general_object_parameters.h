/*
 * general_object_parameters.h
 *
 *  Created on: 19 kwi 2026
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_GENERAL_OBJECT_PARAMETERS_H_
#define MANDELBULBER2_QT_GENERAL_OBJECT_PARAMETERS_H_

#include <QWidget>
#include "my_widget_with_params.h"

namespace Ui
{
class cGeneralObjectParameters;
}

class cGeneralObjectParameters : public QWidget, public cMyWidgetWithParams
{
	Q_OBJECT
public:
	cGeneralObjectParameters(QWidget *parent = nullptr);
	~cGeneralObjectParameters() override;

private:
	Ui::cGeneralObjectParameters *ui;
};

#endif /* MANDELBULBER2_QT_GENERAL_OBJECT_PARAMETERS_H_ */
