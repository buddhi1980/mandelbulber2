/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * MyColorButton class - promoted QPushButton widget with context menu
 */

#include <QtWidgets/QtWidgets>
#include "my_color_button.h"
#include "../src/animation_flight.hpp"

MyColorButton::MyColorButton(QWidget *parent) : QPushButton(parent), CommonMyWidgetWrapper(this)
{
	defaultValue = sRGB();
	currentValue = sRGB();
	setText("");
	w = 40;
	h = 15;
	SetupColor();
}

MyColorButton::~MyColorButton()
{
	delete painter;
	delete pix;
}

void MyColorButton::resetToDefault()
{
	currentValue = defaultValue;
	UpdateColor();
}

sRGB MyColorButton::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		defaultValue = parameterContainer->GetDefault<sRGB>(parameterName);
		setToolTipText();
		gotDefault = true;
	}
	return defaultValue;
}

QString MyColorButton::getDefaultAsString()
{
	return QString("rgb(%1, %2, %3)")
		.arg(QString::number(defaultValue.R / 256), QString::number(defaultValue.G / 256),
			QString::number(defaultValue.B / 256));
}

QString MyColorButton::getFullParameterName()
{
	return parameterName;
}

sRGB MyColorButton::GetColor() const
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
	painter->drawRect(0, 0, w - 1, h - 1);

	if (!(defaultValue.R / 256 == currentValue.R / 256 && defaultValue.G / 256 == currentValue.G / 256
				&& defaultValue.B / 256 == currentValue.B / 256))
	{
		// not (almost) default color -> make border thicker to indicate non default value
		painter->drawRect(1, 1, w - 3, h - 3);
	}

	QIcon icon(*pix);
	setIcon(icon);
	setIconSize(QSize(w, h));
}

void MyColorButton::SetupColor()
{
	pix = new QPixmap(w, h);
	painter = new QPainter(pix);
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
		colorDialog.setWindowTitle(tr("Edit color: %1").arg(parameterName));
		if (colorDialog.exec() == QDialog::Accepted)
		{
			color = colorDialog.currentColor();
			colorRGB = sRGB(color.red() * 256, color.green() * 256, color.blue() * 256);
			SetColor(colorRGB);
		}
	}
}

void MyColorButton::contextMenuEvent(QContextMenuEvent *event)
{
	CommonMyWidgetWrapper::contextMenuEvent(event);
}
