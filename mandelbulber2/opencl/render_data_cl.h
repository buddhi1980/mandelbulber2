/*
 * render_data_cl.h
 *
 *  Created on: 8 kwi 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_OPENCL_RENDER_DATA_CL_H_
#define MANDELBULBER2_OPENCL_RENDER_DATA_CL_H_

typedef struct
{
	float3 lightVector;
	float3 viewVectorNotRotated;
	__global sMaterialCl *material;
	__global float4 *palette;
	__global sVectorsAroundCl *AOVectors;
	__global sLightCl *lights;
	__global sPrimitiveCl *primitives;
	int reflectionsMax;
	int paletteSize;
	int numberOfLights;
	int AOVectorsCount;
	int numberOfPrimitives;
} sRenderData;

#endif /* MANDELBULBER2_OPENCL_RENDER_DATA_CL_H_ */
