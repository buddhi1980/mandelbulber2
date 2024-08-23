/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cFrameSliderPopup - popup slider for quick editing of numerical input fields
 * This widget popups when the associated input field is clicked and
 * contains a slider and different buttons to edit the field
 */

#include "frame_slider_popup.h"

#include <QTimer>

#include "ui_slider_popup.h"

#include "src/system.hpp"

cFrameSliderPopup::cFrameSliderPopup(QWidget *parent)
		: QFrame(parent), ui(new Ui::cFrameSliderPopup)
{
	ui->setupUi(this);

	connect(ui->slider, SIGNAL(sliderReleased()), this, SLOT(slotSliderReleased()));
	connect(ui->slider, SIGNAL(sliderPressed()), this, SIGNAL(sliderPressed()));
	connect(ui->slider, SIGNAL(sliderMoved(int)), this, SIGNAL(sliderMoved(int)));
	connect(ui->buClose, SIGNAL(pressed()), this, SLOT(hide()));
	connect(ui->buDouble, SIGNAL(pressed()), this, SIGNAL(doublePressed()));
	connect(ui->buHalf, SIGNAL(pressed()), this, SIGNAL(halfPressed()));
	connect(ui->buInteger, SIGNAL(pressed()), this, SIGNAL(intPressed()));
	connect(ui->buReset, SIGNAL(pressed()), this, SIGNAL(resetPressed()));
	connect(ui->buZero, SIGNAL(pressed()), this, SIGNAL(zeroPressed()));
	connect(ui->buMinus, SIGNAL(pressed()), this, SIGNAL(minusPressed()));
	connect(ui->radioButton_super_fine, &QRadioButton::pressed, this,
		&cFrameSliderPopup::slotSelectedPrecisionSuperFine);
	connect(ui->radioButton_very_fine, &QRadioButton::pressed, this,
		&cFrameSliderPopup::slotSelectedPrecisionVeryFine);
	connect(ui->radioButton_fine, &QRadioButton::pressed, this,
		&cFrameSliderPopup::slotSelectedPrecisionFine);
	connect(ui->radioButton_normal, &QRadioButton::pressed, this,
		&cFrameSliderPopup::slotSelectedPrecisionNormal);
	connect(ui->radioButton_coarse, &QRadioButton::pressed, this,
		&cFrameSliderPopup::slotSelectedPrecisionCoarse);

	sliderTimer = new QTimer(this);
	sliderTimer->setSingleShot(true);
	connect(sliderTimer, SIGNAL(timeout()), this, SLOT(slotSliderTimerTrigger()));
	sliderTimer->start(100);

	integerMax = 0;
	integerMin = 0;
	integerMode = false;
	dialMode = false;
	dialScale = 1;
	ui->dial->hide();
	ui->buUp->hide();
	ui->buRight->hide();
	ui->buDown->hide();
	ui->buLeft->hide();
}

cFrameSliderPopup::~cFrameSliderPopup()
{
	sliderTimer->stop();
	delete ui;
}

int cFrameSliderPopup::value() const
{
	if (dialMode)
	{
		return ui->dial->value();
	}
	else
	{
		return ui->slider->value();
	}
}

enumSliderPrecision cFrameSliderPopup::precision() const
{
	if (ui->radioButton_super_fine->isChecked())
		return enumSliderPrecision::precisionSuperFine;
	else if (ui->radioButton_very_fine->isChecked())
		return enumSliderPrecision::precisionVeryFine;
	else if (ui->radioButton_fine->isChecked())
		return enumSliderPrecision::precisionFine;
	else if (ui->radioButton_normal->isChecked())
		return enumSliderPrecision::precisionNormal;
	else if (ui->radioButton_coarse->isChecked())
		return enumSliderPrecision::precisionCoarse;
	else
		return enumSliderPrecision::precisionNormal;
}

void cFrameSliderPopup::slotSliderReleased()
{
	if (!integerMode)
	{
		blockSignals(true);
		ui->slider->setValue(500);
		blockSignals(false);
		emit sliderReleased();
	}
}

void cFrameSliderPopup::slotSliderTimerTrigger()
{
	sliderTimer->start(100);
	emit timerTrigger();
}

void cFrameSliderPopup::SetIntegerMode(int min, int max, int val)
{
	ui->slider->setMaximum(max);
	ui->slider->setMinimum(min);
	ui->slider->setValue(val);
	if (!integerMode)
	{
		integerMode = true;
		ui->buInteger->hide();
		ui->buZero->hide();
		ui->buMinus->hide();
		sliderTimer->stop();
		connect(ui->slider, SIGNAL(valueChanged(int)), this, SIGNAL(valueChanged(int)));
	}
}

void cFrameSliderPopup::SetDialMode(int scale, double val)
{
	ui->dial->show();
	ui->buUp->show();
	ui->buRight->show();
	ui->buDown->show();
	ui->buLeft->show();
	ui->buZero->hide();
	ui->buDouble->hide();
	ui->buHalf->hide();
	ui->buMinus->hide();
	// ui->slider->hide();
	ui->dial->setMaximum(scale * 180);
	ui->dial->setMinimum(-scale * 180);
	ui->dial->setValue(int(val * scale));
	dialScale = scale;
	dialMode = true;
	connect(ui->dial, SIGNAL(valueChanged(int)), this, SLOT(slotDialValueChanged(int)));
	connect(ui->buUp, SIGNAL(pressed()), this, SIGNAL(upPressed()));
	connect(ui->buDown, SIGNAL(pressed()), this, SIGNAL(downPressed()));
	connect(ui->buLeft, SIGNAL(pressed()), this, SIGNAL(leftPressed()));
	connect(ui->buRight, SIGNAL(pressed()), this, SIGNAL(rightPressed()));
}

void cFrameSliderPopup::slotUpdateValue(int val)
{
	blockSignals(true);
	ui->slider->setValue(val);
	blockSignals(false);
}

void cFrameSliderPopup::slotUpdateValue(double val)
{
	blockSignals(true);
	ui->dial->setValue(int(val * dialScale));
	blockSignals(false);
}

void cFrameSliderPopup::slotDialValueChanged(int val)
{
	emit valueChanged(double(val) / dialScale);
}

void cFrameSliderPopup::slotSelectedPrecisionSuperFine()
{
	emit changedPrecision(enumSliderPrecision::precisionSuperFine);
}
void cFrameSliderPopup::slotSelectedPrecisionVeryFine()
{
	emit changedPrecision(enumSliderPrecision::precisionVeryFine);
}
void cFrameSliderPopup::slotSelectedPrecisionFine()
{
	emit changedPrecision(enumSliderPrecision::precisionFine);
}
void cFrameSliderPopup::slotSelectedPrecisionNormal()
{
	emit changedPrecision(enumSliderPrecision::precisionNormal);
}
void cFrameSliderPopup::slotSelectedPrecisionCoarse()
{
	emit changedPrecision(enumSliderPrecision::precisionCoarse);
}
