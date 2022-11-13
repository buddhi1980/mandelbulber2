/*
 * manipulations.h
 *
 *  Created on: 9 paź 2021
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_MANIPULATIONS_H_
#define MANDELBULBER2_SRC_MANIPULATIONS_H_

#include <QObject>
#include <QElapsedTimer>
#include <memory>
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
	void MouseDragCameraLeftButton(const sMouseDragTempData &dragTempData);
	void MouseDragCaneraRightButton(int dx, int dy, const sMouseDragTempData &dragTempData);
	void MouseDragCameraMiddleButton(int dx);
	void MouseDragLeftRightButtons(const sMouseDragTempData &dragTempData);
	void LightDragLeftButton(const sMouseDragTempData &dragTempData, int dx, int dy);
	void PrimitiveDragLeftButton(const sMouseDragTempData &dragTempData, int dx, int dy);
	void MoveLightByWheel(double deltaWheel);

	bool isDraggingStarted() { return mouseDragData.draggingStarted; }

	void IncreaseNumberOfStartedRenders() { numberOfStartedRenders++; }
	void DecreaseNumberOfStartedRenders() { numberOfStartedRenders--; }
	int GetNumberOfStartedRenders() { return numberOfStartedRenders; }

public slots:
	void slotSmallPartRendered(double time);

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
