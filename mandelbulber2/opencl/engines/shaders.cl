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
	bool invertMode;
	__global sMaterialCl *material;
	__global float4 *palette;
	__global sVectorsAroundCl *AOVectors;
	__global sLightCl *lights;
	int paletteSize;

} sShaderInputDataCl;

//-------------- background shaders ---------------

float3 BackgroundShader(__constant sClInConstants *consts, sShaderInputDataCl *input)
{
	float3 pixel2;

	float3 vector = (float3){0.0, 0.0, 1.0f};
	vector = normalize(vector);
	float3 viewVectorNorm = input->viewVector;
	viewVectorNorm = normalize(viewVectorNorm);
	float grad = dot(viewVectorNorm, vector) + 1.0f;
	int3 pixel;
	if (grad < 1.0f)
	{
		float gradN = 1.0f - grad;
		pixel.s0 =
			consts->params.background_color3.s0 * gradN + consts->params.background_color2.s0 * grad;
		pixel.s1 =
			consts->params.background_color3.s1 * gradN + consts->params.background_color2.s1 * grad;
		pixel.s2 =
			consts->params.background_color3.s2 * gradN + consts->params.background_color2.s2 * grad;
	}
	else
	{
		grad = grad - 1.0f;
		float gradN = 1.0f - grad;
		pixel.s0 =
			consts->params.background_color2.s0 * gradN + consts->params.background_color1.s0 * grad;
		pixel.s1 =
			consts->params.background_color2.s1 * gradN + consts->params.background_color1.s1 * grad;
		pixel.s2 =
			consts->params.background_color2.s2 * gradN + consts->params.background_color1.s2 * grad;
	}

	pixel2.s0 = pixel.s0 / 65536.0f;
	pixel2.s1 = pixel.s1 / 65536.0f;
	pixel2.s2 = pixel.s2 / 65536.0f;

	return pixel2;
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
		fout = Fractal(consts, input->point, calcParams, calcModeColouring);
		int nCol = floor(fout.colorIndex);
		nCol = abs(nCol) % (248 * 256);
		int color_number =
			(int)(nCol * input->material->coloring_speed + 256 * input->material->paletteOffset) % 65536;

		color = IndexToColour(color_number, input);
	}
	else
	{
		color.x = input->material->color.x / 65536.0f;
		color.y = input->material->color.y / 65536.0f;
		color.z = input->material->color.z / 65536.0f;
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
	out.s0 *= input->material->specularColor.s0 / 65536.0f;
	out.s1 *= input->material->specularColor.s1 / 65536.0f;
	out.s2 *= input->material->specularColor.s2 / 65536.0f;
	return out;
}

#ifdef SHADOWS
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

	// double start = input->delta;
	float start = input->distThresh;
	if (consts->params.interiorMode) start = input->distThresh * DEFactor;

	float opacity;
	float shadowTemp = 1.0f;

	float softRange = tan(consts->params.shadowConeAngle / 180.0f * M_PI_F);
	float maxSoft = 0.0f;

	const bool bSoft = !consts->params.iterFogEnabled && !consts->params.limitsEnabled
										 && !consts->params.common.iterThreshMode && softRange > 0.0f;

	for (float i = start; i < factor; i += dist * DEFactor)
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

		if (consts->params.iterFogEnabled)
		{
			//			opacity = IterOpacity(dist * DEFactor, distanceOut.iters, consts->params.N,
			//				consts->params.iterFogOpacityTrim, consts->params.iterFogOpacity);
		}
		else
		{
			opacity = 0.0f;
		}
		shadowTemp -= opacity * (factor - i) / factor;

		if (dist < dist_thresh || shadowTemp < 0.0f)
		{
			shadowTemp -= (factor - i) / factor;
			if (!consts->params.penetratingLights) shadowTemp = 0.0f;
			if (shadowTemp < 0.0f) shadowTemp = 0.0f;
			break;
		}
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
		aoTemp += 1.0 / pow(2.0, i) * (scan - consts->params.ambientOcclusionFastTune * dist)
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

	for (int i = 0; i < input->AOVectorsCount; i++)
	{
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

			if (consts->params.iterFogEnabled)
			{
				//				opacity = IterOpacity(dist * 2.0, distanceOut.iters, consts->params.N,
				// consts->params.iterFogOpacityTrim,
				//					consts->params.iterFogOpacity);
			}
			else
			{
				opacity = 0.0f;
			}
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

		AO.s0 += intense * v.color.s0;
		AO.s1 += intense * v.color.s1;
		AO.s2 += intense * v.color.s2;
	}
	AO /= input->AOVectorsCount * 65536.0f;

	return AO;
}
#endif

#ifdef AUX_LIGHTS
#ifdef SHADOWS
float AuxShadow(constant sClInConstants *consts, sShaderInputDataCl *input, float distance,
	float3 lightVector, sClCalcParams *calcParam)
{
	// float step = input.delta;
	float dist;
	float light;

	float opacity;
	float shadowTemp = 1.0f;

	float DE_factor = consts->params.DEFactor;
	if (consts->params.iterFogEnabled || consts->params.volumetricLightAnyEnabled) DE_factor = 1.0;

	for (float i = input->delta; i < distance; i += dist * DE_factor)
	{
		float3 point2 = input->point + lightVector * i;

		calcParam->distThresh = input->distThresh;
		formulaOut outF;

		outF = CalculateDistance(consts, point2, calcParam);
		dist = outF.distance;

		if (consts->params.iterFogEnabled)
		{
			//			opacity = IterOpacity(dist * DE_factor, distanceOut.iters, consts->params.N,
			//				consts->params.iterFogOpacityTrim, consts->params.iterFogOpacity);
		}
		else
		{
			opacity = 0.0f;
		}
		shadowTemp -= opacity * (distance - i) / distance;

		float dist_thresh;
		if (consts->params.iterFogEnabled || consts->params.volumetricLightAnyEnabled)
		{
			dist_thresh = CalcDistThresh(point2, consts);
		}
		else
			dist_thresh = input->distThresh;

		if (dist < dist_thresh || shadowTemp < 0.0f)
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
	}
	light = shadowTemp;
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
			shade = 0.0;
			shade2 = 0.0;
		}
	}

	shading.s0 = shade * light->colour.s0 / 65536.0f;
	shading.s1 = shade * light->colour.s1 / 65536.0f;
	shading.s2 = shade * light->colour.s2 / 65536.0f;

	float3 specular;
	specular.s0 = shade2 * light->colour.s0 / 65536.0f;
	specular.s1 = shade2 * light->colour.s1 / 65536.0f;
	specular.s2 = shade2 * light->colour.s2 / 65536.0f;

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

float3 ObjectShader(__constant sClInConstants *consts, sShaderInputDataCl *input,
	sClCalcParams *calcParam, float3 *outSurfaceColor, float3 *outSpecular)
{
	float3 color = 0.7f;

	float3 normal = NormalVector(consts, input->point, input->lastDist, input->distThresh, calcParam);
	input->normal = normal;

	float3 mainLight =
		(float3){consts->params.mainLightColour.s0 / 65536.0f,
			consts->params.mainLightColour.s1 / 65536.0f, consts->params.mainLightColour.s2 / 65536.0f}
		* consts->params.mainLightIntensity;

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

	color =
		surfaceColor * (mainLight * shadow * shade + auxLights + AO) + specular * shadow + auxSpecular;
	*outSurfaceColor = surfaceColor;
	*outSpecular = specular;

	return color;
}

float4 VolumetricShader(__constant sClInConstants *consts, sShaderInputDataCl *input,
	sClCalcParams *calcParam, float4 oldPixel, float *opacityOut)
{
	float4 out4 = oldPixel;
	float3 output = oldPixel.xyz;
	float totalOpacity = 0.0f;

	// volumetric fog init
	float colourThresh = consts->params.volFogColour1Distance;
	float colourThresh2 = consts->params.volFogColour2Distance;
	float fogReduce = consts->params.volFogDistanceFactor;
	float fogIntensity = consts->params.volFogDensity;

	// visible lights init
	int numberOfLights = input->numberOfLights;
	if (numberOfLights < 4) numberOfLights = 4;

#ifdef GLOW
	// glow init
	float glow = input->stepCount * consts->params.glowIntensity / 512.0f * consts->params.DEFactor;
	float glowN = 1.0f - glow;
	if (glowN < 0.0f) glowN = 0.0f;

	float3 glowColor;

	glowColor.s0 =
		(glowN * consts->params.glowColor1.s0 + consts->params.glowColor2.s0 * glow) / 65536.0f;
	glowColor.s1 =
		(glowN * consts->params.glowColor1.s1 + consts->params.glowColor2.s1 * glow) / 65536.0f;
	glowColor.s2 =
		(glowN * consts->params.glowColor1.s2 + consts->params.glowColor2.s2 * glow) / 65536.0f;
#endif // GLOW

#ifdef SIMPLE_GLOW // only simple glow, no another shaders
#ifdef GLOW
	glow *= 0.7;
	float glowOpacity = 1.0f * glow;
	if (glowOpacity > 1.0f) glowOpacity = 1.0f;
	output = glow * glowColor + (1.0f - glowOpacity) * output;
	out4.s3 += glowOpacity;
#endif // GLOW

#else // not SIMPLE_GLOW
	float totalStep = 0.0;
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

		float step = distance * consts->params.DEFactor;
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
			float glowOpacity = glow / input->stepCount;
			if (glowOpacity > 1.0f) glowOpacity = 1.0f;

			output = glowOpacity * glowColor + (1.0f - glowOpacity) * output;
			out4.s3 += glowOpacity;
		}
#endif // GLOW

//----------------------- basic fog
#ifdef BASIC_FOG
		{
			float fogDensity = step / consts->params.fogVisibility;
			if (fogDensity > 1.0f) fogDensity = 1.0f;

			output.s0 =
				fogDensity * consts->params.fogColor.s0 / 65536.0f + (1.0f - fogDensity) * output.s0;
			output.s1 =
				fogDensity * consts->params.fogColor.s1 / 65536.0f + (1.0f - fogDensity) * output.s1;
			output.s2 =
				fogDensity * consts->params.fogColor.s2 / 65536.0f + (1.0f - fogDensity) * output.s2;

			totalOpacity = fogDensity + (1.0f - fogDensity) * totalOpacity;
			out4.s3 = fogDensity + (1.0f - fogDensity) * out4.s3;
		}
#endif // BASIC_FOG

		if (totalOpacity > 1.0f) totalOpacity = 1.0f;
		if (out4.s3 > 1.0f) out4.s3 = 1.0f; // alpha channel

		*opacityOut = totalOpacity;

		if (end) break;
	}
#endif // not SIMPLE_GLOW

	out4.xyz = output;
	return out4;
}