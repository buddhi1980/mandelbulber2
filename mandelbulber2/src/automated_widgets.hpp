/*
 * automated_widgets.hpp
 *
 *  Created on: 3 maj 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_AUTOMATED_WIDGETS_HPP_
#define MANDELBULBER2_SRC_AUTOMATED_WIDGETS_HPP_

#include <QObject>

class cAutomatedWidgets : public QObject
{
	Q_OBJECT
public:
	cAutomatedWidgets(QObject *parent);
	void ConnectSignalsForSlidersInWindow(QWidget *window);

private slots:
	void slotSliderMoved(int value);
	void slotDoubleSpinBoxChanged(double value);
	void slotIntSliderMoved(int value);
	void slotIntSpinBoxChanged(int value);
	void slotSlider3Moved(int value);
	void slotSlider4Moved(int value);
	void slotSpinBox3Changed(double value);
	void slotSpinBox4Changed(double value);
  void slotDial3Moved(int value);
  void slotDial4Moved(int value);
	void slotSpinBoxD3Changed(double value);
	void slotSpinBoxD4Changed(double value);
  void slotDialMoved(int value);
	void slotSpinBoxDChanged(double value);
	void slotLogSliderMoved(int value);
	void slotLogSliderVect3Moved(int value);
	void slotLogLineEditChanged(const QString &text);
	void slotLogVect3Changed(const QString &text);
};



#endif /* MANDELBULBER2_SRC_AUTOMATED_WIDGETS_HPP_ */
