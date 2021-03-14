/*
 * light_widget.h
 *
 *  Created on: 13 mar 2021
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_LIGHT_WIDGET_H_
#define MANDELBULBER2_QT_LIGHT_WIDGET_H_

#include <QWidget>
#include <memory>

#include "src/algebra.hpp"
#include "src/color_structures.hpp"

class cLightWidget : public QWidget
{
	Q_OBJECT
public:
	cLightWidget(QWidget *parent = nullptr);
	~cLightWidget() override;
	void SetLightParameters(CVector3 _angle, sRGB8 _color);

private:
	void paintEvent(QPaintEvent *event) override;

	CVector3 lightAngle;
	sRGB8 lightColor;
	std::vector<sRGB8> image8;
	int size = 0;
};

#endif /* MANDELBULBER2_QT_LIGHT_WIDGET_H_ */
