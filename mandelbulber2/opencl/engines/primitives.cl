/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * calculation of distance to primitive objects for opencl
 */

float PrimitivePlane(__global sPrimitiveCl *primitive, float3 _point)
{
	float3 point = _point - primitive->object.position;
	point = Matrix33MulFloat3(primitive->object.rotationMatrix, point);
	float dist = point.z;
	return primitive->data.plane.empty ? fabs(dist) : dist;
}

float PrimitiveBox(__global sPrimitiveCl *primitive, float3 _point)
{
	float3 point = _point - primitive->object.position;
	point = Matrix33MulFloat3(primitive->object.rotationMatrix, point);
	point = modRepeat(point, primitive->data.box.repeat);

	if (primitive->data.box.empty)
	{
		float boxDist = -1e6f;
		boxDist = max(fabs(point.x) - primitive->object.size.x * 0.5f, boxDist);
		boxDist = max(fabs(point.y) - primitive->object.size.y * 0.5f, boxDist);
		boxDist = max(fabs(point.z) - primitive->object.size.z * 0.5f, boxDist);
		return fabs(boxDist);
	}
	else
	{
		float3 boxTemp;
		boxTemp.x = max(fabs(point.x) - primitive->object.size.x * 0.5, 0.0);
		boxTemp.y = max(fabs(point.y) - primitive->object.size.y * 0.5, 0.0);
		boxTemp.z = max(fabs(point.z) - primitive->object.size.z * 0.5, 0.0);
		return length(boxTemp) - primitive->data.box.rounding;
	}
}

float TotalDistanceToPrimitives(__constant sClInConstants *consts, sRenderData *renderData,
	float3 point, float fractalDistance, int *closestObjectId)
{
	int numberOfPrimitives = renderData->numberOfPrimitives;
	int closestObject = *closestObjectId;
	float dist = fractalDistance;

	// TODO global position and rotation of primituves
	float3 point2 = point;

	for (int i = 0; i < numberOfPrimitives; i++)
	{
		__global sPrimitiveCl *primitive = &renderData->primitives[i];

		if (primitive->object.enable)
		{
			float distTemp = 0.0f;
			switch (primitive->object.objectType)
			{
				case clObjPlane:
				{
					distTemp = PrimitivePlane(primitive, point2);
					break;
				}
				case clObjBox:
				{
					distTemp = PrimitiveBox(primitive, point2);
					break;
				}
				default: break;
			}

			dist = min(dist, distTemp);
		}
	}
	return dist;
}