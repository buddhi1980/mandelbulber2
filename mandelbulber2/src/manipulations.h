/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2024 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * TODO: description
 */

#ifndef MANDELBULBER2_SRC_MANIPULATIONS_H_
#define MANDELBULBER2_SRC_MANIPULATIONS_H_

#include <memory>

#include <QElapsedTimer>
#include <QObject>

#include "algebra.hpp"
#include "camera_target.hpp"
#include "primitive_item.h"
#include "projection_3d.hpp"

class cParameterContainer;
class cFractalContainer;
class cImage;
class RenderedImageWidget;
class RenderedImage;
class cDockNavigation;
class cDockEffects;
class cDockFractal;

class cManipulations : public QObject
{
	Q_OBJECT
public:
	explicit cManipulations(QObject *parent = nullptr);
	~cManipulations();

private:
	enum class enumDragMode
	{
		rotate,
		rotateAroundPoint,
		roll,
		move,
	};

	enum class enumDragOption
	{
		multi,
		rotate,
		rotateAroundPoint,
		roll,
		move,
	};

	struct sMouseDragInitData
	{
		sMouseDragInitData() {}
		bool draggingStarted{false};
		bool cameraDrag{false};
		bool lightDrag{false};
		bool primitiveDrag{false};
		CVector2<double> startScreenPoint;
		CVector2<double> startNormalizedPoint;
		double startZ{0.0};
		CVector3 startCamera;
		CVector3 startTarget;
		CVector3 startTopVector;
		CVector3 startIndicatedPoint;
		Qt::MouseButtons button{Qt::NoButton};
		QElapsedTimer lastRefreshTime;
		qint64 lastStartRenderingTime{0};
		int lightIndex = -1;
		sPrimitiveItem primitiveItem;
		CVector3 objectStartPosition;
	} mouseDragData;

	struct sMouseDragTempData
	{
		params::enumPerspectiveType perspType;
		double sweetSpotHAngle;
		double sweetSpotVAngle;
		bool legacyCoordinateSystem;
		double reverse;
		double fov;
		cCameraTarget::enumRotationMode rollMode;
		CVector2<double> newScreenPoint;
		CVector2<double> imagePoint;
		int width;
		int height;
		double aspectRatio;
	};

public:
	void AssignParameterContainers(std::shared_ptr<cParameterContainer> _params,
		std::shared_ptr<cFractalContainer> _fractalParams);
	void AssingImage(std::shared_ptr<cImage> _image);
	void AssignWidgets(RenderedImage *_imageWidget, cDockNavigation *_navigationWidget,
		cDockEffects *_effectsWidget, cDockFractal *_fractalWidget);

	void MoveCamera(QString buttonName);
	void RotateCamera(QString buttonName);
	void CameraOrTargetEdited();
	void RotationEdited();
	void CameraDistanceEdited();
	void MovementStepModeChanged(int mode);

	void SetByMouse(
		CVector2<double> screenPoint, Qt::MouseButton button, const QList<QVariant> &mode);
	void MouseDragStart(CVector2<double> screenPoint, Qt::MouseButtons, const QList<QVariant> &mode);
	void MouseDragFinish();
	void MouseDragDelta(int dx, int dy);
	void MouseDragCameraRorate(const sMouseDragTempData &dragTempData);
	void MouseDragCaneraRotateAroundPoint(
		double dx, double dy, const sMouseDragTempData &dragTempData);
	void MouseDragCameraRoll(double dx);
	void MouseDragCameraMove(const sMouseDragTempData &dragTempData);
	void LightDragLeftButton(const sMouseDragTempData &dragTempData, double dx, double dy);
	void PrimitiveDragLeftButton(const sMouseDragTempData &dragTempData, double dx, double dy);
	void MoveLightByWheel(double deltaWheel);
	void MovePrimitiveByWheel(double deltaWheel);

	bool isDraggingStarted() { return mouseDragData.draggingStarted; }

	void IncreaseNumberOfStartedRenders() { numberOfStartedRenders++; }
	void DecreaseNumberOfStartedRenders() { numberOfStartedRenders--; }
	int GetNumberOfStartedRenders() { return numberOfStartedRenders; }
	void SetDragOption(enumDragOption option);

public slots:
	void slotSmallPartRendered(double time);
	void slotToggledOtpionMulti(bool checked);
	void slotToggledOtpionRotate(bool checked);
	void slotToggledOtpionRotateAround(bool checked);
	void slotToggledOtpionRoll(bool checked);
	void slotToggledOtpionMove(bool checked);
	void slotToggledOPtionPreciseRotation(bool checked);

private:
	std::shared_ptr<cParameterContainer> par;
	std::shared_ptr<cFractalContainer> parFractal;
	std::shared_ptr<cImage> image;
	RenderedImage *renderedImageWidget = nullptr;
	cDockNavigation *navigationWidget = nullptr;
	cDockEffects *effectsWidget = nullptr;
	cDockFractal *fractalWidget = nullptr;

	int numberOfStartedRenders = 0;
	bool smallPartRendered = true;
	enumDragOption dragOption = enumDragOption::multi;
	bool preciseRotation = false;

signals:
	void signalRender();
	void signalStop();
	void signalWriteInterfaceBasicFog(std::shared_ptr<cParameterContainer> par);
	void signalWriteInterfaceDOF(std::shared_ptr<cParameterContainer> par);
	void signalWriteInterfaceLights(std::shared_ptr<cParameterContainer> par);
	void signalWriteInterfaceJulia(std::shared_ptr<cParameterContainer> par);
	void signalWriteInterfacePrimitives(std::shared_ptr<cParameterContainer> par);
	void signalWriteInterfaceRandomLights(std::shared_ptr<cParameterContainer> par);
	void signalWriteInterfaceMeasuremets(std::shared_ptr<cParameterContainer> par);
	void signalDisablePeriodicRefresh();
	void signalReEnablePeriodicRefresh();
	void signalEnableJuliaMode();
	void signalShowMeasuremetsDock();
	void signalRefreshPostEffects();
};

#endif /* MANDELBULBER2_SRC_MANIPULATIONS_H_ */
