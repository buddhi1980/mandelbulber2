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
