/*
 * displacement_map.cpp
 *
 *  Created on: 2 maj 2016
 *      Author: krzysztof
 */

#include "displacement_map.hpp"
#include "texture_mapping.hpp"

double DisplacementMap(double oldDistance, CVector3 point, int objectId, sRenderData *data)
{
	double distance = oldDistance;
	if (data)
	{
		const cMaterial *mat = &data->materials[data->objectData[objectId].materialId];
		if (mat->displacementTexture.IsLoaded())
		{
			CVector2<double> textureCoordinates;
			textureCoordinates = TextureMapping(point,
																					CVector3(0.0, 0.0, 1.0),
																					data->objectData[objectId],
																					mat) + CVector2<double>(0.5, 0.5);
			sRGBfloat bump3 = mat->displacementTexture.Pixel(textureCoordinates);
			double bump = bump3.R;
			distance -= bump * mat->displacementTextureHeight;
			if (distance < 0.0)
				distance = 0.0;
		}
	}
	return distance;
}


