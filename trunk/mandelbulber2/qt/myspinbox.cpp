/*
 * mylineedit.cpp
 *
 *  Created on: Sep 14, 2014
 *      Author: krzysztof
 */

#include "myspinbox.h"
#include "../src/parameters.hpp"
#include <QLineEdit>

void MySpinBox::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = lineEdit()->createStandardContextMenu();
	menu->addSeparator();
	actionResetToDefault = menu->addAction(tr("Reset to default"));
	QAction *selectedItem = menu->exec(event->globalPos());
	if (selectedItem)
	{
		if (selectedItem == actionResetToDefault)
		{
			if (parameterContainer)
			{
				setValue(defaultValue);
				emit valueChanged(defaultValue);
			}
			else
			{
				qCritical() << "MySpinBox::contextMenuEvent(QContextMenuEvent *event): parameter container not assigned. Object:" << objectName();
			}
		}
	}
	delete menu;
}

void MySpinBox::paintEvent(QPaintEvent *event)
{
	if (value() != GetDefault())
	{
		QFont f = font();
		f.setBold(true);
		setFont(f);
	}
	else
	{
		QFont f = font();
		f.setBold(false);
		setFont(f);
	}
	QSpinBox::paintEvent(event);
}

int MySpinBox::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		int val = parameterContainer->GetDefault<int>(parameterName);
		defaultValue = val;
	}
	return defaultValue;
}
