/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * contains texture mapping fuction
 * used in the full_engine_cl
 */

#ifdef USE_TEXTURES
float2 TextureMapping(float3 inPoint, float3 normalVector, __global sObjectDataCl *objectData,
	__global sMaterialCl *material, float3 *textureVectorX, float3 *textureVectorY)
{
	float2 textureCoordinates;
	float3 point = inPoint - objectData->position;
	point = Matrix33MulFloat3(objectData->rotationMatrix, point);
	point /= objectData->size;
	point = Matrix33MulFloat3(material->rotMatrix, point);
	normalVector = Matrix33MulFloat3(objectData->rotationMatrix, normalVector);

	switch (material->textureMappingType)
	{

		case mappingPlanar: {
#ifdef USE_PLANAR_MAPPING
			textureCoordinates = (float2){point.x, point.y};
			textureCoordinates.x /= -material->textureScale.x;
			textureCoordinates.y /= material->textureScale.y;
			textureCoordinates.x -= material->textureCenter.x;
			textureCoordinates.y -= material->textureCenter.y;

#ifdef USE_NORMAL_MAP_TEXTURE
			if (textureVectorX && textureVectorY)
			{
				float3 texX = (float3){1.0f, 0.0f, 0.0f};
				texX = Matrix33MulFloat3(TransposeMatrix(objectData->rotationMatrix), texX);
				texX = Matrix33MulFloat3(TransposeMatrix(material->rotMatrix), texX);
				*textureVectorX = texX;

				float3 texY = (float3){0.0f, -1.0f, 0.0f};
				texY = Matrix33MulFloat3(TransposeMatrix(objectData->rotationMatrix), texY);
				texY = Matrix33MulFloat3(TransposeMatrix(material->rotMatrix), texY);
				*textureVectorY = texY;
			}
#endif
#endif
			break;
		}

		case mappingCylindrical: {
#ifdef USE_CYLINDRICAL_MAPPING
			float alphaTexture = fmod(GetAlpha(point) + 2.0f * M_PI_F, 2.0f * M_PI_F);
			textureCoordinates.x = alphaTexture / (2.0f * M_PI_F);
			textureCoordinates.y = -point.z;
			textureCoordinates.x /= material->textureScale.x;
			textureCoordinates.y /= material->textureScale.y;
			textureCoordinates.x -= material->textureCenter.x;
			textureCoordinates.y -= material->textureCenter.y;

#ifdef USE_NORMAL_MAP_TEXTURE
			if (textureVectorX && textureVectorY)
			{
				float3 texY = (float3){0.0f, 0.0f, 1.0f};
				float3 texX = cross(point, texY);
				texX = Matrix33MulFloat3(TransposeMatrix(objectData->rotationMatrix), texX);
				texX = Matrix33MulFloat3(TransposeMatrix(material->rotMatrix), texX);
				*textureVectorX = texX;
				texY = Matrix33MulFloat3(TransposeMatrix(objectData->rotationMatrix), texY);
				texY = Matrix33MulFloat3(TransposeMatrix(material->rotMatrix), texY);
				*textureVectorY = texY;
			}
#endif
#endif
			break;
		}

		case mappingSpherical: {
#ifdef USE_SPHERICAL_MAPPING
			float alphaTexture = fmod(GetAlpha(point) + 2.0f * M_PI_F, 2.0f * M_PI_F);
			float betaTexture = -GetBeta(point);
			textureCoordinates.x = alphaTexture / (2.0f * M_PI_F);
			textureCoordinates.y = betaTexture / M_PI_F;
			textureCoordinates.x /= material->textureScale.x;
			textureCoordinates.y /= material->textureScale.y;
			textureCoordinates.x -= material->textureCenter.x;
			textureCoordinates.y -= material->textureCenter.y;

#ifdef USE_NORMAL_MAP_TEXTURE
			if (textureVectorX && textureVectorY)
			{
				float3 texY = (float3){0.0f, 0.0f, -1.0f};
				float3 texX = cross(texY, point);
				texX = normalize(texX);
				texY = cross(texX, point);

				texX = Matrix33MulFloat3(TransposeMatrix(objectData->rotationMatrix), texX);
				texX = Matrix33MulFloat3(TransposeMatrix(material->rotMatrix), texX);
				*textureVectorX = texX;
				texY = Matrix33MulFloat3(TransposeMatrix(objectData->rotationMatrix), texY);
				texY = Matrix33MulFloat3(TransposeMatrix(material->rotMatrix), texY);
				*textureVectorY = texY;
			}
#endif
#endif
			break;
		}

		case mappingCubic: {
#ifdef USE_CUBIC_MAPPING
			point /= material->textureScale;
			point -= material->textureCenter;

			float3 texX, texY;
			if (fabs(normalVector.x) > fabs(normalVector.y))
			{
				if (fabs(normalVector.x) > fabs(normalVector.z))
				{
					// x
					if (normalVector.x > 0.0f)
						textureCoordinates = (float2){point.y, -point.z};
					else
						textureCoordinates = (float2){-point.y, -point.z};

#ifdef USE_NORMAL_MAP_TEXTURE
					if (textureVectorX && textureVectorY)
					{
						if (normalVector.x > 0.0f)
						{
							texX = (float3){0.0f, -1.0f, 0.0f};
							texY = (float3){0.0f, 0.0f, 1.0f};
						}
						else
						{
							texX = (float3)(0.0f, 1.0f, 0.0f);
							texY = (float3)(0.0f, 0.0f, 1.0f);
						}
					}
#endif
				}
				else
				{
					// z
					if (normalVector.z > 0.0f)
						textureCoordinates = (float2){-point.x, point.y};
					else
						textureCoordinates = (float2){point.x, point.y};

#ifdef USE_NORMAL_MAP_TEXTURE
					if (textureVectorX && textureVectorY)
					{
						if (normalVector.z > 0.0f)
						{
							texX = (float3){1.0f, 0.0f, 0.0f};
							texY = (float3){0.0f, -1.0f, 0.0f};
						}
						else
						{
							texX = (float3){-1.0f, 0.0f, 0.0f};
							texY = (float3){0.0f, -1.0f, 0.0f};
						}
					}
#endif
				}
			}
			else
			{
				if (fabs(normalVector.y) > fabs(normalVector.z))
				{
					// y
					if (normalVector.y > 0.0f)
						textureCoordinates = (float2){-point.x, -point.z};
					else
						textureCoordinates = (float2){point.x, -point.z};

#ifdef USE_NORMAL_MAP_TEXTURE
					if (textureVectorX && textureVectorY)
					{
						if (normalVector.y > 0.0f)
						{
							texX = (float3){1.0f, 0.0f, 0.0f};
							texY = (float3){0.0f, 0.0f, 1.0f};
						}
						else
						{
							texX = (float3){-1.0f, 0.0f, 0.0f};
							texY = (float3){0.0f, 0.0f, 1.0f};
						}
					}
#endif
				}
				else
				{
					// z
					if (normalVector.z > 0.0f)
						textureCoordinates = (float2){-point.x, point.y};
					else
						textureCoordinates = (float2){point.x, point.y};

#ifdef USE_NORMAL_MAP_TEXTURE
					if (textureVectorX && textureVectorY)
					{
						if (normalVector.z > 0.0f)
						{
							texX = (float3){1.0f, 0.0f, 0.0f};
							texY = (float3){0.0f, -1.0f, 0.0f};
						}
						else
						{
							texX = (float3){-1.0f, 0.0f, 0.0f};
							texY = (float3){0.0f, -1.0f, 0.0f};
						}
					}
#endif
				}
			}

#ifdef USE_NORMAL_MAP_TEXTURE
			if (textureVectorX && textureVectorY)
			{
				texX = Matrix33MulFloat3(TransposeMatrix(objectData->rotationMatrix), texX);
				texX = Matrix33MulFloat3(TransposeMatrix(material->rotMatrix), texX);
				*textureVectorX = texX;
				texY = Matrix33MulFloat3(TransposeMatrix(objectData->rotationMatrix), texY);
				texY = Matrix33MulFloat3(TransposeMatrix(material->rotMatrix), texY);
				*textureVectorY = texY;
			}
#endif
#endif
			break;
		}
	}
	return textureCoordinates;
}
#endif // USE_TEXTURES
