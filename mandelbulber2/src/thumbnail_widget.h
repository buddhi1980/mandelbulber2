/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * widget to display auto-rendering thumbnails
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#ifndef QT_THUMBNAIL_WIDGET_H_
#define QT_THUMBNAIL_WIDGET_H_

#include <QWidget>
#include "thumbnail.hpp"
#include <QThread>

class cThumbnailWidget: public QWidget
{
	Q_OBJECT
public:
	cThumbnailWidget(int _width, int _height, QObject *_parentWithProgressBar, QWidget *parent);
	~cThumbnailWidget();
	void AssignParameters(const cParameterContainer &_params, const cFractalContainer &_fractal);
	void UseOneCPUCore(bool onlyOne) {useOneCPUCore = onlyOne;}

	static int instanceCount;

private:
	void paintEvent(QPaintEvent *event);

private slots:
	void slotRender();
	void slotFullyRendered();
	void slotRandomRender();

private:
	cImage *image;
	cParameterContainer *params;
	cFractalContainer *fractal;
	int tWidth;
	int tHeight;
	QString hash;
	QProgressBar *progressBar;
	bool stopRequest;
	bool isRendered;
	bool hasParameters;
	QObject *parentWithProgressBar;
	bool useOneCPUCore;
	QTimer *timer; //timer for random trigger for rendering (renders thumbnail even when is not visible)

signals:
	void renderRequest();
	void thumbnailRendered();
};

#endif /* QT_THUMBNAIL_WIDGET_H_ */
