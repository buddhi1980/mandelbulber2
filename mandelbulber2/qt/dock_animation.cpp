/*
 * dock_animation.cpp
 *
 *  Created on: 12 paź 2016
 *      Author: krzysztof
 */

#include "dock_animation.h"
#include "../src/file_image.hpp"
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

void cDockAnimation::DisableEXR()
{
	qobject_cast<QStandardItemModel *>(ui->comboBox_keyframe_animation_image_type->model())
		->item(ImageFileSave::IMAGE_FILE_TYPE_EXR)
		->setEnabled(false);
	qobject_cast<QStandardItemModel *>(ui->comboBox_flight_animation_image_type->model())
		->item(ImageFileSave::IMAGE_FILE_TYPE_EXR)
		->setEnabled(false);
}

void cDockAnimation::DisableTIFF()
{
	qobject_cast<QStandardItemModel *>(ui->comboBox_keyframe_animation_image_type->model())
		->item(ImageFileSave::IMAGE_FILE_TYPE_TIFF)
		->setEnabled(false);
	qobject_cast<QStandardItemModel *>(ui->comboBox_flight_animation_image_type->model())
		->item(ImageFileSave::IMAGE_FILE_TYPE_TIFF)
		->setEnabled(false);
}
