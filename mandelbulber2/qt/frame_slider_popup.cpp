/*
 * frame_slider_popup.cpp
 *
 *  Created on: 25 lis 2017
 *      Author: krzysztof
 */

#include "frame_slider_popup.h"
#include "ui_slider_popup.h"

cFrameSiderPopup::cFrameSiderPopup(QWidget *parent) : QFrame(parent), ui(new Ui::cFrameSiderPopup)
{
	ui->setupUi(this);

	connect(ui->slider, SIGNAL(sliderReleased()), this, SLOT(sliderReleased()));
}

cFrameSiderPopup::~cFrameSiderPopup()
{
	delete ui;
}

int cFrameSiderPopup::value() const
{
	return ui->slider->value();
}

void cFrameSiderPopup::sliderReleased()
{
	ui->slider->setValue(500);
}
