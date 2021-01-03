/*
 * my_tab_widget_with_checkboxes.h
 *
 *  Created on: 3 sty 2021
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_MY_TAB_WIDGET_WITH_CHECKBOXES_H_
#define MANDELBULBER2_QT_MY_TAB_WIDGET_WITH_CHECKBOXES_H_

#include <QTabWidget>

class MyTabWidgetWithCheckboxes : public QTabWidget
{
	Q_OBJECT

public:
	MyTabWidgetWithCheckboxes(QWidget *parent = nullptr);

private slots:
	void slotToggledFractalEnable(int fractalIndex, bool enabled) const;
};

#endif /* MANDELBULBER2_QT_MY_TAB_WIDGET_WITH_CHECKBOXES_H_ */
