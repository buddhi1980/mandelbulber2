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
 * Widget which contains UI for effects
 */

#include "dock_effects.h"

#include "../src/initparameters.hpp"
#include "../src/interface.hpp"
#include "../src/rendered_image_widget.hpp"
#include "../src/synchronize_interface.hpp"
#include "../src/ao_modes.h"
#include "../src/automated_widgets.hpp"
#include "../src/render_window.hpp"
#include "../src/system.hpp"
#include "dock_image_adjustments.h"
#include "ui_dock_effects.h"

cDockEffects::cDockEffects(QWidget *parent) : QWidget(parent), ui(new Ui::cDockEffects)
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

	connect(
		ui->checkBox_DOF_HDR, SIGNAL(stateChanged(int)), this, SLOT(slotChangedCheckBoxDOFHDR(int)));
	connect(ui->comboBox_ambient_occlusion_mode, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotChangedComboAmbientOcclusionMode(int)));

	connect(ui->logedit_aux_light_manual_placement_dist, SIGNAL(textChanged(const QString &)), this,
		SLOT(slotEditedLineEditManualLightPlacementDistance(const QString &)));

	connect(ui->logslider_aux_light_manual_placement_dist, SIGNAL(sliderMoved(int)), this,
		SLOT(slotSliderMovedEditManualLightPlacementDistance(int)));
	connect(
		ui->pushButton_DOF_set_focus, SIGNAL(clicked()), this, SLOT(slotPressedButtonSetDOFByMouse()));
	connect(ui->pushButton_DOF_update, SIGNAL(clicked()), this, SLOT(slotPressedButtonDOFUpdate()));

	connect(ui->pushButton_place_light_by_mouse_1, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonSetLight1ByMouse()));
	connect(ui->pushButton_place_light_by_mouse_2, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonSetLight2ByMouse()));
	connect(ui->pushButton_place_light_by_mouse_3, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonSetLight3ByMouse()));
	connect(ui->pushButton_place_light_by_mouse_4, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonSetLight4ByMouse()));

	connect(ui->pushButton_set_fog_by_mouse, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonSetFogByMouse()));
	connect(ui->pushButton_place_random_lights_by_mouse, SIGNAL(clicked()), this,
		SLOT(slotPressedButtonPlaceRandomLightsByMouse()));
}

void cDockEffects::SynchronizeInterfaceBasicFogEnabled(cParameterContainer *par) const
{
	SynchronizeInterfaceWindow(ui->groupCheck_basic_fog_enabled, par, qInterface::write);
}

void cDockEffects::SynchronizeInterfaceDOFEnabled(cParameterContainer *par) const
{
	SynchronizeInterfaceWindow(ui->groupCheck_DOF_enabled, par, qInterface::write);
}

void cDockEffects::SynchronizeInterfaceLights(cParameterContainer *par) const
{
	SynchronizeInterfaceWindow(ui->groupBox_Lights, par, qInterface::write);
}

void cDockEffects::SynchronizeInterfacerandomLights(cParameterContainer *par) const
{
	SynchronizeInterfaceWindow(ui->groupCheck_random_lights_group, par, qInterface::write);
}

void cDockEffects::slotPressedButtonSetLight1ByMouse() const
{
	QList<QVariant> item;
	item.append(int(RenderedImage::clickPlaceLight));
	item.append(1); // light number
	int index = gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->findData(item);
	gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
	double distance = gMainInterface->GetDistanceForPoint(gPar->Get<CVector3>("camera"));
	ui->logedit_aux_light_manual_placement_dist->setText(QString("%L1").arg(distance * 0.1));
}

void cDockEffects::slotPressedButtonSetLight2ByMouse()
{
	QList<QVariant> item;
	item.append(int(RenderedImage::clickPlaceLight));
	item.append(2); // light number
	int index = gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->findData(item);
	gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void cDockEffects::slotPressedButtonSetLight3ByMouse()
{
	QList<QVariant> item;
	item.append(int(RenderedImage::clickPlaceLight));
	item.append(3); // light number
	int index = gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->findData(item);
	gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

double cDockEffects::GetAuxLightManualPlacementDistance() const
{
	return systemData.locale.toDouble(ui->logedit_aux_light_manual_placement_dist->text());
}

void cDockEffects::SetAuxLightManualPlacementDistance(double dist) const
{
	ui->logedit_aux_light_manual_placement_dist->setText(QString("%L1").arg(dist));
}

void cDockEffects::slotPressedButtonSetLight4ByMouse()
{
	QList<QVariant> item;
	item.append(int(RenderedImage::clickPlaceLight));
	item.append(4); // light number
	int index = gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->findData(item);
	gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void cDockEffects::slotEditedLineEditManualLightPlacementDistance(const QString &text)
{
	gMainInterface->renderedImage->SetFrontDist(systemData.locale.toDouble(text));
}

void cDockEffects::slotSliderMovedEditManualLightPlacementDistance(int value) const
{
	gMainInterface->renderedImage->SetFrontDist(pow(10.0, value / 100.0));
}

void cDockEffects::slotChangedCheckBoxDOFHDR(int state) const
{
	ui->pushButton_DOF_update->setEnabled(!state);
	gMainInterface->mainWindow->GetWidgetDockImageAdjustments()->ApplyImageChangesSetEnabled(!state);
	if (ui->comboBox_ambient_occlusion_mode->currentIndex() == 2 && !state)
	{
		ui->comboBox_ambient_occlusion_mode->setCurrentIndex(0);
	}
}

void cDockEffects::slotChangedComboAmbientOcclusionMode(int index) const
{
	bool enabled = index == params::AOmodeMultipeRays ? true : false;
	ui->frame_lightmap_texture->setEnabled(enabled);
	enabled = index == params::AOmodeFast ? true : false;
	ui->slider_ambient_occlusion_fast_tune->setEnabled(enabled);
	ui->spinbox_ambient_occlusion_fast_tune->setEnabled(enabled);
}

void cDockEffects::slotPressedButtonAutoFog()
{
	gMainInterface->AutoFog();
}

void cDockEffects::slotPressedButtonDOFUpdate()
{
	gMainInterface->RefreshMainImage();
}

void cDockEffects::slotPressedButtonSetDOFByMouse()
{
	QList<QVariant> item;
	item.append(int(RenderedImage::clickDOFFocus));
	int index = gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->findData(item);
	gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void cDockEffects::slotPressedButtonSetFogByMouse()
{
	QList<QVariant> item;
	item.append(int(RenderedImage::clickFogVisibility));
	int index = gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->findData(item);
	gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void cDockEffects::slotPressedButtonPlaceRandomLightsByMouse()
{
	QList<QVariant> item;
	item.append(int(RenderedImage::clickPlaceRandomLightCenter));
	int index = gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->findData(item);
	gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}
