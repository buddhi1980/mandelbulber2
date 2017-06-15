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
	bool invertMode;
	__global sMaterialCl *material;
	__global float4 *palette;
	int paletteSize;

} sShaderInputDataCl;

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

float4 SurfaceColor(
	__constant sClInConstants *consts, sShaderInputDataCl *input, sClCalcParams *calcParams)
{
	float4 out;
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

	out = (float4){color.x, color.y, color.z, 1.0f};

	return out;
}

float4 BackgroundShader(__constant sClInConstants *consts, sShaderInputDataCl *input)
{
	float4 pixel2;

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
	pixel2.s3 = 0.0f;

	return pixel2;
}

float4 MainShadow(
	__constant sClInConstants *consts, sShaderInputDataCl *input, sClCalcParams *calcParam)
{
	float4 shadow = (float4){1.0f, 1.0f, 1.0f, 1.0f};

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
	shadow.s3 = 0.0f;
	return shadow;
}