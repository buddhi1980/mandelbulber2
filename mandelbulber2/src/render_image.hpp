/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cRenderer class - calculates image using multiple CPU cores and does post processing
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


#ifndef RENDER_IMAGE_HPP_
#define RENDER_IMAGE_HPP_

#include "cimage.hpp"
#include "fractparams.hpp"
#include "four_fractals.hpp"
#include "render_data.hpp"
#include "scheduler.hpp"

class cRenderer : public QObject
{
	Q_OBJECT
public:
	cRenderer(const cParamRender *_params, const cFourFractals *_fractal, sRenderData *_renderData, cImage *_image, QObject *_parentObject);
	~cRenderer();
	bool RenderImage();

private:
	void CreateLineData(int y, QByteArray *lineData);

	const cParamRender *params;
	const cFourFractals *fractal;
	sRenderData *data;
	cImage *image;
	QObject *parentObject;
	cScheduler *scheduler;
	bool netRemderAckReceived;

public slots:
	void NewLinesArrived(QList<int> lineNumbers, QList<QByteArray> lines);
	void ToDoListArrived(QList<int> done);
	void AckReceived();

	signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void updateStatistics(cStatistics);
	void sendRenderedLines(QList<int> lineNumbers, QList<QByteArray> lines);
	void SendToDoList(int clientIndex, QList<int> done);
	void StopAllClients();
	void NotifyClientStatus();
};

#endif /* RENDER_IMAGE_HPP_ */
