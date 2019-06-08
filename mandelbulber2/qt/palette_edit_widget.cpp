/*
 * palette_edit_widget.cpp
 *
 *  Created on: 1 cze 2019
 *      Author: krzysztof
 */

#include <QPainter>

#include "palette_edit_widget.h"
#include "src/system.hpp"

cPaletteEditWidget::cPaletteEditWidget(QWidget *parent) : QWidget(parent)
{
	setFixedHeight(systemData.GetPreferredThumbnailSize() / 2);
}

cPaletteEditWidget::~cPaletteEditWidget()
{
	// TODO Auto-generated destructor stub
}

void cPaletteEditWidget::paintEvent(QPaintEvent *event)
{
	int gradientWidth = width() - 2 * margins;
	if (gradientWidth < 2) gradientWidth = 2;
	int gradientHeight = height() / 2;

	QPainter painter(this);
	QVector<sRGB16> grad = gradient.GetGradient(gradientWidth);

	for (int x = 0; x < grad.size(); x++)
	{
		QColor color(QColor(grad[x].R / 256, grad[x].G / 256, grad[x].B / 256));
		painter.setPen(color);
		painter.drawLine(x + margins, 0, x + margins, gradientHeight);
	}
}
