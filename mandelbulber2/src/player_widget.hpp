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

#include <QWidget>
#include <QtWidgets>
#include <QTimer>
#include "../qt/mydoublespinbox.h"

class PlayerWidget : public QWidget
{
	Q_OBJECT

public:
	PlayerWidget(QWidget *parent = 0);
	~PlayerWidget();
	void SetFilePath(QString filePath);
	QSize sizeHint() const { return QSize(800, 600); }

public slots:
	void playPause();
	void stop();

private slots:
	void setPosition(int position);
	void nextFrame();
	void setFPS(double fps);

private:
	void updateFrame();
	QAbstractButton *playPauseButton;
	QAbstractButton *stopButton;
	QSlider *positionSlider;
	QLabel *infoLabel;
	QLabel *imageLabel;
	MyDoubleSpinBox *fpsSpinBox;
	QTimer *playTimer;
	QStringList imageFiles;
	int currentIndex;
	QString dirPath;

protected:
	void closeEvent(QCloseEvent * event);
	void resizeEvent(QResizeEvent * event);
};

#endif
