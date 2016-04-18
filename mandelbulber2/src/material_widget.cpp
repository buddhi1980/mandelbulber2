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
	// see destructor in parent cThumbnailWidget
}

void cMaterialWidget::AssignMaterial(cMaterial &_mat)
{
	// TODO read fract file of a default preview, or hardcode here
	// --> some sphere with maybe black and white tiled background

	// TODO assign to following calls
	cParameterContainer params;
	cFractalContainer fractal;

	// TODO assign material to sphere

	// call parent assignation
	// maybe disable preview saving, to not pollute hard drive?
	AssignParameters(params, fractal);
}
