float3 StereoCalcEyePosition(
	float3 camera, float3 direction, float3 top, float distance, int eye, bool swapped)
{
	if (swapped) distance *= -1.0f;
	float3 eyePosition;
	float3 sideVector = normalize(cross(direction, top));
	if (eye == 0)
	{
		eyePosition = camera + sideVector * distance;
	}
	else
	{
		eyePosition = camera - sideVector * distance;
	}
	return eyePosition;
}

void StereoViewVectorCorrection(float correction, const matrix33 *mRot, const matrix33 *mRotInv,
	int eye, bool swapped, float3 *viewVector)
{
	if (swapped) correction *= -1.0f;
	float3 viewVectorTemp = *viewVector;

#ifdef PERSP_THREE_POINT

	viewVectorTemp = Matrix33MulFloat3(*mRotInv, viewVectorTemp);
	float rxz2 = viewVectorTemp.x * viewVectorTemp.x + viewVectorTemp.z * viewVectorTemp.z;
	float3 viewVectorCorrection = 0.0f;
	if (rxz2 < 1.0f)
	{
		viewVectorCorrection.x = correction / 10.0f * sqrt(1.0f - rxz2);
	}

	if (eye == 0)
	{
		viewVectorTemp -= viewVectorCorrection;
		viewVectorTemp = normalize(viewVectorTemp);
		viewVectorTemp = Matrix33MulFloat3(*mRot, viewVectorTemp);
		*viewVector = viewVectorTemp;
	}
	else
	{
		viewVectorTemp += viewVectorCorrection;
		viewVectorTemp = normalize(viewVectorTemp);
		viewVectorTemp = Matrix33MulFloat3(*mRot, viewVectorTemp);
		*viewVector = viewVectorTemp;
	}

#else	// not PERSP_THREE_POINT

	viewVectorTemp = Matrix33MulFloat3(*mRotInv, viewVectorTemp);
	if (eye == 0)
	{
		viewVectorTemp = RotateAroundVectorByAngle(
			viewVectorTemp, (float3){0.0f, 0.0f, 1.0f}, correction / 10.0f / M_PI);
		viewVectorTemp = Matrix33MulFloat3(*mRot, viewVectorTemp);
		*viewVector = viewVectorTemp;
	}
	else
	{
		viewVectorTemp = RotateAroundVectorByAngle(
			viewVectorTemp, (float3){0.0f, 0.0f, 1.0f}, -correction / 10.0f / M_PI);
		viewVectorTemp = Matrix33MulFloat3(*mRot, viewVectorTemp);
		*viewVector = viewVectorTemp;
	}
#endif // PERSP_THREE_POINT
}