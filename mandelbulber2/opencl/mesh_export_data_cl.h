/*
 * mesh_export_data.h
 *
 *  Created on: 8 lip 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_OPENCL_MESH_EXPORT_DATA_CL_H_
#define MANDELBULBER2_OPENCL_MESH_EXPORT_DATA_CL_H_

typedef struct
{
	cl_float3 limitMin;
	cl_float3 limitMax;
	cl_float distThresh;
	cl_int3 size;
	cl_int maxiter;
	cl_int sliceWidth;	// must be size.x + 1
	cl_int sliceHeight; // must be size.y + 1
	cl_int sliceIndex;
	cl_int coloredMesh;

} sClMeshExport;

#endif /* MANDELBULBER2_OPENCL_MESH_EXPORT_DATA_CL_H_ */
