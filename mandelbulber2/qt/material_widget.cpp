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
#include "../src/settings.hpp"

int cMaterialWidget::previewWidth = 80;
int cMaterialWidget::previewHeight = 80;

cMaterialWidget::cMaterialWidget(QWidget *parent) :
		cThumbnailWidget(previewWidth, previewHeight, 2, parent)
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
	//DisableThumbnailCache();
	DisableTimer();

	timerPeriodicRefresh = new QTimer(parent());
	timerPeriodicRefresh->setSingleShot(true);
	connect(timerPeriodicRefresh, SIGNAL(timeout()), this, SLOT(slotPeriodicRender()));

	timerPeriodicUpdateData = new QTimer(parent());
	timerPeriodicUpdateData->setSingleShot(true);
	connect(timerPeriodicUpdateData, SIGNAL(timeout()), this, SLOT(slotPeriodicUpdateData()));
	timerPeriodicUpdateData->start(1000);

	connect(this, SIGNAL(settingsChanged()), this, SLOT(slotMaterialChanged()));
	actualMaterialIndex = 0;
	setMinimumSize(previewWidth, previewHeight);
	initialized = false;
	dataAssigned = false;
	timeUpdateData = 0;
	timeAssingData = 0;
}

cMaterialWidget::~cMaterialWidget()
{
	// see destructor in parent cThumbnailWidget
}

void cMaterialWidget::AssignMaterial(cParameterContainer *_params, int materialIndex, QWidget *_materialEditorWidget)
{
	paramsHandle = _params;
	paramsCopy = *_params;
	actualMaterialIndex = materialIndex;
	materialEditorWidget = _materialEditorWidget;
	dataAssigned = true;
	initialized = false;
}

void cMaterialWidget::InitializeData()
{
	if(dataAssigned && !initialized)
	{
		QElapsedTimer timer;
		timer.start();

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
			cOneParameter parameter = paramsCopy.GetAsOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), actualMaterialIndex));
			params.SetFromOneParameter(cMaterial::Name(cMaterial::paramsList.at(i), 1), parameter);
		}

		params.Set("camera", CVector3(1.5, -2.5, 0.7));
		params.Set("raytraced_reflections", true);
		params.Set("N", 10);
		params.Set("detail_level", 0.2);
		params.Set("smoothness", 5.0);
		fractal.at(0).Set("power", 5);
		params.Set("julia_mode", true);
		params.Set("textured_background", true);
		params.Set("file_background", QDir::toNativeSeparators(systemData.sharedDir + "textures" + QDir::separator() + "grid.png"));
		params.Set("mat1_texture_scale", CVector3(1.0, 1.0, 1.0));
		params.Set("mat1_displacement_texture_height", 0.01);
		params.Set("main_light_intensity", 1.2);
		params.Set("shadows_enabled", false);

		// call parent assignation
		// maybe disable preview saving, to not pollute hard drive?
		AssignParameters(params, fractal);
		update();
		timeAssingData = timer.nsecsElapsed() / 1e9;

		if(materialEditorWidget)
		{
			int time = (timeUpdateData + timeAssingData + lastRenderTime) * 2000.0 + 1;
			//qDebug() << timeAssingData << timeUpdateData << lastRenderTime << time;
			timerPeriodicRefresh->start(time);
		}
		initialized = true;
	}
}

void cMaterialWidget::slotPeriodicRender(void)
{
	if(!visibleRegion().isEmpty())
	{
		QElapsedTimer timer;
		timer.start();
		if(paramsHandle && materialEditorWidget)
		{
			SynchronizeInterfaceWindow(materialEditorWidget, paramsHandle, qInterface::read);
			AssignMaterial(paramsHandle, actualMaterialIndex, materialEditorWidget);
		}
		update();
		timeUpdateData = timer.nsecsElapsed() / 1e9;
	}
	if(!timerPeriodicRefresh->isActive())
	{
		int time = (timeUpdateData + timeAssingData + lastRenderTime) * 2000.0 + 1;
		if(time > 10000) time = 10000;

		timerPeriodicRefresh->start(time);
	}
}

void cMaterialWidget::slotPeriodicUpdateData(void)
{
	if(!visibleRegion().isEmpty())
	{
		InitializeData();
	}
	timerPeriodicUpdateData->start(500);
}

QSize cMaterialWidget::sizeHint() const
{
	return QSize(previewWidth, previewHeight);
}

void cMaterialWidget::AssignMaterial(const QString &text, int materialIndex)
{
	cSettings settings(cSettings::formatCondensedText);
	settings.LoadFromString(text);
	cParameterContainer params;
	InitMaterialParams(materialIndex, &params);
	settings.Decode(&params, NULL);
	AssignMaterial(&params, materialIndex);
}

void cMaterialWidget::slotMaterialChanged()
{
	emit materialChanged(actualMaterialIndex);
}

void cMaterialWidget::mousePressEvent(QMouseEvent* event)
{
	emit clicked(event->button());
	QWidget::mousePressEvent(event);
}
