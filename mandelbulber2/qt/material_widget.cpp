/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * cMaterialWidget - promoted cThumbnailWidget for displaying a material in a widget
 *
 * This class periodically checks for changes in assigned material and rerenders
 * the material on a "default" mandelbulb fractal.
 * See also other functionality inherited from cThumbnailWidget.
 */

#include "material_widget.h"

#include <QMouseEvent>

#include "src/fractal_container.hpp"
#include "src/fractal_enums.h"
#include "src/fractparams.hpp"
#include "src/global_data.hpp"
#include "src/initparameters.hpp"
#include "src/material.h"
#include "src/settings.hpp"
#include "src/synchronize_interface.hpp"
#include "src/system.hpp"

cMaterialWidget::cMaterialWidget(QWidget *parent)
		: cThumbnailWidget(
				systemData.GetPreferredThumbnailSize(), systemData.GetPreferredThumbnailSize(), 2, parent)
{
	Init();
}

cMaterialWidget::cMaterialWidget(int _width, int _height, int _oversample, QWidget *parent)
		: cThumbnailWidget(_width, _height, _oversample, parent)
{
	Init();
}

void cMaterialWidget::Init()
{
	paramsHandle = nullptr;
	// DisableThumbnailCache();
	DisableTimer();

	previewWidth = systemData.GetPreferredThumbnailSize();
	previewHeight = systemData.GetPreferredThumbnailSize();

	timerPeriodicRefresh = new QTimer(parent());
	timerPeriodicRefresh->setSingleShot(true);
	connect(timerPeriodicRefresh, SIGNAL(timeout()), this, SLOT(slotPeriodicRender()));

	timerPeriodicUpdateData = new QTimer(parent());
	timerPeriodicUpdateData->setSingleShot(true);
	connect(timerPeriodicUpdateData, SIGNAL(timeout()), this, SLOT(slotPeriodicUpdateData()));
	timerPeriodicUpdateData->start(100);

	connect(this, SIGNAL(settingsChanged()), this, SLOT(slotMaterialChanged()));
	actualMaterialIndex = 0;
	setMinimumSize(previewWidth, previewHeight);
	initialized = false;
	dataAssigned = false;
	timeUpdateData = 0;
	timeAssignData = 0;
}

cMaterialWidget::~cMaterialWidget()
{
	// see destructor in parent cThumbnailWidget
}

void cMaterialWidget::AssignMaterial(
	cParameterContainer *_params, int materialIndex, QWidget *_materialEditorWidget)
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
	if (dataAssigned && !initialized)
	{
		QElapsedTimer timerAssignData;
		timerAssignData.start();

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

		if (paramsCopy.IfExists(cMaterial::Name("is_defined", actualMaterialIndex)))
		{

			// copy parameters from main parameter container to temporary container for material
			for (int i = 0; i < cMaterial::paramsList.size(); i++)
			{
				cOneParameter parameter = paramsCopy.GetAsOneParameter(
					cMaterial::Name(cMaterial::paramsList.at(i), actualMaterialIndex));
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
			params.Set("file_background", QDir::toNativeSeparators(systemData.sharedDir + "textures"
																														 + QDir::separator() + "grid.png"));
			params.Set("mat1_texture_scale", CVector3(1.0, 1.0, 1.0));
			params.Set("mat1_displacement_texture_height", 0.01);
			params.Set("main_light_intensity", 1.2);
			params.Set("shadows_enabled", false);
		}
		else
		{
			params.Set("camera", CVector3(1.5, -2.5, 0.7));
			params.Set("fractal_enable_1", false);
			params.Set("textured_background", true);
			params.Set("file_background",
				QDir::toNativeSeparators(
					systemData.sharedDir + "textures" + QDir::separator() + "material is not defined.png"));
			params.Set("textured_background_map_type", int(params::mapFlat));
		}

		// call parent assignation
		// maybe disable preview saving, to not pollute hard drive?
		AssignParameters(params, fractal);
		update();
		timeAssignData = timerAssignData.nsecsElapsed() / 1e9;

		if (materialEditorWidget)
		{
			int time = int((timeUpdateData + timeAssignData + lastRenderTime) * 2000.0 + 1);
			// qDebug() << timeAssignData << timeUpdateData << lastRenderTime << time;
			timerPeriodicRefresh->start(time);
		}
		initialized = true;
	}
}

void cMaterialWidget::slotPeriodicRender()
{
	if (!visibleRegion().isEmpty())
	{
		QElapsedTimer elapsedTimer;
		elapsedTimer.start();
		if (paramsHandle && materialEditorWidget)
		{
			SynchronizeInterfaceWindow(materialEditorWidget, paramsHandle, qInterface::read);
			AssignMaterial(paramsHandle, actualMaterialIndex, materialEditorWidget);
		}
		update();
		timeUpdateData = elapsedTimer.nsecsElapsed() / 1e9;
	}
	if (!timerPeriodicRefresh->isActive())
	{
		int time = int((timeUpdateData + timeAssignData + lastRenderTime) * 2000.0 + 1);
		if (time > 10000) time = 10000;

		timerPeriodicRefresh->start(time);
	}
}

void cMaterialWidget::slotPeriodicUpdateData()
{
	if (!visibleRegion().isEmpty())
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
	settings.Decode(&params, nullptr);
	AssignMaterial(&params, materialIndex);
}

void cMaterialWidget::slotMaterialChanged()
{
	emit materialChanged(actualMaterialIndex);
}

void cMaterialWidget::mousePressEvent(QMouseEvent *event)
{
	emit clicked(event->button());
	QWidget::mousePressEvent(event);
}
