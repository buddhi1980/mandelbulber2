/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * PlayerWidget - promoted QWidget to display video of image sequence from folder
 * The folder can be assigned with SetFilePath(). The widget will play the image
 * sequence inside the folder with simple player functionality.
 */

#include "player_widget.hpp"

#include "my_double_spin_box.h"

#include "src/initparameters.hpp"

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
	imageLabel->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
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
	QApplication::connect(
		&imageFolderWatcher, SIGNAL(directoryChanged(const QString &)), this, SLOT(updateFromFolder()));
}

PlayerWidget::~PlayerWidget()
{
	stop();
	delete playTimer;
}

void PlayerWidget::SetFilePath(QString filePath)
{
	dirPath = filePath;

	if (dirPath.right(1) == QString("/"))
	{
		dirPath.truncate(dirPath.length() - 1);
	}
	QStringList oldWatchedDirs = imageFolderWatcher.directories();
	if (oldWatchedDirs.size() > 0) imageFolderWatcher.removePaths(oldWatchedDirs);
	imageFolderWatcher.addPath(dirPath);
	updateFromFolder();
}

void PlayerWidget::updateFromFolder()
{
	QDir imageDir = QDir(dirPath);
	QStringList imageFileExtensions({"*.jpg", "*.jpeg", "*.png", "*.tiff"});
	imageDir.setNameFilters(imageFileExtensions);
	imageFiles = imageDir.entryList(QDir::NoDotAndDotDot | QDir::Files);

	if (imageFiles.size() == 0)
	{
		imageLabel->setText(QObject::tr("No frames to play"));
	}
	else
	{
		positionSlider->setRange(0, imageFiles.size() - 1);
		setPosition(currentIndex);
	}
}

void PlayerWidget::playPause() const
{
	if (imageFiles.size() > 0)
	{
		if (playTimer->isActive())
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
	if (!gPar) // FIXME it works, but it's ugly solution
	{
		close();
		return;
	}
	if (imageFiles.size() == 0) return;
	if (currentIndex < imageFiles.size())
	{
		QString fileName = dirPath + "/" + imageFiles.at(currentIndex);
		QPixmap pix(fileName);
		if (pix.isNull())
		{
			qWarning() << "Image could not be loaded, " << fileName;
			return;
		}
		infoLabel->setText(QObject::tr("Frame %1 of %2").arg(currentIndex + 1).arg(imageFiles.size()));
		if ((1.0 * imageLabel->width() / imageLabel->height()) > (1.0 * pix.width() / pix.height()))
		{
			// imageLabel is relative wider than pix
			imageLabel->setPixmap(
				pix.scaled((imageLabel->height() * pix.width() / pix.height()), imageLabel->height()));
		}
		else
		{
			imageLabel->setPixmap(
				pix.scaled(imageLabel->width(), (imageLabel->width() * pix.height() / pix.width())));
		}
	}
	else
	{
		qWarning() << "PlayerWidget::updateFrame() - File index out of range";
	}
}

void PlayerWidget::setFPS(double fps) const
{
	playTimer->setInterval(int(1000.0 / fps));
}

void PlayerWidget::closeEvent(QCloseEvent *event)
{
	event->accept();
	stop();
	close();
}

void PlayerWidget::resizeEvent(QResizeEvent *event)
{
	QWidget::resizeEvent(event);
	updateFrame();
}
