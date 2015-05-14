/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * Class to store settings for animation frames
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

#ifndef SRC_ANIMATION_FRAMES_HPP_
#define SRC_ANIMATION_FRAMES_HPP_

#include "parameters.hpp"
#include "fractal_container.hpp"
#include <QPixmap>

class cAnimationFrames
{
public:

	struct sAnimationFrame
	{
		cParameterContainer parameters;
		QPixmap thumnail;
		bool alreadyRendered;
	};

	struct sParameterDescription
	{
		sParameterDescription(QString _parameterName, QString _containerName, parameterContainer::enumVarType _varType)
			: parameterName(_parameterName), containerName(_containerName), varType(_varType) {};
		QString parameterName;
		QString containerName;
		parameterContainer::enumVarType varType;
	};

	cAnimationFrames();
	void AddFrame(const cParameterContainer &params, const cFractalContainer &fractal);
	void ModifyFrame(int index, sAnimationFrame &frame);
	void GetFrameAndConsolidate(int index, cParameterContainer *params, cFractalContainer *fractal);
	sAnimationFrame GetFrame(int index);
	int GetNumberOfFrames();
	int GetUnrenderedTillIndex(int index);
	int GetUnrenderedTotal();
	void Clear();
	void ClearAll();
	void AddAnimagedParameter(const QString &parameterName, const cOneParameter &defaultValue);
	bool AddAnimagedParameter(const QString &fullParameterName, const cParameterContainer *param, const cFractalContainer *fractal);
	void RemoveAnimagedParameter(const QString &fullParameterName);
	QList<sParameterDescription> GetListOfUsedParameters() {return listOfParameters;}
	const cParameterContainer* ContainerSelector(QString containerName, const cParameterContainer *params, const cFractalContainer *fractal) const;
	cParameterContainer* ContainerSelector(QString containerName, cParameterContainer *params, cFractalContainer *fractal) const;
	void DeleteFrames(int begin, int end);

protected:
	int IndexOnList(QString parameterName, QString containerName);

	QList<sAnimationFrame> frames;
	QList<sParameterDescription> listOfParameters;

};

#endif /* SRC_ANIMATION_FRAMES_HPP_ */
