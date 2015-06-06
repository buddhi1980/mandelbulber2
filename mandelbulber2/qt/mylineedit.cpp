/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyLineEdit class - promoted MyLineEdit widget with context menu
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */


#include "mylineedit.h"
#include "../src/parameters.hpp"
#include "../src/global_data.hpp"

void MyLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = createStandardContextMenu();
	menu->addSeparator();
	actionResetToDefault = menu->addAction(tr("Reset to default"));
	actionAddToFlightAnimation = menu->addAction(tr("Add to flight animation"));
	actionAddToKeyframeAnimation = menu->addAction(tr("Add to keyframe animation"));
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
		else if (selectedItem == actionAddToFlightAnimation)
		{
			if (parameterContainer)
			{
				QString parName = parameterName;
				QString type = GetType(objectName());
				if (type == QString("vect3"))
					parName = parameterName.left(parameterName.length() - 2);

				gAnimFrames->AddAnimatedParameter(parName, parameterContainer->GetAsOneParameter(parName));
				gFlightAnimation->RefreshTable();
			}
		}
		else if (selectedItem == actionAddToKeyframeAnimation)
		{
			if (parameterContainer)
			{
				QString parName = parameterName;
				QString type = GetType(objectName());
				if (type == QString("vect3"))
					parName = parameterName.left(parameterName.length() - 2);

				gKeyframes->AddAnimatedParameter(parName, parameterContainer->GetAsOneParameter(parName));
				gKeyframeAnimation->RefreshTable();
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
			QString valS = QString("%L1").arg(val.itemByName(lastChar), 0, 'g', 16);
			defaultText = valS;
			gotDefault = true;
		}
		else
		{
			QString val = parameterContainer->GetDefault<QString>(parameterName);
			defaultText = val;
			gotDefault = true;
		}

		QString toolTipText = toolTip();
		toolTipText += "\nParameter name: " + parameterName + "<br>";
		toolTipText += "Default: " + defaultText;
		setToolTip(toolTipText);
	}
	return defaultText;
}
