/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * widget to display video of animation render
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 */

#include "player_widget.hpp"
#include "system.hpp"
#include "global_data.hpp"

PlayerWidget::PlayerWidget(QWidget *parent) : QWidget(parent)
{
	// TODO resize of imageLabel

	infoLabel = new QLabel;
	imageLabel = new QLabel;
	playPauseButton = new QPushButton;
	stopButton = new QPushButton;
	positionSlider = new QSlider(Qt::Horizontal);
	playTimer = new QTimer;
	fpsSpinBox = new MyDoubleSpinBox;

	currentIndex = 0;
	playTimer->setInterval(30);
	playPauseButton->setEnabled(true);
	playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	stopButton->setEnabled(true);
	stopButton->setIcon(style()->standardIcon(QStyle::SP_MediaStop));

	fpsSpinBox->setMinimum(0.01);
	fpsSpinBox->setMaximum(100);
	fpsSpinBox->setDecimals(2);
	fpsSpinBox->setValue(30);

	QLabel *fpsLabel = new QLabel("FPS");
	QBoxLayout *controlLayout = new QHBoxLayout;
	controlLayout->setMargin(0);
	controlLayout->addWidget(stopButton);
	controlLayout->addWidget(playPauseButton);
	controlLayout->addWidget(positionSlider);
	controlLayout->addWidget(fpsLabel);
	controlLayout->addWidget(fpsSpinBox);
	controlLayout->addWidget(infoLabel);

	QBoxLayout *layout = new QVBoxLayout;

	layout->addWidget(imageLabel);
	layout->addLayout(controlLayout);
	setLayout(layout);

	QDir imageDir = QDir(gPar->Get<QString>("anim_flight_dir"));
	imageFiles = imageDir.entryList(QDir::NoDotAndDotDot | QDir::Files);

	if(imageFiles.size() == 0)
	{
		infoLabel->setText(QObject::tr("No frames to play"));
	}
	else
	{
		positionSlider->setRange(0, imageFiles.size() - 1);

		connect(positionSlider, SIGNAL(sliderMoved(int)), this, SLOT(setPosition(int)));
		connect(playPauseButton, SIGNAL(clicked()), this, SLOT(playPause()));
		connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));
		connect(playTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));
		connect(fpsSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setFPS(double)));

		setPosition(currentIndex);
	}
}

PlayerWidget::~PlayerWidget()
{
}

void PlayerWidget::playPause()
{
	if(playTimer->isActive())
	{
		playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
		playTimer->stop();
	}
	else
	{
		playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
		playTimer->start();
	}
}

void PlayerWidget::stop()
{
	playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
	playTimer->stop();
	currentIndex = 0;
	positionSlider->setSliderPosition(currentIndex);
	setPosition(currentIndex);
}

void PlayerWidget::nextFrame()
{
	currentIndex = (currentIndex + 1) % (imageFiles.size() - 1);
	qDebug() << "play next frame: " << currentIndex;
	positionSlider->setSliderPosition(currentIndex);
	updateFrame();
}


void PlayerWidget::setPosition(int position)
{
	currentIndex = position;
	qDebug() << "setPosition: " << currentIndex;
	updateFrame();
}

void PlayerWidget::updateFrame()
{
	infoLabel->setText(QObject::tr("Frame %1 of %2").arg(currentIndex).arg(imageFiles.size() - 1));
	QString fileName = gPar->Get<QString>("anim_flight_dir") + "/" + imageFiles.at(currentIndex);
	imageLabel->setPixmap(fileName);
}

void PlayerWidget::setFPS(double fps){
	playTimer->setInterval(1000.0 / fps);
}
