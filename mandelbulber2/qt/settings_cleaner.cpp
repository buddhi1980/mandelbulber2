/*
 * settings_cleaner.cpp
 *
 *  Created on: 1 lut 2020
 *      Author: krzysztof
 */

#include "settings_cleaner.h"

#include "ui_settings_cleaner.h"

#include "thumbnail_widget.h"

#include "src/cimage.hpp"
#include "src/fractal_container.hpp"
#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/parameters.hpp"
#include "src/progress_text.hpp"
#include "src/system.hpp"

cSettingsCleaner::cSettingsCleaner(QWidget *parent) : QDialog(parent), ui(new Ui::cSettingsCleaner)
{
	ui->setupUi(this);

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

	// render reference image
	ui->previewwidget_actual->AssignParameters(*gPar, *gParFractal);
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
	ui->previewwidget_cleaned->AssignParameters(*gPar, *gParFractal);
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
	cImage *actualImage = ui->previewwidget_actual->GetImage();
	cImage *cleanedImage = ui->previewwidget_cleaned->GetImage();
	double referenceNoise = cleanedImage->VisualCompare(actualImage, false);
	double referenceDistane = gMainInterface->GetDistanceForPoint(gPar->Get<CVector3>("camera"));

	QApplication::processEvents();

	// Creating list of all non default parameters

	QList<QPair<QString, cParameterContainer *>> listOfAllModifiedParameters;

	QList<QString> listOfMainParameters = gPar->GetListOfParameters();
	for (QString parameterName : listOfMainParameters)
	{
		if (!gPar->isDefaultValue(parameterName)
				&& gPar->GetParameterType(parameterName) == paramStandard)
		{
			// exceptions
			if (parameterName == "image_width") continue;
			if (parameterName == "image_height") continue;
			if (parameterName == "N") continue;
			if (parameterName == "DOF_samples") continue;
			if (parameterName == "DOF_min_samples") continue;
			if (parameterName == "DOF_max_noise") continue;
			if (parameterName == "DOF_monte_carlo") continue;
			if (parameterName == "image_proportion") continue;
			if (parameterName == "antialiasing_enabled") continue;
			if (parameterName == "antialiasing_size") continue;
			if (parameterName == "antialiasing_ocl_depth") continue;
			if (parameterName == "antialiasing_adaptive") continue;
			if (parameterName == "description") continue;
			if (parameterName.contains("animSound")) continue;
			if (parameterName == "camera_distance_to_target") continue;
			if (parameterName == "camera_rotation") continue;
			if (parameterName.contains("flight_")) continue;
			if (parameterName.contains("keyframe_")) continue;
			if (parameterName.contains("voxel_")) continue;
			if (parameterName.contains("mat") && parameterName.contains("is_defined")) continue;
			if (parameterName.contains("mat") && parameterName.contains("name")) continue;

			listOfAllModifiedParameters.append(
				QPair<QString, cParameterContainer *>(parameterName, gPar));
		}
	}
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		QList<QString> listOfFractalParameters = gParFractal->at(i).GetListOfParameters();
		for (QString parameterName : listOfFractalParameters)
		{
			if (!gParFractal->at(i).isDefaultValue(parameterName)
					&& gParFractal->at(i).GetParameterType(parameterName) == paramStandard)
			{
				listOfAllModifiedParameters.append(
					QPair<QString, cParameterContainer *>(parameterName, &gParFractal->at(i)));
			}
		}
	}

	// checking all listed parameters if can be defaulted

	cProgressText progressText;
	progressText.ResetTimer();
	int count = 0;

	for (QPair<QString, cParameterContainer *> par : listOfAllModifiedParameters)
	{
		if (stopRequest) break;

		cParameterContainer *container = par.second;
		QString parameterName = par.first;

		//	qDebug() << container->GetContainerName() << parameterName;

		cOneParameter settingBeforeClean = container->GetAsOneParameter(parameterName);

		cOneParameter settingCleaned = settingBeforeClean;
		settingCleaned.SetMultiVal(settingCleaned.GetMultiVal(valueDefault), valueActual);
		container->SetFromOneParameter(parameterName, settingCleaned);

		ui->label_parameterName->setText(
			tr("Trying parameter: %1:%2").arg(container->GetContainerName()).arg(parameterName));

		double distanceAfterCleaning =
			gMainInterface->GetDistanceForPoint(gPar->Get<CVector3>("camera"), gPar, gParFractal);
		bool changedDistance = false;
		if (distanceAfterCleaning < referenceDistane * 0.9
				|| distanceAfterCleaning > referenceDistane * 1.1)
		{
			changedDistance = true;
			// qDebug() << "changed distance" << referenceDistane << distanceAfterCleaning;
		}

		if (!changedDistance)
		{
			ui->previewwidget_cleaned->AssignParameters(*gPar, *gParFractal);
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
			QString itemText = QString("%1: %2 -> %3")
													 .arg(parameterName)
													 .arg(settingBeforeClean.Get<QString>(valueActual))
													 .arg(settingCleaned.Get<QString>(valueActual));
			ui->listWidget->addItem(itemText);
		}

		count++;
		double progress = double(count) / listOfAllModifiedParameters.size();
		QString progressBarText = progressText.getText(progress);
		ui->progressBar->setFormat(progressBarText);
		ui->progressBar->setValue(progress * 1000);

		QApplication::processEvents();
	}

	// render last version of image
	ui->previewwidget_cleaned->AssignParameters(*gPar, *gParFractal);
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

void cSettingsCleaner::closeEvent(QCloseEvent *event)
{
	event->accept();
	stopRequest = true;
}

void cSettingsCleaner::slotPressedStop()
{
	stopRequest = true;
}
