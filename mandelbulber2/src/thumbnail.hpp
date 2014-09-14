/*
 * thumbnail.hpp
 *
 *  Created on: Sep 14, 2014
 *      Author: krzysztof
 */

#ifndef THUMBNAIL_HPP_
#define THUMBNAIL_HPP_

#include <QtCore>
#include "render_job.hpp"

class cThumbnail
{
public:
	cThumbnail(const cParameterContainer *_params, const cParameterContainer *_fractal, int _width, int _height);
	~cThumbnail();
	void AssignWidget(QWidget *widget) {qwidget = widget;}
	void Render();
	void Save(QString filename);

private:
	cImage *image;
	const cParameterContainer *params;
	const cParameterContainer *fractal;
	QWidget *qwidget;
	const int width;
	const int height;
};

#endif /* THUMBNAIL_HPP_ */
