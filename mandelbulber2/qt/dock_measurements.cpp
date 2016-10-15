/*
 * dock_measurements.cpp
 *
 *  Created on: 15 paź 2016
 *      Author: krzysztof
 */

#include "dock_measurements.h"

#include "../src/interface.hpp"
#include "../src/rendered_image_widget.hpp"
#include "ui_dock_measurements.h"

cDockMeasurements::cDockMeasurements(QWidget *parent) : QWidget(parent), ui(new Ui::cDockMeasurements)
{
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	ConnectSignals();
}

cDockMeasurements::~cDockMeasurements()
{
	delete ui;
}

void cDockMeasurements::ConnectSignals()
{
	connect(
		ui->pushButton_meas_get_point, SIGNAL(clicked()), this, SLOT(slotPressedButtonGetPoint()));
}

void cDockMeasurements::slotPressedButtonGetPoint()
{
	QList<QVariant> item;
	item.append((int)RenderedImage::clickGetPoint);
	int index = gMainInterface->mainWindow->ui->comboBox_mouse_click_function->findData(item);
	gMainInterface->mainWindow->ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}
