/*
 * material_widget.h
 *
 *  Created on: 19 mar 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_MATERIAL_WIDGET_H_
#define MANDELBULBER2_SRC_MATERIAL_WIDGET_H_

#include "thumbnail_widget.h"

class cMaterialWidget: public cThumbnailWidget
{
public:
  cMaterialWidget(int _width, int _height, int _oversample, QWidget *parent);
  ~cMaterialWidget();
  void AssignParameters(const cParameterContainer &_params, const cFractalContainer &_fractal);
};

#endif /* MANDELBULBER2_SRC_MATERIAL_WIDGET_H_ */
