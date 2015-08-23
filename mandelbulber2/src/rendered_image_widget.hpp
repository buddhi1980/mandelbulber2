/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * RenderedImage class - extension for QWidget class. Widget prepared for displaying rendered image and 3D cursor
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
	clickDoNothing = 0,
	clickMoveCamera = 1,
	clickFogVisibility = 2,
	clickDOFFocus = 3,
	clickGetJuliaConstant = 4,
	clickPlaceLight = 5,
	clickPlacePrimitive = 6,
	clickFlightSpeedControl = 7
};

struct sFlightData
{
	//numbers
	int frame;
	CVector3 camera;
	double speed;
	double distance;
	double speedSp;

	//HUD
	CVector3 rotation;
	CVector3 speedVector;
	CVector3 forwardVector;
	CVector3 topVector;
};

	RenderedImage(QWidget *parent = 0);
	void AssignImage(cImage *_image) {image = _image;}
	void AssignParameters(cParameterContainer *_mainParams) {params = _mainParams;}
	void setNewZ(double z) {smoothLastZMouse = z;}
	void setClickMode(QList<QVariant> _clickMode) {clickModeData = _clickMode;}
	void SetFrontDist(double dist) {frontDist = dist;}
	void SetCursorVisibility(bool enable) {cursorVisible = enable;};
	void SetFlightData(const sFlightData &fData) {flightData = fData;}
	CVector2<double> GetLastMousePositionScaled(void);

signals:
	void flightSpeedIncrease();
	void flightSpeedDecrease();
	void flightStrafe(CVector2<double> arrows);
	void flightYawAndPitch(CVector2<double> yawAndPitch);
	void flightRotation(double direction);
	void flightPause();

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
	void DisplayCrosshair();
	void DrawHud(CVector3 rotation);
	CVector3 CalcPointPersp(const CVector3 &point, const CRotationMatrix &rot, double persp);

	cImage *image;
	QList<QVariant> clickModeData;
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
	sFlightData flightData;
	CVector2<double> keyArrows;
	double flightRotationDirection;
	QTimer *timerRefreshImage;

signals:
	void mouseMoved(int x, int y);
	void singleClick(int x, int y, Qt::MouseButton button);
	void keyPress(Qt::Key key);
	void keyRelease(Qt::Key key);
	void mouseWheelRotated(int delta);
};


#endif /* RENDERED_IMAGE_WIDGET_HPP_ */
