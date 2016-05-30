/*
 * texture_mapping.cpp
 *
 *  Created on: 2 maj 2016
 *      Author: krzysztof
 */

#include "texture_mapping.hpp"

CVector2<double> TextureMapping(CVector3 inPoint, CVector3 normalVector,
		const cObjectData &objectData, const cMaterial *material,
		CVector3 *textureVectorX, CVector3 *textureVectorY)
{
	CVector2<double> textureCoordinates;
	CVector3 point = inPoint - objectData.position;
	point = objectData.rotationMatrix.RotateVector(point);
	point *= objectData.size;
	point -= material->textureCenter;
	point = material->rotMatrix.RotateVector(point);

	switch(material->textureMappingType)
	{
		case cMaterial::mappingPlanar:
		{
			textureCoordinates = CVector2<double>(point.x, point.y);
			textureCoordinates.x /= -material->textureScale.x;
			textureCoordinates.y /= material->textureScale.y;

			if(textureVectorX && textureVectorY)
			{
				CVector3 texX(1.0, 0.0, 0.0);
				texX = objectData.rotationMatrix.Transpose().RotateVector(texX);
				texX = material->rotMatrix.Transpose().RotateVector(texX);
				*textureVectorX = texX;

				CVector3 texY(0.0, -1.0, 0.0);
				texY = objectData.rotationMatrix.Transpose().RotateVector(texY);
				texY = material->rotMatrix.Transpose().RotateVector(texY);
				*textureVectorY = texY;
			}
			break;
		}
		case cMaterial::mappingCylindrical:
		{
			double alphaTexture = fmod(point.GetAlpha() + 2.0 * M_PI, 2.0 * M_PI);
			textureCoordinates.x = alphaTexture / (2.0 * M_PI);
			textureCoordinates.y = -point.z;
			textureCoordinates.x /= material->textureScale.x;
			textureCoordinates.y /= material->textureScale.y;

			if(textureVectorX && textureVectorY)
			{
				CVector3 texY(0.0, 0.0, 1.0);
				CVector3 texX = point.Cross(texY);
				texX = objectData.rotationMatrix.Transpose().RotateVector(texX);
				texX = material->rotMatrix.Transpose().RotateVector(texX);
				*textureVectorX = texX;
				texY = objectData.rotationMatrix.Transpose().RotateVector(texY);
				texY = material->rotMatrix.Transpose().RotateVector(texY);
				*textureVectorY = texY;
			}

			break;
		}
		case cMaterial::mappingSpherical:
		{
			double alphaTexture = fmod(point.GetAlpha() + 2.0 * M_PI, 2.0 * M_PI);
			double betaTexture = -point.GetBeta();
			textureCoordinates.x = alphaTexture / (2.0 * M_PI);
			textureCoordinates.y = (betaTexture / M_PI);
			textureCoordinates.x /= material->textureScale.x;
			textureCoordinates.y /= material->textureScale.y;

			CVector3 texY(0.0, 0.0, -1.0);
			CVector3 texX = texY.Cross(point);
			texX.Normalize();
			texY = texX.Cross(point);

			if(textureVectorX && textureVectorY)
			{
				texX = objectData.rotationMatrix.Transpose().RotateVector(texX);
				texX = material->rotMatrix.Transpose().RotateVector(texX);
				*textureVectorX = texX;
				texY = objectData.rotationMatrix.Transpose().RotateVector(texY);
				texY = material->rotMatrix.Transpose().RotateVector(texY);
				*textureVectorY = texY;
			}

			break;
		}
		case cMaterial::mappingCubic:
		{
			point /= material->textureScale;
			CVector3 texX, texY;
			if(fabs(normalVector.x) > fabs(normalVector.y))
			{
				if(fabs(normalVector.x) > fabs(normalVector.z))
				{
					//x
					if(normalVector.x > 0)
						textureCoordinates = CVector2<double>(point.y, -point.z);
					else
						textureCoordinates = CVector2<double>(-point.y, -point.z);

					if(textureVectorX && textureVectorY)
					{
						if(normalVector.x > 0)
						{
							texX = CVector3(0.0, -1.0, 0.0);
							texY = CVector3(0.0, 0.0, 1.0);
						}
						else
						{
							texX = CVector3(0.0, 1.0, 0.0);
							texY = CVector3(0.0, 0.0, 1.0);
						}
					}
				}
				else
				{
					//z
					if(normalVector.z > 0)
						textureCoordinates = CVector2<double>(-point.x, point.y);
					else
						textureCoordinates = CVector2<double>(point.x, point.y);

					if(textureVectorX && textureVectorY)
					{
						if(normalVector.z > 0)
						{
							texX = CVector3(1.0, 0.0, 0.0);
							texY = CVector3(0.0, -1.0, 0.0);
						}
						else
						{
							texX = CVector3(-1.0, 0.0, 0.0);
							texY = CVector3(0.0, -1.0, 0.0);
						}
					}
				}
			}
			else
			{
				if(fabs(normalVector.y) > fabs(normalVector.z))
				{
					//y
					if(normalVector.y > 0)
						textureCoordinates = CVector2<double>(-point.x, -point.z);
					else
						textureCoordinates = CVector2<double>(point.x, -point.z);

					if(textureVectorX && textureVectorY)
					{
						if(normalVector.y > 0)
						{
							texX = CVector3(1.0, 0.0, 0.0);
							texY = CVector3(0.0, 0.0, 1.0);
						}
						else
						{
							texX = CVector3(-1.0, 0.0, 0.0);
							texY = CVector3(0.0, 0.0, 1.0);
						}
					}
				}
				else
				{
					//z
					if(normalVector.z > 0)
						textureCoordinates = CVector2<double>(-point.x, point.y);
					else
						textureCoordinates = CVector2<double>(point.x, point.y);

					if(textureVectorX && textureVectorY)
					{
						if(normalVector.z > 0)
						{
							texX = CVector3(1.0, 0.0, 0.0);
							texY = CVector3(0.0, -1.0, 0.0);
						}
						else
						{
							texX = CVector3(-1.0, 0.0, 0.0);
							texY = CVector3(0.0, -1.0, 0.0);
						}
					}
				}
			}

			if(textureVectorX && textureVectorY)
			{
				texX = objectData.rotationMatrix.Transpose().RotateVector(texX);
				texX = material->rotMatrix.Transpose().RotateVector(texX);
				*textureVectorX = texX;
				texY = objectData.rotationMatrix.Transpose().RotateVector(texY);
				texY = material->rotMatrix.Transpose().RotateVector(texY);
				*textureVectorY = texY;
			}

			break;
		}

	}
	return textureCoordinates;
}
