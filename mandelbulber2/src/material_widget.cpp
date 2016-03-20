/*
 * material_widget.cpp
 *
 *  Created on: 19 mar 2016
 *      Author: krzysztof
 */

#include "material_widget.h"

cMaterialWidget::cMaterialWidget(int _width, int _height, int _oversample, QWidget *parent) :
    cThumbnailWidget(_width, _height, _oversample, parent)
{

}

cMaterialWidget::~cMaterialWidget()
{
  // TODO Auto-generated destructor stub
}

