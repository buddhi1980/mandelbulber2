/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cRenderQueue class - processes queue render request
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
 * Authors: Sebastian Jennen, Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "render_queue.hpp"

#include <QtCore>

#include "progress_text.hpp"
#include "global_data.hpp"

cRenderQueue::cRenderQueue(cImage *_image) : QObject()
{
	image = _image;
}

cRenderQueue::~cRenderQueue()
{
}

void cRenderQueue::slotRenderQueue()
{
	int queueFinished = 0;
	int queueTotal = 1;

	WriteLog("cRenderQueue::slotRenderQueue()");

	emit updateProgressAndStatus(
		QObject::tr("Queue Render"),
		QObject::tr("Queue Item %1 of %2").arg(queueFinished).arg(queueTotal),
		1.0 * (queueFinished / queueTotal),
		cProgressText::progress_ANIMATION);
	emit finished();
}

void cRenderQueue::slotUpdateProgressAndStatus(const QString &text, const QString &progressText, double progress, cProgressText::enumProgressType progressType)
{
	emit updateProgressAndStatus(text, progressText, progress, progressType);
}
