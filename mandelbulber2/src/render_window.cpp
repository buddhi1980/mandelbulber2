/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * RenderWindow class - main program window
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "render_window.hpp"
#include "fractal_container.hpp"

RenderWindow::RenderWindow(QWidget *parent) :
		QMainWindow(parent), ui(new Ui::RenderWindow)
{
	ui->setupUi(this);
	fractalWidgets = new QWidget*[NUMBER_OF_FRACTALS];
	preferencesDialog = NULL;
	volumeSliceDialog = NULL;

	//store default geometry and state
	defaultGeometry = saveGeometry();
	defaultState = saveState();

#ifdef USE_GAMEPAD
	QApplication::connect(&gamepad,
												SIGNAL(axisLeftXChanged(double)),
												this,
												SLOT(slotGamepadPitch(double)));
	QApplication::connect(&gamepad,
												SIGNAL(axisLeftYChanged(double)),
												this,
												SLOT(slotGamepadYaw(double)));
	QApplication::connect(&gamepad, SIGNAL(buttonL2Changed(double)), this, SLOT(slotGamepadRoll()));
	QApplication::connect(&gamepad, SIGNAL(buttonR2Changed(double)), this, SLOT(slotGamepadRoll()));
	QApplication::connect(&gamepad,
												SIGNAL(buttonL1Changed(bool)),
												this,
												SLOT(slotShiftModeChange(bool)));

	QApplication::connect(&gamepad,
												SIGNAL(axisRightXChanged(double)),
												this,
												SLOT(slotGamepadX(double)));
	QApplication::connect(&gamepad,
												SIGNAL(axisRightYChanged(double)),
												this,
												SLOT(slotGamepadY(double)));
	QApplication::connect(&gamepad, SIGNAL(buttonAChanged(bool)), this, SLOT(slotGamepadZ()));
	QApplication::connect(&gamepad, SIGNAL(buttonBChanged(bool)), this, SLOT(slotGamepadZ()));
	QApplication::connect(this->ui->groupCheck_gamepad_enabled,
												SIGNAL(toggled(bool)),
												&gamepad,
												SLOT(setConnected(bool)));
#else
	ui->menuView->removeAction(ui->actionShow_gamepad_dock);
	removeDockWidget(ui->dockWidget_gamepad_dock);
#endif
}

RenderWindow::~RenderWindow()
{
	delete ui;
	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
		delete fractalWidgets[i];
	delete[] fractalWidgets;
}




