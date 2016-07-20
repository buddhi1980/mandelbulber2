/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016 Krzysztof Marczak        §R-==%w["'~5]m%=L.=~5N
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
 * RenderWindow class - main program window
 *
 * This file contains implementation of the gamepad slots in RenderWindow class.
 * See also header render_window.hpp and whole implementation of class
 * spread over render_window_*.cpp
 */

#include "render_window.hpp"
#include "interface.hpp"

#ifdef USE_GAMEPAD
void RenderWindow::slotChangeGamepadIndex(int index)
{
	gamepad.setDeviceId(index);
	WriteLog("Gamepad - slotChangeGamepadIndex: " + QString::number(index), 2);
}

void RenderWindow::slotGamePadDeviceConnected(int index)
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

void RenderWindow::slotGamePadDeviceDisconnected(int index)
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

void RenderWindow::slotGamepadPitch(double value)
{
	WriteLog("Gamepad - slotGamepadPitch | value: " + QString::number(value), 3);
	ui->sl_gamepad_angle_pitch->setValue(-100 + 200 * value);
	CVector2<double> yawPitch(value, gamepad.axisLeftY());
	yawPitch = (yawPitch * 2) - CVector2<double>(1, 1);
	emit gMainInterface->renderedImage->YawAndPitchChanged(yawPitch);
}

void RenderWindow::slotGamepadYaw(double value)
{
	WriteLog("Gamepad - slotGamepadYaw | value: " + QString::number(value), 3);
	ui->sl_gamepad_angle_yaw->setValue(-100 + 200 * value);
	CVector2<double> yawPitch(gamepad.axisLeftX(), value);
	yawPitch = (yawPitch * 2) - CVector2<double>(1, 1);
	emit gMainInterface->renderedImage->YawAndPitchChanged(yawPitch);
}

void RenderWindow::slotGamepadRoll()
{
	double value = 0.5 + (gamepad.buttonR2() - gamepad.buttonL2()) / 2.0;
	WriteLog("Gamepad - slotGamepadRoll | value: " + QString::number(value), 3);
	ui->sl_gamepad_angle_roll->setValue(-100 + 200 * value);
	emit gMainInterface->renderedImage->RotationChanged(value * 2.0 - 1.0);
}

void RenderWindow::slotShiftModeChange(bool isShifting)
{
	WriteLog("Gamepad - slotShiftModeChange | value: " + QString::number(isShifting), 3);
	emit gMainInterface->renderedImage->ShiftModeChanged(isShifting);
}

void RenderWindow::slotGamepadX(double value)
{
	WriteLog("Gamepad - slotGamepadX | value: " + QString::number(value), 3);
	ui->sl_gamepad_movement_x->setValue(-100 + 200 * value);
	CVector2<double> strafe(value, gamepad.axisRightY());
	strafe = (strafe * 2) - CVector2<double>(1, 1);
	emit gMainInterface->renderedImage->StrafeChanged(strafe);
}

void RenderWindow::slotGamepadY(double value)
{
	WriteLog("Gamepad - slotGamepadY | value: " + QString::number(value), 3);
	ui->sl_gamepad_movement_y->setValue(-100 + 200 * value);
	CVector2<double> strafe(gamepad.axisRightX(), value);
	strafe = (strafe * 2) - CVector2<double>(1, 1);
	emit gMainInterface->renderedImage->StrafeChanged(strafe);
}

void RenderWindow::slotGamepadZ()
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
