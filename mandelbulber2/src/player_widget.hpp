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

#ifndef PLAYERWIDGET_H
#define PLAYERWIDGET_H

#include <QMediaPlayer>
#include <QMovie>
#include <QWidget>
#include <QtWidgets>
#include <QVideoSurfaceFormat>
#include <QGraphicsVideoItem>
#include <QMediaPlaylist>

class QAbstractButton;
class QSlider;
class QGraphicsVideoItem;

class PlayerWidget : public QWidget
{
		Q_OBJECT

public:
		PlayerWidget(QWidget *parent = 0);
		~PlayerWidget();

		QSize sizeHint() const { return QSize(800, 600); }

public slots:
		void playPause();

private slots:
		void mediaStateChanged(QMediaPlayer::State state);
		void positionChanged(qint64 position);
		void durationChanged(qint64 duration);
		void setPosition(int position);

private:
		QMediaPlaylist *mediaPlaylist;
		QMediaPlayer mediaPlayer;
		QGraphicsVideoItem *videoItem;
		QAbstractButton *playPauseButton;
		QSlider *positionSlider;
		QLabel *infoLabel;
};

#endif
