/*
 * color_palette_widget.h
 *
 *  Created on: Aug 4, 2014
 *      Author: krzysztof
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
