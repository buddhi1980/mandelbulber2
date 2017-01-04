/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Widget which contains UI for image adjustments
 */

#include "../src/automated_widgets.hpp"
#include "../src/fractal_container.hpp"
#include "../src/fractparams.hpp"
#include "../src/initparameters.hpp"
#include "../src/interface.hpp"
#include "../src/projection_3d.hpp"
#include "../src/render_window.hpp"
#include "dock_image_adjustments.h"
#include "dock_rendering_engine.h"
#include "ui_dock_image_adjustments.h"

cDockImageAdjustments::cDockImageAdjustments(QWidget *parent)
		: QWidget(parent), ui(new Ui::cDockImageAdjustments)
{
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	ConnectSignals();
}

cDockImageAdjustments::~cDockImageAdjustments()
{
	delete ui;
}

void cDockImageAdjustments::slotDisableAutoRefresh()
{
	gMainInterface->DisablePeriodicRefresh();
}

void cDockImageAdjustments::ConnectSignals() const
{
	connect(ui->comboBox_perspective_type, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotChangedComboPerspectiveType(int)));
	connect(ui->pushButton_apply_image_changes, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonImageApply()));
	// image resolution
	connect(ui->comboBox_image_proportion, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotChangedComboImageProportion(int)));
	connect(ui->pushButton_resolution_preset_1080, SIGNAL(clicked()), this,
		SLOT(slotPressedResolutionPreset()));
	connect(ui->pushButton_resolution_preset_1200, SIGNAL(clicked()), this,
		SLOT(slotPressedResolutionPreset()));
	connect(ui->pushButton_resolution_preset_1440, SIGNAL(clicked()), this,
		SLOT(slotPressedResolutionPreset()));
	connect(ui->pushButton_resolution_preset_2160, SIGNAL(clicked()), this,
		SLOT(slotPressedResolutionPreset()));
	connect(ui->pushButton_resolution_preset_240, SIGNAL(clicked()), this,
		SLOT(slotPressedResolutionPreset()));
	connect(ui->pushButton_resolution_preset_4320, SIGNAL(clicked()), this,
		SLOT(slotPressedResolutionPreset()));
	connect(ui->pushButton_resolution_preset_480, SIGNAL(clicked()), this,
		SLOT(slotPressedResolutionPreset()));
	connect(ui->pushButton_resolution_preset_600, SIGNAL(clicked()), this,
		SLOT(slotPressedResolutionPreset()));
	connect(ui->pushButton_resolution_preset_720, SIGNAL(clicked()), this,
		SLOT(slotPressedResolutionPreset()));
	connect(ui->pushButton_imagesize_increase, SIGNAL(clicked()), this,
		SLOT(slotPressedImagesizeIncrease()));
	connect(ui->pushButton_imagesize_decrease, SIGNAL(clicked()), this,
		SLOT(slotPressedImagesizeDecrease()));
	connect(ui->spinboxInt_image_height, SIGNAL(valueChanged(int)), this,
		SLOT(slotImageHeightChanged(int)));
	connect(ui->checkBox_connect_detail_level, SIGNAL(stateChanged(int)), this,
		SLOT(slotCheckedDetailLevelLock(int)));

	// quality presets
	connect(ui->pushButton_quality_preset_very_low, SIGNAL(clicked()), this,
		SLOT(slotQualityPresetVeryLow()));
	connect(ui->pushButton_quality_preset_low, SIGNAL(clicked()), this, SLOT(slotQualityPresetLow()));
	connect(
		ui->pushButton_quality_preset_normal, SIGNAL(clicked()), this, SLOT(slotQualityPresetNormal()));
	connect(
		ui->pushButton_quality_preset_high, SIGNAL(clicked()), this, SLOT(slotQualityPresetHigh()));

	connect(
		ui->spinbox_brightness, SIGNAL(valueChanged(double)), this, SLOT(slotDisableAutoRefresh()));
	connect(ui->spinbox_contrast, SIGNAL(valueChanged(double)), this, SLOT(slotDisableAutoRefresh()));
	connect(ui->spinbox_gamma, SIGNAL(valueChanged(double)), this, SLOT(slotDisableAutoRefresh()));
	connect(ui->checkBox_hdr, SIGNAL(stateChanged(int)), this, SLOT(slotDisableAutoRefresh()));
}

void cDockImageAdjustments::slotChangedComboImageProportion(int index) const
{
	bool enableSlider = false;
	double ratio = 1.0;
	enumImageProportion proportionSelection = enumImageProportion(index);

	switch (proportionSelection)
	{
		case proportionFree: enableSlider = true; break;
		case proportion1_1: ratio = 1.0; break;
		case proportion4_3: ratio = 4.0 / 3.0; break;
		case proportion3_2: ratio = 3.0 / 2.0; break;
		case proportion16_9: ratio = 16.0 / 9.0; break;
		case proportion16_10: ratio = 16.0 / 10.0; break;
		case proportion2_1: ratio = 2.0 / 1.0; break;
		default: ratio = 1.0; break;
	}

	ui->sliderInt_image_width->setEnabled(enableSlider);
	ui->spinboxInt_image_width->setEnabled(enableSlider);

	int height = ui->spinboxInt_image_height->value();
	int width = height * ratio;

	if (!enableSlider)
	{
		ui->spinboxInt_image_width->setValue(width);
	}

	if (ui->checkBox_connect_detail_level->isChecked())
	{
		double sizeRatio = double(height) / gMainInterface->lockedImageResolution.y;
		gPar->Set("detail_level", gMainInterface->lockedDetailLevel / sizeRatio);
		gMainInterface->mainWindow->GetWidgetDockRenderingEngine()
			->SynchronizeInterfaceDistanceEstimation(gPar);
	}
}

void cDockImageAdjustments::slotPressedResolutionPreset() const
{
	int width = 0, height = 0;
	enumImageProportion proportion = proportionFree;
	QString buttonName = this->sender()->objectName();
	if (buttonName == QString("pushButton_resolution_preset_480"))
	{
		width = 720;
		height = 480;
		proportion = proportion3_2;
	}
	else if (buttonName == QString("pushButton_resolution_preset_720"))
	{
		width = 1280;
		height = 720;
		proportion = proportion16_9;
	}
	else if (buttonName == QString("pushButton_resolution_preset_1080"))
	{
		width = 1920;
		height = 1080;
		proportion = proportion16_9;
	}
	else if (buttonName == QString("pushButton_resolution_preset_1440"))
	{
		width = 2560;
		height = 1440;
		proportion = proportion16_9;
	}
	else if (buttonName == QString("pushButton_resolution_preset_2160"))
	{
		width = 4096;
		height = 2160;
		proportion = proportionFree;
	}
	else if (buttonName == QString("pushButton_resolution_preset_4320"))
	{
		width = 7680;
		height = 4320;
		proportion = proportion16_9;
	}
	else if (buttonName == QString("pushButton_resolution_preset_240"))
	{
		width = 320;
		height = 240;
		proportion = proportion4_3;
	}
	else if (buttonName == QString("pushButton_resolution_preset_600"))
	{
		width = 800;
		height = 600;
		proportion = proportion4_3;
	}
	else if (buttonName == QString("pushButton_resolution_preset_1200"))
	{
		width = 1600;
		height = 1200;
		proportion = proportion4_3;
	}

	ui->spinboxInt_image_width->setValue(width);
	ui->spinboxInt_image_height->setValue(height);
	ui->comboBox_image_proportion->setCurrentIndex(proportion);

	if (ui->checkBox_connect_detail_level->isChecked())
	{
		double sizeRatio = double(height) / gMainInterface->lockedImageResolution.y;
		gPar->Set("detail_level", gMainInterface->lockedDetailLevel / sizeRatio);
		gMainInterface->mainWindow->GetWidgetDockRenderingEngine()
			->SynchronizeInterfaceDistanceEstimation(gPar);
	}
}

void cDockImageAdjustments::slotChangedComboPerspectiveType(int index) const
{
	params::enumPerspectiveType perspType = params::enumPerspectiveType(index);
	if (perspType == params::perspFishEyeCut)
	{
		ui->comboBox_image_proportion->setCurrentIndex(proportion1_1);
		ui->spinbox_fov->setValue(1.0);
	}
	else if (perspType == params::perspEquirectangular)
	{
		ui->comboBox_image_proportion->setCurrentIndex(proportion2_1);
		ui->spinbox_fov->setValue(1.0);
	}
}

void cDockImageAdjustments::slotImageHeightChanged(int value) const
{
	(void)value;
	int index = ui->comboBox_image_proportion->currentIndex();
	slotChangedComboImageProportion(index);
}

void cDockImageAdjustments::slotCheckedDetailLevelLock(int state) const
{
	if (state)
	{
		gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::read);
		gMainInterface->lockedDetailLevel = gPar->Get<double>("detail_level");
		gMainInterface->lockedImageResolution =
			CVector2<int>(gPar->Get<int>("image_width"), gPar->Get<int>("image_height"));
	}

	if (this->sender()->objectName() == "checkBox_connect_detail_level")
	{
		gMainInterface->mainWindow->GetWidgetDockRenderingEngine()
			->CheckboxConnectDetailLevelSetCheckState(Qt::CheckState(state));
	}
	else
	{
		ui->checkBox_connect_detail_level->setCheckState(Qt::CheckState(state));
	}
}

void cDockImageAdjustments::slotQualityPresetVeryLow()
{
	gPar->Set("DE_factor", 1.0);
	gPar->Set("ambient_occlusion_enabled", false);
	gPar->Set("shadows_enabled", false);
	gPar->Set("raytraced_reflections", false);
	gPar->Set("detail_level", 0.5);
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
}
void cDockImageAdjustments::slotQualityPresetLow()
{
	gPar->Set("DE_factor", 1.0);
	gPar->Set("ambient_occlusion_enabled", false);
	gPar->Set("shadows_enabled", true);
	gPar->Set("raytraced_reflections", true);
	gPar->Set("detail_level", 1.0);
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
}
void cDockImageAdjustments::slotQualityPresetNormal()
{
	gPar->Set("DE_factor", 1.0);
	gPar->Set("ambient_occlusion_enabled", true);
	gPar->Set("ambient_occlusion_mode", int(params::AOmodeScreenSpace));
	gPar->Set("ambient_occlusion_quality", 4);
	gPar->Set("shadows_enabled", true);
	gPar->Set("raytraced_reflections", true);
	gPar->Set("detail_level", 1.0);
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
}

bool cDockImageAdjustments::IsConnectDetailLevelEnabled() const
{
	return ui->checkBox_connect_detail_level->isChecked();
}

void cDockImageAdjustments::slotQualityPresetHigh()
{
	gPar->Set("DE_factor", 0.2);
	gPar->Set("ambient_occlusion_enabled", true);
	gPar->Set("ambient_occlusion_mode", int(params::AOmodeMultipeRays));
	gPar->Set("ambient_occlusion_quality", 4);
	gPar->Set("shadows_enabled", true);
	gPar->Set("raytraced_reflections", true);
	gPar->Set("detail_level", 1.0);
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
}

void cDockImageAdjustments::ApplyImageChangesSetEnabled(bool enable) const
{
	ui->pushButton_apply_image_changes->setEnabled(enable);
}

void cDockImageAdjustments::slotPressedButtonImageApply()
{
	gMainInterface->RefreshMainImage();
}

void cDockImageAdjustments::slotPressedImagesizeIncrease() const
{
	int width = ui->spinboxInt_image_width->value();
	int height = ui->spinboxInt_image_height->value();
	width *= 2;
	height *= 2;
	ui->spinboxInt_image_width->setValue(width);
	ui->spinboxInt_image_height->setValue(height);
}

void cDockImageAdjustments::slotPressedImagesizeDecrease() const
{
	int width = ui->spinboxInt_image_width->value();
	int height = ui->spinboxInt_image_height->value();
	width /= 2;
	height /= 2;
	ui->spinboxInt_image_width->setValue(width);
	ui->spinboxInt_image_height->setValue(height);
}
