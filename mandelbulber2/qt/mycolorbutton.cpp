/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * MyColorButton class - promoted QPushButton widget with context menu
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 */

#include "mycolorbutton.h"
#include "../src/parameters.hpp"
#include <QPushButton>
#include "../src/global_data.hpp"
#include <algorithm>

MyColorButton::MyColorButton(QWidget *parent) : QPushButton(parent)
{
	actionResetToDefault = NULL;
	actionAddToFlightAnimation = NULL;
	actionAddToKeyframeAnimation = NULL;
	parameterContainer = NULL;
	gotDefault = false;
	defaultValue = sRGB();
	currentValue = sRGB();
	setText("");
	w = 40;
	h = 15;
	SetupColor();
}

MyColorButton::~MyColorButton(void)
{
	delete painter;
	delete pix;
}

void MyColorButton::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = new QMenu;
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
				currentValue = defaultValue;
				UpdateColor();
			}
			else
			{
				qCritical() << "MyColorButton::contextMenuEvent(QContextMenuEvent *event): parameter container not assigned. Object:" << objectName();
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

sRGB MyColorButton::GetColor()
{
	return currentValue;
}

void MyColorButton::SetColor(sRGB newColor)
{
	this->currentValue = newColor;
	UpdateColor();
}

void MyColorButton::UpdateColor()
{
	GetDefault();
	QColor color(currentValue.R / 256, currentValue.G / 256, currentValue.B / 256);
	painter->fillRect(QRect(0, 0, w, h), color);
	painter->drawRect(0,0,w-1,h-1);

	if(!(defaultValue.R / 256 == currentValue.R / 256 && defaultValue.G / 256 == currentValue.G / 256 && defaultValue.B / 256 == currentValue.B / 256))
	{
		// not (almost) default color -> make border thicker to indicate non default value
		painter->drawRect(1,1,w-3,h-3);
	}

	QIcon icon(*pix);
	setIcon(icon);
	setIconSize(QSize(w, h));
}

void MyColorButton::SetupColor(){
	pix = new QPixmap(w,h);
	painter = new QPainter(pix);
}

sRGB MyColorButton::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		defaultValue = parameterContainer->GetDefault<sRGB>(parameterName);
		gotDefault = true;

		QString toolTipText = toolTip();
		toolTipText += "\nParameter name: " + parameterName + "<br>";
		toolTipText += "Default: rgb(" + QString::number(defaultValue.R / 256) + ", " + QString::number(defaultValue.G / 256) + ", " + QString::number(defaultValue.B / 256) + ")";
		setToolTip(toolTipText);
	}
	return defaultValue;
}

void MyColorButton::mousePressEvent(QMouseEvent *event)
{
	QPushButton::mousePressEvent(event);

	Qt::MouseButton button = event->button();

	if (button == Qt::LeftButton)
	{
		QColorDialog colorDialog(this);
		sRGB colorRGB = GetColor();
		QColor color(colorRGB.R / 256, colorRGB.G / 256, colorRGB.B / 256);
		colorDialog.setCurrentColor(color);
		colorDialog.setWindowTitle(QString("Edit color: " + parameterName));
		if(colorDialog.exec() == QDialog::Accepted)
		{
			color = colorDialog.currentColor();
			colorRGB = sRGB(color.red() * 256, color.green() * 256, color.blue() * 256);
			SetColor(colorRGB);
		}
	}
}
