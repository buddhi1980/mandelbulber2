/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cRenderSSAO class - renders Screen Space Ambient Occlusion effect
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

#ifndef RENDER_SSAO_H_
#define RENDER_SSAO_H_

#include "fractparams.hpp"
#include "render_data.hpp"
#include <QProgressBar>
#include <QStatusBar>

class cRenderSSAO : public QObject
{
	Q_OBJECT
public:
	cRenderSSAO(const cParamRender *_params, const sRenderData *_renderData, cImage *_image);
	~cRenderSSAO();

	void RenderSSAO(QList<int> *list = NULL);
	void setQuiet() {quiet = true;}
	void setProgressive(double step) {progressive = step;}

private:
	const cParamRender *params;
	const sRenderData *data;
	cImage *image;
	bool quiet;
	double qualityFactor;
	int progressive;

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
};

#endif /* RENDER_SSAO_H_ */
