/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * global method TextureMapping() - coordinate transformer for textures
 *
 * For the given parameters TextureMapping(...) determines the point x-y
 * on the texture to use for processing
 */

#include "texture_mapping.hpp"

#include "material.h"
#include "object_data.hpp"
#include "texture_enums.hpp"

CVector2<double> TextureMapping(CVector3 inPoint, CVector3 normalVector,
	const cObjectData &objectData, const cMaterial *material, CVector3 *textureVectorX,
	CVector3 *textureVectorY)
{
	CVector2<double> textureCoordinates;
	CVector3 point = inPoint - objectData.position;
	point = objectData.rotationMatrix.RotateVector(point);
	point /= objectData.size;
	point = material->rotMatrix.RotateVector(point);
	normalVector = objectData.rotationMatrix.RotateVector(normalVector);

	switch (material->textureMappingType)
	{
		case texture::mappingPlanar:
		{
			textureCoordinates = CVector2<double>(point.x, point.y);
			textureCoordinates.x /= -material->textureScale.x;
			textureCoordinates.y /= material->textureScale.y;
			textureCoordinates.x -= material->textureCenter.x;
			textureCoordinates.y -= material->textureCenter.y;

			if (textureVectorX && textureVectorY)
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
		case texture::mappingCylindrical:
		{
			double alphaTexture = fmod(point.GetAlpha() + 2.0 * M_PI, 2.0 * M_PI);
			textureCoordinates.x = alphaTexture / (2.0 * M_PI);
			textureCoordinates.y = -point.z;
			textureCoordinates.x /= material->textureScale.x;
			textureCoordinates.y /= material->textureScale.y;
			textureCoordinates.x -= material->textureCenter.x;
			textureCoordinates.y -= material->textureCenter.y;

			if (textureVectorX && textureVectorY)
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
		case texture::mappingSpherical:
		{
			double alphaTexture = fmod(point.GetAlpha() + 2.0 * M_PI, 2.0 * M_PI);
			double betaTexture = -point.GetBeta();
			textureCoordinates.x = alphaTexture / (2.0 * M_PI);
			textureCoordinates.y = betaTexture / M_PI;
			textureCoordinates.x /= material->textureScale.x;
			textureCoordinates.y /= material->textureScale.y;
			textureCoordinates.x -= material->textureCenter.x;
			textureCoordinates.y -= material->textureCenter.y;

			if (textureVectorX && textureVectorY)
			{
				CVector3 texY(0.0, 0.0, -1.0);
				CVector3 texX = texY.Cross(point);
				texX.Normalize();
				texY = texX.Cross(point);

				texX = objectData.rotationMatrix.Transpose().RotateVector(texX);
				texX = material->rotMatrix.Transpose().RotateVector(texX);
				*textureVectorX = texX;
				texY = objectData.rotationMatrix.Transpose().RotateVector(texY);
				texY = material->rotMatrix.Transpose().RotateVector(texY);
				*textureVectorY = texY;
			}

			break;
		}
		case texture::mappingCubic:
		{
			point /= material->textureScale;
			point -= material->textureCenter;

			CVector3 texX, texY;
			if (fabs(normalVector.x) > fabs(normalVector.y))
			{
				if (fabs(normalVector.x) > fabs(normalVector.z))
				{
					// x
					if (normalVector.x > 0)
						textureCoordinates = CVector2<double>(point.y, -point.z);
					else
						textureCoordinates = CVector2<double>(-point.y, -point.z);

					if (textureVectorX && textureVectorY)
					{
						if (normalVector.x > 0)
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
					// z
					if (normalVector.z > 0)
						textureCoordinates = CVector2<double>(-point.x, point.y);
					else
						textureCoordinates = CVector2<double>(point.x, point.y);

					if (textureVectorX && textureVectorY)
					{
						if (normalVector.z > 0)
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
				if (fabs(normalVector.y) > fabs(normalVector.z))
				{
					// y
					if (normalVector.y > 0)
						textureCoordinates = CVector2<double>(-point.x, -point.z);
					else
						textureCoordinates = CVector2<double>(point.x, -point.z);

					if (textureVectorX && textureVectorY)
					{
						if (normalVector.y > 0)
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
					// z
					if (normalVector.z > 0)
						textureCoordinates = CVector2<double>(-point.x, point.y);
					else
						textureCoordinates = CVector2<double>(point.x, point.y);

					if (textureVectorX && textureVectorY)
					{
						if (normalVector.z > 0)
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

			if (textureVectorX && textureVectorY)
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
