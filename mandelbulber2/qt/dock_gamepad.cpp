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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Robert Pancoast (RobertPancoast77@gmail.com)
 *
 * These objects enable control of flight animation using a gamepad controller.
 */

#include "dock_gamepad.h"

#include "ui_dock_gamepad.h"

#include "src/automated_widgets.hpp"
#include "src/interface.hpp"
#include "src/rendered_image_widget.hpp"
#include "src/system.hpp"

cDockGamepad::cDockGamepad(QWidget *parent) : QWidget(parent), ui(new Ui::cDockGamepad)
{
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	ConnectSignals();

#ifdef USE_GAMEPAD
	QList<int> deviceIds = QGamepadManager::instance()->connectedGamepads();
	if (deviceIds.size() > 0) gamepad.setDeviceId(deviceIds[0]);
	populateGamepadList();
#endif
}

cDockGamepad::~cDockGamepad()
{
	delete ui;
}

void cDockGamepad::ConnectSignals() const
{
#ifdef USE_GAMEPAD
	connect(ui->comboBox_gamepad_device, SIGNAL(currentIndexChanged(int)), this,
		SLOT(slotChangeGamepadIndex(int)));

	// Left Joystick controls Look Angle
	connect(&gamepad, SIGNAL(axisLeftYChanged(double)), this, SLOT(slotGamepadLook()));
	connect(&gamepad, SIGNAL(axisLeftXChanged(double)), this, SLOT(slotGamepadLook()));

	// Right Joystick controls Movement Direction
	connect(&gamepad, SIGNAL(axisRightXChanged(double)), this, SLOT(slotGamepadMove()));
	connect(&gamepad, SIGNAL(axisRightYChanged(double)), this, SLOT(slotGamepadMove()));

	// Left and Right Triggers control Reverse and Accelerator
	connect(&gamepad, SIGNAL(buttonL2Changed(double)), this, SLOT(slotGamepadMove()));
	connect(&gamepad, SIGNAL(buttonR2Changed(double)), this, SLOT(slotGamepadMove()));

	// Start Button will pause the flight
	connect(&gamepad, SIGNAL(buttonStartChanged(bool)), this, SLOT(slotGamepadPause(bool)));

	// Left and Right Shoulder Buttons control Roll Rotation
	connect(&gamepad, SIGNAL(buttonL1Changed(bool)), this, SLOT(slotGamepadRoll()));
	connect(&gamepad, SIGNAL(buttonR1Changed(bool)), this, SLOT(slotGamepadRoll()));

	// A and B buttons control the Movement Speed
	connect(&gamepad, SIGNAL(buttonAChanged(bool)), this, SLOT(slotGamepadSpeed()));
	connect(&gamepad, SIGNAL(buttonBChanged(bool)), this, SLOT(slotGamepadSpeed()));

	connect(QGamepadManager::instance(), SIGNAL(gamepadConnected(int)), this,
		SLOT(slotGamePadDeviceChanged()));
	connect(QGamepadManager::instance(), SIGNAL(gamepadDisconnected(int)), this,
		SLOT(slotGamePadDeviceChanged()));
#endif // USE_GAMEPAD
}

#ifdef USE_GAMEPAD
void cDockGamepad::slotChangeGamepadIndex(int index)
{
	QList<int> deviceIds = QGamepadManager::instance()->connectedGamepads();
	if (deviceIds.size() > index - 1)
	{
		gamepad.setDeviceId(deviceIds[index]);
		WriteLog("Gamepad - slotChangeGamepadIndex: " + QString::number(index), 2);
	}
}

void cDockGamepad::slotGamePadDeviceChanged() const
{
	populateGamepadList();
}

void cDockGamepad::populateGamepadList() const
{
	ui->comboBox_gamepad_device->clear();
	QList<int> deviceIds = QGamepadManager::instance()->connectedGamepads();

	for (int i = 0; i < deviceIds.size(); i++)
	{
		QGamepad gp(deviceIds[i]);
		QString deviceName = gp.name();
		if (deviceName == "") deviceName = "Device #" + QString::number(i);
		WriteLog("Gamepad - device list changed", 2);
		ui->comboBox_gamepad_device->addItem(deviceName, i);
	}
	if (deviceIds.size() == 0)
	{
		ui->comboBox_gamepad_device->setEnabled(false);
		ui->label_gamepad_no_device->show();
	}
	else
	{
		ui->comboBox_gamepad_device->setEnabled(true);
		ui->label_gamepad_no_device->hide();
	}
}

void cDockGamepad::slotGamepadLook() const
{
	if (ui->groupCheck_gamepad_enabled->isChecked())
	{
		// Joystick Axis values vary from -1 to 0 to 1
		const double pitch = gamepad.axisLeftX();
		const double yaw = gamepad.axisLeftY();
		WriteLog("Gamepad - slotGamepadLook-Yaw | value: " + QString::number(yaw), 3);
		WriteLog("Gamepad - slotGamepadLook-Pitch | value: " + QString::number(pitch), 3);
		ui->sl_gamepad_angle_yaw->setValue(int(100 * yaw));
		ui->sl_gamepad_angle_pitch->setValue(int(100 * pitch));
		const double sensitivity = 0.5;
		CVector2<double> yawPitch(pitch, yaw);
		yawPitch = yawPitch.Deadband() * sensitivity;
		emit gMainInterface->renderedImage->YawAndPitchChanged(yawPitch);
	}
}

void cDockGamepad::slotGamepadMove() const
{
	if (ui->groupCheck_gamepad_enabled->isChecked())
	{
		// Joystick Axis values vary from -1 to 0 to 1
		// -1 for down, and 1 for up, 0 for neutral
		// Invert the Y Axis for right Joystick
		const double x = gamepad.axisRightX();
		const double y = gamepad.axisRightY() * -1.0;
		CVector2<double> strafe(x, y);
		double sensitivity = 5.0;
		strafe = strafe.Deadband() * sensitivity;
		const bool joystick = fabs(strafe.x) > 0 || fabs(strafe.y) > 0;
		// Trigger values vary from 0 to 1
		const double reverse = gamepad.buttonL2();
		const double forward = gamepad.buttonR2();
		double z = forward - reverse / 2.0;
		const bool trigger = fabs(z) > 0;
		WriteLog("Gamepad - slotGamepadMove-X | value: " + QString::number(x), 3);
		WriteLog("Gamepad - slotGamepadMove-Y | value: " + QString::number(y), 3);
		WriteLog("Gamepad - slotGamepadMove-Z | value: " + QString::number(z), 3);
		ui->sl_gamepad_movement_x->setValue(int(100 * x));
		ui->sl_gamepad_movement_y->setValue(int(100 * y));
		ui->sl_gamepad_movement_z->setValue(int(100 * z));
		// Maintain z-axis speed
		sensitivity = 1 / 10.0;
		const double threshold = .001;
		z = z * sensitivity;
		// Forward Accelerate [threshold to 1 / sensitivity]
		if (fabs(z) < threshold && z >= 0.0) z = threshold;
		// Reverse Backwards [-threshold to -1 / sensitivity]
		if (fabs(z) < threshold && z <= 0.0) z = -threshold;
		if (joystick) emit gMainInterface->renderedImage->StrafeChanged(strafe / 2.0);
		if (trigger) emit gMainInterface->renderedImage->SpeedSet(z / 2.0);
	}
}

void cDockGamepad::slotGamepadPause(bool value)
{
	if (ui->groupCheck_gamepad_enabled->isChecked())
	{
		if (!value)
		{
			WriteLog("Gamepad - slotGamepadPause | activated", 3);
			emit gMainInterface->renderedImage->Pause();
		}
	}
}

void cDockGamepad::slotGamepadRoll() const
{
	if (ui->groupCheck_gamepad_enabled->isChecked())
	{
		// Button values are either false to true
		double value = 0;
		if (gamepad.buttonL1()) value += 1;
		if (gamepad.buttonR1()) value -= 1;
		WriteLog("Gamepad - slotGamepadRoll | value: " + QString::number(value), 3);
		ui->sl_gamepad_angle_roll->setValue(int(100 * value));
		emit gMainInterface->renderedImage->RotationChanged(value);
	}
}

void cDockGamepad::slotGamepadSpeed() const
{
	if (ui->groupCheck_gamepad_enabled->isChecked())
	{
		// Button values are either false to true
		double value = 0;
		if (gamepad.buttonA()) value += 1;
		if (gamepad.buttonB()) value -= 1;
		WriteLog("Gamepad - slotGamepadSpeed | value: " + QString::number(value), 3);
		ui->sl_gamepad_movement_z->setValue(int(100 * value));
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
}

#endif // USE_GAMEPAD
