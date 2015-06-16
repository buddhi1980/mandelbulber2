/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * Class to store settings for animation keyframes
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen (jenzebas@gmail.com)
 */


#ifndef MANDELBULBER2_SRC_KEYFRAMES_HPP_
#define MANDELBULBER2_SRC_KEYFRAMES_HPP_

#include "animation_frames.hpp"
#include "morph.hpp"

class cKeyframes: public cAnimationFrames
{
	//definitions:
	//frame means animation keyframe
	//interpolatedFrame means animation frame

public:
	cKeyframes();
	~cKeyframes();
	cKeyframes(const cKeyframes &source);
	cKeyframes& operator=(const cKeyframes &source);
	sAnimationFrame GetInterpolatedFrame(int index);
	void GetInterpolatedFrameAndConsolidate(int index, cParameterContainer *params, cFractalContainer *fractal);
	void SetFramesPerKeyframe(int frPerKey) {framesPerKeyframe = frPerKey;}
	int GetFramesPerKeyframe() { return framesPerKeyframe;}
	int GetUnrenderedTotal();
	int GetUnrenderedTillIndex(int frameIndex);
	void ChangeMorphType(int parameterIndex, parameterContainer::enumMorphType morphType);
	void ClearMorphCache() {morph.clear(); }
	void AddAnimatedParameter(const QString &parameterName, const cOneParameter &defaultValue);
	bool AddAnimatedParameter(const QString &fullParameterName, const cParameterContainer *param, const cFractalContainer *fractal);
	void RemoveAnimatedParameter(const QString &fullParameterName);

private:
	int framesPerKeyframe;
	QList<cMorph*> morph;
};

#endif /* MANDELBULBER2_SRC_KEYFRAMES_HPP_ */
