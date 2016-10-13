/*
 * dock_navigation.cpp
 *
 *  Created on: 13 paź 2016
 *      Author: krzysztof
 */

#include "dock_navigation.h"

#include "../src/interface.hpp"
#include "ui_dock_navigation.h"

cDockNavigation::cDockNavigation(QWidget *parent) : QWidget(parent), ui(new Ui::cDockNavigation)
{
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	ConnectSignals();
}

cDockNavigation::~cDockNavigation()
{
	delete ui;
}

void cDockNavigation::RenderButtonSetEnabled(bool enabled)
{
	ui->pushButton_render->setEnabled(enabled);
}

void cDockNavigation::AutoRefreshSetChecked(bool state)
{
	ui->checkBox_auto_refresh->setChecked(state);
}

bool cDockNavigation::AutoRefreshIsChecked()
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

void cDockNavigation::ConnectSignals()
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
	connect(ui->logslider_camera_distance_to_target, SIGNAL(sliderMoved(int)), this,
		SLOT(slotCameraDistanceSlider(int)));
	connect(ui->comboBox_camera_absolute_distance_mode, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotMovementStepModeChanged(int)));
	connect(ui->pushButton_undo, SIGNAL(clicked()), this, SLOT(slotUndo()));
	connect(ui->pushButton_redo, SIGNAL(clicked()), this, SLOT(slotRedo()));
	connect(ui->pushButton_render, SIGNAL(clicked()), this, SLOT(slotStartRender()));

	connect(ui->pushButton_stop, SIGNAL(clicked()), this, SLOT(slotStopRender()));
	connect(ui->pushButton_reset_view, SIGNAL(clicked()), this, SLOT(slotPressedButtonResetView()));
}

void cDockNavigation::slotCameraMove()
{
	QString buttonName = this->sender()->objectName();
	gMainInterface->MoveCamera(buttonName);
}

void cDockNavigation::slotCameraRotation()
{
	QString buttonName = this->sender()->objectName();
	gMainInterface->RotateCamera(buttonName);
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

void cDockNavigation::slotStartRender(void)
{
	gMainInterface->StartRender();
}

void cDockNavigation::slotStopRender(void)
{
	gMainInterface->stopRequest = true;
}
