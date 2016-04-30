/*
 * material_widget.cpp
 *
 *  Created on: 19 mar 2016
 *      Author: krzysztof
 */

#include "../qt/material_widget.h"
#include "../src/initparameters.hpp"

cMaterialWidget::cMaterialWidget(QWidget *parent) :
		cThumbnailWidget(128, 128, 1, parent)
{

}

cMaterialWidget::cMaterialWidget(int _width, int _height, int _oversample, QWidget *parent) :
		cThumbnailWidget(_width, _height, _oversample, parent)
{

}

cMaterialWidget::~cMaterialWidget()
{
	// see destructor in parent cThumbnailWidget
}

void cMaterialWidget::AssignMaterial(const cParameterContainer &_params, int materialIndex)
{
	cParameterContainer params;
	cFractalContainer fractal;

	params.SetContainerName("material");
	InitParams(&params);
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		fractal.at(i).SetContainerName(QString("fractal") + QString::number(i));
		InitFractalParams(&fractal.at(i));
	}
	InitMaterialParams(1, &params);

	//copy parameters from main parameter container to temporary container for material
	for(int i=0; i < cMaterial::paramsList.size(); i++)
	{
		cOneParameter parameter = _params.GetAsOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), materialIndex));
		params.SetFromOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), 1), parameter);
	}

	params.Set("julia_mode", true);
	params.Set("camera", CVector3(1.0, -2.0, 0.7));
	params.Set("mat1_texture_scale", CVector3(1.0, 1.0, 1.0));
	params.Set("mat1_displacement_texture_height", 0.01);
	params.Set("raytraced_reflections", true);

	// call parent assignation
	// maybe disable preview saving, to not pollute hard drive?
	AssignParameters(params, fractal);
}
