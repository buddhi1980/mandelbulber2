/*
 * multiple_container.h
 *
 *  Created on: Nov 20, 2014
 *      Author: krzysztof
 */

#ifndef SRC_ANIMATION_FRAMES_HPP_
#define SRC_ANIMATION_FRAMES_HPP_

#include "parameters.hpp"
#include "fractal_container.hpp"

class cAnimationFrames
{
public:
	cAnimationFrames();
	void AddFrame(const cParameterContainer &params, const cFractalContainer &fractal);
	bool GetFrame(int index, cParameterContainer *params, cFractalContainer *fractal);
	int GetNumberOfFrames();
	void Clear();

	template<class T>
	void SubstituteValueForAll(QString parameterName, T value);

	QStringList GetListOfUsedParameters();

	QList<cParameterContainer> listPar;
	QList<cFractalContainer> listFract;
};

#endif /* SRC_ANIMATION_FRAMES_HPP_ */
