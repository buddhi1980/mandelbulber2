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

	// Stores the objects tree node ID that the "Place object" button activates
	// in the mouse click function combo box
	void setPlaceObjectNodeId(int nodeId);

private slots:
	void onPushButtonPlaceObjectClicked();

private:
	Ui::cGeneralObjectParameters *ui;
	int placeObjectNodeId = -1;
};

#endif /* MANDELBULBER2_QT_GENERAL_OBJECT_PARAMETERS_H_ */
