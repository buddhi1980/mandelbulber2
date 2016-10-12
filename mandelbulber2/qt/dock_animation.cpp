/*
 * dock_animation.cpp
 *
 *  Created on: 12 paź 2016
 *      Author: krzysztof
 */

#include "dock_animation.h"
#include "ui_dock_animation.h"

cDockAnimation::cDockAnimation(QWidget *parent) : QWidget(parent), ui(new Ui::cDockAnimation)
{
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
}

cDockAnimation::~cDockAnimation()
{
	delete ui;
}
