/*
 * manipulations.h
 *
 *  Created on: 9 paź 2021
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_MANIPULATIONS_H_
#define MANDELBULBER2_SRC_MANIPULATIONS_H_

#include <QObject>
#include <memory>
#include "algebra.hpp"

class cParameterContainer;
class cFractalContainer;
class cImage;
class RenderedImageWidget;
class RenderedImage;

class cManipulations : public QObject
{
	Q_OBJECT
public:
	explicit cManipulations(QObject *parent = nullptr);
	~cManipulations();

	void AssignParameterContainers(std::shared_ptr<cParameterContainer> _params,
		std::shared_ptr<cFractalContainer> _fractalParams);
	void AssingImage(std::shared_ptr<cImage> _image);
	void AssignRenderedImageWidget(RenderedImage *widget);

	void MoveCamera(QString buttonName);
	void RotateCamera(QString buttonName);
	void CameraOrTargetEdited(QWidget *navigationWidget);
	void RotationEdited(QWidget *navigationWidget);
	void CameraDistanceEdited(QWidget *navigationWidget);

	void SetByMouse(QWidget *navigationWidget, CVector2<double> screenPoint, Qt::MouseButton button,
		const QList<QVariant> &mode);

private:
	std::shared_ptr<cParameterContainer> par;
	std::shared_ptr<cFractalContainer> parFractal;
	std::shared_ptr<cImage> image;
	RenderedImage *renderedImageWidget;

signals:
	void signalRender();
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
