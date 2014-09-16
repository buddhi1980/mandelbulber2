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
			int val = parameterContainer->GetDefault<int>(parameterName);
			setValue(val);
			emit valueChanged(val);
		}
	}
	delete menu;
}

