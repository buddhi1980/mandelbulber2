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