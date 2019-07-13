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
 * Widget which contains UI for navigation
 */

#include "dock_navigation.h"

#include "ui_dock_navigation.h"

#include "src/automated_widgets.hpp"
#include "src/interface.hpp"
#include "src/system.hpp"

cDockNavigation::cDockNavigation(QWidget *parent) : QWidget(parent), ui(new Ui::cDockNavigation)
{
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	ConnectSignals();
	SetIconSizes();
}

cDockNavigation::~cDockNavigation()
{
	delete ui;
}

void cDockNavigation::RenderButtonSetEnabled(bool enabled) const
{
	ui->pushButton_render->setEnabled(enabled);
}

void cDockNavigation::AutoRefreshSetChecked(bool state) const
{
	ui->checkBox_auto_refresh->setChecked(state);
}

bool cDockNavigation::AutoRefreshIsChecked() const
{
	return ui->checkBox_auto_refresh->isChecked();
}

void cDockNavigation::slotUndo()
{
	gMainInterface->Undo();
}

void cDockNavigation::slotRedo()
{
	gMainInterface->Redo();
}

void cDockNavigation::SetIconSizes()
{
	QFontMetrics fm(font());
	int pixelFontSize = fm.height();
	QSize iconSize(int(pixelFontSize * 2.5), int(pixelFontSize * 2.5));
	ui->bu_move_backward->setIconSize(iconSize);
	ui->bu_move_down->setIconSize(iconSize);
	ui->bu_move_forward->setIconSize(iconSize);
	ui->bu_move_left->setIconSize(iconSize);
	ui->bu_move_right->setIconSize(iconSize);
	ui->bu_move_up->setIconSize(iconSize);
	ui->bu_rotate_down->setIconSize(iconSize);
	ui->bu_rotate_left->setIconSize(iconSize);
	ui->bu_rotate_right->setIconSize(iconSize);
	ui->bu_rotate_roll_left->setIconSize(iconSize);
	ui->bu_rotate_roll_right->setIconSize(iconSize);
	ui->bu_rotate_up->setIconSize(iconSize);
}

void cDockNavigation::ConnectSignals() const
{
	// ------------ camera manipulation -----------
	connect(ui->bu_move_up, SIGNAL(clicked()), this, SLOT(slotCameraMove()));
	connect(ui->bu_move_down, SIGNAL(clicked()), this, SLOT(slotCameraMove()));
	connect(ui->bu_move_left, SIGNAL(clicked()), this, SLOT(slotCameraMove()));
	connect(ui->bu_move_right, SIGNAL(clicked()), this, SLOT(slotCameraMove()));
	connect(ui->bu_move_forward, SIGNAL(clicked()), this, SLOT(slotCameraMove()));
	connect(ui->bu_move_backward, SIGNAL(clicked()), this, SLOT(slotCameraMove()));

	connect(ui->bu_rotate_up, SIGNAL(clicked()), this, SLOT(slotCameraRotation()));
	connect(ui->bu_rotate_down, SIGNAL(clicked()), this, SLOT(slotCameraRotation()));
	connect(ui->bu_rotate_left, SIGNAL(clicked()), this, SLOT(slotCameraRotation()));
	connect(ui->bu_rotate_right, SIGNAL(clicked()), this, SLOT(slotCameraRotation()));
	connect(ui->bu_rotate_roll_left, SIGNAL(clicked()), this, SLOT(slotCameraRotation()));
	connect(ui->bu_rotate_roll_right, SIGNAL(clicked()), this, SLOT(slotCameraRotation()));

	connect(ui->vect3_camera_x, SIGNAL(editingFinished()), this, SLOT(slotCameraOrTargetEdited()));
	connect(ui->vect3_camera_y, SIGNAL(editingFinished()), this, SLOT(slotCameraOrTargetEdited()));
	connect(ui->vect3_camera_z, SIGNAL(editingFinished()), this, SLOT(slotCameraOrTargetEdited()));
	connect(ui->vect3_target_x, SIGNAL(editingFinished()), this, SLOT(slotCameraOrTargetEdited()));
	connect(ui->vect3_target_y, SIGNAL(editingFinished()), this, SLOT(slotCameraOrTargetEdited()));
	connect(ui->vect3_target_z, SIGNAL(editingFinished()), this, SLOT(slotCameraOrTargetEdited()));
	connect(ui->vect3_camera_rotation_x, SIGNAL(editingFinished()), this, SLOT(slotRotationEdited()));
	connect(ui->vect3_camera_rotation_y, SIGNAL(editingFinished()), this, SLOT(slotRotationEdited()));
	connect(ui->vect3_camera_rotation_z, SIGNAL(editingFinished()), this, SLOT(slotRotationEdited()));
	connect(ui->logedit_camera_distance_to_target, SIGNAL(editingFinished()), this,
		SLOT(slotCameraDistanceEdited()));
	connect(ui->comboBox_camera_absolute_distance_mode, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotMovementStepModeChanged(int)));
	connect(ui->comboBox_camera_movement_mode, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotCameraMovementModeChanged(int)));
	connect(ui->pushButton_undo, SIGNAL(clicked()), this, SLOT(slotUndo()));
	connect(ui->pushButton_redo, SIGNAL(clicked()), this, SLOT(slotRedo()));
	connect(ui->pushButton_render, SIGNAL(clicked()), this, SLOT(slotStartRender()));

	connect(ui->pushButton_stop, SIGNAL(clicked()), this, SLOT(slotStopRender()));
	connect(ui->pushButton_reset_view, SIGNAL(clicked()), this, SLOT(slotPressedButtonResetView()));
}

void cDockNavigation::slotCameraMove() const
{
	QString buttonName = sender()->objectName();
	gMainInterface->MoveCamera(buttonName, true);
}

void cDockNavigation::slotCameraRotation() const
{
	QString buttonName = sender()->objectName();

	gMainInterface->RotateCamera(buttonName, true);
}

void cDockNavigation::slotCameraOrTargetEdited()
{
	gMainInterface->CameraOrTargetEdited();
}

void cDockNavigation::slotRotationEdited()
{
	gMainInterface->RotationEdited();
}

void cDockNavigation::slotCameraDistanceEdited()
{
	gMainInterface->CameraDistanceEdited();
}

void cDockNavigation::slotCameraDistanceSlider(int value)
{
	(void)value;
	gMainInterface->CameraDistanceEdited();
}

void cDockNavigation::slotMovementStepModeChanged(int index)
{
	gMainInterface->MovementStepModeChanged(index);
}

void cDockNavigation::slotPressedButtonResetView()
{
	gMainInterface->ResetView();
}

void cDockNavigation::slotStartRender()
{
	gMainInterface->StartRender();
}

void cDockNavigation::slotStopRender()
{
	gMainInterface->stopRequest = true;
}

void cDockNavigation::LockAllFunctions() const
{
	ui->scrollArea_navi->setEnabled(false);
	ui->pushButton_render->setEnabled(false);
	ui->pushButton_undo->setEnabled(false);
	ui->pushButton_redo->setEnabled(false);
}

void cDockNavigation::UnlockAllFunctions() const
{
	ui->scrollArea_navi->setEnabled(true);
	ui->pushButton_render->setEnabled(true);
	ui->pushButton_undo->setEnabled(true);
	ui->pushButton_redo->setEnabled(true);
}

void cDockNavigation::slotCameraMovementModeChanged(int index)
{
	gMainInterface->CameraMovementModeChanged(index);
}

void cDockNavigation::EnableOpenCLModeComboBox(bool enabled) const
{
	ui->comboBox_opencl_mode->setVisible(enabled);
	ui->label_opencl_mode->setVisible(enabled);
}
