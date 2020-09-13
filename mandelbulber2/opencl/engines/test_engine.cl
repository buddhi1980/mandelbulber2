/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Test kernel for rendering opencl with minimal footprint
 */

#define MAX_RAYMARCHING 5000

float3 NormalVector(__constant sClInConstants *consts, float3 point, float mainDistance,
	float distThresh, sClCalcParams *calcParam)
{
	float delta = distThresh;
	float s1 = CalculateDistance(consts, point + (float3){delta, 0.0f, 0.0f}, calcParam).distance;
	float s2 = CalculateDistance(consts, point + (float3){0.0f, delta, 0.0f}, calcParam).distance;
	float s3 = CalculateDistance(consts, point + (float3){0.0f, 0.0f, delta}, calcParam).distance;
	float3 normal = (float3){s1 - mainDistance, s2 - mainDistance, s3 - mainDistance};
	normal = normalize(normal);
	return normal;
}

//------------------ MAIN RENDER FUNCTION --------------------
kernel void fractal3D(__global sClPixel *out, __global sClInBuff *inBuff,
	__constant sClInConstants *consts, int Gcl_offset)
{
	int cl_offset = Gcl_offset;

	// get actual pixel
	const int id = get_global_id(0) + cl_offset;
	const int imageX = id % consts->params.imageWidth;
	const int imageYTemp = id / consts->params.imageWidth;
	const int buffIndex = (id - cl_offset);

	const int imageY = clamp(imageYTemp, 0, consts->params.imageHeight - 1);

	float2 screenPoint = (float2){convert_float(imageX), convert_float(imageY)};
	float width = convert_float(consts->params.imageWidth);
	float height = convert_float(consts->params.imageHeight);
	float resolution = 1.0f / width;

	// auxiliary vectors
	const float3 one = (float3){1.0f, 0.0f, 0.0f};
	const float3 ones = 1.0f;

	// main rotation matrix
	matrix33 rot;
	rot.m1 = (float3){1.0f, 0.0f, 0.0f};
	rot.m2 = (float3){0.0f, 1.0f, 0.0f};
	rot.m3 = (float3){0.0f, 0.0f, 1.0f};

	rot = RotateZ(rot, consts->params.viewAngle.x);
	rot = RotateX(rot, consts->params.viewAngle.y);
	rot = RotateY(rot, consts->params.viewAngle.z);

	// starting point for ray-marching
	float3 start = consts->params.camera;

	// view vector
	float aspectRatio = width / height;
	float x2, z2;
	x2 = (screenPoint.x / width - 0.5f) * aspectRatio;
	z2 = -(screenPoint.y / height - 0.5f);
	float3 viewVector = (float3){x2 * consts->params.fov, 1.0f, z2 * consts->params.fov};
	viewVector = Matrix33MulFloat3(rot, viewVector);

	bool found = false;
	int count;

	float3 point;
	float scan, distThresh, distance;

	scan = 1e-10f;

	sClCalcParams calcParam;
	calcParam.N = consts->params.N;
	distThresh = 1e-6f;

	formulaOut outF;
	float step = 0.0f;

	// ray-marching
	for (count = 0; count < MAX_RAYMARCHING && scan < consts->params.viewDistanceMax; count++)
	{
		point = start + viewVector * scan;
		calcParam.distThresh = distThresh;
		outF = CalculateDistance(consts, point, &calcParam);
		distance = outF.distance;
		distThresh = scan * resolution * consts->params.fov;

		if (distance < distThresh)
		{
			found = true;
			break;
		}

		step = (distance - 0.5f * distThresh) * consts->params.DEFactor;
		scan += step;
	}

	// final binary searching
	if (found)
	{
		step *= 0.5f;
		for (int i = 0; i < 5; i++)
		{
			if (distance < distThresh && distance > distThresh * 0.95f)
			{
				break;
			}
			else
			{
				if (distance > distThresh)
				{
					point += viewVector * step;
				}
				else if (distance < distThresh * 0.95f)
				{
					point -= viewVector * step;
				}
			}
			outF = CalculateDistance(consts, point, &calcParam);
			distance = outF.distance;
			step *= 0.5f;
		}
	}

	float3 colour = 0.7f;
	float3 surfaceColour = 1.0f;
	if (found)
	{
		float3 normal = NormalVector(consts, point, distance, distThresh, &calcParam);

		float3 lightVector = (float3){
			cos(consts->params.mainLightAlpha - 0.5f * M_PI_F) * cos(-consts->params.mainLightBeta),
			sin(consts->params.mainLightAlpha - 0.5f * M_PI_F) * cos(-consts->params.mainLightBeta),
			sin(-consts->params.mainLightBeta)};
		lightVector = Matrix33MulFloat3(rot, lightVector);
		float shade = dot(lightVector, normal);
		if (shade < 0.0f) shade = 0.0f;

		float3 halfVector = lightVector - viewVector;
		halfVector = fast_normalize(halfVector);
		float specular = dot(normal, halfVector);
		if (specular < 0.0f) specular = 0.0f;
		specular = pown(specular, 30.0f);
		if (specular > 15.0f) specular = 15.0f;

		colour = colour * (shade + specular);
	}
	else
	{
		colour = 0.0f;
	}

	sClPixel pixel;

	float glow = count / 100.0f * consts->params.DEFactor;

	pixel.R = colour.s0 + glow;
	pixel.G = colour.s1 * glow * 10.0f;
	pixel.B = colour.s2;
	pixel.zBuffer = scan;
	pixel.colR = 0.0f;
	pixel.colG = 0.0f;
	pixel.colB = 0.0f;
	pixel.opacity = 0;
	pixel.alpha = 65535;

	out[buffIndex] = pixel;
}
