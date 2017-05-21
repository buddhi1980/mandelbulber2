
#ifndef MANDELBULBER2_OPENCL_ENGINES_CALCULATE_DISTANCE_CL_
#define MANDELBULBER2_OPENCL_ENGINES_CALCULATE_DISTANCE_CL_

formulaOut CalculateDistance(
	__constant sClInConstants *consts, float3 point, sClCalcParams *calcParam)
{
	formulaOut out;
	out.z = 0.0f;
	out.iters = 0;
	out.distance = 0.0f;
	out.colourIndex = 0.0f;

#if defined(ANALYTIC_LOG_DE) || defined(ANALYTIC_LINEAR_DE) || defined(ANALYTIC_PSEUDO_KLEINIAN_DE)
	out = Fractal(consts, point, calcParam);

	if (out.iters == calcParam->N)
	{
		out.distance = 0.0f;
	}
	else
	{
		if (isinf(out.distance)) out.distance = 0.0f;
		if (out.distance < 0.0f) out.distance = 0.0f;
		if (out.distance > 10.0f) out.distance = 10.0f;
	}
#endif
	return out;
}

#endif // MANDELBULBER2_OPENCL_ENGINES_CALCULATE_DISTANCE_CL_