/*
 * my_tab_widget_with_checkboxes.cpp
 *
 *  Created on: 3 sty 2021
 *      Author: krzysztof
 */

#include "my_tab_widget_with_checkboxes.h"
#include "my_tab_bar_with_checkbox.h"

MyTabWidgetWithCheckboxes::MyTabWidgetWithCheckboxes(QWidget *parent) : QTabWidget(parent)
{
	MyTabBarWithCheckBox *bar = new MyTabBarWithCheckBox(this);
	setTabBar(bar);

	connect(bar, &MyTabBarWithCheckBox::toggledEnable, this, &MyTabWidgetWithCheckboxes::slotToggledFractalEnable);
}

void MyTabWidgetWithCheckboxes::slotToggledFractalEnable(int fractalIndex, bool enabled) const
{
	widget(fractalIndex - 1)->setEnabled(enabled);
}
