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


#ifndef COLOR_PALETTE_WIDGET_H_
#define COLOR_PALETTE_WIDGET_H_

#include <QWidget>
#include "../src/color_palette.hpp"
#include <QtCore>
#include <QPainter>
#include <QPoint>
#include <QMouseEvent>
#include <QColorDialog>

class ColorPaletteWidget: public QWidget
{
Q_OBJECT

public:
	ColorPaletteWidget(QWidget *parent = 0) :
			QWidget(parent)
	{
		this->setFixedHeight(50);
		paletteOffset = 0.0;
	}

	void SetPalette(const cColorPalette &_palette)
	{
		palette = _palette;
		update();
	}

	cColorPalette GetPalette(void)
	{
		return palette;
	}

	void SetOffset(double offset)
	{
		paletteOffset = offset;
		update();
	}

private:
	cColorPalette palette;
	double paletteOffset;

	void paintEvent(QPaintEvent *event)
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

protected:
	void mousePressEvent(QMouseEvent *event)
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
			colorDialog.exec();
			color = colorDialog.currentColor();
			colorRGB = sRGB(color.red(), color.green(), color.blue());
			palette.ChangeColor(index, colorRGB);
		}
	}

};

#endif /* COLOR_PALETTE_H_ */
