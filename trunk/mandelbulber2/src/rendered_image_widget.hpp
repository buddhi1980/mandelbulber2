/*
 * rendered_image_widget.hpp
 *
 *  Created on: Aug 15, 2014
 *      Author: krzysztof
 */

#ifndef RENDERED_IMAGE_WIDGET_HPP_
#define RENDERED_IMAGE_WIDGET_HPP_

#include <QWidget>
#include "cimage.hpp"
#include "parameters.hpp"

using namespace Qt;

class RenderedImage: public QWidget
{
Q_OBJECT

public:

enum enumClickMode
{
	clickMoveCamera = 0,
	clickFogVisibility = 1
};

	RenderedImage(QWidget *parent = 0);
	void AssignImage(cImage *_image) {image = _image;}
	void AssignParameters(cParameterContainer *_mainParams) {params = _mainParams;}

protected:
	void paintEvent(QPaintEvent *event);
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);

private:
	cImage *image;
	enumClickMode clickMode;
	cParameterContainer *params;
	bool cursorVisible;
	double smoothLastZMouse;
	bool redrawed;

signals:
	void mouseMoved(int x, int y);
	void singleClick(int x, int y, MouseButton button);
};


#endif /* RENDERED_IMAGE_WIDGET_HPP_ */
