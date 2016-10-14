/*
 * dock_queue.cpp
 *
 *  Created on: 14 paź 2016
 *      Author: krzysztof
 */

#include "dock_queue.h"

#include "ui_dock_queue.h"

cDockQueue::cDockQueue(QWidget *parent) : QWidget(parent), ui(new Ui::cDockQueue)
{
	ui->setupUi(this);
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);

	ui->queueProgressBarImage->setAlignment(Qt::AlignCenter);
	ui->queueProgressBarImage->hide();

	ui->queueProgressBarAnimation->setAlignment(Qt::AlignCenter);
	ui->queueProgressBarAnimation->hide();

	renderedImageWidget = new RenderedImage;
	renderedImageWidget->SetCursorVisibility(false);
	ui->verticalLayout_queue_preview->addWidget(renderedImageWidget);
}

cDockQueue::~cDockQueue()
{
	delete ui;
	if (renderedImageWidget) delete renderedImageWidget;
}

MyProgressBar* cDockQueue::GetProgressBarImage()
{
	return ui->queueProgressBarImage;
}

MyProgressBar* cDockQueue::GetProgressBarAnimation()
{
	return ui->queueProgressBarAnimation;
}

