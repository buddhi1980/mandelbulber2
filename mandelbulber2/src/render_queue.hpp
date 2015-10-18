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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include <QtCore>
#include "cimage.hpp"
#include "progress_text.hpp"

#ifndef RENDER_QUEUE_HPP_
#define RENDER_QUEUE_HPP_

class cRenderQueue : public QObject
{
	Q_OBJECT
public:
	cRenderQueue(cImage *_image);
	~cRenderQueue();

public slots:
	void slotRenderQueue();
	void slotUpdateProgressAndStatus(const QString &text, const QString &progressText, double progress, cProgressText::enumProgressType progressType = cProgressText::progress_QUEUE);

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress, cProgressText::enumProgressType progressType);
	void finished();

private:
	cImage *image;
};

#endif /* RENDER_QUEUE_HPP_ */
