/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * This file contains implementation of constructor and destructor
 * of the RenderWindow class.
 * See also header render_window.hpp and whole implementation of class
 * spread over render_window_*.cpp
 */

#include "render_window.hpp"

#include "ui_render_window.h"

#include "automated_widgets.hpp"

RenderWindow::RenderWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::RenderWindow)
{
	ui->setupUi(this);
	preferencesDialog = nullptr;
	voxelExportDialog = nullptr;
	meshExportDialog = nullptr;
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);

	// store default geometry and state
	defaultGeometry = saveGeometry();
	defaultState = saveState();

	buttonPressTimer = new QTimer(this);
	connect(buttonPressTimer, SIGNAL(timeout()), this, SLOT(slotButtonLongPress()));
	buttonPressTimer->start(100);

#ifndef USE_GAMEPAD
	ui->menuView->removeAction(ui->actionShow_gamepad_dock);
	removeDockWidget(ui->dockWidget_gamepad_dock);
#endif
}

RenderWindow::~RenderWindow()
{
	delete ui;
}

cDockAnimation *RenderWindow::GetWidgetDockAnimation() const
{
	return ui->widgetDockAnimation;
}

QWidget *RenderWindow::GetCentralWidget() const
{
	return ui->centralwidget;
}

cDockNavigation *RenderWindow::GetWidgetDockNavigation() const
{
	return ui->widgetDockNavigation;
}

cDockStatistics *RenderWindow::GetWidgetDockStatistics() const
{
	return ui->widgetDockStatistics;
}

cDockQueue *RenderWindow::GetWidgetDockQueue() const
{
	return ui->widgetDockQueue;
}

cDockImageAdjustments *RenderWindow::GetWidgetDockImageAdjustments() const
{
	return ui->widgetImageAdjustments;
}

cDockRenderingEngine *RenderWindow::GetWidgetDockRenderingEngine() const
{
	return ui->widgetDockRenderingEngine;
}

cDockFractal *RenderWindow::GetWidgetDockFractal() const
{
	return ui->widgetDockFractal;
}

cDockEffects *RenderWindow::GetWidgetDockEffects() const
{
	return ui->widgetEffects;
}

QComboBox *RenderWindow::GetComboBoxMouseClickFunction() const
{
	return ui->comboBox_mouse_click_function;
}
