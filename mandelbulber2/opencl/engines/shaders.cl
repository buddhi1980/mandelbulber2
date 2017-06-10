typedef struct
{
	float3 point;
	float3 viewVector;
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
		color = (color1 + (color2 * delta)).xyz;
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
		fout = Fractal(consts, input->point, calcParams);
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