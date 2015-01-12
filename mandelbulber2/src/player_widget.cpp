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
	// TODO hardware accceleration - 30 fps with HD images should play smooth on reasonable system

	videoItem = new QGraphicsVideoItem;
	videoItem->setSize(QSizeF(640, 480));

	QGraphicsScene *scene = new QGraphicsScene(this);
	QGraphicsView *graphicsView = new QGraphicsView(scene);

	scene->addItem(videoItem);

	infoLabel = new QLabel;

	playPauseButton = new QPushButton;
	playPauseButton->setEnabled(true);
	playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));

	positionSlider = new QSlider(Qt::Horizontal);

	QBoxLayout *controlLayout = new QHBoxLayout;
	controlLayout->setMargin(0);
	controlLayout->addWidget(playPauseButton);
	controlLayout->addWidget(positionSlider);
	controlLayout->addWidget(infoLabel);

	QBoxLayout *layout = new QVBoxLayout;
	layout->addWidget(graphicsView);
	layout->addLayout(controlLayout);

	setLayout(layout);

	mediaPlayer.setVideoOutput(videoItem);

	// Load image sequence
	QDir imageDir = QDir(gPar->Get<QString>("anim_flight_dir"));
	QStringList imageFiles = imageDir.entryList(QDir::NoDotAndDotDot | QDir::Files);

	mediaPlaylist = new QMediaPlaylist(this);
	for(int i = 0; i < imageFiles.size(); i++)
	{
		QString fileName = gPar->Get<QString>("anim_flight_dir") + "/" + imageFiles.at(i);
		mediaPlaylist->addMedia(QUrl::fromLocalFile(fileName));
	}

	positionSlider->setRange(0, mediaPlaylist->mediaCount() - 1);
	infoLabel->setText(QObject::tr("Frame %1 of %2").arg(0).arg(mediaPlaylist->mediaCount() - 1));

	mediaPlayer.setPlaylist(mediaPlaylist);

	connect(&mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(mediaStateChanged(QMediaPlayer::State)));
	connect(&mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(positionChanged(qint64)));
	connect(&mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(durationChanged(qint64)));
	connect(positionSlider, SIGNAL(sliderMoved(int)), this, SLOT(setPosition(int)));
	connect(playPauseButton, SIGNAL(clicked()), this, SLOT(playPause()));
}

PlayerWidget::~PlayerWidget()
{
}

void PlayerWidget::playPause()
{
	switch(mediaPlayer.state()) {
	case QMediaPlayer::PlayingState:
		playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
		mediaPlayer.pause();
		break;
	default:
		playPauseButton->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
		mediaPlayer.play();
		break;
	}
}

void PlayerWidget::mediaStateChanged(QMediaPlayer::State state)
{

}

void PlayerWidget::positionChanged(qint64 position)
{
	positionSlider->setSliderPosition(mediaPlaylist->currentIndex());
	infoLabel->setText(QObject::tr("Frame %1 of %2").arg(mediaPlaylist->currentIndex()).arg(mediaPlaylist->mediaCount() - 1));
	// TODO set duration on playlist item
	Wait(10);
}

void PlayerWidget::durationChanged(qint64 duration)
{
	qDebug() << "durationChanged: " << duration;
}

void PlayerWidget::setPosition(int position)
{
	qDebug() << "setPosition: " << position;
	if(position != mediaPlaylist->currentIndex()){
		mediaPlaylist->setCurrentIndex(position);
		infoLabel->setText(QObject::tr("Frame %1 of %2").arg(position).arg(mediaPlaylist->mediaCount() - 1));
	}
}

