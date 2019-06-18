/*
 * palette_edit_widget.cpp
 *
 *  Created on: 1 cze 2019
 *      Author: krzysztof
 */

#include "gradient_edit_widget.h"

#include <QPainter>
#include <QMouseEvent>
#include <QColorDialog>

#include "src/system.hpp"

cGradientEditWidget::cGradientEditWidget(QWidget *parent) : QWidget(parent)
{
	viewMode = false;
	mouseDragStarted = false;
	pressedColorIndex = 0;
	dragStartX = 0;

	int fixHeight = systemData.GetPreferredThumbnailSize() / 2;
	setFixedHeight(fixHeight);

	buttonWidth = fixHeight / 6;
	if (buttonWidth % 2 == 0) buttonWidth += 1; // to always have odd width

	margins = buttonWidth / 2 + 2;
}

cGradientEditWidget::~cGradientEditWidget()
{
	// TODO Auto-generated destructor stub
}

void cGradientEditWidget::SetViewModeOnly()
{
	viewMode = true;
	setFixedHeight(systemData.GetPreferredThumbnailSize() / 4);
	margins = 0;
}

void cGradientEditWidget::paintEvent(QPaintEvent *event)
{
	int gradientWidth = width() - 2 * margins;
	if (gradientWidth < 2) gradientWidth = 2;
	int gradientHeight = (viewMode) ? height() : height() / 2;

	QPainter painter(this);
	QVector<sRGB> grad = gradient.GetGradient(gradientWidth, false);

	for (int x = 0; x < grad.size(); x++)
	{
		QColor color(QColor(grad[x].R, grad[x].G, grad[x].B));
		painter.setPen(color);
		painter.drawLine(x + margins, 0, x + margins, gradientHeight);
	}

	if (!viewMode)
	{
		QList<cColorGradient::sColor> listOfColors = gradient.GetListOfColors();
		for (cColorGradient::sColor posColor : listOfColors)
		{
			PaintButton(posColor, painter);
		}
	}
}

int cGradientEditWidget::CalcButtonPosition(double position)
{
	return margins + position * (width() - 2 * margins - 1);
}

void cGradientEditWidget::PaintButton(const cColorGradient::sColor &posColor, QPainter &painter)
{
	int buttonPosition = CalcButtonPosition(posColor.position);

	int buttonTop = height() / 2 + buttonWidth / 2;

	QRect rect(QPoint(buttonPosition - buttonWidth / 2, buttonTop),
		QPoint(buttonPosition + buttonWidth / 2, height() - 2));

	QColor color(posColor.color.R, posColor.color.G, posColor.color.B);

	QBrush brush(color, Qt::SolidPattern);
	painter.fillRect(rect, brush);

	QVector<QPoint> triangle = {QPoint(buttonPosition, height() / 2),
		QPoint(buttonPosition - buttonWidth / 2, buttonTop),
		QPoint(buttonPosition + buttonWidth / 2, buttonTop)};
	QPolygon pTriangle(triangle);
	QPainterPath pathTriangle;
	pathTriangle.addPolygon(pTriangle);
	painter.fillPath(pathTriangle, brush);

	int avgColor = (posColor.color.R + posColor.color.G + posColor.color.B) / 3;
	if (avgColor > 100)
		painter.setPen(Qt::black);
	else
		painter.setPen(Qt::white);

	painter.drawRect(rect);
}

int cGradientEditWidget::FindButtonAtPosition(int x)
{
	QList<cColorGradient::sColor> listOfColors = gradient.GetListOfColors();

	for (int i = listOfColors.size() - 1; i >= 0; i--)
	{
		int xButton = CalcButtonPosition(listOfColors[i].position);
		if ((x > xButton - buttonWidth / 2) && (x <= xButton + buttonWidth / 2))
		{
			return i;
		}
	}
	return -1; //-1 means nothing found
}

void cGradientEditWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (pressedColorIndex >= 2)
	{
		if (event->x() != dragStartX)
		{
			mouseDragStarted = true;
		}

		if (mouseDragStarted)
		{
			double pos = double(event->x() - margins) / (width() - 2 * margins - 1);
			gradient.ModifyPosition(pressedColorIndex, pos);
			emit update();
		}
	}
}

void cGradientEditWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (viewMode)
		{
			emit openEditor();
		}
		else
		{
			int mouseX = event->x();
			int mouseY = event->y();

			if (mouseY > height() / 2)
			{
				int index = FindButtonAtPosition(mouseX);
				if (index >= 2)
				{
					dragStartX = mouseX;
					pressedColorIndex = index;
				}
			}
		}
	}
}

void cGradientEditWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (!viewMode)
		{
			if (pressedColorIndex >= 0 && !mouseDragStarted)
			{
				QList<cColorGradient::sColor> listOfColors = gradient.GetListOfColors();

				QColorDialog colorDialog(this);
				colorDialog.setOption(QColorDialog::DontUseNativeDialog);
				sRGB colorRGB = listOfColors[pressedColorIndex].color;
				QColor color(colorRGB.R, colorRGB.G, colorRGB.B);
				colorDialog.setCurrentColor(color);
				colorDialog.setWindowTitle(
					tr("Edit color #%1").arg(QString::number(pressedColorIndex + 1)));
				if (colorDialog.exec() == QDialog::Accepted)
				{
					color = colorDialog.currentColor();
					colorRGB = sRGB(color.red(), color.green(), color.blue());
					gradient.ModifyColor(pressedColorIndex, colorRGB);

					if (pressedColorIndex == 0) gradient.ModifyColor(1, colorRGB);

					if (pressedColorIndex == 1) gradient.ModifyColor(0, colorRGB);

					emit update();
				}
			}

			pressedColorIndex = -1;
			mouseDragStarted = false;
		}
	}
}

void cGradientEditWidget::SetColors(const QString &colorsString)
{
	gradient.SetColorsFromString(colorsString);
	update();
}
