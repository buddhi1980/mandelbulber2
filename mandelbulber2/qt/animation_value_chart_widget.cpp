/*
 * nimation_value_chart_widget.cpp
 *
 *  Created on: 6 paź 2024
 *      Author: krzysztof
 */

#include "animation_value_chart_widget.h"
#include <QPainter>
#include "../src/system_data.hpp"

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

	QPen pen;

	// draw grid based on keyframe indices
	pen.setColor(Qt::gray);
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
	for (int i = 0; i < animationPath.keyframeIndices.size(); i++)
	{

		QPointF center = polyline[animationPath.keyframeIndices[i]];
		painter.drawEllipse(center, 3, 3);
	}

	//draw cursor
	pen.setColor(Qt::green);
	pen.setWidth(2);
	painter.setPen(pen);
	QPointF cursor((currentFrame - xShift) * zoom / animationPath.values.size() * width(), 0);
	painter.drawLine(cursor, QPointF(cursor.x(), height()));
}
void cAnimationValueChartWidget::SetAnimationPath(const cAnimationPath &path)
{
	animationPath = path;
	if (animationPath.values.size() > 0)
	{
		// find min and max values
		min = animationPath.values[0];
		max = animationPath.values[0];
		for (int i = 0; i < animationPath.values.size(); i++)
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
