#ifdef MONTE_CARLO_DOF

inline void MonteCarloDOF(float3 *startRay, float3 *viewVector, __constant sClInConstants *consts,
	matrix33 mRot, int *randomSeed)
{
#ifdef PERSP_THREE_POINT
	double randR = 0.0015f * consts->params.DOFRadius * consts->params.DOFFocus
								 * sqrt(Random(65536, randomSeed) / 65536.0f);
	float randAngle = Random(65536, randomSeed);
	float3 randVector = (float3){randR * sin(randAngle), 0.0f, randR * cos(randAngle)};
	float3 randVectorRot = Matrix33MulFloat3(mRot, randVector);
	float3 viewVectorTemp = *viewVector;
	viewVectorTemp -= randVectorRot / consts->params.DOFFocus;
	*viewVector = viewVectorTemp;
	*startRay = consts->params.camera + randVectorRot;
#else
	float3 viewVectorTemp = *viewVector;
	float randR = 0.0015 * consts->params.DOFRadius * consts->params.DOFFocus
								* sqrt(Random(65536, randomSeed) / 65536.0);
	float randAngle = Random(65536, randomSeed);
	float3 randVector = (float3){randR * sin(randAngle), 0.0f, randR * cos(randAngle)};

	float3 side = cross(viewVectorTemp, consts->params.topVector);
	side = normalize(side);
	float3 topTemp = cross(side, viewVectorTemp);
	topTemp = normalize(topTemp);
	float3 randVectorRot = side * randVector.x + topTemp * randVector.z;

	viewVectorTemp -= randVectorRot / consts->params.DOFFocus;
	*viewVector = viewVectorTemp;
	*startRay = consts->params.camera + randVectorRot;
#endif
}

#endif