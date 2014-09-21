/*
 * mylineedit.cpp
 *
 *  Created on: Sep 14, 2014
 *      Author: krzysztof
 */

#include "mycheckbox.h"
#include "../src/parameters.hpp"
#include <QLineEdit>

void MyCheckBox::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu;
	actionResetToDefault = menu->addAction(tr("Reset to default"));
	QAction *selectedItem = menu->exec(event->globalPos());
	if (selectedItem)
	{
		if (selectedItem == actionResetToDefault)
		{
			if (parameterContainer)
			{
				setChecked(defaultValue);
				emit stateChanged(defaultValue);
			}
			else
			{
				qCritical() << " MyCheckBox::contextMenuEvent(QContextMenuEvent *event): parameter container not assigned. Object:" << objectName();
			}
		}
	}
	delete menu;
}

void MyCheckBox::paintEvent(QPaintEvent *event)
{
	if (isChecked() != GetDefault())
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
	QCheckBox::paintEvent(event);
}

bool MyCheckBox::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		int val = parameterContainer->GetDefault<int>(parameterName);
		defaultValue = val;
		gotDefault = true;

		QString toolTipText;
		toolTipText += "Name: " + parameterName + "\n";
		toolTipText += "Default: " + ((defaultValue) ? QString("true") : QString("false"));
		setToolTip(toolTipText);
	}
	return defaultValue;
}
