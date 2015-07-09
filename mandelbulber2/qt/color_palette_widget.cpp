/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * ColorPaletteWidget class - promoted QWidget for displaying color palette
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

#include "color_palette_widget.h"
#include "../src/parameters.hpp"
#include "../src/global_data.hpp"

ColorPaletteWidget::ColorPaletteWidget(QWidget *parent) :	QWidget(parent)
{
	this->setFixedHeight(50);
	paletteOffset = 0.0;

	actionResetToDefault = NULL;
	actionAddToFlightAnimation = NULL;
	actionAddToKeyframeAnimation = NULL;
	parameterContainer = NULL;
	gotDefault = false;
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
	(void) event;
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

			if(!isEnabled())
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
		if(colorDialog.exec() == QDialog::Accepted)
		{
			color = colorDialog.currentColor();
			colorRGB = sRGB(color.red(), color.green(), color.blue());
			palette.ChangeColor(index, colorRGB);
		}
	}
}

cColorPalette ColorPaletteWidget::GetDefault(void)
{
	if (parameterContainer && !gotDefault)
	{
		cColorPalette val = parameterContainer->GetDefault<cColorPalette>(parameterName);
		defaultValue = val;
		gotDefault = true;

		QString toolTipText = toolTip();
		toolTipText += "\nParameter name: " + parameterName + "\n";
		setToolTip(toolTipText);
	}
	return defaultValue;
}

void ColorPaletteWidget::contextMenuEvent(QContextMenuEvent *event)
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
				palette = defaultValue;
				update();
			}
			else
			{
				qCritical() << " MyCheckBox::contextMenuEvent(QContextMenuEvent *event): parameter container not assigned. Object:" << objectName();
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
