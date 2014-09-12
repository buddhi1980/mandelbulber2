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
#include "algebra.hpp"

using namespace Qt;

class RenderedImage: public QWidget
{
Q_OBJECT

public:

enum enumClickMode
{
	clickMoveCamera = 0,
	clickFogVisibility = 1,
	clickDOFFocus = 2,
	clickPlaceLight1 = 3,
	clickPlaceLight2 = 4,
	clickPlaceLight3 = 5,
	clickPlaceLight4 = 6
};

	RenderedImage(QWidget *parent = 0);
	void AssignImage(cImage *_image) {image = _image;}
	void AssignParameters(cParameterContainer *_mainParams) {params = _mainParams;}
	void setNewZ(double z) {smoothLastZMouse = z;}
	void setClickMode(enumClickMode _clickMode) {clickMode = _clickMode;}
	void SetFrontDist(double dist) {frontDist = dist;}

protected:
	void paintEvent(QPaintEvent *event);
	void mouseMoveEvent(QMouseEvent * event);
	void mousePressEvent(QMouseEvent * event);
	void mouseReleaseEvent(QMouseEvent * event);
	void keyPressEvent(QKeyEvent * event);
	void keyReleaseEvent(QKeyEvent * event);
	void enterEvent(QEvent * event);
	void leaveEvent(QEvent * event);
	void wheelEvent(QWheelEvent * event);

private:
	void DisplayCoordinates();
	void Display3DCursor(CVector2<int> screenPoint, double z);

	cImage *image;
	enumClickMode clickMode;
	cParameterContainer *params;
	bool cursorVisible;
	double smoothLastZMouse;
	bool redrawed;
	CVector2<int> lastMousePosition;
	CVector3 lastCoordinates;
	double lastDepth;
	bool isFocus;
	bool isOnObject;
	double frontDist;

signals:
	void mouseMoved(int x, int y);
	void singleClick(int x, int y, Qt::MouseButton button);
	void keyPress(Qt::Key key);
	void keyRelease(Qt::Key key);
	void mouseWheelRotated(int delta);
};


#endif /* RENDERED_IMAGE_WIDGET_HPP_ */
