/*
 * dock_statistics.h
 *
 *  Created on: 14 paź 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_DOCK_STATISTICS_H_
#define MANDELBULBER2_QT_DOCK_STATISTICS_H_

#include <QWidget>

#include "../src/automated_widgets.hpp"
#include "../src/statistics.h"

namespace Ui
{
class cDockStatistics;
}

class cDockStatistics : public QWidget
{
	Q_OBJECT
public:
	explicit cDockStatistics(QWidget *parent = NULL);
	~cDockStatistics();

	void UpdateDistanceToFractal(double distance);

public slots:
	void slotUpdateStatistics(cStatistics);

private:
	Ui::cDockStatistics *ui;
};

#endif /* MANDELBULBER2_QT_DOCK_STATISTICS_H_ */
