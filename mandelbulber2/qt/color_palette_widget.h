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
#include <QMenu>
#include "../src/parameters.hpp"

class ColorPaletteWidget: public QWidget
{
Q_OBJECT

public:
	ColorPaletteWidget(QWidget *parent = 0);
	void SetPalette(const cColorPalette &_palette);
	cColorPalette GetPalette(void) {return palette;};
	void SetOffset(double offset);

	void AssignParameterContainer(cParameterContainer *container) {parameterContainer = container;}
	void AssingParameterName(QString name) {parameterName = name;}

private:
	cColorPalette palette;
	double paletteOffset;

	QAction *actionResetToDefault;
	QAction *actionAddToFlightAnimation;
	QAction *actionAddToKeyframeAnimation;
	QString GetType(const QString &name);
	cParameterContainer *parameterContainer;
	QString parameterName;

	cColorPalette GetDefault();
	cColorPalette defaultValue;
	bool gotDefault;

	void paintEvent(QPaintEvent *event);


protected:
	void mousePressEvent(QMouseEvent *event);
	void contextMenuEvent(QContextMenuEvent *event);
};

#endif /* COLOR_PALETTE_H_ */
