
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

#ifdef ANALYTIC_DE
	out = Fractal(consts, point, calcParam);

	if (out.iters == calcParam->N)
	{
		out.distance = 0.0f;
	}
	else
	{
		if (isinf(out.distance)) out.distance = 0.0f;
		if (out.distance < 0.0f) out.distance = 0.0f;
		if (out.distance > 1.0f) out.distance = 1.0f;
	}

#elif DELTA_DE
	// float delta = length(point)*consts->fractal.deltaDEStep;
	float delta = 1.0e-5;
	float3 dr = 0.0;

	out = Fractal(consts, point, calcParam);

	if (out.iters == calcParam->N)
	{
		out.distance = 0.0f;
	}
	else
	{
		float r = length(out.z);
		float r11 = length(Fractal(consts, point + (float3){delta, 0.0, 0.0}, calcParam).z);
		float r12 = length(Fractal(consts, point + (float3){-delta, 0.0, 0.0}, calcParam).z);
		dr.x = min(fabs(r11 - r), fabs(r12 - r)) / delta;
		float r21 = length(Fractal(consts, point + (float3){0.0, delta, 0.0}, calcParam).z);
		float r22 = length(Fractal(consts, point + (float3){0.0, -delta, 0.0}, calcParam).z);
		dr.y = min(fabs(r21 - r), fabs(r22 - r)) / delta;
		float r31 = length(Fractal(consts, point + (float3){0.0, 0.0, delta}, calcParam).z);
		float r32 = length(Fractal(consts, point + (float3){0.0, 0.0, -delta}, calcParam).z);
		dr.z = min(fabs(r31 - r), fabs(r32 - r)) / delta;
		float d = length(dr);

		if (isinf(r) || isinf(d))
		{
			out.distance = 0.0f;
		}
		else
		{
#ifdef DELTA_LINEAR_DE
			out.distance = 0.5 * r / d;
#elif DELTA_LOG_DE
			out.distance = 0.5 * r * native_log(r) / d;
#elif DELTA_PSEUDO_KLEINIAN_DE
			out.distance = .... // TODO to be written later
#endif
		}

		if (out.distance < 0.0f) out.distance = 0.0f;
		if (out.distance > 1.0f) out.distance = 1.0f;
	}

#endif

	return out;
}

#endif // MANDELBULBER2_OPENCL_ENGINES_CALCULATE_DISTANCE_CL_