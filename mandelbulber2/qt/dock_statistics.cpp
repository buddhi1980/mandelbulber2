/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Widget which contains UI for statistics
 */

#include "dock_statistics.h"

#include "../src/initparameters.hpp"
#include "../src/interface.hpp"
#include "../src/render_window.hpp"
#include "dock_rendering_engine.h"
#include "ui_dock_statistics.h"

cDockStatistics::cDockStatistics(QWidget *parent) : QWidget(parent), ui(new Ui::cDockStatistics)
{
	ui->setupUi(this);
	ui->tableWidget_statistics->verticalHeader()->setDefaultSectionSize(
		gPar->Get<int>("ui_font_size") + 6);
}

cDockStatistics::~cDockStatistics()
{
	delete ui;
}

void cDockStatistics::UpdateDistanceToFractal(double distance) const
{
	ui->tableWidget_statistics->item(5, 0)->setText(QString::number(distance));
}

void cDockStatistics::slotUpdateStatistics(cStatistics stat) const
{
	ui->label_histogram_de->SetBarcolor(QColor(0, 255, 0));
	ui->label_histogram_de->UpdateHistogram(stat.histogramStepCount);
	ui->label_histogram_iter->UpdateHistogram(stat.histogramIterations);

	ui->tableWidget_statistics->item(0, 0)->setText(
		QString::number(stat.GetTotalNumberOfIterations()));
	ui->tableWidget_statistics->item(1, 0)->setText(
		QString::number(stat.GetNumberOfIterationsPerPixel()));
	ui->tableWidget_statistics->item(2, 0)->setText(
		QString::number(stat.GetNumberOfIterationsPerSecond()));
	ui->tableWidget_statistics->item(3, 0)->setText(stat.GetDETypeString());
	ui->tableWidget_statistics->item(4, 0)->setText(QString::number(stat.GetMissedDEPercentage()));
	gMainInterface->mainWindow->GetWidgetDockRenderingEngine()->UpdateLabelWrongDEPercentage(
		tr("Percentage of wrong distance estimations: %1").arg(stat.GetMissedDEPercentage()));
	gMainInterface->mainWindow->GetWidgetDockRenderingEngine()->UpdateLabelUsedDistanceEstimation(
		tr("Used distance estimation algorithm: %1").arg(stat.GetDETypeString()));
}
