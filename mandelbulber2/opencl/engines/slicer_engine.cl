/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * Fast kernel for rendering opencl with missing effects
 */

int GetInteger(int byte, __global char *array)
{
	__global int *intPointer = (__global int *)&array[byte];
	return *intPointer;
}

//------------------ MAIN RENDER FUNCTION --------------------
kernel void fractal3D(__global float *outDistances, __global float *outColor, __global char *inBuff,
	__constant sClInConstants *consts, __constant sClMeshExport *meshConfig)
{
	// get actual pixel
	const int imageX = get_global_id(0);
	const int imageY = get_global_id(1);
	const int cl_offsetX = get_global_offset(0);
	const int cl_offsetY = get_global_offset(1);
	const int buffIndex = (imageX - cl_offsetX) + (imageY - cl_offsetY) * get_global_size(0);

	float2 screenPoint = (float2){convert_float(imageX), convert_float(imageY)};
	float width = convert_float(consts->params.imageWidth);
	float height = convert_float(consts->params.imageHeight);

	float3 delta = (meshConfig->limitMax - meshConfig->limitMin) / convert_float3(meshConfig->size);

	float resolution = 1.0f / height;

	//-------- decode data file ----------------
	// main offset for materials

	int materialsMainOffset = GetInteger(0, inBuff);

	int primitivesMainOffset = GetInteger(3 * sizeof(int), inBuff);

	//--- main material

	// number of materials
	int numberOfMaterials = GetInteger(materialsMainOffset, inBuff);

	// materials 1 offset:
	const int materialAddressOffset = materialsMainOffset + 2 * sizeof(int);
	int material0Offset = GetInteger(materialAddressOffset, inBuff);

	// material header
	int materialClOffset = GetInteger(material0Offset, inBuff);

	// material data
	__global sMaterialCl *material = (__global sMaterialCl *)&inBuff[materialClOffset];

	//--- Primitives

	// primitives count
	int numberOfPrimitives = GetInteger(primitivesMainOffset, inBuff);
	int primitivesGlobalPositionOffset = GetInteger(primitivesMainOffset + 1 * sizeof(int), inBuff);
	int primitivesOffset = GetInteger(primitivesMainOffset + 2 * sizeof(int), inBuff);

	// global position of primitives
	__global sPrimitiveGlobalPositionCl *primitivesGlobalPosition =
		(__global sPrimitiveGlobalPositionCl *)&inBuff[primitivesGlobalPositionOffset];

	// data for primitives
	__global sPrimitiveCl *__attribute__((aligned(16))) primitives =
		(__global sPrimitiveCl *)&inBuff[primitivesOffset];

	//--------- end of data file ----------------------------------

	float3 point;
	point.x = meshConfig->limitMin.x + meshConfig->sliceIndex * delta.x;
	point.y = meshConfig->limitMin.y + imageY * delta.y;
	point.z = meshConfig->limitMin.z + imageX * delta.z;

	sClCalcParams calcParam;
	calcParam.N = meshConfig->maxiter;
	calcParam.normalCalculationMode = false;
	calcParam.iterThreshMode = consts->params.iterThreshMode;
	calcParam.distThresh = meshConfig->distThresh;
	calcParam.detailSize = meshConfig->distThresh;

	float distThresh = meshConfig->distThresh;

	sRenderData renderData;
	renderData.materials = 0;
	renderData.primitives = primitives;
	renderData.numberOfPrimitives = numberOfPrimitives;
	renderData.primitivesGlobalPosition = primitivesGlobalPosition;

	formulaOut outF;

	outF = CalculateDistance(consts, point, &calcParam, &renderData);
	float distance = outF.distance;

#ifdef MESH_EXPORT_COLOR
	int formulaIndex = -1;

	outF = Fractal(consts, point, &calcParam, calcModeColouring, material, formulaIndex);
	float color = outF.colorIndex;
#else
	float color = 0.0f;
#endif

	outDistances[buffIndex] = distance;
	outColor[buffIndex] = color; // TODO color calculation
}
