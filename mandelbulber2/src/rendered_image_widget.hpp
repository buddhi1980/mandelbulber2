/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
#include "animation_path_data.hpp"
#include "rendered_tile_data.hpp"
#include "stereo.h"

// forward declarations
class cImage;
class cParameterContainer;
class cFractalContainer;
struct sAnimationPathData;

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
		clickGetPoint = 9,
		clickWrapLimitsAroundObject = 10,
	};

	enum enumGridType
	{
		gridTypeCrosshair = 0,
		gridTypeThirds = 1,
		gridTypeGolden = 2
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
	void AssignParameters(cParameterContainer *_mainParams, cFractalContainer *_fractals)
	{
		params = _mainParams;
		fractals = _fractals;
	}
	void setNewZ(double z) { smoothLastZMouse = z; }
	void setClickMode(QList<QVariant> _clickMode);
	void SetEnableClickModes(bool enable) { clickModesEnables = enable; }
	bool GetEnableClickModes() const { return clickModesEnables; }
	void SetFrontDist(double dist) { frontDist = dist; }
	void SetCursorVisibility(bool enable) { cursorVisible = enable; }
	void SetGridType(enumGridType gridType);
	void SetFlightData(const sFlightData &fData) { flightData = fData; }
	void SetPlaceBehindObjects(bool behind) { placeLightBehind = behind; }
	void SetCameraMovementMode(int index) { cameraMovementMode = index; }
	void SetAnimationPath(const sAnimationPathData &_animationPath);
	// CVector2<double> GetLastMousePositionScaled();

public slots:
	void slotSetMinimumSize(int width, int height);
	void showRenderedTilesList(QList<sRenderedTileData> listOfRenderedTiles);

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
	void Compass(CVector3 rotation, QPointF center, double size);
	void Draw3DBox(
		float scale, float fov, CVector2<float> point, float z, cStereo::enumEye eye) const;
	static QPointF CalcPointPersp(const CVector3 &point, const CRotationMatrix &rot, double persp);
	void DrawAnimationPath();
	void PaintLastRenderedTilesInfo();

	bool anaglyphMode;
	bool cursorVisible;
	bool isFocus;
	bool isOnObject;
	bool placeLightBehind;
	bool redrawed;
	bool clickModesEnables;
	int draggingStarted;
	bool draggingInitStarted;
	cFractalContainer *fractals;
	cImage *image;
	cParameterContainer *params;
	CVector2<double> keyArrows;
	CVector2<int> lastMousePosition;
	CVector2<int> dragStartPosition;
	int buttonsPressed;
	CVector3 lastCoordinates;
	Qt::MouseButtons dragButtons;
	double flightRotationDirection;
	double frontDist;
	double lastDepth;
	double smoothLastZMouse;
	enumClickMode clickMode;
	enumGridType gridType;
	int cameraMovementMode;
	QList<QVariant> clickModeData;
	QTimer *timerRefreshImage;
	sFlightData flightData;
	sAnimationPathData animationPathData;
	QList<sRenderedTileData> listOfRenderedTilesData;

signals:
	void mouseMoved(int x, int y);
	void singleClick(int x, int y, Qt::MouseButton button);
	void keyPress(QKeyEvent *event);
	void keyRelease(QKeyEvent *event);
	void mouseWheelRotatedWithCtrl(int x, int y, int delta);
	void mouseDragStart(int x, int y, Qt::MouseButtons button);
	void mouseDragFinish();
	void mouseDragDelta(int dx, int dy);
};

#endif /* MANDELBULBER2_SRC_RENDERED_IMAGE_WIDGET_HPP_ */
