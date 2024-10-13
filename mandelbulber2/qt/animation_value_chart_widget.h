/*
 * animation_value_chart_widget.h
 *
 *  Created on: 6 paź 2024
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_ANIMATION_VALUE_CHART_WIDGET_H_
#define MANDELBULBER2_QT_ANIMATION_VALUE_CHART_WIDGET_H_

#include <QWidget>
#include <vector>

class cAnimationValueChartWidget : public QWidget
{
	Q_OBJECT
public:
	cAnimationValueChartWidget(QWidget *parent = nullptr);
	~cAnimationValueChartWidget() override;

	class cAnimationPath
	{
	public:
		std::vector<double> values;
		std::vector<int> keyframeIndices;
	};

	void SetAnimationPath(const cAnimationPath &path);
	void resetZoom();

public slots:
	void slotZoomIn();
	void slotZoomOut();
	void slotSetCurrentFrame(int frame);

private:
	void paintEvent(QPaintEvent *event) override;

	cAnimationPath animationPath;
	float zoom = 1.0;
	int currentFrame = 0;
	double min = 0.0;
	double max = 1.0;

	const float margin = 0.1;
};

#endif /* MANDELBULBER2_QT_ANIMATION_VALUE_CHART_WIDGET_H_ */
