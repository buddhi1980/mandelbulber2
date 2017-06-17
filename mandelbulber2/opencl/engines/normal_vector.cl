
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

	// TODO add brute force calculation of normal vector
}
