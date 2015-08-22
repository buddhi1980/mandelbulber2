/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cThumbnail class - thumbnail rendering
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

#ifndef THUMBNAIL_HPP_
#define THUMBNAIL_HPP_

#include <QtCore>
#include "render_job.hpp"

class cThumbnail
{
public:
	cThumbnail(const cParameterContainer *_params, const cFractalContainer *_fractal, int _width, int _height, const QString &_hash);
	~cThumbnail();
	void AssignWidget(QWidget *widget) {qwidget = widget;}
	QPixmap Render();
	void Save(QString filename);

private:
	cImage *image;
	const cParameterContainer *params;
	const cFractalContainer *fractal;
	QWidget *qwidget;
	const int width;
	const int height;
	QString hash;
};

#endif /* THUMBNAIL_HPP_ */
