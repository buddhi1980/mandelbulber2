/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * RenderedImage class - extension for QWidget class. Widget prepared for displaying rendered image
 * and 3D cursor
 */

#ifndef MANDELBULBER2_SRC_RENDERED_IMAGE_WIDGET_HPP_
#define MANDELBULBER2_SRC_RENDERED_IMAGE_WIDGET_HPP_

#include <QWidget>
#include "QVariant"

#include "algebra.hpp"
#include "stereo.h"

// forward declarations
class cImage;
class cParameterContainer;

class RenderedImage : public QWidget
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
		clickFlightSpeedControl = 7,
		clickPlaceRandomLightCenter = 8,
		clickGetPoint = 9
	};

	struct sFlightData
	{
		// numbers
		int frame;
		CVector3 camera;
		double speed;
		double distance;
		double speedSp;

		// HUD
		CVector3 rotation;
		CVector3 speedVector;
		CVector3 forwardVector;
		CVector3 topVector;
	};

	RenderedImage(QWidget *parent = nullptr);
	void AssignImage(cImage *_image) { image = _image; }
	void AssignParameters(cParameterContainer *_mainParams) { params = _mainParams; }
	void setNewZ(double z) { smoothLastZMouse = z; }
	void setClickMode(QList<QVariant> _clickMode);
	void SetFrontDist(double dist) { frontDist = dist; }
	void SetCursorVisibility(bool enable) { cursorVisible = enable; }
	void SetFlightData(const sFlightData &fData) { flightData = fData; }
	// CVector2<double> GetLastMousePositionScaled();

public slots:
	void slotSetMinimumSize(int width, int height);

signals:
	void SpeedChanged(double amount);
	void SpeedSet(double amount);
	void StrafeChanged(CVector2<double> arrows);
	void YawAndPitchChanged(CVector2<double> yawAndPitch);
	void RotationChanged(double direction);
	void ShiftModeChanged(bool shiftPressed);
	void Pause();

protected:
	void paintEvent(QPaintEvent *event) override;
	void mouseMoveEvent(QMouseEvent *event) override;
	void mousePressEvent(QMouseEvent *event) override;
	void mouseReleaseEvent(QMouseEvent *event) override;
	void keyPressEvent(QKeyEvent *event) override;
	void keyReleaseEvent(QKeyEvent *event) override;
	void enterEvent(QEvent *event) override;
	void leaveEvent(QEvent *event) override;
	void wheelEvent(QWheelEvent *event) override;

private:
	void DisplayCoordinates();
	void Display3DCursor(CVector2<int> screenPoint, double z);
	void DisplayCrosshair() const;
	void DrawHud(CVector3 rotation) const;
	void Draw3DBox(
		double scale, double fov, CVector2<double> point, double z, cStereo::enumEye eye) const;
	static CVector3 CalcPointPersp(const CVector3 &point, const CRotationMatrix &rot, double persp);

	cImage *image;
	QList<QVariant> clickModeData;
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
	sFlightData flightData;
	CVector2<double> keyArrows;
	double flightRotationDirection;
	QTimer *timerRefreshImage;
	bool anaglyphMode;

signals:
	void mouseMoved(int x, int y);
	void singleClick(int x, int y, Qt::MouseButton button);
	void keyPress(QKeyEvent *event);
	void keyRelease(QKeyEvent *event);
	void mouseWheelRotated(int delta);
};

#endif /* MANDELBULBER2_SRC_RENDERED_IMAGE_WIDGET_HPP_ */
