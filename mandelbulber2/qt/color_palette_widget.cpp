/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * ColorPaletteWidget class - promoted QWidget for displaying color palette
 */

#include "color_palette_widget.h"
#include "../src/animation_flight.hpp"
#include "../src/animation_keyframes.hpp"

ColorPaletteWidget::ColorPaletteWidget(QWidget *parent)
		: QWidget(parent), CommonMyWidgetWrapper(this)
{
	this->setFixedHeight(50);
	paletteOffset = 0.0;
}

void ColorPaletteWidget::SetPalette(const cColorPalette &_palette)
{
	palette = _palette;
	GetDefault();
	update();
}

void ColorPaletteWidget::SetOffset(double offset)
{
	paletteOffset = offset;
	update();
}

void ColorPaletteWidget::paintEvent(QPaintEvent *event)
{
	(void)event;
	if (palette.IsInitialized())
	{
		QPainter painter(this);

		for (int x = 0; x < this->width(); x++)
		{
			sRGB color = palette.IndexToColour(x * 16 + paletteOffset * 256);
			int offset = paletteOffset * 16;
			if ((x + offset) % 16 == 8)
			{
				color.R += 32;
				if (color.R > 255) color.R = 255;
				color.G += 32;
				if (color.G > 255) color.G = 255;
				color.B += 32;
				if (color.B > 255) color.B = 255;
			}
			if ((x + offset) % 16 == 7)
			{
				color.R -= 32;
				if (color.R < 0) color.R = 0;
				color.G -= 32;
				if (color.G < 0) color.G = 0;
				color.B -= 32;
				if (color.B < 0) color.B = 0;
			}

			if (!isEnabled())
			{
				int grey = (color.R + color.G + color.B) / 3;
				grey = (grey - 128) * 0.5 + 128;
				color.R = color.G = color.B = grey;
			}

			painter.setPen(QColor(color.R, color.G, color.B));
			painter.drawLine(x, 0, x, this->height());
		}
	}
}

void ColorPaletteWidget::mousePressEvent(QMouseEvent *event)
{
	QWidget::mousePressEvent(event);

	QPoint point = event->pos();
	Qt::MouseButton button = event->button();

	if (button == Qt::LeftButton)
	{
		int x = point.x() + 8 + paletteOffset * 16;
		int index = (x / 16) % palette.GetSize();
		QColorDialog colorDialog(this);
		sRGB colorRGB = palette.GetColor(index);
		QColor color(colorRGB.R, colorRGB.G, colorRGB.B);
		colorDialog.setCurrentColor(color);
		colorDialog.setWindowTitle(QString("Edit color # ") + QString::number(index + 1));
		if (colorDialog.exec() == QDialog::Accepted)
		{
			color = colorDialog.currentColor();
			colorRGB = sRGB(color.red(), color.green(), color.blue());
			palette.ChangeColor(index, colorRGB);
		}
	}
}

cColorPalette ColorPaletteWidget::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		cColorPalette val = parameterContainer->GetDefault<cColorPalette>(parameterName);
		defaultValue = val;
		gotDefault = true;
		setToolTipText();
	}
	return defaultValue;
}

void ColorPaletteWidget::resetToDefault()
{
	palette = defaultValue;
	update();
}

QString ColorPaletteWidget::getDefaultAsString()
{
	return "---";
}

QString ColorPaletteWidget::getFullParameterName()
{
	return parameterName;
}

void ColorPaletteWidget::contextMenuEvent(QContextMenuEvent *event)
{
	CommonMyWidgetWrapper::contextMenuEvent(event);
}
