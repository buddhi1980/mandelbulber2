/*
 * light.h
 *
 *  Created on: 22 gru 2020
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_LIGHT_H_
#define MANDELBULBER2_SRC_LIGHT_H_

#include <memory>
#include <QString>

#include "algebra.hpp"
#include "color_structures.hpp"
#include "texture.hpp"

// forward declarations
class cParameterContainer;

class cLight
{
public:
	cLight();
	cLight(int _id, const std::shared_ptr<cParameterContainer> lightParam, bool loadTextures,
		bool quiet, bool useNetRender);
	~cLight();

	void setParameters(int _id, const std::shared_ptr<cParameterContainer> lightParam,
		bool loadTextures, bool quiet, bool useNetRender);

	static QString Name(const QString &name, int materialId)
	{
		return QString("light%1_").arg(materialId) + name;
	}

	static QStringList paramsList;

	enum enumLightType
	{
		lightGlobal = 0,
		lightSpot = 1,
		lightConical = 2,
		lightProjection = 3
	};

	enum enumLightDecayFunction
	{
		lightDecay1R = 0,
		lightDecay1R2 = 1,
		lightDecay1R3 = 2,
	};

	bool castShadows = false;
	bool penetrating = false;
	bool relativePosition = false;
	bool volumetric = false;

	int id = -1;

	float coneAngle = 0.0f;
	float coneSoftAngle = 0.0f;
	float intensity = 0.0f;
	float visibility = 0.0f;
	float volumetricVisibility = 0.0f;
	float size = 0.0f;
	float softShadowCone = 0.0f;

	CVector3 position;
	CVector3 rotation;

	sRGBFloat color;

	CRotationMatrix rotMatrix;

	enumLightType type = lightGlobal;
	enumLightDecayFunction decayFunction = lightDecay1R;

	cTexture colorTexture;
};

#endif /* MANDELBULBER2_SRC_LIGHT_H_ */
