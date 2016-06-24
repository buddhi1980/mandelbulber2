/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyGroupBox class - promoted MyGroupBox widget with context menu
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

#include "mygroupbox.h"
#include <QLineEdit>
#include "../src/animation_flight.hpp"
#include "../src/animation_keyframes.hpp"

MyGroupBox::MyGroupBox(QWidget *parent) : QGroupBox(parent), CommonMyWidgetWrapper(this)
{
	defaultValue = 0;
	firstDisplay = true;
	connect(this, SIGNAL(toggled(bool)), this, SLOT(slotToggled(bool)));
}

void MyGroupBox::resetToDefault()
{
	setChecked(defaultValue);
	emit toggled(defaultValue);
}

QString MyGroupBox::getDefaultAsString()
{
	return defaultValue ? "true" : "false";
}

QString MyGroupBox::getFullParameterName()
{
	return parameterName;
}

void MyGroupBox::paintEvent(QPaintEvent *event)
{
	if (firstDisplay)
	{
		originalText = title();
		firstDisplay = false;
	}
	QString displayTitle = originalText;
	if (!isChecked())
	{
		displayTitle += " ... ";
	}

	if (isChecked() != GetDefault())
	{
		displayTitle += " *";
	}
	setTitle(displayTitle);
	QGroupBox::paintEvent(event);
}

bool MyGroupBox::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		int val = parameterContainer->GetDefault<int>(parameterName);
		defaultValue = val;
		gotDefault = true;

		QString toolTipText = toolTip();
		toolTipText += "\nParameter name: " + parameterName + "<br>";
		toolTipText += "Default: " + ((defaultValue) ? QString("true") : QString("false"));
		setToolTipText();
	}
	return defaultValue;
}

void MyGroupBox::slotToggled(bool on)
{
	QList<QWidget *> list = this->findChildren<QWidget *>(QString(), Qt::FindDirectChildrenOnly);
	for (int i = 0; i < list.size(); ++i)
	{
		if (on)
		{
			list[i]->show();
		}
		else
		{
			list[i]->hide();
		}
	}
}

void MyGroupBox::contextMenuEvent(QContextMenuEvent *event)
{
	CommonMyWidgetWrapper::contextMenuEvent(event);
}
