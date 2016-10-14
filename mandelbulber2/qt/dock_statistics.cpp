/*
 * dock_statistics.cpp
 *
 *  Created on: 14 paź 2016
 *      Author: krzysztof
 */

#include "dock_statistics.h"

#include "../src/interface.hpp"
#include "ui_dock_statistics.h"
#include "../src/initparameters.hpp"

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

void cDockStatistics::UpdateDistanceToFractal(double distance)
{
	ui->tableWidget_statistics->item(5, 0)->setText(QString::number(distance));
}

void cDockStatistics::slotUpdateStatistics(cStatistics stat)
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
	gMainInterface->mainWindow->ui->widgetDockRenderingEngine->UpdateLabelWrongDEPercentage(
		tr("Percentage of wrong distance estimations: %1").arg(stat.GetMissedDEPercentage()));
	gMainInterface->mainWindow->ui->widgetDockRenderingEngine->UpdateLabelUsedDistanceEstimation(
		tr("Used distance estimation algorithm: %1").arg(stat.GetDETypeString()));
}
