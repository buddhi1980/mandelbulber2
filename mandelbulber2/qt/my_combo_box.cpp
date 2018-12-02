/*
 * my_combo_box.cpp
 *
 *  Created on: 2 gru 2018
 *      Author: krzysztof
 */

#include "my_combo_box.h"
#include "src/parameters.hpp"

MyComboBox::MyComboBox(QWidget *parent) : QComboBox(parent), CommonMyWidgetWrapper(this)
{
	defaultValue = 0;
}

MyComboBox::~MyComboBox() {}

void MyComboBox::contextMenuEvent(QContextMenuEvent *event)
{
	CommonMyWidgetWrapper::contextMenuEvent(event);
}

void MyComboBox::paintEvent(QPaintEvent *event)
{
	GetDefault();
	QComboBox::paintEvent(event);
}

void MyComboBox::resetToDefault()
{
	setCurrentIndex(defaultValue);
}

QString MyComboBox::getDefaultAsString()
{
	return QString::number(defaultValue);
}

QString MyComboBox::getFullParameterName()
{
	return parameterName;
}

int MyComboBox::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		int val = parameterContainer->GetDefault<int>(parameterName);
		defaultValue = val;
		gotDefault = true;
		setToolTipText();
	}
	return defaultValue;
}
