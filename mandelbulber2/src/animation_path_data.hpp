/*
 * animation_path_data.hpp
 *
 *  Created on: 2 sty 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_ANIMATION_PATH_DATA_HPP_
#define MANDELBULBER2_SRC_ANIMATION_PATH_DATA_HPP_
#include <QList>
#include "algebra.hpp"

struct sAnimationPathPoint
{
	CVector3 camera;
	CVector3 target;
	CVector3 lights[4];
};

struct sAnimationPathData
{
	QList<sAnimationPathPoint> animationPath;
	int numberOfKeyframes;
	int framesPeyKey;
	int actualSelectedFrameNo;
	bool cameraPathEnable;
	bool targetPathEnable;
	bool lightPathEnable[4];
};

#endif /* MANDELBULBER2_SRC_ANIMATION_PATH_DATA_HPP_ */
