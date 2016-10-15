/*
 * dock_measurements.h
 *
 *  Created on: 15 paź 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_DOCK_MEASUREMENTS_H_
#define MANDELBULBER2_QT_DOCK_MEASUREMENTS_H_

#include <QWidget>

#include "../src/automated_widgets.hpp"

namespace Ui
{
class cDockMeasurements;
}

class cDockMeasurements : public QWidget
{
	Q_OBJECT
public:
	explicit cDockMeasurements(QWidget *parent = NULL);
	~cDockMeasurements();

private slots:
	void slotPressedButtonGetPoint();

private:
	void ConnectSignals();

	Ui::cDockMeasurements *ui;

	cAutomatedWidgets *automatedWidgets;
};

#endif /* MANDELBULBER2_QT_DOCK_MEASUREMENTS_H_ */
