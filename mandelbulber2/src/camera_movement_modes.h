/*
 * camera_movement_modes.h
 *
 *  Created on: 13 mar 2017
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_CAMERA_MOVEMENT_MODES_H_
#define MANDELBULBER2_SRC_CAMERA_MOVEMENT_MODES_H_

namespace cameraMovementEnums
{
enum enumCameraMovementStepMode
{
	relative,
	absolute
};
enum enumCameraMovementMode
{
	fixedDistance,
	moveCamera,
	moveTarget
};
enum enumCameraRotationMode
{
	rotateCamera,
	rotateAroundTarget
};
}

#endif /* MANDELBULBER2_SRC_CAMERA_MOVEMENT_MODES_H_ */
