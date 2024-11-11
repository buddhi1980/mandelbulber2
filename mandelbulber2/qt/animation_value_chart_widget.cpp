/*
 * nimation_value_chart_widget.cpp
 *
 *  Created on: 6 paź 2024
 *      Author: krzysztof
 */

#include "animation_value_chart_widget.h"

#include <QPainter>
#include <algorithm>
#include <QApplication>
#include <QDebug>
#include <QMouseEvent>

#include "../src/system_data.hpp"

using std::min;

cAnimationValueChartWidget::cAnimationValueChartWidget(QWidget *parent) : QWidget(parent)
{
	setFixedHeight(systemData.GetPreferredThumbnailSize());
	setMinimumHeight(systemData.GetPreferredThumbnailSize());
}

cAnimationValueChartWidget::~cAnimationValueChartWidget()
{
	// nothing to do
}

void cAnimationValueChartWidget::paintEvent(QPaintEvent *event)
{
	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(rect(), Qt::white);

	int xShift = (1.0 - 1.0 / zoom) * currentFrame;
	int firstVisibleFrame = xShift;
	int lastVisibleFrame =
		std::min(xShift + animationPath.values.size() / zoom, float(animationPath.values.size()));
	if (animationPath.values.size() > 0)
	{
		// find min and max values
		min = animationPath.values[firstVisibleFrame];
		max = animationPath.values[firstVisibleFrame];
		for (int i = firstVisibleFrame; i < lastVisibleFrame; i++)
		{
			if (animationPath.values[i] < min)
			{
				min = animationPath.values[i];
			}
			if (animationPath.values[i] > max)
			{
				max = animationPath.values[i];
			}
		}
	}

	// adding text in the left with min and max value
	QFont font = QApplication::font();
	int fontPixelSize = QFontInfo(font).pixelSize();
	painter.setFont(font);
	painter.drawText(fontPixelSize, 2 + fontPixelSize, QString::number(max, 'g', 6));
	painter.drawText(fontPixelSize, height() - 2, QString::number(min, 'g', 6));

	QPen pen;

	// draw grid based on keyframe indices
	pen.setColor(Qt::lightGray);
	pen.setWidth(1);
	painter.setPen(pen);
	for (int i = 0; i < animationPath.keyframeIndices.size(); i++)
	{
		float x =
			(animationPath.keyframeIndices[i] - xShift) * zoom / animationPath.values.size() * width();
		painter.drawLine(x, 0, x, height());
	}

	int topMargin = margin * height();
	int bottomMargin = (1.0 - margin) * height();
	float yRatio = (bottomMargin - topMargin) / (max - min); // calculate ratio outside the loop

	// add texts with keyframe indices at the bottom of chart
	pen.setColor(Qt::gray);
	painter.setPen(pen);
	for (int i = 0; i < animationPath.keyframeIndices.size(); i++)
	{
		float x =
			(animationPath.keyframeIndices[i] - xShift) * zoom / animationPath.values.size() * width();
		painter.drawText(x + 2, height() / 2, QString::number(i));
	}

	pen.setColor(Qt::black);
	pen.setWidth(1);
	painter.setPen(pen);

	// draw polyline
	QPolygonF polyline;
	for (int i = 0; i < animationPath.values.size(); i++)
	{
		if (min != max)
		{
			// draw line
			float y = (max - animationPath.values[i]) * yRatio + topMargin; // use pre-calculated ratio
			QPointF point((i - xShift) * zoom / animationPath.values.size() * width(), y);
			polyline << point;
		}
		else
		{
			// draw straight line
			QPointF point((i - xShift) * zoom / animationPath.values.size() * width(), height() / 2);
			polyline << point;
		}
	}
	painter.drawPolyline(polyline);

	// draw small circles at keyframes using polygon coordinates
	pen.setColor(Qt::red);
	pen.setWidth(2);
	painter.setPen(pen);

	float circleSize = 0.3f * fontPixelSize;
	for (int i = 0; i < animationPath.keyframeIndices.size(); i++)
	{
		if (!animationPath.emptyKeyframes[i])
		{
			QPointF center = polyline[animationPath.keyframeIndices[i]];
			painter.drawEllipse(center, circleSize, circleSize);
			keyButtons.append({i, QRect(center.x() - circleSize, center.y() - circleSize,
															circleSize * 2.0f, circleSize * 2.0f)});
		}
	}

	// draw cursor
	pen.setColor(Qt::green);
	pen.setWidth(2);
	painter.setPen(pen);
	QPointF cursor((currentFrame - xShift) * zoom / animationPath.values.size() * width(), 0);
	painter.drawLine(cursor, QPointF(cursor.x(), height()));

	// adding text with parameter name at the top center of chart
	pen.setColor(Qt::black);
	painter.setPen(pen);
	painter.drawText(width() / 2, fontPixelSize, "Parameter: " + animationPath.parameterName);
}
void cAnimationValueChartWidget::SetAnimationPath(const cAnimationPath &path)
{
	animationPath = path;
	update();
}

void cAnimationValueChartWidget::resetZoom()
{
	zoom = 1.0;
	update();
}

void cAnimationValueChartWidget::slotZoomIn()
{
	zoom *= 2.0;
	update();
}

void cAnimationValueChartWidget::slotZoomOut()
{
	zoom /= 2.0;
	if (zoom < 1.0)
	{
		zoom = 1.0;
	}
	update();
}

void cAnimationValueChartWidget::slotSetCurrentFrame(int frame)
{
	currentFrame = frame;
	update();
}

void cAnimationValueChartWidget::mousePressEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		int mouseX = event->x();
		int mouseY = event->y();

		int index = FindButtonAtPosition(mouseX, mouseY);

		dragStartX = mouseX;
		dragStartY = mouseY;

		pressedKeyIndex = index;
	}
}

void cAnimationValueChartWidget::mouseMoveEvent(QMouseEvent *event)
{
	if (pressedKeyIndex >= 0)
	{
		if (event->x() != dragStartX)
		{
			mouseDragStarted = true;
		}

		if (mouseDragStarted)
		{
			qDebug() << pressedKeyIndex << event->x() << event->y();
			emit update();
		}
	}
}

void cAnimationValueChartWidget::mouseReleaseEvent(QMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		if (pressedKeyIndex >= 0 && !mouseDragStarted)
		{
			emit update();
		}
		pressedKeyIndex = -1;
		mouseDragStarted = false;
	}
}

int cAnimationValueChartWidget::FindButtonAtPosition(int x, int y)
{
	for (int i = 0; i < keyButtons.size(); i++)
	{
		if (keyButtons[i].rect.contains(x, y))
		{
			return keyButtons[i].keyIndex;
		}
	}
	return -1; //-1 means nothing found
}
