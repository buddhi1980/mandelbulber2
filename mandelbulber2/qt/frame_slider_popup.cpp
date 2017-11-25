/*
 * frame_slider_popup.cpp
 *
 *  Created on: 25 lis 2017
 *      Author: krzysztof
 */

#include "frame_slider_popup.h"
#include "ui_slider_popup.h"
#include "src/system.hpp"

cFrameSiderPopup::cFrameSiderPopup(QWidget *parent) : QFrame(parent), ui(new Ui::cFrameSiderPopup)
{
	ui->setupUi(this);

	connect(ui->slider, SIGNAL(sliderReleased()), this, SLOT(sliderReleased()));
	connect(ui->buClose, SIGNAL(pressed()), this, SLOT(hide()));
	connect(ui->buDouble, SIGNAL(pressed()), this, SIGNAL(doublePressed()));
	connect(ui->buHalf, SIGNAL(pressed()), this, SIGNAL(halfPressed()));
	connect(ui->buInteger, SIGNAL(pressed()), this, SIGNAL(intPressed()));
	connect(ui->buReset, SIGNAL(pressed()), this, SIGNAL(resetPressed()));
	connect(ui->buZero, SIGNAL(pressed()), this, SIGNAL(zeroPressed()));

	sliderTimer = new QTimer();
	sliderTimer->setSingleShot(true);
	connect(sliderTimer, SIGNAL(timeout()), this, SLOT(slotSliderTimerTrigger()));
	sliderTimer->start(100);
}

cFrameSiderPopup::~cFrameSiderPopup()
{
	delete ui;
	sliderTimer->stop();
	delete sliderTimer;
}

int cFrameSiderPopup::value() const
{
	return ui->slider->value();
}

void cFrameSiderPopup::sliderReleased()
{
	ui->slider->setValue(500);
}

void cFrameSiderPopup::slotSliderTimerTrigger()
{
	sliderTimer->start(100);
	emit timerTrigger();
}
