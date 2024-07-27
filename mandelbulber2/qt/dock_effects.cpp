/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Widget which contains UI for effects
 */

#include "dock_effects.h"

#include "ui_dock_effects.h"

#include "dock_image_adjustments.h"
#include "navigator_window.h"

#include "src/ao_modes.h"
#include "src/automated_widgets.hpp"
#include "src/fractal_container.hpp"
#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/primitives.h"
#include "src/random.hpp"
#include "src/render_window.hpp"
#include "src/rendered_image_widget.hpp"
#include "src/synchronize_interface.hpp"
#include "src/system_data.hpp"

cDockEffects::cDockEffects(QWidget *parent)
		: QWidget(parent), cMyWidgetWithParams(), ui(new Ui::cDockEffects)
{
	ui->setupUi(this);

	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	ConnectSignals();
}

cDockEffects::~cDockEffects()
{
	delete ui;
}

void cDockEffects::ConnectSignals() const
{
	connect(ui->button_calculateFog, SIGNAL(clicked()), this, SLOT(slotPressedButtonAutoFog()));

	connect(ui->comboBox_ambient_occlusion_mode, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotChangedComboAmbientOcclusionMode(int)));

	connect(ui->logedit_aux_light_manual_placement_dist, SIGNAL(textChanged(const QString &)), this,
		SLOT(slotEditedLineEditManualLightPlacementDistance(const QString &)));

	connect(
		ui->pushButton_DOF_set_focus, SIGNAL(clicked()), this, SLOT(slotPressedButtonSetDOFByMouse()));
	connect(
		ui->pushButton_DOF_update, SIGNAL(clicked()), this, SLOT(slotPressedButtonUpdatePostEffects()));
	connect(ui->pushButton_post_effects_update, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonUpdatePostEffects()));

	connect(ui->pushButton_set_fog_by_mouse, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonSetFogByMouse()));
	connect(ui->pushButton_place_random_lights_by_mouse, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonPlaceRandomLightsByMouse()));

	connect(ui->toolButton_fog_distance_double, &QToolButton::clicked, this,
		&cDockEffects::slotPressedButtonFogDistDouble);

	connect(ui->toolButton_fog_distance_half, &QToolButton::clicked, this,
		&cDockEffects::slotPressedButtonFogDistHalf);

	connect(ui->checkBox_aux_light_place_behind, SIGNAL(stateChanged(int)), this,
		SLOT(slotChangedPlaceLightBehindObjects(int)));

	connect(ui->groupCheck_DOF_monte_carlo, SIGNAL(toggled(bool)), this,
		SLOT(slotChangedEnableMCDOF(bool)));
	connect(ui->checkBox_DOF_MC_global_illumination, SIGNAL(stateChanged(int)), this,
		SLOT(slotChangedEnableGI(int)));
	connect(ui->checkBox_MC_global_illumination_volumetric, SIGNAL(stateChanged(int)), this,
		SLOT(slotChangedEnableGIVolumetric(int)));
	connect(ui->checkBox_MC_fog_illumination, SIGNAL(stateChanged(int)), this,
		SLOT(slotChangedEnableGIFog(int)));

	connect(ui->comboBox_random_lights_coloring_type, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotRandomLightColoringTypeChanged(int)));

	connect(ui->pushButton_clouds_randomize, &QPushButton::clicked, this,
		&cDockEffects::slotPressedButtonCloudsRandomize);

	connect(ui->widget_light_sources_manager,
		&cLightSourcesManager::signalChangeLightPlacementDistance, this,
		&cDockEffects::slotSetAuxLightManualPlacementDistance);

	connect(
		ui->pushButton_local_navi, &QPushButton::clicked, this, &cDockEffects::slotPressedButtonNavi);
}

void cDockEffects::slotSynchronizeInterfaceBasicFog(std::shared_ptr<cParameterContainer> par) const
{
	SynchronizeInterfaceWindow(ui->groupCheck_basic_fog_enabled, par, qInterface::write);
}

void cDockEffects::slotSynchronizeInterfaceDOF(std::shared_ptr<cParameterContainer> par) const
{
	SynchronizeInterfaceWindow(ui->groupCheck_DOF_enabled, par, qInterface::write);
}

void cDockEffects::slotSynchronizeInterfaceLights(std::shared_ptr<cParameterContainer> par) const
{
	SynchronizeInterfaceWindow(ui->widget_light_sources_manager, par, qInterface::write);
}

void cDockEffects::slotSynchronizeInterfaceRandomLights(
	std::shared_ptr<cParameterContainer> par) const
{
	SynchronizeInterfaceWindow(ui->groupCheck_random_lights_group, par, qInterface::write);
}

double cDockEffects::GetAuxLightManualPlacementDistance() const
{
	return systemData.locale.toDouble(ui->logedit_aux_light_manual_placement_dist->text());
}

void cDockEffects::slotSetAuxLightManualPlacementDistance(double dist) const
{
	ui->logedit_aux_light_manual_placement_dist->setText(QString("%L1").arg(dist));
}

void cDockEffects::slotEditedLineEditManualLightPlacementDistance(const QString &text)
{
	renderedImageWidget->SetFrontDist(systemData.locale.toDouble(text));
}

void cDockEffects::slotChangedPlaceLightBehindObjects(int state)
{
	renderedImageWidget->SetPlaceBehindObjects(state);
}

void cDockEffects::slotChangedComboAmbientOcclusionMode(int index) const
{
	bool enabled = index == params::AOModeMultipleRays ? true : false;
	ui->frame_lightmap_texture->setEnabled(enabled);
	enabled = index == params::AOModeFast ? true : false;
	ui->spinbox_ambient_occlusion_fast_tune->setEnabled(enabled);
}

void cDockEffects::slotPressedButtonAutoFog()
{
	SynchronizeInterfaceWindow(this, params, qInterface::read);
	gMainInterface->AutoFog(params, fractalParams);
	SynchronizeInterfaceWindow(this, params, qInterface::write);
}

void cDockEffects::slotPressedButtonUpdatePostEffects()
{
	emit signalRefreshPostEffects();
}

void cDockEffects::slotPressedButtonSetDOFByMouse()
{
	QList<QVariant> item;
	item.append(int(RenderedImage::clickDOFFocus));
	int index = mouseFunctionComboWidget->findData(item);
	mouseFunctionComboWidget->setCurrentIndex(index);
	renderedImageWidget->setClickMode(item);
}

void cDockEffects::slotPressedButtonSetFogByMouse()
{
	QList<QVariant> item;
	item.append(int(RenderedImage::clickFogVisibility));
	int index = mouseFunctionComboWidget->findData(item);
	mouseFunctionComboWidget->setCurrentIndex(index);
	renderedImageWidget->setClickMode(item);
}

void cDockEffects::slotPressedButtonPlaceRandomLightsByMouse()
{
	QList<QVariant> item;
	item.append(int(RenderedImage::clickPlaceRandomLightCenter));
	int index = mouseFunctionComboWidget->findData(item);
	mouseFunctionComboWidget->setCurrentIndex(index);
	renderedImageWidget->setClickMode(item);
}

void cDockEffects::UpdateLabelAverageDOFSamples(const QString &avg)
{
	ui->label_average_DOF_samples->setText(avg);
}

void cDockEffects::UpdateLabelAverageDOFNoise(const QString &avg)
{
	ui->label_average_DOF_noise->setText(avg);
}

void cDockEffects::UpdateLabelMaskedPixels(const QString &avg)
{
	ui->label_masked_pixels->setText(avg);
}

void cDockEffects::UpdateLabelTilesDone(const QString &avg)
{
	ui->label_tiles_done->setText(avg);
}

void cDockEffects::slotChangedEnableMCDOF(bool state)
{
	if (state)
	{
		params::enumAOMode AOMode =
			params::enumAOMode(ui->comboBox_ambient_occlusion_mode->currentIndex());
		if (AOMode == params::AOModeScreenSpace)
		{
			ui->groupCheck_ambient_occlusion_enabled->setChecked(false);
		}
	}
}

void cDockEffects::slotChangedEnableGI(int state)
{
	ui->checkBox_MC_global_illumination_volumetric->setEnabled(state);
	ui->checkBox_MC_fog_illumination->setEnabled(state);
}

void cDockEffects::slotChangedEnableGIVolumetric(int state)
{
	if (state == Qt::Checked) ui->checkBox_MC_fog_illumination->setChecked(false);
}

void cDockEffects::slotChangedEnableGIFog(int state)
{
	if (state == Qt::Checked) ui->checkBox_MC_global_illumination_volumetric->setChecked(false);
}

void cDockEffects::slotPressedButtonCloudsRandomize()
{
	cRandom random;
	random.Initialize(QTime::currentTime().msec());
	int rnd = random.Random(100000);
	ui->spinboxInt_clouds_random_seed->setValue(rnd);
}

void cDockEffects::RegenerateLights()
{
	ui->widget_light_sources_manager->Regenerate();
}

void cDockEffects::slotPressedButtonNavi()
{
	gMainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	cNavigatorWindow *navigator = new cNavigatorWindow();
	cDockEffects *leftWidget = new cDockEffects();
	navigator->AddLeftWidget(leftWidget);
	navigator->setAttribute(Qt::WA_DeleteOnClose);
	navigator->SetInitialParameters(params, fractalParams);
	navigator->SynchronizeInterface(qInterface::write);
	navigator->SetMouseClickFunction(gMainInterface->GetMouseClickFunction());

	connect(navigator, &cNavigatorWindow::signalChangesAccepted, this,
		&cDockEffects::slotNewParametersFromNavi);

	navigator->show();
	navigator->AllPrepared();
}

void cDockEffects::AssignParameterContainers(
	std::shared_ptr<cParameterContainer> _params, std::shared_ptr<cFractalContainer> _fractalParams)
{
	cMyWidgetWithParams::AssignParameterContainers(_params, _fractalParams);
	ui->widget_light_sources_manager->AssignParameterContainers(_params, _fractalParams);

	ui->widget_light_sources_manager->Init();
}

void cDockEffects::AssignSpecialWidgets(
	RenderedImage *_renderedImage, QComboBox *_mouseFunctionCombo)
{
	cMyWidgetWithParams::AssignSpecialWidgets(_renderedImage, _mouseFunctionCombo);
	ui->widget_light_sources_manager->AssignSpecialWidgets(
		renderedImageWidget, mouseFunctionComboWidget);
}

void cDockEffects::slotNewParametersFromNavi()
{
	RegenerateLights();
}

void cDockEffects::slotPressedButtonFogDistDouble()
{
	SynchronizeInterfaceWindow(this, params, qInterface::read);

	params->Set("volumetric_fog_colour_1_distance",
		params->Get<double>("volumetric_fog_colour_1_distance") * 2.0);

	params->Set("volumetric_fog_colour_2_distance",
		params->Get<double>("volumetric_fog_colour_2_distance") * 2.0);

	params->Set(
		"volumetric_fog_distance_factor", params->Get<double>("volumetric_fog_distance_factor") * 2.0);

	params->Set("volumetric_fog_distance_from_surface",
		params->Get<double>("volumetric_fog_distance_from_surface") * 2.0);

	SynchronizeInterfaceWindow(this, params, qInterface::write);
}

void cDockEffects::slotPressedButtonFogDistHalf()
{
	SynchronizeInterfaceWindow(this, params, qInterface::read);

	params->Set("volumetric_fog_colour_1_distance",
		params->Get<double>("volumetric_fog_colour_1_distance") / 2.0);

	params->Set("volumetric_fog_colour_2_distance",
		params->Get<double>("volumetric_fog_colour_2_distance") / 2.0);

	params->Set(
		"volumetric_fog_distance_factor", params->Get<double>("volumetric_fog_distance_factor") / 2.0);

	params->Set("volumetric_fog_distance_from_surface",
		params->Get<double>("volumetric_fog_distance_from_surface") / 2.0);

	SynchronizeInterfaceWindow(this, params, qInterface::write);
}

void cDockEffects::slotRandomLightColoringTypeChanged(int index)
{
	ui->label_random_lights_first_color->setVisible(index > 0);
	ui->colorButton_random_lights_color->setVisible(index > 0);
	ui->label_random_lights_second_color->setVisible(index >= 2);
	ui->colorButton_random_lights_color_2->setVisible(index >= 2);
}

void cDockEffects::slotUpdatePrimitivesCombos()
{
	cPrimitives::PrepareComboBox(ui->comboBox_basic_fog_primitive, params);
	cPrimitives::PrepareComboBox(ui->comboBox_distance_fog_primitive, params);
	cPrimitives::PrepareComboBox(ui->comboBox_iteration_fog_primitive, params);
	cPrimitives::PrepareComboBox(ui->comboBox_clouds_primitive, params);
}
