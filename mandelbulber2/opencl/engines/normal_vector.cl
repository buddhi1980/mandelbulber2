
float3 NormalVector(__constant sClInConstants *consts, float3 point, float mainDistance,
	float distThresh, sClCalcParams *calcParam)
{
#ifndef SLOW_SHADING
	float delta = distThresh;
	calcParam->distThresh = distThresh;
	float s1 = CalculateDistance(consts, point + (float3){delta, 0.0f, 0.0f}, calcParam).distance;
	float s2 = CalculateDistance(consts, point + (float3){0.0f, delta, 0.0f}, calcParam).distance;
	float s3 = CalculateDistance(consts, point + (float3){0.0f, 0.0f, delta}, calcParam).distance;
	float3 normal = (float3){s1 - mainDistance, s2 - mainDistance, s3 - mainDistance};
	normal = normalize(normal);
	return normal;
#else

	// calculating normal vector based on average value of binary central difference

	float3 point2;
	float3 point3;
	float delta = distThresh * consts->params.smoothness * 0.5f;

	calcParam->distThresh = distThresh;

	float3 normal = 0.0f;
	for (point2.x = -1.0; point2.x <= 1.0; point2.x += 0.2) //+0.2
	{
		for (point2.y = -1.0; point2.y <= 1.0; point2.y += 0.2)
		{
			for (point2.z = -1.0; point2.z <= 1.0; point2.z += 0.2)
			{
				point3 = point + point2 * delta;
				float dist = CalculateDistance(consts, point3, calcParam).distance;
				normal += point2 * dist;
			}
		}
	}

	normal = normalize(normal);
	return normal;
#endif

	// TODO add brute force calculation of normal vector
}
