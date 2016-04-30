/*
 * material_widget.h
 *
 *  Created on: 19 mar 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_QT_MATERIAL_WIDGET_H_
#define MANDELBULBER2_QT_MATERIAL_WIDGET_H_

#include "thumbnail_widget.h"

class cMaterialWidget: public cThumbnailWidget
{
public:
  cMaterialWidget(QWidget *parent);
	cMaterialWidget(int _width, int _height, int _oversample, QWidget *parent);
  ~cMaterialWidget();
	void AssignMaterial(const cParameterContainer &_params, int materialIndex);
};

#endif /* MANDELBULBER2_QT_MATERIAL_WIDGET_H_ */
