/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2020 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * TODO: description
 */

#include "settings_cleaner.h"

#include <memory>

#include "ui_settings_cleaner.h"

#include "thumbnail_widget.h"

#include "src/cimage.hpp"
#include "src/fractal_container.hpp"
#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/parameters.hpp"
#include "src/progress_text.hpp"
#include "src/system_data.hpp"
#include "src/wait.hpp"

cSettingsCleaner::cSettingsCleaner(QWidget *parent) : QDialog(parent), ui(new Ui::cSettingsCleaner)
{
	ui->setupUi(this);

	actualParams.reset(new cParameterContainer());
	actualFractalParams.reset(new cFractalContainer());

	setModal(true);

	int baseSize = int(systemData.GetPreferredThumbnailSize());
	int sizeMultiply = 2.0;
	int previewWidth = sizeMultiply * baseSize * 4 / 3;
	int previewHeight = sizeMultiply * baseSize;

	ui->previewwidget_actual->SetSize(previewWidth, previewHeight, 1);
	ui->previewwidget_actual->DisableThumbnailCache();
	ui->previewwidget_actual->DisableTimer();
	ui->previewwidget_actual->DisableRenderOnPaint();
	ui->previewwidget_cleaned->SetSize(previewWidth, previewHeight, 1);
	ui->previewwidget_cleaned->DisableThumbnailCache();
	ui->previewwidget_cleaned->DisableTimer();
	ui->previewwidget_cleaned->DisableRenderOnPaint();

	connect(ui->pushButton_stop, &QPushButton::clicked, this, &cSettingsCleaner::slotPressedStop);
	connect(ui->pushButton_ok, &QPushButton::clicked, this, &cSettingsCleaner::slotPressedOK);
	connect(ui->pushButton_cancel, &QPushButton::clicked, this, &cSettingsCleaner::slotPressedCancel);
	connect(ui->listWidget, &QListWidget::itemDoubleClicked, this, &cSettingsCleaner::slotRemoveItem);
}

cSettingsCleaner::~cSettingsCleaner()
{
	delete ui;
}

void cSettingsCleaner::runCleaner()
{
	Wait(100);
	QApplication::processEvents();

	ui->progressBar->setFormat(tr("Initializing"));
	ui->progressBar->setValue(0.0);

	*actualParams = *gPar;
	*actualFractalParams = *gParFractal;

	// render reference image
	ui->previewwidget_actual->AssignParameters(actualParams, actualFractalParams);
	ui->previewwidget_actual->update();

	if (!ui->previewwidget_actual->IsRendered())
	{
		ui->previewwidget_actual->slotRender();
		while (!ui->previewwidget_actual->IsRendered())
		{
			QApplication::processEvents();
			Wait(10);
		}
	}

	// render the same settings to check noise level
	ui->previewwidget_cleaned->AssignParameters(actualParams, actualFractalParams);
	ui->previewwidget_cleaned->update();
	if (!ui->previewwidget_cleaned->IsRendered())
	{
		ui->previewwidget_cleaned->slotRender();
		while (!ui->previewwidget_cleaned->IsRendered())
		{
			QApplication::processEvents();
			Wait(10);
		}
	}
	std::shared_ptr<cImage> actualImage = ui->previewwidget_actual->GetImage();
	std::shared_ptr<cImage> cleanedImage = ui->previewwidget_cleaned->GetImage();
	double referenceNoise = cleanedImage->VisualCompare(actualImage, false);
	double referenceDistane =
		gMainInterface->GetDistanceForPoint(actualParams->Get<CVector3>("camera"));

	QApplication::processEvents();

	// Creating list of all non default parameters

	QList<sDefaultedParameter> listOfAllModifiedParameters;

	QList<QString> listOfMainParameters = actualParams->GetListOfParameters();
	for (QString parameterName : listOfMainParameters)
	{
		if (!actualParams->isDefaultValue(parameterName)
				&& actualParams->GetParameterType(parameterName) == paramStandard)
		{
			// exceptions
			if (parameterName == "image_width") continue;
			if (parameterName == "image_height") continue;
			if (parameterName == "N") continue;
			if (parameterName == "reflections_max") continue;
			if (parameterName == "DOF_samples") continue;
			if (parameterName == "DOF_min_samples") continue;
			if (parameterName == "DOF_max_noise") continue;
			if (parameterName == "DOF_monte_carlo") continue;
			if (parameterName == "image_proportion") continue;
			if (parameterName == "MC_soft_shadows_enable") continue;
			if (parameterName == "antialiasing_enabled") continue;
			if (parameterName == "antialiasing_size") continue;
			if (parameterName == "antialiasing_ocl_depth") continue;
			if (parameterName == "antialiasing_adaptive") continue;
			if (parameterName == "description") continue;
			if (parameterName.contains("animSound")) continue;
			if (parameterName == "camera_distance_to_target") continue;
			if (parameterName == "camera_rotation") continue;
			if (parameterName == "view_distance_max") continue;
			if (parameterName == "stereo_mode") continue;
			if (parameterName == "formula_code") continue;
			if (parameterName.contains("flight_")) continue;
			if (parameterName.contains("keyframe_")) continue;
			if (parameterName.contains("voxel_")) continue;
			if (parameterName.contains("mat") && parameterName.contains("is_defined")) continue;
			if (parameterName.contains("mat") && parameterName.contains("name")) continue;
			if (parameterName.contains("mat") && parameterName.contains("rough_surface")) continue;
			if (parameterName.contains("mat") && parameterName.contains("use_colors_from_palette"))
				continue;

			sDefaultedParameter par;
			par.parameterName = parameterName;
			par.actualContainer = actualParams;
			par.originalContainer = gPar;
			listOfAllModifiedParameters.append(par);
		}
	}
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		QList<QString> listOfFractalParameters = actualFractalParams->at(i)->GetListOfParameters();
		for (QString parameterName : listOfFractalParameters)
		{
			if (!actualFractalParams->at(i)->isDefaultValue(parameterName)
					&& actualFractalParams->at(i)->GetParameterType(parameterName) == paramStandard)
			{
				sDefaultedParameter par;
				par.parameterName = parameterName;
				par.actualContainer = actualFractalParams->at(i);
				par.originalContainer = gParFractal->at(i);
				listOfAllModifiedParameters.append(par);
			}
		}
	}

	// checking all listed parameters if can be defaulted

	cProgressText progressText;
	progressText.ResetTimer();
	int count = 0;

	for (sDefaultedParameter par : listOfAllModifiedParameters)
	{
		if (stopRequest) break;

		std::shared_ptr<cParameterContainer> container = par.actualContainer;
		QString parameterName = par.parameterName;

		//	qDebug() << container->GetContainerName() << parameterName;

		cOneParameter settingBeforeClean = container->GetAsOneParameter(parameterName);

		cOneParameter settingCleaned = settingBeforeClean;
		settingCleaned.SetMultiVal(settingCleaned.GetMultiVal(valueDefault), valueActual);
		container->SetFromOneParameter(parameterName, settingCleaned);

		ui->label_parameterName->setText(
			tr("Trying parameter: %1:%2").arg(container->GetContainerName()).arg(parameterName));

		double distanceAfterCleaning = gMainInterface->GetDistanceForPoint(
			actualParams->Get<CVector3>("camera"), actualParams, actualFractalParams);
		bool changedDistance = false;
		if (distanceAfterCleaning < referenceDistane * 0.9
				|| distanceAfterCleaning > referenceDistane * 1.1)
		{
			changedDistance = true;
			// qDebug() << "changed distance" << referenceDistane << distanceAfterCleaning;
		}

		if (!changedDistance)
		{
			ui->previewwidget_cleaned->AssignParameters(actualParams, actualFractalParams);
			QApplication::processEvents();
			ui->previewwidget_cleaned->slotRender();
			QApplication::processEvents();
			while (!ui->previewwidget_cleaned->IsRendered())
			{
				QApplication::processEvents();
				Wait(10);
			}
		}

		cleanedImage = ui->previewwidget_cleaned->GetImage();
		double diff = cleanedImage->VisualCompare(actualImage, false);

		if (diff > 1.5 * referenceNoise || changedDistance)
		{
			container->SetFromOneParameter(parameterName, settingBeforeClean);
		}
		else
		{
			QString itemText = QString("%1_%2: %3 -> %4")
													 .arg(container->GetContainerName())
													 .arg(parameterName)
													 .arg(settingBeforeClean.Get<QString>(valueActual))
													 .arg(settingCleaned.Get<QString>(valueActual));
			ui->listWidget->addItem(itemText);
			listOfAllDefaultedParameters.append(par);
		}

		count++;
		double progress = double(count) / listOfAllModifiedParameters.size();
		QString progressBarText = progressText.getText(progress);
		ui->progressBar->setFormat(progressBarText);
		ui->progressBar->setValue(progress * 1000);

		QApplication::processEvents();
	}

	// render last version of image
	ui->previewwidget_cleaned->AssignParameters(actualParams, actualFractalParams);
	ui->previewwidget_cleaned->update();
	if (!ui->previewwidget_cleaned->IsRendered())
	{
		ui->previewwidget_cleaned->slotRender();
		while (!ui->previewwidget_cleaned->IsRendered())
		{
			QApplication::processEvents();
			Wait(10);
		}
	}

	ui->pushButton_stop->setDisabled(true);
}

void cSettingsCleaner::closeEvent(QCloseEvent *event)
{
	event->accept();
	stopRequest = true;
}

void cSettingsCleaner::slotPressedStop()
{
	stopRequest = true;
}

void cSettingsCleaner::slotPressedOK()
{
	*gPar = *actualParams;
	*gParFractal = *actualFractalParams;
	close();
}
void cSettingsCleaner::slotPressedCancel()
{
	close();
}

void cSettingsCleaner::slotRemoveItem(QListWidgetItem *item)
{
	int row = ui->listWidget->row(item);
	delete item;

	sDefaultedParameter defaultedParameter = listOfAllDefaultedParameters.at(row);

	defaultedParameter.actualContainer->SetFromOneParameter(defaultedParameter.parameterName,
		defaultedParameter.originalContainer->GetAsOneParameter(defaultedParameter.parameterName));

	listOfAllDefaultedParameters.removeAt(row);

	ui->previewwidget_cleaned->AssignParameters(actualParams, actualFractalParams);
	ui->previewwidget_cleaned->update();
	if (!ui->previewwidget_cleaned->IsRendered())
	{
		ui->previewwidget_cleaned->slotRender();
		while (!ui->previewwidget_cleaned->IsRendered())
		{
			QApplication::processEvents();
			Wait(10);
		}
	}
}
