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
#include "rendered_image_widget.hpp"
#include "four_fractals.hpp"
#include "animation_keyframes.hpp"
#include "animation_flight.hpp"

#ifndef MANDELBULBER2_SRC_RENDER_QUEUE_HPP_
#define MANDELBULBER2_SRC_RENDER_QUEUE_HPP_

class cRenderQueue : public QObject
{
	Q_OBJECT
public:
	cRenderQueue(cImage *_image, RenderedImage *widget = NULL);
	~cRenderQueue();
	bool RenderStill(const QString& filename);
	bool RenderFlight();
	bool RenderKeyframe();

public slots:
	void slotRenderQueue();
signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress,
			cProgressText::enumProgressType progressType = cProgressText::progress_IMAGE);
	void updateStatistics(cStatistics stats);
	void updateProgressHide(cProgressText::enumProgressType progressType =
			cProgressText::progress_ANIMATION);
	void finished();

private:
	cImage *image;
	RenderedImage *imageWidget;
	cParameterContainer *queuePar;
	cFractalContainer *queueParFractal;
	cAnimationFrames *queueAnimFrames;
	cFlightAnimation *queueFlightAnimation;
	cKeyframeAnimation *queueKeyframeAnimation;
	cKeyframes *queueKeyframes;
};

#endif /* MANDELBULBER2_SRC_RENDER_QUEUE_HPP_ */
