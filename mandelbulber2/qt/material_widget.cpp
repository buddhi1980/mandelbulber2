/*
 * material_widget.cpp
 *
 *  Created on: 19 mar 2016
 *      Author: krzysztof
 */

#include "../qt/material_widget.h"
#include "../src/initparameters.hpp"
#include "../src/synchronize_interface.hpp"
#include "../src/system.hpp"

cMaterialWidget::cMaterialWidget(QWidget *parent) :
		cThumbnailWidget(128, 128, 2, parent)
{
	Init();
}

cMaterialWidget::cMaterialWidget(int _width, int _height, int _oversample, QWidget *parent) :
		cThumbnailWidget(_width, _height, _oversample, parent)
{
	Init();
}

void cMaterialWidget::Init()
{
	paramsHandle = NULL;
	DisableThumbnailCache();
	DisableTimer();

	timerPeriodicRefresh = new QTimer(parent());
	timerPeriodicRefresh->setSingleShot(true);
	connect(timerPeriodicRefresh, SIGNAL(timeout()), this, SLOT(slotPeriodicRender()));
	lastMaterialIndex = 0;
}

cMaterialWidget::~cMaterialWidget()
{
	// see destructor in parent cThumbnailWidget
}

void cMaterialWidget::AssignMaterial(cParameterContainer *_params, int materialIndex, QWidget *_materialEditorWidget = NULL)
{
	if(paramsHandle == NULL)
	{
		paramsHandle = _params;
		lastMaterialIndex = materialIndex;
		materialEditorWidget = _materialEditorWidget;
	}

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
		cOneParameter parameter = _params->GetAsOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), materialIndex));
		params.SetFromOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), 1), parameter);
	}

	params.Set("camera", CVector3(1.5, -2.5, 0.7));
	params.Set("raytraced_reflections", true);
	params.Set("N", 10);
	params.Set("detail_level", 0.1);
	params.Set("smoothness", 1.0);
	fractal.at(0).Set("power", 5);
	params.Set("textured_background", true);
	params.Set("file_background", QDir::toNativeSeparators(systemData.sharedDir + "textures" + QDir::separator() + "grid.png"));
	params.Set("mat1_texture_scale", CVector3(1.0, 1.0, 1.0));
	params.Set("mat1_displacement_texture_height", 0.01);
	params.Set("main_light_intensity", 1.2);
	params.Set("shadows_enabled", false);

	// call parent assignation
	// maybe disable preview saving, to not pollute hard drive?
	AssignParameters(params, fractal);

	if(materialEditorWidget)
	{
		timerPeriodicRefresh->start(1000);
	}
}

void cMaterialWidget::slotPeriodicRender(void)
{
	if(!visibleRegion().isEmpty())
	{
		if(materialEditorWidget)
		{
			SynchronizeInterfaceWindow(materialEditorWidget, paramsHandle, interface::read);
		}

		if(paramsHandle)
		{
			AssignMaterial(paramsHandle, lastMaterialIndex, materialEditorWidget);
		}
		update();
	}
	if(!timerPeriodicRefresh->isActive())
	{
		timerPeriodicRefresh->start(1000);
	}
}
