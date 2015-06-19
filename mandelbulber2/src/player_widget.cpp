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
	imageLabel->setSizePolicy( QSizePolicy::Ignored, QSizePolicy::Ignored );
	imageLabel->setAlignment(Qt::AlignCenter);
	fpsSpinBox->setMinimum(0.01);
	fpsSpinBox->setMaximum(100);
	fpsSpinBox->setDecimals(2);
	fpsSpinBox->setValue(30);

	QLabel *fpsLabel = new QLabel("FPS");
	QHBoxLayout *controlLayout = new QHBoxLayout;
	controlLayout->setMargin(5);
	controlLayout->addWidget(stopButton);
	controlLayout->addWidget(playPauseButton);
	controlLayout->addWidget(positionSlider);
	controlLayout->addWidget(fpsLabel);
	controlLayout->addWidget(fpsSpinBox);
	controlLayout->addWidget(infoLabel);

	QBoxLayout *layout = new QVBoxLayout;

	layout->setMargin(0);
	layout->addWidget(imageLabel);
	layout->addLayout(controlLayout);
	layout->setStretch(0, 1);
	setLayout(layout);

//	QDir imageDir = QDir(gPar->Get<QString>("anim_flight_dir"));
//	QStringList imageFileExtensions;
//	imageFileExtensions << "*.jpg" << "*.jpeg" << "*.png";
//	imageDir.setNameFilters(imageFileExtensions);
//	imageFiles = imageDir.entryList(QDir::NoDotAndDotDot | QDir::Files);

	connect(positionSlider, SIGNAL(sliderMoved(int)), this, SLOT(setPosition(int)));
	connect(playPauseButton, SIGNAL(clicked()), this, SLOT(playPause()));
	connect(stopButton, SIGNAL(clicked()), this, SLOT(stop()));
	connect(playTimer, SIGNAL(timeout()), this, SLOT(nextFrame()));
	connect(fpsSpinBox, SIGNAL(valueChanged(double)), this, SLOT(setFPS(double)));
}

PlayerWidget::~PlayerWidget()
{
	stop();
	delete playTimer;
}

void PlayerWidget::SetFilePath(QString filePath)
{
	dirPath = filePath;
	if(dirPath.right(1) == QString("/"))
	{
		dirPath.truncate(dirPath.length() - 1);
	}

	QDir imageDir = QDir(filePath);
	QStringList imageFileExtensions;
	imageFileExtensions << "*.jpg" << "*.jpeg" << "*.png";
	imageDir.setNameFilters(imageFileExtensions);
	imageFiles = imageDir.entryList(QDir::NoDotAndDotDot | QDir::Files);

	if(imageFiles.size() == 0)
	{
		imageLabel->setText(QObject::tr("No frames to play"));
	}
	else
	{
		positionSlider->setRange(0, imageFiles.size() - 1);
		setPosition(currentIndex);
	}
}

void PlayerWidget::playPause()
{
	if(imageFiles.size() > 0)
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
	currentIndex = (currentIndex + 1) % imageFiles.size();
	positionSlider->setSliderPosition(currentIndex);
	updateFrame();
}


void PlayerWidget::setPosition(int position)
{
	currentIndex = position;
	updateFrame();
}

void PlayerWidget::updateFrame()
{
	if(!gPar)
	{
		close();
		return;
	}
	if(imageFiles.size() == 0) return;
	QString fileName = dirPath + "/" + imageFiles.at(currentIndex);
	QPixmap pix(fileName);
	if(pix.isNull())
	{
		qWarning() << "Image could not be loaded, " << fileName;
		return;
	}
	infoLabel->setText(QObject::tr("Frame %1 of %2").arg(currentIndex + 1).arg(imageFiles.size()));
	if((1.0 * imageLabel->width() / imageLabel->height()) > (1.0 * pix.width() / pix.height()))
	{
		// imageLabel is relative wider than pix
		imageLabel->setPixmap(pix.scaled((imageLabel->height() * pix.width() / pix.height()), imageLabel->height()));
	}
	else
	{
		imageLabel->setPixmap(pix.scaled(imageLabel->width(), (imageLabel->width() * pix.height() / pix.width())));
	}
}


void PlayerWidget::setFPS(double fps){
	playTimer->setInterval(1000.0 / fps);
}

void PlayerWidget::closeEvent(QCloseEvent * event)
{
	event->accept();
	stop();
	deleteLater();
}

void PlayerWidget::resizeEvent(QResizeEvent * event)
{
	QWidget::resizeEvent(event);
	updateFrame();
}
