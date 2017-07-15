inline float3 CalculateViewVector(float2 normalizedPoint, float fov)
{
	float3 viewVector;

#if defined(PERSP_FISH_EYE) || defined(PERSP_FISH_EYE_CUT)
	float2 v = normalizedPoint * M_PI;
	float r = length(v);
	if (r == 0.0f)
	{
		viewVector = 0.0f;
	}
	else
	{
		viewVector.x = v.x / r * sin(r * fov);
		viewVector.z = v.y / r * sin(r * fov);
		viewVector.y = cos(r * fov);
	}
	viewVector = normalize(viewVector);
#endif

#ifdef PERSP_EQUIRECTANGULAR
	float2 v = normalizedPoint * M_PI;
	viewVector.x = sin(fov * v.x) * cos(fov * v.y);
	viewVector.z = sin(fov * v.y);
	viewVector.y = cos(fov * v.x) * cos(fov * v.y);
	viewVector = normalize(viewVector);
#endif

#ifdef PERSP_THREE_POINT
	viewVector.x = normalizedPoint.x * fov;
	viewVector.y = 1.0f;
	viewVector.z = normalizedPoint.y * fov;
	viewVector = normalize(viewVector);
#endif
	return viewVector;
}