/*
 * light_cl.h
 *
 *  Created on: 30 sty 2021
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_OPENCL_LIGHT_CL_H_
#define MANDELBULBER2_OPENCL_LIGHT_CL_H_

#ifndef OPENCL_KERNEL_CODE
#include "opencl_algebra.h"
#endif /* OPENCL_KERNEL_CODE */

typedef enum
{
	lightDirectional = 0,
	lightPoint = 1,
	lightConical = 2,
	lightProjection = 3
} enumLightTypeCl;

typedef enum
{
	lightDecay1R = 0,
	lightDecay1R2 = 1,
	lightDecay1R3 = 2,
} enumLightDecayFunctionCl;

typedef struct
{
	// booleans
	cl_int enabled;
	cl_int castShadows;
	cl_int penetrating;
	cl_int relativePosition;
	cl_int volumetric;

	// enums
	enumLightTypeCl type;
	enumLightDecayFunctionCl decayFunction;

	cl_float coneAngle;
	cl_float coneSoftAngle;
	cl_float intensity;
	cl_float visibility;
	cl_float volumetricVisibility;
	cl_float size;
	cl_float softShadowCone;
	cl_float contourSharpness;

	cl_float coneRatio;
	cl_float coneSoftRatio;

	cl_float3 color;

	cl_float3 position;
	cl_float3 rotation;
	cl_float3 lightDirection;

	matrix33 rotMatrix;

} sLightCl;

#endif /* MANDELBULBER2_OPENCL_LIGHT_CL_H_ */
