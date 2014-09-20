/*
 * mylineedit.cpp
 *
 *  Created on: Sep 14, 2014
 *      Author: krzysztof
 */

#include "mylineedit.h"
#include "../src/parameters.hpp"

void MyLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = createStandardContextMenu();
	menu->addSeparator();
	actionResetToDefault = menu->addAction(tr("Reset to default"));
	QAction *selectedItem = menu->exec(event->globalPos());
	if (selectedItem)
	{
		if (selectedItem == actionResetToDefault)
		{
			if (parameterContainer)
			{
				setText(defaultText);
				emit editingFinished();
			}
			else
			{
				qCritical() << "MyLineEdit::contextMenuEvent(QContextMenuEvent *event): parameter container not assigned. Object:" << objectName();
			}
		}
	}
	delete menu;
}

QString MyLineEdit::GetType(const QString &name)
{
	size_t firstDashPosition = name.indexOf("_");
	return name.left(firstDashPosition);
}

void MyLineEdit::paintEvent(QPaintEvent *event)
{
	if (text() != GetDefault())
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
	QLineEdit::paintEvent(event);
}

QString MyLineEdit::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		QString type = GetType(objectName());
		if (type == QString("vect3"))
		{
			char lastChar = (parameterName.at(parameterName.length() - 1)).toLatin1();
			QString nameVect = parameterName.left(parameterName.length() - 2);
			CVector3 val = parameterContainer->GetDefault<CVector3>(nameVect);
			QString valS = QString::number(val.itemByName(lastChar), 'g', 16);
			defaultText = valS;
			gotDefault = true;
		}
		else
		{
			QString val = parameterContainer->GetDefault<QString>(parameterName);
			defaultText = val;
			gotDefault = true;
		}

		QString toolTipText;
		toolTipText += "Name: " + parameterName + "\n";
		toolTipText += "Default: " + defaultText;
		setToolTip(toolTipText);
	}
	return defaultText;
}
