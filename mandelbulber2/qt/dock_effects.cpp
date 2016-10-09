/*
 * dock_effects.cpp
 *
 *  Created on: 8 paź 2016
 *      Author: krzysztof
 */

#include "dock_effects.h"

#include "../src/rendered_image_widget.hpp"
#include "ui_dock_effects.h"
#include "../src/synchronize_interface.hpp"
#include "../src/interface.hpp"
#include "../src/initparameters.hpp"

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

void cDockEffects::ConnectSignals()
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

void cDockEffects::SynchronizeInterfaceBasicFogEnabled(cParameterContainer *par)
{
	SynchronizeInterfaceWindow(ui->groupCheck_basic_fog_enabled, par, qInterface::write);
}

void cDockEffects::SynchronizeInterfaceDOFEnabled(cParameterContainer *par)
{
	SynchronizeInterfaceWindow(ui->groupCheck_DOF_enabled, par, qInterface::write);
}

void cDockEffects::SynchronizeInterfaceLights(cParameterContainer *par)
{
	SynchronizeInterfaceWindow(ui->groupBox_Lights, par, qInterface::write);
}

void cDockEffects::SynchronizeInterfacerandomLights(cParameterContainer *par)
{
	SynchronizeInterfaceWindow(ui->groupCheck_random_lights_group, par, qInterface::write);
}

void cDockEffects::slotPressedButtonSetLight1ByMouse()
{
	QList<QVariant> item;
	item.append((int)RenderedImage::clickPlaceLight);
	item.append(1); // light number
	int index = gMainInterface->mainWindow->ui->comboBox_mouse_click_function->findData(item);
	gMainInterface->mainWindow->ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
	double distance = gMainInterface->GetDistanceForPoint(gPar->Get<CVector3>("camera"));
	ui->logedit_aux_light_manual_placement_dist->setText(QString("%L1").arg(distance * 0.1));
}

void cDockEffects::slotPressedButtonSetLight2ByMouse()
{
	QList<QVariant> item;
	item.append((int)RenderedImage::clickPlaceLight);
	item.append(2); // light number
	int index = gMainInterface->mainWindow->ui->comboBox_mouse_click_function->findData(item);
	gMainInterface->mainWindow->ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void cDockEffects::slotPressedButtonSetLight3ByMouse()
{
	QList<QVariant> item;
	item.append((int)RenderedImage::clickPlaceLight);
	item.append(3); // light number
	int index = gMainInterface->mainWindow->ui->comboBox_mouse_click_function->findData(item);
	gMainInterface->mainWindow->ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

double cDockEffects::GetAuxLightManualPlacementDistance()
{
	return systemData.locale.toDouble(ui->logedit_aux_light_manual_placement_dist->text());
}

void cDockEffects::SetAuxLightManualPlacementDistance(double dist)
{
	ui->logedit_aux_light_manual_placement_dist->setText(QString("%L1").arg(dist));
}

void cDockEffects::slotPressedButtonSetLight4ByMouse()
{
	QList<QVariant> item;
	item.append((int)RenderedImage::clickPlaceLight);
	item.append(4); // light number
	int index = gMainInterface->mainWindow->ui->comboBox_mouse_click_function->findData(item);
	gMainInterface->mainWindow->ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void cDockEffects::slotEditedLineEditManualLightPlacementDistance(const QString &text)
{
	gMainInterface->renderedImage->SetFrontDist(systemData.locale.toDouble(text));
}

void cDockEffects::slotSliderMovedEditManualLightPlacementDistance(int value)
{
	gMainInterface->renderedImage->SetFrontDist(pow(10.0, value / 100.0));
}

void cDockEffects::slotChangedCheckBoxDOFHDR(int state)
{
	ui->pushButton_DOF_update->setEnabled(!state);
	gMainInterface->mainWindow->ui->widgetImageAjustments->ApplyImageChangesSetEnabled(!state);
	if (ui->comboBox_ambient_occlusion_mode->currentIndex() == 2 && !state)
	{
		ui->comboBox_ambient_occlusion_mode->setCurrentIndex(0);
	}
}

void cDockEffects::slotChangedComboAmbientOcclusionMode(int index)
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
	item.append((int)RenderedImage::clickDOFFocus);
	int index = gMainInterface->mainWindow->ui->comboBox_mouse_click_function->findData(item);
	gMainInterface->mainWindow->ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void cDockEffects::slotPressedButtonSetFogByMouse()
{
	QList<QVariant> item;
	item.append((int)RenderedImage::clickFogVisibility);
	int index = gMainInterface->mainWindow->ui->comboBox_mouse_click_function->findData(item);
	gMainInterface->mainWindow->ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void cDockEffects::slotPressedButtonPlaceRandomLightsByMouse()
{
	QList<QVariant> item;
	item.append((int)RenderedImage::clickPlaceRandomLightCenter);
	int index = gMainInterface->mainWindow->ui->comboBox_mouse_click_function->findData(item);
	gMainInterface->mainWindow->ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}
