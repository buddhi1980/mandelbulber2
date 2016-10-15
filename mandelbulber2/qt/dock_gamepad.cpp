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
	connect(&gamepad, SIGNAL(axisLeftXChanged(double)), this, SLOT(slotGamepadPitch(double)));
	connect(&gamepad, SIGNAL(axisLeftYChanged(double)), this, SLOT(slotGamepadYaw(double)));
	connect(&gamepad, SIGNAL(buttonL2Changed(double)), this, SLOT(slotGamepadRoll()));
	connect(&gamepad, SIGNAL(buttonR2Changed(double)), this, SLOT(slotGamepadRoll()));
	connect(&gamepad, SIGNAL(buttonL1Changed(bool)), this, SLOT(slotShiftModeChange(bool)));

	connect(&gamepad, SIGNAL(axisRightXChanged(double)), this, SLOT(slotGamepadX(double)));
	connect(&gamepad, SIGNAL(axisRightYChanged(double)), this, SLOT(slotGamepadY(double)));
	connect(&gamepad, SIGNAL(buttonAChanged(bool)), this, SLOT(slotGamepadZ()));
	connect(&gamepad, SIGNAL(buttonBChanged(bool)), this, SLOT(slotGamepadZ()));
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
	QString deviceName = ""; // TODO gamepad devicename
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

void cDockGamepad::slotGamepadPitch(double value)
{
	WriteLog("Gamepad - slotGamepadPitch | value: " + QString::number(value), 3);
	ui->sl_gamepad_angle_pitch->setValue(-100 + 200 * value);
	CVector2<double> yawPitch(value, gamepad.axisLeftY());
	yawPitch = (yawPitch * 2) - CVector2<double>(1, 1);
	emit gMainInterface->renderedImage->YawAndPitchChanged(yawPitch);
}

void cDockGamepad::slotGamepadYaw(double value)
{
	WriteLog("Gamepad - slotGamepadYaw | value: " + QString::number(value), 3);
	ui->sl_gamepad_angle_yaw->setValue(-100 + 200 * value);
	CVector2<double> yawPitch(gamepad.axisLeftX(), value);
	yawPitch = (yawPitch * 2) - CVector2<double>(1, 1);
	emit gMainInterface->renderedImage->YawAndPitchChanged(yawPitch);
}

void cDockGamepad::slotGamepadRoll()
{
	double value = 0.5 + (gamepad.buttonR2() - gamepad.buttonL2()) / 2.0;
	WriteLog("Gamepad - slotGamepadRoll | value: " + QString::number(value), 3);
	ui->sl_gamepad_angle_roll->setValue(-100 + 200 * value);
	emit gMainInterface->renderedImage->RotationChanged(value * 2.0 - 1.0);
}

void cDockGamepad::slotShiftModeChange(bool isShifting)
{
	WriteLog("Gamepad - slotShiftModeChange | value: " + QString::number(isShifting), 3);
	emit gMainInterface->renderedImage->ShiftModeChanged(isShifting);
}

void cDockGamepad::slotGamepadX(double value)
{
	WriteLog("Gamepad - slotGamepadX | value: " + QString::number(value), 3);
	ui->sl_gamepad_movement_x->setValue(-100 + 200 * value);
	CVector2<double> strafe(value, gamepad.axisRightY());
	strafe = (strafe * 2) - CVector2<double>(1, 1);
	emit gMainInterface->renderedImage->StrafeChanged(strafe);
}

void cDockGamepad::slotGamepadY(double value)
{
	WriteLog("Gamepad - slotGamepadY | value: " + QString::number(value), 3);
	ui->sl_gamepad_movement_y->setValue(-100 + 200 * value);
	CVector2<double> strafe(gamepad.axisRightX(), value);
	strafe = (strafe * 2) - CVector2<double>(1, 1);
	emit gMainInterface->renderedImage->StrafeChanged(strafe);
}

void cDockGamepad::slotGamepadZ()
{
	double value = 0.5 + ((gamepad.buttonA() ? 1 : 0) - (gamepad.buttonB() ? 1 : 0)) / 2.0;
	WriteLog("Gamepad - slotGamepadZ | value: " + QString::number(value), 3);
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
