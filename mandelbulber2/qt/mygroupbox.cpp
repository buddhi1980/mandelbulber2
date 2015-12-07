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
#include "../src/parameters.hpp"
#include <QLineEdit>
#include "../src/global_data.hpp"

MyGroupBox::MyGroupBox(QWidget *parent) :	QGroupBox(parent)
{
	actionResetToDefault = NULL;
	actionAddToFlightAnimation = NULL;
	actionAddToKeyframeAnimation = NULL;
	parameterContainer = NULL;
	gotDefault = false;
	defaultValue = 0;
	firstDisplay = true;
	connect(this, SIGNAL(toggled(bool)), this, SLOT(slotToggled(bool)));
}

void MyGroupBox::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu;
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
				setChecked(defaultValue);
				emit toggled(defaultValue);
			}
			else
			{
				qCritical() << " MyGroupBox::contextMenuEvent(QContextMenuEvent *event): parameter container not assigned. Object:" << objectName();
			}
		}
		else if (selectedItem == actionAddToFlightAnimation)
		{
			if (parameterContainer)
			{
				gAnimFrames->AddAnimatedParameter(parameterName, parameterContainer->GetAsOneParameter(parameterName));
				gFlightAnimation->RefreshTable();
			}
		}
		else if (selectedItem == actionAddToKeyframeAnimation)
		{
			if (parameterContainer)
			{
				gKeyframes->AddAnimatedParameter(parameterName, parameterContainer->GetAsOneParameter(parameterName));
				gKeyframeAnimation->RefreshTable();
			}
		}
	}
	delete menu;
}

void MyGroupBox::paintEvent(QPaintEvent *event)
{
	if(firstDisplay)
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
		setToolTip(toolTipText);
	}
	return defaultValue;
}

void MyGroupBox::slotToggled(bool on)
{
	QList<QWidget*> list = this->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
	for (int i = 0; i < list.size(); ++i) {
		if(on)
		{
			list[i]->show();
		}
		else
		{
			list[i]->hide();
		}
	}
}
