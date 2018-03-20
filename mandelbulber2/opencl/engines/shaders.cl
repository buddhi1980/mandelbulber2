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
 * misc shader function optimized for opencl
 */

typedef struct
{
	float3 point;
	float3 viewVector;
	float3 viewVectorNotRotated;
	float3 normal;
	float3 lightVect;
	float distThresh; // distance threshold depend on 'detailLevel'
	float lastDist;
	float delta; // initial step distance for shaders based on distance form camera
	float depth;
	int stepCount;
	int AOVectorsCount;
	int numberOfLights;
	int randomSeed;
	bool invertMode;
	__global sMaterialCl *material;
	__global float4 *palette;
	__global sVectorsAroundCl *AOVectors;
	__global sLightCl *lights;
	int paletteSize;

} sShaderInputDataCl;

#ifdef ITER_FOG
float IterOpacity(const float step, float iters, float maxN, float trim, float opacitySp)
{
	float opacity = (iters - trim) / maxN;
	if (opacity < 0.0f) opacity = 0.0f;
	opacity *= opacity;
	opacity *= step * opacitySp;
	if (opacity > 1.0f) opacity = 1.0f;
	return opacity;
}
#endif // ITER_FOG

#ifdef USE_IRIDESCENCE
float3 Hsv2rgb(float hue, float sat, float val)
{
	float3 rgb;
	float h = hue / 60.0f;
	int i = (int)h;
	float f = h - i;
	float p = val * (1.0f - sat);
	float q = val * (1.0f - (sat * f));
	float t = val * (1.0f - (sat * (1.0f - f)));
	switch (i)
	{
		case 0: rgb = (float3){val, t, p}; break;
		case 1: rgb = (float3){q, val, p}; break;
		case 2: rgb = (float3){p, val, t}; break;
		case 3: rgb = (float3){p, q, val}; break;
		case 4: rgb = (float3){t, p, val}; break;
		case 5: rgb = (float3){val, p, q}; break;
	}

	return rgb;
}
#endif

//-------------- background shaders ---------------

float3 BackgroundShader(__constant sClInConstants *consts, sShaderInputDataCl *input)
{
	float3 pixel;
	if (consts->params.background3ColorsEnable)
	{
		float3 vector = (float3){0.0f, 0.0f, 1.0f};
		vector = normalize(vector);
		float3 viewVectorNorm = input->viewVector;
		viewVectorNorm = normalize(viewVectorNorm);
		float grad = dot(viewVectorNorm, vector) + 1.0f;

		if (grad < 1.0f)
		{
			float gradN = 1.0f - grad;
			pixel = consts->params.background_color3 * gradN + consts->params.background_color2 * grad;
		}
		else
		{
			grad = grad - 1.0f;
			float gradN = 1.0f - grad;
			pixel = consts->params.background_color2 * gradN + consts->params.background_color1 * grad;
		}
		pixel *= consts->params.background_brightness;

		float light = (dot(viewVectorNorm, input->lightVect) - 1.0f) * 360.0f
									/ consts->params.mainLightVisibilitySize;
		light = 1.0f / (1.0f + pow(light, 6.0)) * consts->params.mainLightVisibility
						* consts->params.mainLightIntensity;

		pixel += light * consts->params.mainLightColour;
	}
	else
	{
		pixel = consts->params.background_color1 * consts->params.background_brightness;
	}
	return pixel;
}

//----------------- object shaders -----------------

float3 IndexToColour(int index, sShaderInputDataCl *input)
{
	float3 color = (float3){1.0f, 1.0f, 1.0f};

	if (index < 0)
	{
		color = input->palette[input->paletteSize - 1].xyz;
	}
	else
	{
		int col = (index / 256) % (input->paletteSize);
		int colPlus1 = (col + 1) % (input->paletteSize);

		float4 color1 = input->palette[col];
		float4 color2 = input->palette[colPlus1];
		float4 deltaC = (color2 - color1) / 256.0f;

		float delta = index % 256;
		color = (color1 + (deltaC * delta)).xyz;
	}
	return color;
}

float3 SurfaceColor(
	__constant sClInConstants *consts, sShaderInputDataCl *input, sClCalcParams *calcParams)
{
	float3 out;
	calcParams->distThresh = input->distThresh;
	calcParams->detailSize = input->delta;

	formulaOut fout;
	fout.z = 0.0f;
	fout.iters = 0;
	fout.distance = 0.0f;
	fout.colorIndex = 0.0f;
	fout.maxiter = false;

	float3 color = (float3){1.0f, 1.0f, 1.0f};
	if (input->material->useColorsFromPalette)
	{
		__global sFractalColoringCl *fractalColoring = &input->material->fractalColoring;
		fout = Fractal(consts, input->point, calcParams, calcModeColouring, fractalColoring);
		int nCol = floor(fout.colorIndex);
		nCol = abs(nCol) % (248 * 256);
		int color_number =
			(int)(nCol * input->material->coloring_speed + 256 * input->material->paletteOffset) % 65536;

		color = IndexToColour(color_number, input);
	}
	else
	{
		color = input->material->color;
	}

	out = (float3){color.x, color.y, color.z};

	return out;
}

float3 MainShading(sShaderInputDataCl *input)
{
	float shade = dot(input->normal, input->lightVect);
	if (shade < 0.0f) shade = 0.0f;
	return shade;
}

float3 MainSpecular(sShaderInputDataCl *input)
{
	float3 halfVector = input->lightVect - input->viewVector;
	halfVector = fast_normalize(halfVector);
	float specular = dot(input->normal, halfVector);
	if (specular < 0.0f) specular = 0.0f;
	specular = pow(specular, 30.0f / input->material->specularWidth);
	if (specular > 15.0f) specular = 15.0f;
	float3 out = specular;
	out *= input->material->specularColor;
	return out;
}

#if defined(SHADOWS) || defined(VOLUMETRIC_LIGHTS)
float3 MainShadow(
	__constant sClInConstants *consts, sShaderInputDataCl *input, sClCalcParams *calcParam)
{
	float3 shadow = (float3){1.0f, 1.0f, 1.0f};

	// starting point
	float3 point2;

	float factor = input->delta / consts->params.resolution;
	if (!consts->params.penetratingLights) factor = consts->params.viewDistanceMax;
	float dist;

	float DEFactor = consts->params.DEFactor;
	if (consts->params.iterFogEnabled || consts->params.volumetricLightEnabled[0]) DEFactor = 1.0f;

	// float start = input->delta;
	float start = input->distThresh;
	if (consts->params.interiorMode) start = input->distThresh * DEFactor;

	float opacity;
	float shadowTemp = 1.0f;
	float iterFogSum = 0.0f;

	float softRange = tan(consts->params.shadowConeAngle / 180.0f * M_PI_F);
	float maxSoft = 0.0f;

	const bool bSoft = !consts->params.iterFogEnabled && !consts->params.limitsEnabled
										 && !consts->params.common.iterThreshMode && softRange > 0.0f;

	int count = 0;
	float step = 0.0f;
	for (float i = start; i < factor; i += step)
	{
		point2 = input->point + input->lightVect * i;

		float dist_thresh;
		if (consts->params.iterFogEnabled || consts->params.volumetricLightEnabled[0])
		{
			dist_thresh = CalcDistThresh(point2, consts);
		}
		else
			dist_thresh = input->distThresh;

		calcParam->distThresh = dist_thresh;
		formulaOut outF;

		outF = CalculateDistance(consts, point2, calcParam);
		dist = outF.distance;

		if (bSoft)
		{
			float angle = (dist - dist_thresh) / i;
			if (angle < 0.0f) angle = 0.0f;
			if (dist < dist_thresh) angle = 0;
			float softShadow = 1.0f - angle / softRange;
			if (consts->params.penetratingLights) softShadow *= (factor - i) / factor;
			if (softShadow < 0.0f) softShadow = 0.0f;
			if (softShadow > maxSoft) maxSoft = softShadow;
		}

#ifdef ITER_FOG
		opacity = IterOpacity(dist * DEFactor, outF.iters, consts->params.N,
			consts->params.iterFogOpacityTrim, consts->params.iterFogOpacity);
		opacity *= (factor - i) / factor;
		opacity = min(opacity, 1.0f);
		iterFogSum = opacity + (1.0f - opacity) * iterFogSum;
#else
		opacity = 0.0f;
#endif

		shadowTemp = 1.0f - iterFogSum;

		if (dist < dist_thresh || shadowTemp <= 0.0f)
		{
			shadowTemp -= (factor - i) / factor;
			if (!consts->params.penetratingLights) shadowTemp = 0.0f;
			if (shadowTemp < 0.0f) shadowTemp = 0.0f;
			break;
		}

		step = dist * DEFactor;
		step = max(step, 1e-6f);

		count++;
		if (count > MAX_RAYMARCHING) break;
	}
	if (!bSoft)
	{
		shadow.s0 = shadowTemp;
		shadow.s1 = shadowTemp;
		shadow.s2 = shadowTemp;
	}
	else
	{
		shadow.s0 = 1.0f - maxSoft;
		shadow.s1 = 1.0f - maxSoft;
		shadow.s2 = 1.0f - maxSoft;
	}
	return shadow;
}
#endif

#ifdef AO_MODE_FAST
float3 FastAmbientOcclusion(
	__constant sClInConstants *consts, sShaderInputDataCl *input, sClCalcParams *calcParam)
{
	// reference Iñigo Quilez –iq/rgba:
	// http://www.iquilezles.org/www/material/nvscene2008/rwwtt.pdf
	float delta = input->distThresh;
	float aoTemp = 0;
	float quality = consts->params.ambientOcclusionQuality;
	float lastDist = 1e20f;
	for (int i = 1; i < quality * quality; i++)
	{
		float scan = i * i * delta;
		float3 pointTemp = input->point + input->normal * scan;

		calcParam->distThresh = input->distThresh;
		formulaOut outF;
		outF = CalculateDistance(consts, pointTemp, calcParam);
		float dist = outF.distance;

		if (dist > lastDist * 2.0f) dist = lastDist * 2.0f;
		lastDist = dist;
		aoTemp += 1.0f / pow(2.0f, i) * (scan - consts->params.ambientOcclusionFastTune * dist)
							/ input->distThresh;
	}
	float ao = 1.0f - 0.2f * aoTemp;
	if (ao < 0) ao = 0;
	float3 output = (float3){ao, ao, ao};
	return output;
}
#endif

#ifdef AO_MODE_MULTIPLE_RAYS
float3 AmbientOcclusion(
	__constant sClInConstants *consts, sShaderInputDataCl *input, sClCalcParams *calcParam)
{
	float3 AO = 0.0f;

	float start_dist = input->delta;
	float end_dist = input->delta / consts->params.resolution;
	float intense;

#ifndef MONTE_CARLO_DOF
	for (int i = 0; i < input->AOVectorsCount; i++)
	{
#else
	int i = Random(input->AOVectorsCount, &calcParam->randomSeed);
#endif
		sVectorsAroundCl v = input->AOVectors[i];

		float dist;

		float opacity;
		float shadowTemp = 1.0f;

		for (float r = start_dist; r < end_dist; r += dist * 2.0f)
		{
			float3 point2 = input->point + v.v * r;

			calcParam->distThresh = input->distThresh;
			formulaOut outF;
			outF = CalculateDistance(consts, point2, calcParam);
			dist = outF.distance;

#ifdef ITER_FOG
			opacity = IterOpacity(dist * 2.0, outF.iters, consts->params.N,
				consts->params.iterFogOpacityTrim, consts->params.iterFogOpacity);
#else
		opacity = 0.0f;
#endif

			shadowTemp -= opacity * (end_dist - r) / end_dist;

			float dist_thresh;
			if (consts->params.iterFogEnabled || consts->params.volumetricLightEnabled[0])
			{
				dist_thresh = CalcDistThresh(point2, consts);
			}
			else
				dist_thresh = input->distThresh;

			if (dist < dist_thresh || outF.maxiter || shadowTemp < 0.0f)
			{
				shadowTemp -= (end_dist - r) / end_dist;
				if (shadowTemp < 0.0f) shadowTemp = 0.0f;
				break;
			}
		}

		intense = shadowTemp;

		AO += intense * v.color;

#ifndef MONTE_CARLO_DOF
	}
	AO /= input->AOVectorsCount;
#endif

	return AO;
}
#endif

#ifdef AUX_LIGHTS
#if defined(SHADOWS) || defined(VOLUMETRIC_LIGHTS)
float AuxShadow(constant sClInConstants *consts, sShaderInputDataCl *input, float distance,
	float3 lightVector, sClCalcParams *calcParam)
{
	// float step = input.delta;
	float dist;
	float light;

	float opacity;
	float shadowTemp = 1.0f;

	float DE_factor = consts->params.DEFactor;
	if (consts->params.iterFogEnabled || consts->params.volumetricLightAnyEnabled) DE_factor = 1.0f;

	float softRange = tan(consts->params.shadowConeAngle / 180.0f * M_PI_F);
	float maxSoft = 0.0f;

	const bool bSoft = !consts->params.iterFogEnabled && !consts->params.limitsEnabled
										 && !consts->params.common.iterThreshMode && softRange > 0.0f;

	int count = 0;
	float step = 0.0f;

	float iterFogSum = 0.0f;

	for (float i = input->delta; i < distance; i += step)
	{
		float3 point2 = input->point + lightVector * i;

		float dist_thresh;
		if (consts->params.iterFogEnabled || consts->params.volumetricLightAnyEnabled)
		{
			dist_thresh = CalcDistThresh(point2, consts);
		}
		else
			dist_thresh = input->distThresh;

		calcParam->distThresh = dist_thresh;
		formulaOut outF;

		outF = CalculateDistance(consts, point2, calcParam);
		dist = outF.distance;

		if (bSoft)
		{
			float angle = (dist - dist_thresh) / i;
			if (angle < 0.0f) angle = 0.0f;
			if (dist < dist_thresh) angle = 0.0f;
			float softShadow = 1.0f - angle / softRange;
			if (consts->params.penetratingLights) softShadow *= (distance - i) / distance;
			if (softShadow < 0.0f) softShadow = 0.0f;
			if (softShadow > maxSoft) maxSoft = softShadow;
		}

#ifdef ITER_FOG
		opacity = IterOpacity(dist * DE_factor, outF.iters, consts->params.N,
			consts->params.iterFogOpacityTrim, consts->params.iterFogOpacity);

		opacity *= (distance - i) / distance;
		opacity = min(opacity, 1.0f);
		iterFogSum = opacity + (1.0f - opacity) * iterFogSum;
#else
		opacity = 0.0f;
#endif

		shadowTemp = 1.0f - iterFogSum;

		if (dist < dist_thresh || shadowTemp <= 0.0f)
		{
			if (consts->params.penetratingLights)
			{
				shadowTemp -= (distance - i) / distance;
				if (shadowTemp < 0.0f) shadowTemp = 0.0f;
			}
			else
			{
				shadowTemp = 0.0f;
			}
			break;
		}

		step = dist * DE_factor;
		step = max(step, 1e-6f);

		count++;
		if (count > MAX_RAYMARCHING) break;
	}

	if (!bSoft)
	{
		light = shadowTemp;
	}
	else
	{
		light = 1.0f - maxSoft;
	}

	return light;
}
#endif // SHADOWS

float3 LightShading(__constant sClInConstants *consts, sShaderInputDataCl *input,
	sClCalcParams *calcParam, __global sLightCl *light, float3 *outSpecular)
{
	float3 shading = 0.0f;

	float3 d = light->position - input->point;

	float distance = length(d);

	// angle of incidence
	float3 lightVector = d;
	lightVector = normalize(lightVector);

	// intensity of lights is divided by 6 because of backward compatibility. There was an error
	// where numberOfLights of light was always 24
	float intensity = 100.0f * light->intensity / (distance * distance) / input->numberOfLights / 6.0;
	float shade = dot(input->normal, lightVector);
	if (shade < 0.0) shade = 0.0;
	shade = 1.0f - input->material->shading + shade * input->material->shading;

	shade = shade * intensity;
	if (shade > 500.0f) shade = 500.0f;

	// specular
	float3 halfVector = lightVector - input->viewVector;
	halfVector = normalize(halfVector);
	float shade2 = dot(input->normal, halfVector);
	if (shade2 < 0.0f) shade2 = 0.0f;

	shade2 = pow(shade2, 30.0f / input->material->specularWidth);
	shade2 *= intensity * input->material->specular;
	if (shade2 > 15.0f) shade2 = 15.0f;

	// calculate shadow
	if ((shade > 0.01f || shade2 > 0.01f) && consts->params.shadow)
	{
		float auxShadow = 1.0f;
#ifdef SHADOWS
		auxShadow = AuxShadow(consts, input, distance, lightVector, calcParam);
#endif
		shade *= auxShadow;
		shade2 *= auxShadow;
	}
	else
	{
		if (consts->params.shadow)
		{
			shade = 0.0f;
			shade2 = 0.0f;
		}
	}

	shading = shade * light->colour;

	float3 specular;
	specular = shade2 * light->colour;

	*outSpecular = specular;

	return shading;
}

float3 AuxLightsShader(__constant sClInConstants *consts, sShaderInputDataCl *input,
	sClCalcParams *calcParam, float3 *specularOut)
{

	int numberOfLights = input->numberOfLights;
	if (numberOfLights < 4) numberOfLights = 4;
	float3 shadeAuxSum = 0.0f;
	float3 specularAuxSum = 0.0f;
	for (int i = 0; i < numberOfLights; i++)
	{
		__global sLightCl *light = &input->lights[i];

		if (i < consts->params.auxLightNumber || light->enabled)
		{
			float3 specularAuxOutTemp;
			float3 shadeAux = LightShading(consts, input, calcParam, light, &specularAuxOutTemp);
			shadeAuxSum += shadeAux;
			specularAuxSum += specularAuxOutTemp;
		}
	}

	*specularOut = specularAuxSum;
	return shadeAuxSum;
}
#endif // AUX_LIGHTS

#ifdef FAKE_LIGHTS
float3 FakeLightsShader(__constant sClInConstants *consts, sShaderInputDataCl *input,
	sClCalcParams *calcParams, float3 *specularOut)
{
	float3 fakeLights;

	float delta = input->distThresh * consts->params.smoothness;

	formulaOut outF;

	outF = Fractal(consts, input->point, calcParams, calcModeOrbitTrap, NULL);
	float rr = outF.orbitTrapR;
	float r = 1.0f / (rr + 1e-20f);

	float fakeLight = consts->params.fakeLightsIntensity / r;

	float3 out;
	calcParams->distThresh = input->distThresh;
	calcParams->detailSize = input->delta;

	outF = Fractal(
		consts, input->point + (float3){delta, 0.0f, 0.0f}, calcParams, calcModeOrbitTrap, NULL);
	float rx = 1.0f / (outF.orbitTrapR + 1e-30f);

	outF = Fractal(
		consts, input->point + (float3){0.0f, delta, 0.0f}, calcParams, calcModeOrbitTrap, NULL);
	float ry = 1.0f / (outF.orbitTrapR + 1e-30f);

	outF = Fractal(
		consts, input->point + (float3){0.0f, 0.0f, delta}, calcParams, calcModeOrbitTrap, NULL);
	float rz = 1.0f / (outF.orbitTrapR + 1e-30f);

	float3 fakeLightNormal;
	fakeLightNormal.x = r - rx;
	fakeLightNormal.y = r - ry;
	fakeLightNormal.z = r - rz;

	fakeLightNormal = normalize(fakeLightNormal);

	float fakeLight2 = fakeLight * dot(input->normal, fakeLightNormal);
	if (fakeLight2 < 0.0f) fakeLight2 = 0.0f;

	fakeLights = fakeLight2 * consts->params.fakeLightsColor;

	float3 halfVector = normalize(fakeLightNormal - input->viewVector);
	float fakeSpecular = dot(input->normal, halfVector);
	if (fakeSpecular < 0.0f) fakeSpecular = 0.0f;
	fakeSpecular = pow(fakeSpecular, 30.0f / input->material->specularWidth);
	if (fakeSpecular > 15.0f) fakeSpecular = 15.0f;
	float3 fakeSpec = fakeSpecular * consts->params.fakeLightsColor;

	fakeSpec = 0.0f; // TODO to check why in CPU code it's zero
	*specularOut = fakeSpec;
	return fakeLights;
}
#endif // FAKE_LIGTS

//------------- Iridescence shader -------------
#ifdef USE_IRIDESCENCE
float3 IridescenceShader(
	__constant sClInConstants *consts, sShaderInputDataCl *input, sClCalcParams *calcParam)
{
	float3 rainbowColor = 1.0f;
	if (input->material->iridescenceIntensity > 0.0f)
	{
		float dist1 = input->lastDist;
		float3 pointTemp = input->point - input->viewVector * input->delta;

		calcParam->distThresh = input->distThresh;
		formulaOut outF;
		outF = CalculateDistance(consts, pointTemp, calcParam);
		float dist2 = outF.distance;

		float diff = fabs(dist1 - dist2);
		float surfaceThickness =
			(diff > 0.0f) ? input->delta * input->material->iridescenceSubsurfaceThickness / diff : 1e6f;
		float rainbowIndex = fmod(surfaceThickness, 1.0f) * 360.0f;
		float sat = input->material->iridescenceIntensity / (0.1f + surfaceThickness);
		if (sat > 1.0f) sat = 1.0f;
		rainbowColor = Hsv2rgb(rainbowIndex, sat, 1.0f);
	}
	return rainbowColor;
}
#endif

//------------ Object shader ----------------
float3 ObjectShader(__constant sClInConstants *consts, sShaderInputDataCl *input,
	sClCalcParams *calcParam, float3 *outSurfaceColor, float3 *outSpecular, float3 *iridescenceOut)
{
	float3 color = 0.7f;
	float3 mainLight = consts->params.mainLightColour * consts->params.mainLightIntensity;

	float3 shade = 0.0f;
	float3 specular = 0.0f;
	float3 shadow = 1.0f;

	if (consts->params.mainLightEnable)
	{
		shade = MainShading(input);
		shade = consts->params.mainLightIntensity
						* ((float3)1.0f - input->material->shading + input->material->shading * shade);

		specular = MainSpecular(input) * input->material->specular;

#ifdef SHADOWS
		if (consts->params.shadow)
		{
			shadow = MainShadow(consts, input, calcParam);
		}
#endif
	}

	float3 surfaceColor = SurfaceColor(consts, input, calcParam);

	float3 AO = 0.0f;
	if (consts->params.ambientOcclusionEnabled)
	{
#ifdef AO_MODE_FAST
		AO = FastAmbientOcclusion(consts, input, calcParam);
#endif
#ifdef AO_MODE_MULTIPLE_RAYS
		AO = AmbientOcclusion(consts, input, calcParam);
#endif
		AO *= consts->params.ambientOcclusion;
	}

	float3 auxLights = 0.0f;
	float3 auxSpecular = 0.0f;

#ifdef AUX_LIGHTS
	auxLights = AuxLightsShader(consts, input, calcParam, &auxSpecular);
#endif

	float3 fakeLights = 0.0f;
	float3 fakeLightsSpecular = 0.0f;
#ifdef FAKE_LIGHTS
	fakeLights = FakeLightsShader(consts, input, calcParam, &fakeLightsSpecular);
#endif

	float3 iridescence = 1.0f;
#ifdef USE_IRIDESCENCE
	if (input->material->iridescenceEnabled)
	{
		iridescence = IridescenceShader(consts, input, calcParam);
	}
#endif
	*iridescenceOut = iridescence;

	float3 totalSpecular =
		(mainLight * shadow * specular + fakeLightsSpecular + auxSpecular) * iridescence;

	if (input->material->metallic)
	{
		color = surfaceColor * (mainLight * shadow * shade + auxLights + fakeLights + AO)
						+ totalSpecular * surfaceColor;
		*outSpecular = totalSpecular * surfaceColor;
	}
	else
	{
		color =
			surfaceColor * (mainLight * shadow * shade + auxLights + fakeLights + AO) + totalSpecular;
		*outSpecular = totalSpecular;
	}

	*outSurfaceColor = surfaceColor;
	return color;
}

//------------ Volumetric shader ----------------
float4 VolumetricShader(__constant sClInConstants *consts, sShaderInputDataCl *input,
	sClCalcParams *calcParam, float4 oldPixel, float *opacityOut)
{
	float4 out4 = oldPixel;
	float3 output = oldPixel.xyz;
	float totalOpacity = 0.0f;

	// visible lights init
	int numberOfLights = input->numberOfLights;
	if (numberOfLights < 4) numberOfLights = 4;

#ifdef GLOW
	// glow init
	float glow = input->stepCount * consts->params.glowIntensity / 512.0f * consts->params.DEFactor;
	float glowN = 1.0f - glow;
	if (glowN < 0.0f) glowN = 0.0f;

	float3 glowColor;

	glowColor = (glowN * consts->params.glowColor1 + consts->params.glowColor2 * glow);

#endif // GLOW

#ifdef SIMPLE_GLOW // only simple glow, no another shaders
#ifdef GLOW
	glow *= 0.7f;
	float glowOpacity = 1.0f * glow;
	if (glowOpacity > 1.0f) glowOpacity = 1.0f;
	output = glow * glowColor + (1.0f - glowOpacity) * output;
	out4.s3 += glowOpacity;
#endif // GLOW

#else // not SIMPLE_GLOW
	float totalStep = 0.0f;
	float scan = CalcDelta(input->point, consts);

	sShaderInputDataCl input2 = *input;

	for (int i = 0; i < MAX_RAYMARCHING; i++)
	{
		float3 point = input->point - input->viewVector * scan;

		calcParam->distThresh = input->distThresh;

		formulaOut outF;
		outF = CalculateDistance(consts, point, calcParam);
		float distance = outF.distance;

		input2.point = point;
		input2.distThresh = CalcDistThresh(point, consts);
		input2.delta = CalcDelta(point, consts);

		float step = distance * consts->params.DEFactor * consts->params.volumetricLightDEFactor;

		step *= (1.0f - Random(1000, &input->randomSeed) / 10000.0f);

		step = max(step, input2.delta);

		bool end = false;
		if (step > input->depth - scan)
		{
			step = input->depth - scan;
			end = true;
		}
		scan += step;

//------------------- glow
#ifdef GLOW
		{
			if (input->stepCount > 0)
			{
				float glowOpacity = glow / input->stepCount;
				if (glowOpacity > 1.0f) glowOpacity = 1.0f;

				output = glowOpacity * glowColor + (1.0f - glowOpacity) * output;
				out4.s3 += glowOpacity;
			}
		}
#endif // GLOW

#ifdef VISIBLE_AUX_LIGHTS
		//------------------ visible light
		{
			float miniStep = 0.0f;
			float lastMiniSteps = -1.0f;

			for (float miniSteps = 0.0f; miniSteps < step; miniSteps += miniStep)
			{
				float lowestLightSize = 1e10f;
				float lowestLightDist = 1e10f;
				for (int i = 0; i < numberOfLights; ++i)
				{
					__global sLightCl *light = &input->lights[i];
					if (light->enabled)
					{
						float3 lightDistVect = point - input->viewVector * miniSteps - light->position;
						float lightDist = fast_length(lightDistVect);
						float lightSize = native_sqrt(light->intensity) * consts->params.auxLightVisibilitySize;
						float distToLightSurface = lightDist - lightSize;
						distToLightSurface = max(distToLightSurface, 0.0f);
						if (distToLightSurface <= lowestLightDist)
						{
							lowestLightSize = min(lowestLightSize, lightSize);
							lowestLightDist = distToLightSurface;
						}
					}
				}

				miniStep = 0.1f * (lowestLightDist + 0.1f * lowestLightSize);
				miniStep = clamp(miniStep, step * 0.01f, step - miniSteps);
				miniStep = max(miniStep, 1e-6f);

				for (int i = 0; i < numberOfLights; ++i)
				{
					__global sLightCl *light = &input->lights[i];
					if (light->enabled && light->intensity > 0.0f)
					{
						float3 lightDistVect = point - input->viewVector * miniSteps - light->position;
						float lightDist = fast_length(lightDistVect);
						float lightSize = native_sqrt(light->intensity) * consts->params.auxLightVisibilitySize;
						float r2 = native_divide(lightDist, lightSize);
						float bellFunction = native_divide(1.0f, (1.0f + native_powr(r2, 4.0f)));
						float lightDensity =
							native_divide(miniStep * bellFunction * consts->params.auxLightVisibility, lightSize);

						output += lightDensity * light->colour;
						out4.s3 += lightDensity;
					}
				}
				if (miniSteps == lastMiniSteps)
				{
					// Dead computation
					break;
				}
				lastMiniSteps = miniSteps;
			}
		}
#endif

#ifdef FAKE_LIGHTS
		// fake lights (orbit trap)
		{
			formulaOut outF;
			outF = Fractal(consts, input2.point, calcParam, calcModeOrbitTrap, NULL);
			float r = outF.orbitTrapR;
			r = sqrt(1.0f / (r + 1.0e-20f));
			float fakeLight = 1.0f / (pow(r, 10.0f / consts->params.fakeLightsVisibilitySize)
																	 * pow(10.0f, 10.0f / consts->params.fakeLightsVisibilitySize)
																 + 1e-20f);
			float3 light = fakeLight * step * consts->params.fakeLightsVisibility;
			output += light * consts->params.fakeLightsColor;
			out4.s3 += fakeLight * step * consts->params.fakeLightsVisibility;
		}
#endif // FAKE_LIGHTS

#ifdef VOLUMETRIC_LIGHTS
		for (int i = 0; i < 5; i++)
		{
			if (i == 0 && consts->params.volumetricLightEnabled[0])
			{
				float3 shadowOutputTemp = MainShadow(consts, &input2, calcParam);
				output += shadowOutputTemp.s0 * step * consts->params.volumetricLightIntensity[0]
									* consts->params.mainLightColour;
				out4.s3 += (shadowOutputTemp.s0 + shadowOutputTemp.s1 + shadowOutputTemp.s2) / 3.0f * step
									 * consts->params.volumetricLightIntensity[0];
			}
#ifdef AUX_LIGHTS
			if (i > 0)
			{
				__global sLightCl *light = &input->lights[i - 1];
				if (light->enabled && consts->params.volumetricLightEnabled[i])
				{
					float3 lightVectorTemp = light->position - point;
					float distanceLight = length(lightVectorTemp);
					float distanceLight2 = distanceLight * distanceLight;
					lightVectorTemp = normalize(lightVectorTemp);
					float lightShadow = AuxShadow(consts, &input2, distanceLight, lightVectorTemp, calcParam);

					output += lightShadow * light->colour * consts->params.volumetricLightIntensity[i] * step
										/ distanceLight2;
					out4.s3 +=
						lightShadow * consts->params.volumetricLightIntensity[i] * step / distanceLight2;
				}
			}
#endif // AUX_LIGHTS
		}
#endif // VOLUMETRIC_LIGHTS

//----------------------- basic fog
#ifdef BASIC_FOG
		{
			float fogDensity = step / consts->params.fogVisibility;
			if (fogDensity > 1.0f) fogDensity = 1.0f;

			output = fogDensity * consts->params.fogColor + (1.0f - fogDensity) * output;

			totalOpacity = fogDensity + (1.0f - fogDensity) * totalOpacity;
			out4.s3 = fogDensity + (1.0f - fogDensity) * out4.s3;
		}
#endif // BASIC_FOG

//-------------------- volumetric fog
#ifdef VOLUMETRIC_FOG
		{
			float densityTemp =
				step * consts->params.volFogDistanceFactor
				/ (distance * distance
						+ consts->params.volFogDistanceFactor * consts->params.volFogDistanceFactor);

			float k = distance / consts->params.volFogColour1Distance;
			if (k > 1.0f) k = 1.0f;
			float kn = 1.0f - k;
			float3 fogTemp;
			fogTemp = consts->params.volFogColour1 * kn + consts->params.volFogColour2 * k;

			float k2 = distance / consts->params.volFogColour2Distance * k;
			if (k2 > 1) k2 = 1.0;
			kn = 1.0f - k2;
			fogTemp = fogTemp * kn + consts->params.volFogColour3 * k2;

			float fogDensity = 0.3f * consts->params.volFogDensity * densityTemp
												 / (1.0f + consts->params.volFogDensity * densityTemp);
			if (fogDensity > 1.0f) fogDensity = 1.0f;

			output = fogDensity * fogTemp + (1.0f - fogDensity) * output;

			totalOpacity = fogDensity + (1.0f - fogDensity) * totalOpacity;
			out4.s3 = fogDensity + (1.0f - fogDensity) * out4.s3;
		}
#endif // VOLUMETRIC_FOG

// ---------- iter fog
#ifdef ITER_FOG
		{
			int L = outF.iters;
			float opacity = IterOpacity(step, L, consts->params.N, consts->params.iterFogOpacityTrim,
				consts->params.iterFogOpacity);

			float3 newColour = 0.0f;

			if (opacity > 0.0f)
			{
				// fog colour
				float iterFactor1 =
					(L - consts->params.iterFogOpacityTrim)
					/ (consts->params.iterFogColor1Maxiter - consts->params.iterFogOpacityTrim);
				float k = iterFactor1;
				if (k > 1.0f) k = 1.0f;
				if (k < 0.0f) k = 0.0f;
				float kn = 1.0f - k;
				float3 fogCol = consts->params.iterFogColour1 * kn + consts->params.iterFogColour2 * k;

				float iterFactor2 =
					(L - consts->params.iterFogColor1Maxiter)
					/ (consts->params.iterFogColor2Maxiter - consts->params.iterFogColor1Maxiter);
				float k2 = iterFactor2;
				if (k2 < 0.0f) k2 = 0.0f;
				if (k2 > 1.0f) k2 = 1.0f;
				kn = 1.0f - k2;
				fogCol = fogCol * kn + consts->params.iterFogColour3 * k2;
				//----

				for (int i = 0; i < 5; i++)
				{
					if (i == 0)
					{
						if (consts->params.mainLightEnable && consts->params.mainLightIntensity > 0.0f)
						{
							float3 shadowOutputTemp = MainShadow(consts, &input2, calcParam);
							newColour += shadowOutputTemp * consts->params.mainLightColour
													 * consts->params.mainLightIntensity;
						}
					}

#ifdef AUX_LIGHTS
					if (i > 0)
					{
						__global sLightCl *light = &input->lights[i - 1];
						if (light->enabled)
						{
							float3 lightVectorTemp = light->position - point;
							float distanceLight = length(lightVectorTemp);
							float distanceLight2 = distanceLight * distanceLight;
							lightVectorTemp = normalize(lightVectorTemp);
							float lightShadow =
								AuxShadow(consts, &input2, distanceLight, lightVectorTemp, calcParam);
							float intensity = light->intensity * consts->params.iterFogBrightnessBoost;
							newColour += lightShadow * light->colour / distanceLight2 * intensity;
						}
					}
#endif // AUX_LIGHTS
				}

#ifdef AO_MODE_MULTIPLE_RAYS
				float3 AO = AmbientOcclusion(consts, &input2, calcParam);
				newColour += AO * consts->params.ambientOcclusion;
#endif // AO_MODE_MULTIPLE_RAYS

				if (opacity > 1.0f) opacity = 1.0f;

				output = output * (1.0f - opacity) + newColour * opacity * fogCol;
				totalOpacity = opacity + (1.0f - opacity) * totalOpacity;
				out4.s3 = opacity + (1.0f - opacity) * out4.s3;
			}
		}
#endif // ITER FOG

		if (totalOpacity > 1.0f) totalOpacity = 1.0f;
		if (out4.s3 > 1.0f) out4.s3 = 1.0f; // alpha channel

		*opacityOut = totalOpacity;

		if (end) break;
	}
#endif // not SIMPLE_GLOW

	out4.xyz = output;
	return out4;
}
