/*
 * mylineedit.cpp
 *
 *  Created on: Sep 14, 2014
 *      Author: krzysztof
 */

#include "mydoublespinbox.h"
#include "../src/parameters.hpp"
#include <QLineEdit>

void MyDoubleSpinBox::contextMenuEvent(QContextMenuEvent *event)
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
				qCritical() << "MyDoubleSpinBox::contextMenuEvent(QContextMenuEvent *event): parameter container not assigned. Object:" << objectName();
			}
		}
	}
	delete menu;
}

void MyDoubleSpinBox::paintEvent(QPaintEvent *event)
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
	QDoubleSpinBox::paintEvent(event);
}

QString MyDoubleSpinBox::GetType(const QString &name)
{
	size_t firstDashPosition = name.indexOf("_");
	return name.left(firstDashPosition);
}

double MyDoubleSpinBox::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		QString type = GetType(objectName());
		if (type == QString("vect3"))
		{
			char lastChar = (parameterName.at(parameterName.length() - 1)).toLatin1();
			QString nameVect = parameterName.left(parameterName.length() - 2);
			CVector3 val = parameterContainer->GetDefault<CVector3>(nameVect);
			defaultValue = val.itemByName(lastChar);
			gotDefault = true;
		}
		else
		{
			defaultValue = parameterContainer->GetDefault<double>(parameterName);
			gotDefault = true;
		}

		QString toolTipText;
		toolTipText += "Name: " + parameterName + "\n";
		toolTipText += "Default: " + QString::number(defaultValue, 'g', 16);
		setToolTip(toolTipText);
	}
	return defaultValue;
}
