/*
 * dock_gamepad.cpp
 *
 *  Created on: 15 paź 2016
 *      Author: krzysztof
 */

#include "dock_gamepad.h"

#include "../src/interface.hpp"
#include "../src/system.hpp"
#include "ui_dock_gamepad.h"

cDockGamepad::cDockGamepad(QWidget *parent) : QWidget(parent), ui(new Ui::cDockGamepad)
{
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	ConnectSignals();
}

cDockGamepad::~cDockGamepad()
{
	delete ui;
}

void cDockGamepad::ConnectSignals()
{
#ifdef USE_GAMEPAD
	connect(ui->comboBox_gamepad_device, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotChangeGamepadIndex(int)));

	// Left Joystick controls Look Angle
	connect(&gamepad, SIGNAL(axisLeftYChanged(double)), this, SLOT(slotGamepadLook()));
	connect(&gamepad, SIGNAL(axisLeftXChanged(double)), this, SLOT(slotGamepadLook()));

	// Right Joystick controls Movement Direction
	connect(&gamepad, SIGNAL(axisRightYChanged(double)), this, SLOT(slotGamepadMove()));
	connect(&gamepad, SIGNAL(axisRightXChanged(double)), this, SLOT(slotGamepadMove()));

	// Left and Right Triggers control Roll Rotation
	connect(&gamepad, SIGNAL(buttonL2Changed(double)), this, SLOT(slotGamepadRoll()));
	connect(&gamepad, SIGNAL(buttonR2Changed(double)), this, SLOT(slotGamepadRoll()));

	// A and B buttons control the Movement Speed
	connect(&gamepad, SIGNAL(buttonAChanged(bool)), this, SLOT(slotGamepadSpeed()));
	connect(&gamepad, SIGNAL(buttonBChanged(bool)), this, SLOT(slotGamepadSpeed()));

	connect(this->ui->groupCheck_gamepad_enabled, SIGNAL(toggled(bool)), &gamepad,
		SLOT(setConnected(bool)));

	connect(QGamepadManager::instance(), SIGNAL(gamepadConnected(int)), this,
		SLOT(slotGamePadDeviceConnected(int)));
	connect(QGamepadManager::instance(), SIGNAL(gamepadDisconnected(int)), this,
		SLOT(slotGamePadDeviceDisconnected(int)));
#endif // USE_GAMEPAD
}

#ifdef USE_GAMEPAD
void cDockGamepad::slotChangeGamepadIndex(int index)
{
	gamepad.setDeviceId(index);
	WriteLog("Gamepad - slotChangeGamepadIndex: " + QString::number(index), 2);
}

void cDockGamepad::slotGamePadDeviceConnected(int index)
{
	QString deviceName = gamepad.name();
	if (deviceName == "") deviceName = "Device #" + QString::number(index);
	WriteLog(
		"Gamepad - device connected | index: " + QString::number(index) + ", name: " + deviceName, 2);

	if (ui->comboBox_gamepad_device->count() == 0)
	{
		ui->comboBox_gamepad_device->setEnabled(true);
		ui->label_gamepad_no_device->hide();
	}
	ui->comboBox_gamepad_device->addItem(deviceName, index);
}

void cDockGamepad::slotGamePadDeviceDisconnected(int index)
{
	WriteLog("Gamepad - device disconnected | index: " + QString::number(index) + ", name: "
						 + ui->comboBox_gamepad_device->itemText(index),
		2);
	ui->comboBox_gamepad_device->removeItem(index);
	if (ui->comboBox_gamepad_device->count() == 0)
	{
		ui->comboBox_gamepad_device->setEnabled(false);
		ui->label_gamepad_no_device->show();
	}
}

void cDockGamepad::slotGamepadLook()
{
	// Joystick Axis values vary from -1 to 0 to 1
	double pitch = gamepad.axisLeftX();
	double yaw = gamepad.axisLeftY();
	WriteLog("Gamepad - slotGamepadLook-Yaw | value: " + QString::number(yaw), 3);
	WriteLog("Gamepad - slotGamepadLook-Pitch | value: " + QString::number(pitch), 3);
	ui->sl_gamepad_angle_yaw->setValue(100 * yaw);
	ui->sl_gamepad_angle_pitch->setValue(100 * pitch);
	CVector2<double> yawPitch(pitch, yaw);
	emit gMainInterface->renderedImage->YawAndPitchChanged(yawPitch);
}

void cDockGamepad::slotGamepadMove()
{
	// Joystick Axis values vary from -1 to 0 to 1
	double x = gamepad.axisRightX();
	double y = gamepad.axisRightY() * -1.0;
	WriteLog("Gamepad - slotGamepadMove-X | value: " + QString::number(x), 3);
	WriteLog("Gamepad - slotGamepadMove-Y | value: " + QString::number(y), 3);
	ui->sl_gamepad_movement_x->setValue(100 * x);
	ui->sl_gamepad_movement_y->setValue(100 * y);
	CVector2<double> strafe(x, y);
	emit gMainInterface->renderedImage->ShiftModeChanged(true);
	emit gMainInterface->renderedImage->StrafeChanged(strafe);
}

void cDockGamepad::slotGamepadRoll()
{
	// Trigger values vary from -1 to 0 to 1
	// TODO: verify rotation value calculation
	double value = 0.5 + (gamepad.buttonL2() - gamepad.buttonR2()) / 2.0;
	WriteLog("Gamepad - slotGamepadRoll | value: " + QString::number(value), 3);
	ui->sl_gamepad_angle_roll->setValue(-100 + 200 * value);
	emit gMainInterface->renderedImage->RotationChanged(value * 2.0 - 1.0);
}

void cDockGamepad::slotGamepadSpeed()
{
	// Joystick button values vary from false to true
	double value = 0.5 + ((gamepad.buttonA() ? 1 : 0) - (gamepad.buttonB() ? 1 : 0)) / 2.0;
	WriteLog("Gamepad - slotGamepadSpeed | value: " + QString::number(value), 3);
	ui->sl_gamepad_movement_z->setValue(-100 + 200 * value);
	if (gamepad.buttonA() != gamepad.buttonB())
	{
		if (gamepad.buttonA())
		{
			emit gMainInterface->renderedImage->SpeedChanged(1.1);
		}
		else
		{
			emit gMainInterface->renderedImage->SpeedChanged(0.9);
		}
	}
}

#endif // USE_GAMEPAD
