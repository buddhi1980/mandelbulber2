/*
 * projection_3d.hpp
 *
 *  Created on: 2 maj 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_PROJECTION_3D_HPP_
#define MANDELBULBER2_SRC_PROJECTION_3D_HPP_

#include "algebra.hpp"

namespace params
{
enum enumPerspectiveType
{
	perspThreePoint = 0, perspFishEye = 1, perspEquirectangular = 2, perspFishEyeCut = 3
};
}

CVector3 InvProjection3D(CVector3 point, CVector3 vp, CRotationMatrix mRotInv,
		params::enumPerspectiveType perspectiveType, double fov, double zoom, double imgWidth,
		double imgHeight);
CVector3 CalculateViewVector(CVector2<double> normalizedPoint, double fov,
		params::enumPerspectiveType perspType, const CRotationMatrix &mRot);


#endif /* MANDELBULBER2_SRC_PROJECTION_3D_HPP_ */
