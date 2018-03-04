/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include "dock_image_adjustments.h"

#include "ui_dock_image_adjustments.h"

#include "dock_rendering_engine.h"

#include "src/automated_widgets.hpp"
#include "src/fractal_container.hpp"
#include "src/fractparams.hpp"
#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/parameters.hpp"
#include "src/projection_3d.hpp"
#include "src/render_window.hpp"
#include "src/settings.hpp"

cDockImageAdjustments::cDockImageAdjustments(QWidget *parent)
		: QWidget(parent), ui(new Ui::cDockImageAdjustments)
{
	ui->setupUi(this);
	listOfPresetBushbuttons = ui->groupBox_presets->findChildren<QToolButton *>();

	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	ConnectSignals();
	resolutionPresets = new cParameterContainer;
	InitResolutionPresets();
}

cDockImageAdjustments::~cDockImageAdjustments()
{
	delete ui;
	delete resolutionPresets;
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

	foreach (QToolButton *button, listOfPresetBushbuttons)
	{
		connect(button, SIGNAL(clicked()), this, SLOT(slotPressedResolutionPreset()));
	}

	connect(ui->pushButton_imagesize_increase, SIGNAL(clicked()), this,
		SLOT(slotPressedImageSizeIncrease()));
	connect(ui->pushButton_imagesize_decrease, SIGNAL(clicked()), this,
		SLOT(slotPressedImageSizeDecrease()));
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

double cDockImageAdjustments::ProportionEnumToRatio(enumImageProportion proportion) const
{
	double ratio;
	switch (proportion)
	{
		case proportionFree: ratio = 0.0; break;
		case proportion1_1: ratio = 1.0; break;
		case proportion4_3: ratio = 4.0 / 3.0; break;
		case proportion3_2: ratio = 3.0 / 2.0; break;
		case proportion16_9: ratio = 16.0 / 9.0; break;
		case proportion16_10: ratio = 16.0 / 10.0; break;
		case proportion2_1: ratio = 2.0 / 1.0; break;
		default: ratio = 0.0; break;
	}
	return ratio;
}

void cDockImageAdjustments::slotChangedComboImageProportion(int index) const
{
	bool enableSlider = false;
	double ratio = 1.0;
	const enumImageProportion proportionSelection = enumImageProportion(index);

	if (proportionSelection == proportionFree)
	{
		enableSlider = true;
	}

	ratio = ProportionEnumToRatio(proportionSelection);

	ui->spinboxInt_image_width->setEnabled(enableSlider);

	const int height = ui->spinboxInt_image_height->value();
	int width = height * ratio;

	if (!enableSlider)
	{
		ui->spinboxInt_image_width->setValue(width);
	}

	if (ui->checkBox_connect_detail_level->isChecked())
	{
		const double sizeRatio = double(height) / gMainInterface->lockedImageResolution.y;
		gPar->Set("detail_level", gMainInterface->lockedDetailLevel / sizeRatio);
		gMainInterface->mainWindow->GetWidgetDockRenderingEngine()
			->SynchronizeInterfaceDistanceEstimation(gPar);
	}
}

void cDockImageAdjustments::slotPressedResolutionPreset() const
{
	int width = 0, height = 0;
	enumImageProportion proportion = proportionFree;
	const QString buttonName = this->sender()->objectName();

	int lastDash = buttonName.lastIndexOf('_');
	QString indexText = buttonName.mid(lastDash + 1);
	int index = indexText.toInt();

	QString resolutionText = resolutionPresets->Get<QString>("resolution_preset", index);

	int xPosition = resolutionText.indexOf('x');
	width = resolutionText.left(xPosition).toInt();
	height = resolutionText.mid(xPosition + 1).toInt();

	double calculatedProportion = double(width) / height;
	for (int i = 0; i < numberOfProportions; i++)
	{
		if (calculatedProportion == ProportionEnumToRatio(enumImageProportion(i)))
		{
			proportion = enumImageProportion(i);
		}
	}

	ui->comboBox_image_proportion->setCurrentIndex(proportion);
	ui->spinboxInt_image_width->setValue(width);
	ui->spinboxInt_image_height->setValue(height);

	if (ui->checkBox_connect_detail_level->isChecked())
	{
		const double sizeRatio = double(height) / gMainInterface->lockedImageResolution.y;
		gPar->Set("detail_level", gMainInterface->lockedDetailLevel / sizeRatio);
		gMainInterface->mainWindow->GetWidgetDockRenderingEngine()
			->SynchronizeInterfaceDistanceEstimation(gPar);
	}
}

void cDockImageAdjustments::slotChangedComboPerspectiveType(int index) const
{
	const params::enumPerspectiveType perspType = params::enumPerspectiveType(index);
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
	const int index = ui->comboBox_image_proportion->currentIndex();
	slotChangedComboImageProportion(index);

	if (ui->checkBox_connect_detail_level->isChecked())
	{
		SynchronizeInterfaceWindow(ui->groupBox_imageResolution, gPar, qInterface::read);
		const double sizeRatio =
			double(gPar->Get<int>("image_height")) / gMainInterface->lockedImageResolution.y;
		gPar->Set("detail_level", gMainInterface->lockedDetailLevel / sizeRatio);
		gMainInterface->mainWindow->GetWidgetDockRenderingEngine()
			->SynchronizeInterfaceDistanceEstimation(gPar);
	}
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
	gPar->Set("ambient_occlusion_mode", int(params::AOModeScreenSpace));
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
	gPar->Set("ambient_occlusion_mode", int(params::AOModeMultipleRays));
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

void cDockImageAdjustments::slotPressedImageSizeIncrease() const
{
	int width = ui->spinboxInt_image_width->value();
	int height = ui->spinboxInt_image_height->value();
	width *= 2;
	height *= 2;
	ui->spinboxInt_image_width->setValue(width);
	ui->spinboxInt_image_height->setValue(height);
}

void cDockImageAdjustments::slotPressedImageSizeDecrease() const
{
	int width = ui->spinboxInt_image_width->value();
	int height = ui->spinboxInt_image_height->value();
	width /= 2;
	height /= 2;
	ui->spinboxInt_image_width->setValue(width);
	ui->spinboxInt_image_height->setValue(height);
}

void cDockImageAdjustments::InitResolutionPresets()
{
	resolutionPresets->addParam("resolution_preset", 1, QString("720x480"), morphNone, paramApp);
	resolutionPresets->addParam("resolution_preset", 2, QString("1280x720"), morphNone, paramApp);
	resolutionPresets->addParam("resolution_preset", 3, QString("1920x1080"), morphNone, paramApp);
	resolutionPresets->addParam("resolution_preset", 4, QString("2560x1440"), morphNone, paramApp);
	resolutionPresets->addParam("resolution_preset", 5, QString("4096x2160"), morphNone, paramApp);
	resolutionPresets->addParam("resolution_preset", 6, QString("7680x4320"), morphNone, paramApp);
	resolutionPresets->addParam("resolution_preset", 7, QString("320x240"), morphNone, paramApp);
	resolutionPresets->addParam("resolution_preset", 8, QString("800x600"), morphNone, paramApp);
	resolutionPresets->addParam("resolution_preset", 9, QString("1600x1200"), morphNone, paramApp);

	QString presetsFile = systemData.GetResolutionPresetsFile();
	if (QFileInfo::exists(presetsFile))
	{
		cSettings settings(cSettings::formatAppSettings);
		settings.BeQuiet(true);
		settings.LoadFromFile(presetsFile);
		settings.Decode(resolutionPresets, nullptr);
	}
	else
	{
		cSettings settings(cSettings::formatAppSettings);
		settings.CreateText(resolutionPresets, nullptr);
		settings.SaveToFile(presetsFile);
	}

	foreach (QToolButton *button, listOfPresetBushbuttons)
	{
		button->setPopupMode(QToolButton::MenuButtonPopup);
		button->setText(resolutionPresets->Get<QString>(
			"resolution_preset", PresetButtonIndex(button->objectName())));
		QMenu *menu = new QMenu(button);
		QAction *action = new QAction(tr("Change preset"));
		action->setObjectName(QString("preset_%1").arg(PresetButtonIndex(button->objectName())));
		menu->addAction(action);
		button->setMenu(menu);
		connect(action, SIGNAL(triggered()), this, SLOT(slotChangeResolutionPreset()));
	}
}

int cDockImageAdjustments::PresetButtonIndex(const QString &name) const
{
	int lastDash = name.lastIndexOf('_');
	QString indexText = name.mid(lastDash + 1);
	int index = indexText.toInt();
	return index;
}

void cDockImageAdjustments::slotChangeResolutionPreset()
{
	int index = PresetButtonIndex(this->sender()->objectName());
	QString oldPreset = resolutionPresets->Get<QString>("resolution_preset", index);
	QString newPreset = QInputDialog::getText(this, tr("Edit resolution preset"),
		tr("Type new preset in format WIDTHxHEIGHT"), QLineEdit::Normal, oldPreset);

	int xPosition = newPreset.indexOf('x');
	int width = newPreset.left(xPosition).toInt();
	int height = newPreset.mid(xPosition + 1).toInt();
	width = max(32, width);
	height = max(32, height);
	QString newPresetCorrected = QString("%1x%2").arg(width).arg(height);

	QToolButton *button = ui->groupBox_presets->findChild<QToolButton *>(
		QString("toolButton_resolution_preset_%1").arg(index));

	resolutionPresets->Set("resolution_preset", index, newPresetCorrected);
	button->setText(newPresetCorrected);

	QString presetsFile = systemData.GetResolutionPresetsFile();
	cSettings settings(cSettings::formatAppSettings);
	settings.CreateText(resolutionPresets, nullptr);
	settings.SaveToFile(presetsFile);
}
