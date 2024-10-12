/*
 * nimation_value_chart_widget.cpp
 *
 *  Created on: 6 paź 2024
 *      Author: krzysztof
 */

#include "animation_value_chart_widget.h"
#include <QPainter>

cAnimationValueChartWidget::cAnimationValueChartWidget(QWidget *parent) : QWidget(parent)
{
	setFixedHeight(100);
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

	QPen pen;

	// draw grid based on keyframe indices
	pen.setColor(Qt::gray);
	pen.setWidth(1);
	painter.setPen(pen);
	for (int i = 0; i < animationPath.keyframeIndices.size(); i++)
	{
		float x = animationPath.keyframeIndices[i] * zoom / animationPath.values.size() * width();
		painter.drawLine(x, 0, x, height());
	}

	pen.setColor(Qt::black);
	pen.setWidth(1);
	painter.setPen(pen);

	QPolygonF polyline;
	for (int i = 0; i < animationPath.values.size(); i++)
	{
		if (min != max)
		{
			// draw line
			QPointF point(i * zoom / animationPath.values.size() * width(),
				height() * (max - animationPath.values[i]) / (max - min));
			polyline << point;
		}
		else
		{
			// draw straight line
			QPointF point(i * zoom / animationPath.values.size() * width(), height() / 2);
			polyline << point;
		}
	}
	painter.drawPolyline(polyline);
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
