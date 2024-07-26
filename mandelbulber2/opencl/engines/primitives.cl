/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#ifdef USE_PRIMITIVES

#ifdef USE_PRIMITIVE_PLANE
float PrimitivePlane(__global sPrimitiveCl *primitive, float3 _point)
{
	float3 point = _point - primitive->object.position;
	point = Matrix33MulFloat3(primitive->object.rotationMatrix, point);
	float dist = point.z;
	return primitive->data.plane.empty ? fabs(dist) : dist;
}
#endif

#ifdef USE_PRIMITIVE_BOX
float PrimitiveBox(__global sPrimitiveCl *primitive, float3 _point)
{
	float3 point = _point - primitive->object.position;
	point = Matrix33MulFloat3(primitive->object.rotationMatrix, point);
	point = modRepeat(point, primitive->data.box.repeat);

	float boxDist = -1e6f;
	if (primitive->data.box.empty)
	{
		boxDist = max(fabs(point.x) - primitive->object.size.x * 0.5f, boxDist);
		boxDist = max(fabs(point.y) - primitive->object.size.y * 0.5f, boxDist);
		boxDist = max(fabs(point.z) - primitive->object.size.z * 0.5f, boxDist);
		boxDist = fabs(boxDist);
	}
	else
	{
		float3 boxTemp;
		boxTemp.x = max(fabs(point.x) - primitive->object.size.x * 0.5f, 0.0f);
		boxTemp.y = max(fabs(point.y) - primitive->object.size.y * 0.5f, 0.0f);
		boxTemp.z = max(fabs(point.z) - primitive->object.size.z * 0.5f, 0.0f);
		boxDist = length(boxTemp) - primitive->data.box.rounding;
	}

	if (primitive->data.box.limitsEnable)
	{
		float3 distanceAxial =
			max(point - primitive->data.box.limitsMax, primitive->data.box.limitsMin - point);
		float limitBoxDist = max(max(distanceAxial.x, distanceAxial.y), distanceAxial.z);
		boxDist = max(boxDist, limitBoxDist);
	}
	return boxDist;
}
#endif

#ifdef USE_PRIMITIVE_SPHERE
float PrimitiveSphere(__global sPrimitiveCl *primitive, float3 _point)
{
	float3 point = _point - primitive->object.position;
	point = Matrix33MulFloat3(primitive->object.rotationMatrix, point);
	point = modRepeat(point, primitive->data.sphere.repeat);
	float dist = length(point) - primitive->data.sphere.radius;
	dist = primitive->data.sphere.empty ? fabs(dist) : dist;

	if (primitive->data.sphere.limitsEnable)
	{
		float3 distanceAxial =
			max(point - primitive->data.sphere.limitsMax, primitive->data.sphere.limitsMin - point);
		float limitBoxDist = max(max(distanceAxial.x, distanceAxial.y), distanceAxial.z);
		dist = max(dist, limitBoxDist);
	}
	return dist;
}
#endif

#ifdef USE_PRIMITIVE_RECTANGLE
float PrimitiveRectangle(__global sPrimitiveCl *primitive, float3 _point)
{
	float3 point = _point - primitive->object.position;
	point = Matrix33MulFloat3(primitive->object.rotationMatrix, point);
	float3 boxTemp;
	boxTemp.x = max(fabs(point.x) - primitive->data.rectangle.width * 0.5f, 0.0f);
	boxTemp.y = max(fabs(point.y) - primitive->data.rectangle.height * 0.5f, 0.0f);
	boxTemp.z = fabs(point.z);
	return length(boxTemp);
}
#endif

#ifdef USE_PRIMITIVE_CYLINDER
float PrimitiveCylinder(__global sPrimitiveCl *primitive, float3 _point)
{
	float3 point = _point - primitive->object.position;
	point = Matrix33MulFloat3(primitive->object.rotationMatrix, point);
	point = modRepeat(point, primitive->data.cylinder.repeat);
	float2 cylTemp = (float2){point.x, point.y};
	float dist = length(cylTemp) - primitive->data.cylinder.radius;
	if (!primitive->data.cylinder.caps) dist = fabs(dist);
	dist = max(fabs(point.z) - primitive->data.cylinder.height * 0.5f, dist);
	dist = primitive->data.cylinder.empty ? fabs(dist) : dist;

	if (primitive->data.cylinder.limitsEnable)
	{
		float3 distanceAxial =
			max(point - primitive->data.cylinder.limitsMax, primitive->data.cylinder.limitsMin - point);
		float limitBoxDist = max(max(distanceAxial.x, distanceAxial.y), distanceAxial.z);
		dist = max(dist, limitBoxDist);
	}
	return dist;
}
#endif

#ifdef USE_PRIMITIVE_CIRCLE
float PrimitiveCircle(__global sPrimitiveCl *primitive, float3 _point)
{
	float3 point = _point - primitive->object.position;
	point = Matrix33MulFloat3(primitive->object.rotationMatrix, point);
	float2 circleTemp = (float2){point.x, point.y};
	float distTemp = length(circleTemp) - primitive->data.circle.radius;
	distTemp = max(fabs(point.z), distTemp);
	return distTemp;
}
#endif

#ifdef USE_PRIMITIVE_CONE
float PrimitiveCone(__global sPrimitiveCl *primitive, float3 _point)
{
	float3 point = _point - primitive->object.position;
	point = Matrix33MulFloat3(primitive->object.rotationMatrix, point);
	point = modRepeat(point, primitive->data.cone.repeat);
	point.z -= primitive->data.cone.height;
	float q = sqrt(point.x * point.x + point.y * point.y);
	float2 vect = (float2){q, point.z};
	float dist = dot(primitive->data.cone.wallNormal, vect);
	if (!primitive->data.cone.caps) dist = fabs(dist);
	dist = max(-point.z - primitive->data.cone.height, dist);
	dist = primitive->data.cone.empty ? fabs(dist) : dist;

	if (primitive->data.cone.limitsEnable)
	{
		float3 distanceAxial =
			max(point - primitive->data.cone.limitsMax, primitive->data.cone.limitsMin - point);
		float limitBoxDist = max(max(distanceAxial.x, distanceAxial.y), distanceAxial.z);
		dist = max(dist, limitBoxDist);
	}
	return dist;
}
#endif

#ifdef USE_PRIMITIVE_WATER
float PrimitiveWater(__global sPrimitiveCl *primitive, float3 _point, float distanceFromAnother)
{
	float3 point = _point - primitive->object.position;
	point = Matrix33MulFloat3(primitive->object.rotationMatrix, point);

	float length = primitive->data.water.length;
	if (primitive->data.water.waveFromObjectsEnable)
	{
		point.x += length * 20.0f * exp(-distanceFromAnother / length / 5.0f)
							 * primitive->data.water.waveFromObjectsRelativeAmplitude;
	}

	float amplitude = length * primitive->data.water.relativeAmplitude;
	float objectWave = 0.0f;

	float planeDistance = point.z;
	if (planeDistance < amplitude * 10.0f)
	{
		float phase = -primitive->data.water.animSpeed * primitive->data.water.animFrame * 0.1f;

		if (primitive->data.water.waveFromObjectsEnable)
		{
			objectWave = sin(distanceFromAnother / length * 5.0f - phase * 2.0f)
									 * exp(-distanceFromAnother / length / 5.0f)
									 * primitive->data.water.waveFromObjectsRelativeAmplitude;
		}

		float k = 0.23f;
		float waveXTemp;
		float waveYTemp;
		float waveX = objectWave;
		float waveY = -objectWave;
		float p = 1.0f;
		float p2 = 0.05f;

		point.x += phase * 0.05f * (primitive->data.water.animProgressionSpeed * 3.0f - 3.0f);

		for (int i = 1; i <= primitive->data.water.iterations; i++)
		{
			float p3 = p * p2;
			float shift = phase / (i / 2.697f + 1.0f);
			waveXTemp = sin(i + 0.4f * (waveX)*p3 + sin(k * point.y / length * p3 + shift * 0.134f * p3)
											+ point.x / length * p3 + shift * p3)
									/ p;
			waveYTemp = cos(i + 0.4f * (waveY)*p3 + sin(point.x / length * p3 + shift * 0.0179f * p3)
											+ k * point.y / length * p3 + shift * 0.023f * p3)
									/ p;
			waveX += waveXTemp;
			waveY += waveYTemp;
			p2 = p2 + (1.0f - p2 + 0.5f * sin(shift * 0.0323f)) * 0.7f;
			p *= 1.872f;
		}

		planeDistance += (waveX + waveY) * amplitude;
	}
	planeDistance = primitive->data.water.empty ? fabs(planeDistance) : planeDistance;

	if (primitive->data.water.limitsEnable)
	{
		float3 distanceAxial =
			max(point - primitive->data.water.limitsMax, primitive->data.water.limitsMin - point);
		float limitBoxDist = max(max(distanceAxial.x, distanceAxial.y), distanceAxial.z);
		planeDistance = max(planeDistance, limitBoxDist);
	}
	return planeDistance;
}
#endif

#ifdef USE_PRIMITIVE_TORUS
float PrimitiveTorus(__global sPrimitiveCl *primitive, float3 _point)
{
	float3 point = _point - primitive->object.position;
	point = Matrix33MulFloat3(primitive->object.rotationMatrix, point);
	point = modRepeat(point, primitive->data.torus.repeat);

	float2 pointXY = (float2){point.x, point.y};
	float d1 =
		LengthPow(pointXY, pow(2.0f, primitive->data.torus.radiusLPow)) - primitive->data.torus.radius;
	float2 pointDZ = (float2){d1, point.z};

	float dist = LengthPow(pointDZ, pow(2.0f, primitive->data.torus.tubeRadiusLPow))
							 - primitive->data.torus.tubeRadius;
	dist = primitive->data.torus.empty ? fabs(dist) : dist;

	if (primitive->data.torus.limitsEnable)
	{
		float3 distanceAxial =
			max(point - primitive->data.torus.limitsMax, primitive->data.torus.limitsMin - point);
		float limitBoxDist = max(max(distanceAxial.x, distanceAxial.y), distanceAxial.z);
		dist = max(dist, limitBoxDist);
	}
	return dist;
}
#endif

#ifdef USE_PRIMITIVE_PRISM
float PrimitivePrism(__global sPrimitiveCl *primitive, float3 _point)
{
	float3 point = _point - primitive->object.position;
	point = Matrix33MulFloat3(primitive->object.rotationMatrix, point);
	point = modRepeat(point, primitive->data.prism.repeat);

	float3 q = fabs(point);

	float prismDistance = max(q.z - primitive->data.prism.height,
		max(q.x * primitive->data.prism.normals.y + point.y * primitive->data.prism.normals.z,
			-point.y + primitive->data.prism.triangleHeight)
			- primitive->data.prism.triangleHeight);

	return primitive->data.prism.empty ? fabs(prismDistance) : prismDistance;
}
#endif

float TotalDistanceToPrimitives(__constant sClInConstants *consts, sRenderData *renderData,
	float3 point, float fractalDistance, float detailSize, bool normalCalculationMode,
	int *closestObjectId, int objectIdForVolumetrics)
{
	int numberOfPrimitives = renderData->numberOfPrimitives;
	int closestObject = *closestObjectId;
	float dist = fractalDistance;

	float3 point2 = point - renderData->primitivesGlobalData->allPrimitivesPosition;
	point2 = Matrix33MulFloat3(renderData->primitivesGlobalData->mRotAllPrimitivesRotation, point2);

	for (int i = 0; i < numberOfPrimitives; i++)
	{
		__global sPrimitiveCl *primitive = &renderData->primitives[i];

		if (primitive->object.enable)
		{
			float distTemp = 0.0f;
			switch (primitive->object.objectType)
			{
#ifdef USE_PRIMITIVE_PLANE
				case objPlane:
				{
					distTemp = PrimitivePlane(primitive, point2);
					break;
				}
#endif

#ifdef USE_PRIMITIVE_BOX
				case objBox:
				{
					distTemp = PrimitiveBox(primitive, point2);
					break;
				}
#endif

#ifdef USE_PRIMITIVE_SPHERE
				case objSphere:
				{
					distTemp = PrimitiveSphere(primitive, point2);
					break;
				}
#endif

#ifdef USE_PRIMITIVE_RECTANGLE
				case objRectangle:
				{
					distTemp = PrimitiveRectangle(primitive, point2);
					break;
				}
#endif

#ifdef USE_PRIMITIVE_CYLINDER
				case objCylinder:
				{
					distTemp = PrimitiveCylinder(primitive, point2);
					break;
				}
#endif

#ifdef USE_PRIMITIVE_CIRCLE
				case objCircle:
				{
					distTemp = PrimitiveCircle(primitive, point2);
					break;
				}
#endif

#ifdef USE_PRIMITIVE_CONE
				case objCone:
				{
					distTemp = PrimitiveCone(primitive, point2);
					break;
				}
#endif

#ifdef USE_PRIMITIVE_WATER
				case objWater:
				{
					distTemp = PrimitiveWater(primitive, point2, dist);
					break;
				}
#endif

#ifdef USE_PRIMITIVE_TORUS
				case objTorus:
				{
					distTemp = PrimitiveTorus(primitive, point2);
					break;
				}
#endif

#ifdef USE_PRIMITIVE_PRISM
				case objPrism:
				{
					distTemp = PrimitivePrism(primitive, point2);
					break;
				}
#endif
				default: break;
			}

			if (objectIdForVolumetrics == primitive->object.objectId)
			{
				return distTemp;
			}
			else
			{
				if (primitive->object.usedForVolumetric)
					continue; // skip distance calculation if primitive is used for volumetric effects
			}

#ifdef USE_DISPLACEMENT_TEXTURE
			distTemp = DisplacementMap(distTemp, point2, primitive->object.objectId, renderData, 1.0f);
#endif

#if defined(USE_PERLIN_NOISE) && defined(USE_PERLIN_NOISE_DISPLACEMENT)
			distTemp = PerlinNoiseDisplacement(distTemp, point2, renderData, primitive->object.objectId);
#endif // USE_PERLIN_NOISE

			switch (primitive->booleanOperator)
			{
				case clPrimBooleanOperatorOR:
				{
					if (distTemp < dist)
					{
						closestObject = primitive->object.objectId;
					}

					if (primitive->object.smoothDeCombineEnable)
					{
						dist = opSmoothUnion(distTemp, dist, primitive->object.smoothDeCombineDistance);
					}
					else
					{
						dist = min(distTemp, dist);
					}
					break;
				}
				case clPrimBooleanOperatorAND:
				{
					if (distTemp > dist)
					{
						closestObject = primitive->object.objectId;
					}
					dist = max(dist, distTemp);
					break;
				}
				case clPrimBooleanOperatorSUB:
				{
					const float limit = 1.5f;
					if (dist < detailSize) // if inside 1st
					{
						if (distTemp < detailSize * limit * 1.5f)
						{
							closestObject = primitive->object.objectId;
						}

						if (distTemp < detailSize * limit) // if inside 2nd
						{
							if (normalCalculationMode)
							{
								dist = max(detailSize * limit - distTemp, dist);
							}
							else
							{
								dist = detailSize * limit;
							}
						}
						else // if outside of 2nd
						{
							dist = max(detailSize * limit - distTemp, dist);
							if (dist < 0.0f) dist = 0.0f;
						}
					}
					break;
				}
				case clPrimBooleanOperatorRevSUB:
				{
					int closestObjectTemp = closestObject;
					closestObject = primitive->object.objectId;
					const float limit = 1.5f;
					if (distTemp < detailSize) // if inside 2nd
					{
						if (dist < detailSize * limit * 1.5f)
						{
							closestObject = closestObjectTemp;
						}

						if (dist < detailSize * limit) // if inside 1st
						{
							if (normalCalculationMode)
							{
								dist = max(detailSize * limit - dist, distTemp);
							}
							else
							{
								dist = detailSize * limit;
							}
						}
						else // if outside of 1st
						{
							distTemp = max(detailSize * limit - dist, distTemp);
							dist = distTemp;
							if (dist < 0.0f) dist = 0.0f;
						}
					}
					else
					{
						dist = distTemp;
					}
					break;
				}
			} // switch
		}
	}

	*closestObjectId = closestObject;
	return dist;
}

#endif // USE_PRIMITIVES
