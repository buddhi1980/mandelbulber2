/*
 * mylineedit.cpp
 *
 *  Created on: Sep 14, 2014
 *      Author: krzysztof
 */

#include "mygroupbox.h"
#include "../src/parameters.hpp"
#include <QLineEdit>

void MyGroupBox::contextMenuEvent(QContextMenuEvent *event)
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
				emit toggled(defaultValue);
			}
			else
			{
				qCritical() << " MyGroupBox::contextMenuEvent(QContextMenuEvent *event): parameter container not assigned. Object:" << objectName();
			}
		}
	}
	delete menu;
}

void MyGroupBox::paintEvent(QPaintEvent *event)
{
	if (isChecked() != GetDefault())
	{
		setTitle(originalText + " *");
	}
	else
	{
		setTitle(originalText);
	}
	QGroupBox::paintEvent(event);

}

bool MyGroupBox::GetDefault()
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

		originalText = title();
	}
	return defaultValue;
}
