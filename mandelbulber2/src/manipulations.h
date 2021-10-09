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

class cParameterContainer;
class cFractalContainer;

class cManipulations : public QObject
{
	Q_OBJECT
public:
	explicit cManipulations(QObject *parent = nullptr);
	~cManipulations();

	void AssignParameterContainers(std::shared_ptr<cParameterContainer> _params,
		std::shared_ptr<cFractalContainer> _fractalParams);

	void MoveCamera(QString buttonName);
	void RotateCamera(QString buttonName);
	void CameraOrTargetEdited(QWidget *navigationWidget);
	void RotationEdited(QWidget *navigationWidget);
	void CameraDistanceEdited(QWidget *navigationWidget);

private:
	std::shared_ptr<cParameterContainer> params;
	std::shared_ptr<cFractalContainer> fractalParams;
};

#endif /* MANDELBULBER2_SRC_MANIPULATIONS_H_ */
