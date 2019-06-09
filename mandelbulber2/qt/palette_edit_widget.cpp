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
	int fixHeight = systemData.GetPreferredThumbnailSize() / 2;
	setFixedHeight(fixHeight);

	buttonWidth = fixHeight / 6;
	if (buttonWidth % 2 == 0) buttonWidth += 1; // to always have odd width

	margins = buttonWidth / 2 + 2;
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
	QVector<sRGB16> grad = gradient.GetGradient(gradientWidth, true);

	for (int x = 0; x < grad.size(); x++)
	{
		QColor color(QColor(grad[x].R / 256, grad[x].G / 256, grad[x].B / 256));
		painter.setPen(color);
		painter.drawLine(x + margins, 0, x + margins, gradientHeight);
	}

	QList<cColorGradient::sColor> listOfColors = gradient.GetListOfColors();
	for (cColorGradient::sColor posColor : listOfColors)
	{
		PaintButton(posColor, painter);
	}
}

int cPaletteEditWidget::CalcButtonPosition(double position)
{
	return margins + position * (width() - 2 * margins - 1);
}

void cPaletteEditWidget::PaintButton(const cColorGradient::sColor &posColor, QPainter &painter)
{
	int buttonPosition = CalcButtonPosition(posColor.position);

	int buttonTop = height() / 2 + buttonWidth / 2;

	QRect rect(QPoint(buttonPosition - buttonWidth / 2, buttonTop),
		QPoint(buttonPosition + buttonWidth / 2, height() - 2));

	QColor color(posColor.color.R / 256, posColor.color.G / 256, posColor.color.B / 256);

	QBrush brush(color, Qt::SolidPattern);
	painter.fillRect(rect, brush);

	QVector<QPoint> triangle = {QPoint(buttonPosition, height() / 2),
		QPoint(buttonPosition - buttonWidth / 2, buttonTop),
		QPoint(buttonPosition + buttonWidth / 2, buttonTop)};
	QPolygon pTriangle(triangle);
	QPainterPath pathTriangle;
	pathTriangle.addPolygon(pTriangle);
	painter.fillPath(pathTriangle, brush);

	int avgColor = (posColor.color.R + posColor.color.G + posColor.color.B)/3;
	if(avgColor > 10000) painter.setPen(Qt::black);
	else  painter.setPen(Qt::white);

	painter.drawRect(rect);
}
