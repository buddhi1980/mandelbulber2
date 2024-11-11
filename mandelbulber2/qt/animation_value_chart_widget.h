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
		std::vector<bool> emptyKeyframes;
		QString parameterName;
	};

	void SetAnimationPath(const cAnimationPath &path);
	void resetZoom();

private:
	struct sKeyframeButton
	{
		int keyIndex;
		QRect rect;
	};

public slots:
	void slotZoomIn();
	void slotZoomOut();
	void slotSetCurrentFrame(int frame);

private:
	void paintEvent(QPaintEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;

	int FindButtonAtPosition(int x, int y);

	cAnimationPath animationPath;
	float zoom = 1.0;
	int currentFrame = 0;
	double min = 0.0;
	double max = 1.0;

	const float margin = 0.1;

	QList<sKeyframeButton> keyButtons;

	int dragStartX = -1;
	int dragStartY = -1;
	int pressedKeyIndex = -1;
	bool mouseDragStarted = false;
};

#endif /* MANDELBULBER2_QT_ANIMATION_VALUE_CHART_WIDGET_H_ */
