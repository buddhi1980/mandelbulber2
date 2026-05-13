/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-23 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Widget which contains UI for fractals
 */

#include "dock_fractal.h"

#include "ui_dock_fractal.h"

#include "dock_rendering_engine.h"
#include "my_tab_bar.h"

#include "src/ao_modes.h"
#include "src/automated_widgets.hpp"
#include "src/error_message.hpp"
#include "src/fractal_enums.h"
#include "src/global_data.hpp"
#include "src/initparameters.hpp"
#include "src/interface.hpp"
#include "src/render_window.hpp"
#include "src/rendered_image_widget.hpp"
#include "src/write_log.hpp"

#include "formula/definition/all_fractal_list.hpp"
#include "navigator_window.h"

cDockFractal::cDockFractal(QWidget *parent)
		: QWidget(parent), cMyWidgetWithParams(), ui(new Ui::cDockFractal)
{
	ui->setupUi(this);

	ConnectSignals();
}

cDockFractal::~cDockFractal()
{
	delete ui;
}

void cDockFractal::SynchronizeInterfaceFractals(std::shared_ptr<cParameterContainer> par,
	std::shared_ptr<cFractalContainer> parFractal, qInterface::enumReadWrite mode) const
{

	WriteLog("cInterface::SynchronizeInterface: tab_description", 3);
	SynchronizeInterfaceWindow(ui->tab_description, par, mode);

	ui->widget_objectsTree->SynchronizeInterface(par, parFractal, mode);
}

void cDockFractal::ConnectSignals() const {}

// initialize ui for hybrid fractal components
void cDockFractal::InitializeFractalUi() const
{
	/*
	WriteLog("cInterface::InitializeFractalUi(QString &uiFileName) started", 2);
	// MyUiLoader loader;

	// QFile uiFile(uiFileName);

	// if (uiFile.exists())
	//{
	//		uiFile.open(QFile::ReadOnly);
	//		fractalWidgets[0] = loader.load(&uiFile);
	//		ui->verticalLayout_fractal_1->addWidget(fractalWidgets[0]);
	//		fractalWidgets[0]->show();

	for (int i = 0; i < NUMBER_OF_FRACTALS; i++)
	{
		if (i == 0)
			ui->tabWidget_fractals->setTabText(i, QString("#1: Mandelbulb"));
		else
		{
			ui->tabWidget_fractals->setTabText(i, QString("#%1: None").arg(i + 1));
			ui->tabWidget_fractals
				->findChild<QScrollArea *>("scrollArea_fractal_" + QString::number(i + 1))
				->setEnabled(false);
		}
		fractalTabs[i]->AssignParameterContainers(params, fractalParams);
		fractalTabs[i]->Init(i == 0, i);
		fractalTabs[i]->AssignParentDockFractal(this);
	}

	static_cast<MyTabBar *>(ui->tabWidget_fractals->tabBar())->setupMoveButtons();

	//}
	WriteLog("cInterface::InitializeFractalUi(QString &uiFileName) finished", 2);
	*/
}

void cDockFractal::slotPressedButtonGetJuliaConstant()
{
	//	QList<QVariant> item;
	//	item.append(int(RenderedImage::clickGetJuliaConstant));
	//	int index = gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->findData(item);
	//	gMainInterface->mainWindow->GetComboBoxMouseClickFunction()->setCurrentIndex(index);
	//	gMainInterface->renderedImage->setClickMode(item);
}

void cDockFractal::AssignParameterContainers(
	std::shared_ptr<cParameterContainer> _params, std::shared_ptr<cFractalContainer> _fractalParams)
{
	cMyWidgetWithParams::AssignParameterContainers(_params, _fractalParams);
	//	ui->widgetPrimitivesManager->AssignParameterContainers(_params, _fractalParams);
	//	ui->widgetPrimitivesManager->Init();
}

void cDockFractal::AssignSpecialWidgets(
	RenderedImage *_renderedImage, QComboBox *_mouseFunctionCombo)
{
	cMyWidgetWithParams::AssignSpecialWidgets(_renderedImage, _mouseFunctionCombo);
	//	ui->widgetPrimitivesManager->AssignSpecialWidgets(_renderedImage, _mouseFunctionCombo);
}

void cDockFractal::slotPressedButtonNavi()
{
	gMainInterface->SynchronizeInterface(params, fractalParams, qInterface::read);
	cNavigatorWindow *navigator = new cNavigatorWindow();
	cDockFractal *leftWidget = new cDockFractal();
	navigator->AddLeftWidget(leftWidget);
	navigator->setAttribute(Qt::WA_DeleteOnClose);
	navigator->SetInitialParameters(params, fractalParams);
	navigator->SetMouseClickFunction(gMainInterface->GetMouseClickFunction());
	leftWidget->InitializeFractalUi();
	navigator->SynchronizeInterface(qInterface::write);

	// FIXME slotNewParametersFromNavi
	//	connect(navigator, &cNavigatorWindow::signalChangesAccepted, this,
	//		&cDockFractal::slotNewParametersFromNavi);

	navigator->show();
	navigator->AllPrepared();
}
