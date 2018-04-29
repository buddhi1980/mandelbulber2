/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * opencl dynamic data for transferring to the worker device
 */

#include "opencl_dynamic_data.hpp"

#include "lights.hpp"
#include "material.h"
#include "primitives.h"
#include "render_worker.hpp"

// custom includes
#ifdef USE_OPENCL
#include "opencl/material_cl.h"
#include "opencl/input_data_structures.h"
#include "opencl/primitives_cl.h"
#endif

#ifdef USE_OPENCL
cOpenClDynamicData::cOpenClDynamicData()
{
	totalDataOffset = 0; // counter for actual address in structure
	materialsOffset = 0;
	AOVectorsOffset = 0;
	lightsOffset = 0;
	primitivesOffset = 0;
	objectsOffset = 0;
	materialsOffsetAddress = 0;
	AOVectorsOffsetAddress = 0;
	lightsOffsetAddress = 0;
	primitivesOffsetAddress = 0;
	objectsOffsetAddress = 0;
}

cOpenClDynamicData::~cOpenClDynamicData()
{
}

int cOpenClDynamicData::PutDummyToAlign(int dataLength, int alignmentSize, QByteArray *array)
{
	int missingBytes = alignmentSize - dataLength % alignmentSize;
	if (missingBytes > 0 && missingBytes != alignmentSize)
	{
		char *dummyData = new char[missingBytes];
		memset(dummyData, 0, missingBytes);
		array->append(dummyData, missingBytes);
		delete[] dummyData;
		return missingBytes;
	}
	else
	{
		return 0;
	}
}

void cOpenClDynamicData::ReserveHeader()
{
	/* main header:
	 * cl_int materialsOffset
	 * cl_int AOVectorsOffset
	 * cl_int lightsOffset
	 * cl_int primitivesOffset
	 */

	// reserve bytes for array offset
	materialsOffsetAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(&materialsOffset), sizeof(materialsOffset));
	totalDataOffset += sizeof(materialsOffset);

	AOVectorsOffsetAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(&AOVectorsOffset), sizeof(AOVectorsOffset));
	totalDataOffset += sizeof(AOVectorsOffset);

	lightsOffsetAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(&lightsOffset), sizeof(lightsOffset));
	totalDataOffset += sizeof(lightsOffset);

	primitivesOffsetAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(&primitivesOffset), sizeof(primitivesOffset));
	totalDataOffset += sizeof(primitivesOffset);

	objectsOffsetAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(&objectsOffset), sizeof(objectsOffset));
	totalDataOffset += sizeof(objectsOffset);
}

void cOpenClDynamicData::FillHeader()
{
	data.replace(materialsOffsetAddress, sizeof(materialsOffset),
		reinterpret_cast<char *>(&materialsOffset), sizeof(materialsOffset));

	data.replace(AOVectorsOffsetAddress, sizeof(AOVectorsOffset),
		reinterpret_cast<char *>(&AOVectorsOffset), sizeof(AOVectorsOffset));

	data.replace(lightsOffsetAddress, sizeof(lightsOffset), reinterpret_cast<char *>(&lightsOffset),
		sizeof(lightsOffset));

	data.replace(primitivesOffsetAddress, sizeof(primitivesOffset),
		reinterpret_cast<char *>(&primitivesOffset), sizeof(primitivesOffset));

	data.replace(objectsOffsetAddress, sizeof(objectsOffset),
		reinterpret_cast<char *>(&objectsOffset), sizeof(objectsOffset));
}

void cOpenClDynamicData::Clear()
{
	data.clear();
	totalDataOffset = 0;
}

QByteArray &cOpenClDynamicData::GetData(void)
{
	return data;
}

int cOpenClDynamicData::BuildMaterialsData(const QMap<int, cMaterial> &materials)
{
	/* material dynamic data structure

	header:
	cl_int numberOfMaterials

	materials offsets:
	cl_int material[0] offset
	cl_int material[1] offset
	...
	cl_int material[numberOfMaterials] offset

	---- material 0 ---
	+0	cl_int materialClOffset (offset for material data)
	+4	cl_int paletteItemsOffset
	+8	cl_int palette size (bytes)
	+12	cl_int paletteLength (number of color palette items)

	+16	sMaterialCl material

		palette items:
			cl_float3 color[0]
			cl_float3 color[1]
			...
			cl_float3 color[paletteLength]
	-------------------

	*/

	totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);
	materialsOffset = totalDataOffset;

	// number of materials is a maximum material index
	// Empty material indexes will be filled with zero data
	QList<int> keys = materials.keys();

	qSort(keys.begin(), keys.end());
	cl_int numberOfMaterials = keys.last() + 1;

	// numberOfMaterials
	data.append(reinterpret_cast<char *>(&numberOfMaterials), sizeof(numberOfMaterials));
	int headerSize = sizeof(numberOfMaterials);
	totalDataOffset += headerSize;

	// reserve bytes for material offsets
	cl_int *materialOffsets = new cl_int[numberOfMaterials];
	int materialOffsetsSize = sizeof(cl_int) * numberOfMaterials;
	memset(materialOffsets, 0, materialOffsetsSize);

	int materialOffsetsAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(materialOffsets), materialOffsetsSize);
	totalDataOffset += materialOffsetsSize;

	// add dummy bytes for alignment to 16
	totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);

	for (int materialIndex = 0; materialIndex < numberOfMaterials; materialIndex++)
	{
		sMaterialCl materialCl;
		cl_float4 *paletteCl;
		int paletteSize;
		if (materials.contains(materialIndex))
		{
			cMaterial material = materials[materialIndex];
			materialCl = clCopySMaterialCl(material);
			cColorPalette palette = material.palette;
			paletteSize = palette.GetSize();
			paletteCl = new cl_float4[paletteSize];
			for (int i = 0; i < paletteSize; i++)
			{
				paletteCl[i] = toClFloat4(CVector4(palette.GetColor(i).R / 256.0,
					palette.GetColor(i).G / 256.0, palette.GetColor(i).B / 256.0, 0.0));
			}
		}
		else
		{
			// fill not used material with dummy
			memset(&materialCl, 0, sizeof(materialCl));
			paletteSize = 1;
			paletteCl = new cl_float4[1];
			paletteCl[0] = toClFloat4(CVector4());
		}

		materialOffsets[materialIndex] = totalDataOffset;

		cl_int materialClOffset = 0;
		cl_int paletteItemsOffset = 0;
		cl_int paletteSizeBytes = sizeof(cl_float4) * paletteSize;
		cl_int paletteLength = paletteSize;

		// reserve bytes for cl_int materialClOffset
		int materialClOffsetAddress = totalDataOffset;
		data.append(reinterpret_cast<char *>(&materialClOffset), sizeof(materialClOffset));
		totalDataOffset += sizeof(materialClOffset);

		// reserve bytes cl_int paletteItemsOffset
		int paletteItemsOffsetAddress = totalDataOffset;
		data.append(reinterpret_cast<char *>(&paletteItemsOffset), sizeof(paletteItemsOffset));
		totalDataOffset += sizeof(paletteItemsOffset);

		// cl_int palette size (bytes)
		data.append(reinterpret_cast<char *>(&paletteSizeBytes), sizeof(paletteSizeBytes));
		totalDataOffset += sizeof(paletteSizeBytes);

		// cl_int paletteLength (number of color palette items)
		data.append(reinterpret_cast<char *>(&paletteLength), sizeof(paletteLength));
		totalDataOffset += sizeof(paletteLength);

		// add dummy bytes for alignment to 16
		totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);

		// sMaterialCl material
		materialClOffset = totalDataOffset;
		data.append(reinterpret_cast<char *>(&materialCl), sizeof(materialCl));
		totalDataOffset += sizeof(materialCl);

		// fill materialClOffset value
		data.replace(materialClOffsetAddress, sizeof(materialClOffset),
			reinterpret_cast<char *>(&materialClOffset), sizeof(materialClOffset));

		// add dummy bytes for alignment to 16
		totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);

		// palette data
		paletteItemsOffset = totalDataOffset;
		data.append(reinterpret_cast<char *>(paletteCl), paletteSizeBytes);
		totalDataOffset += paletteSizeBytes;

		// fill paletteItemsOffset value
		data.replace(paletteItemsOffsetAddress, sizeof(paletteItemsOffset),
			reinterpret_cast<char *>(&paletteItemsOffset), sizeof(paletteItemsOffset));

		delete[] paletteCl;
	}

	// fill materials offsets:
	data.replace(materialOffsetsAddress, materialOffsetsSize,
		reinterpret_cast<char *>(materialOffsets), materialOffsetsSize);

	delete[] materialOffsets;

	return numberOfMaterials;
}

void cOpenClDynamicData::BuildAOVectorsData(const sVectorsAround *AOVectors, cl_int vectorsCount)
{
	/* use __attribute__((aligned(16))) in kernel code for array
	 *
	 * header:
	 * cl_int vectorsCount
	 * cl_int arrayOffset;
	 *
	 * array (aligned to 16):
	 * 	sVectorsAroundCl item1
	 * 	sVectorsAroundCl item2
	 * 	...
	 *	sVectorsAroundCl itemN
	 */

	totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);
	AOVectorsOffset = totalDataOffset;

	data.append(reinterpret_cast<char *>(&vectorsCount), sizeof(vectorsCount));
	totalDataOffset += sizeof(vectorsCount);

	// reserve bytes for array offset
	cl_int arrayOffset = 0;
	int arrayOffsetAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(&arrayOffset), sizeof(arrayOffset));
	totalDataOffset += sizeof(arrayOffset);

	// copy AO vectors aligned to 16
	for (int i = 0; i < vectorsCount; i++)
	{
		totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);
		if (i == 0) arrayOffset = totalDataOffset;

		sVectorsAroundCl vector;
		vector.v = toClFloat3(AOVectors[i].v);
		vector.color = toClFloat3(sRGB(AOVectors[i].R, AOVectors[i].G, AOVectors[i].B));
		vector.alpha = AOVectors[i].alpha;
		vector.beta = AOVectors[i].beta;

		data.append(reinterpret_cast<char *>(&vector), sizeof(vector));
		totalDataOffset += sizeof(vector);
	}

	// replace arrayOffset:
	data.replace(arrayOffsetAddress, sizeof(arrayOffset), reinterpret_cast<char *>(&arrayOffset),
		sizeof(arrayOffset));
}

void cOpenClDynamicData::BuildLightsData(const cLights *lights)
{
	/* use __attribute__((aligned(16))) in kernel code for array
	 *
	 * header:
	 * cl_int numberOfLights
	 * cl_int arrayOffset;
	 *
	 * array (aligned to 16):
	 * 	sLightCl light1
	 * 	sLightCl light1
	 *  ...
	 * 	sLightCl lightN
	 */

	totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);
	lightsOffset = totalDataOffset;

	cl_int numberOfLights = lights->GetNumberOfLights();
	data.append(reinterpret_cast<char *>(&numberOfLights), sizeof(numberOfLights));
	totalDataOffset += sizeof(numberOfLights);

	// reserve bytes for array offset
	cl_int arrayOffset = 0;
	int arrayOffsetAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(&arrayOffset), sizeof(arrayOffset));
	totalDataOffset += sizeof(arrayOffset);

	// copy lights aligned to 16
	for (int i = 0; i < numberOfLights; i++)
	{
		// align struct to 16
		totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);
		if (i == 0) arrayOffset = totalDataOffset;

		sLightCl lightCl;
		sLight *light = lights->GetLight(i);
		lightCl.position = toClFloat3(light->position);
		lightCl.colour = toClFloat3(light->colour);
		lightCl.intensity = light->intensity;
		lightCl.enabled = light->enabled;

		data.append(reinterpret_cast<char *>(&lightCl), sizeof(lightCl));
		totalDataOffset += sizeof(lightCl);
	}

	// replace arrayOffset:
	data.replace(arrayOffsetAddress, sizeof(arrayOffset), reinterpret_cast<char *>(&arrayOffset),
		sizeof(arrayOffset));
}

void cOpenClDynamicData::BuildPrimitivesData(const cPrimitives *primitivesContainer)
{
	/* use __attribute__((aligned(16))) in kernel code for array
	 *
	 * header:
	 * cl_int numberOfPrimitives
	 * cl_int globalPositionOffset
	 * cl_int arrayOffset;
	 *
	 * sPrimitiveGlobalPositionCl (aligned to 16)
	 *
	 * array (aligned to 16):
	 * 	sPrimitiveCl primitive1
	 * 	sPrimitiveCl primitive2
	 *  ...
	 * 	sPrimitiveCl primitiveN
	 */

	const QList<sPrimitiveBasic *> *primitives = primitivesContainer->GetListOfPrimitives();

	totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);
	primitivesOffset = totalDataOffset;

	cl_int numberOfPrimitives = primitives->size();
	data.append(reinterpret_cast<char *>(&numberOfPrimitives), sizeof(numberOfPrimitives));
	totalDataOffset += sizeof(numberOfPrimitives);

	// reserve bytes for sPrimitiveGlobalPosition offset
	cl_int globalPositionOffset = 0;
	int globalPositionOffsetAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(&globalPositionOffset), sizeof(globalPositionOffset));
	totalDataOffset += sizeof(globalPositionOffset);

	// reserve bytes for array offset
	cl_int arrayOffset = 0;
	int arrayOffsetAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(&arrayOffset), sizeof(arrayOffset));
	totalDataOffset += sizeof(arrayOffset);

	totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);
	globalPositionOffset = totalDataOffset;
	sPrimitiveGlobalPositionCl globalPosition;
	globalPosition.allPrimitivesPosition = toClFloat3(primitivesContainer->allPrimitivesPosition);
	globalPosition.allPrimitivesRotation = toClFloat3(primitivesContainer->allPrimitivesRotation);
	globalPosition.mRotAllPrimitivesRotation =
		toClMatrix33(primitivesContainer->mRotAllPrimitivesRotation);
	data.append(reinterpret_cast<char *>(&globalPosition), sizeof(globalPosition));
	totalDataOffset += sizeof(globalPosition);
	data.replace(globalPositionOffsetAddress, sizeof(globalPositionOffset),
		reinterpret_cast<char *>(&globalPositionOffset), sizeof(globalPositionOffset));

	// copy primitives data aligned to 16
	for (int i = 0; i < numberOfPrimitives; i++)
	{
		// align struct to 16
		totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);

		if (i == 0) arrayOffset = totalDataOffset;

		sPrimitiveCl primitiveCl;
		const sPrimitiveBasic *primitive = primitives->at(i);

		primitiveCl.object.enable = primitive->enable;
		primitiveCl.object.materialId = primitive->materialId;
		primitiveCl.object.objectId = primitive->objectId;
		primitiveCl.object.objectType = static_cast<enumObjectTypeCl>(primitive->objectType);
		primitiveCl.object.position = toClFloat3(primitive->position);
		primitiveCl.object.rotationMatrix = toClMatrix33(primitive->rotationMatrix);
		primitiveCl.object.size = toClFloat3(primitive->size);

		try
		{
			switch (primitive->objectType)
			{
				case fractal::objPlane:
				{
					const sPrimitivePlane *plane = dynamic_cast<const sPrimitivePlane *>(primitive);
					if (plane)
					{
						primitiveCl.data.plane.empty = plane->empty;
					}
					else
						throw QString("sPrimitivePlane");
					break;
				}

				case fractal::objBox:
				{
					const sPrimitiveBox *box = dynamic_cast<const sPrimitiveBox *>(primitive);
					if (box)
					{
						primitiveCl.data.box.empty = box->empty;
						primitiveCl.data.box.rounding = box->rounding;
						primitiveCl.data.box.repeat = toClFloat3(box->repeat);
					}
					else
						throw QString("sPrimitivePlane");
					break;
				}

				case fractal::objSphere:
				{
					const sPrimitiveSphere *sphere = dynamic_cast<const sPrimitiveSphere *>(primitive);
					if (sphere)
					{
						primitiveCl.data.sphere.empty = sphere->empty;
						primitiveCl.data.sphere.radius = sphere->radius;
						primitiveCl.data.sphere.repeat = toClFloat3(sphere->repeat);
					}
					else
						throw QString("sPrimitiveSphere");
					break;
				}

				case fractal::objWater:
				{
					const sPrimitiveWater *water = dynamic_cast<const sPrimitiveWater *>(primitive);
					if (water)
					{
						primitiveCl.data.water.empty = water->empty;
						primitiveCl.data.water.waveFromObjectsEnable = water->waveFromObjectsEnable;
						primitiveCl.data.water.relativeAmplitude = water->relativeAmplitude;
						primitiveCl.data.water.animSpeed = water->animSpeed;
						primitiveCl.data.water.length = water->length;
						primitiveCl.data.water.waveFromObjectsRelativeAmplitude =
							water->waveFromObjectsRelativeAmplitude;
						primitiveCl.data.water.iterations = water->iterations;
						primitiveCl.data.water.animFrame = water->animFrame;
					}
					else
						throw QString("sPrimitiveWater");
					break;
				}

				case fractal::objCone:
				{
					const sPrimitiveCone *cone = dynamic_cast<const sPrimitiveCone *>(primitive);
					if (cone)
					{
						primitiveCl.data.cone.empty = cone->empty;
						primitiveCl.data.cone.caps = cone->caps;
						primitiveCl.data.cone.radius = cone->radius;
						primitiveCl.data.cone.height = cone->height;
						primitiveCl.data.cone.wallNormal = toClFloat2(cone->wallNormal);
						primitiveCl.data.cone.repeat = toClFloat3(cone->repeat);
					}
					else
						throw QString("sPrimitiveCone");
					break;
				}

				case fractal::objCylinder:
				{
					const sPrimitiveCylinder *cylinder = dynamic_cast<const sPrimitiveCylinder *>(primitive);
					if (cylinder)
					{
						primitiveCl.data.cylinder.empty = cylinder->empty;
						primitiveCl.data.cylinder.caps = cylinder->caps;
						primitiveCl.data.cylinder.radius = cylinder->radius;
						primitiveCl.data.cylinder.height = cylinder->height;
						primitiveCl.data.cylinder.repeat = toClFloat3(cylinder->repeat);
					}
					else
						throw QString("sPrimitiveCylinder");
					break;
				}

				case fractal::objTorus:
				{
					const sPrimitiveTorus *torus = dynamic_cast<const sPrimitiveTorus *>(primitive);
					if (torus)
					{
						primitiveCl.data.torus.empty = torus->empty;
						primitiveCl.data.torus.radius = torus->radius;
						primitiveCl.data.torus.radiusLPow = torus->radiusLPow;
						primitiveCl.data.torus.tubeRadius = torus->tubeRadius;
						primitiveCl.data.torus.tubeRadiusLPow = torus->tubeRadiusLPow;
						primitiveCl.data.torus.repeat = toClFloat3(torus->repeat);
					}
					else
						throw QString("sPrimitiveTorus");
					break;
				}

				case fractal::objCircle:
				{
					const sPrimitiveCircle *circle = dynamic_cast<const sPrimitiveCircle *>(primitive);
					if (circle)
					{
						primitiveCl.data.circle.radius = circle->radius;
					}
					else
						throw QString("sPrimitiveCircle");
					break;
				}

				case fractal::objRectangle:
				{
					const sPrimitiveRectangle *rectangle =
						dynamic_cast<const sPrimitiveRectangle *>(primitive);
					if (rectangle)
					{
						primitiveCl.data.rectangle.height = rectangle->height;
						primitiveCl.data.rectangle.width = rectangle->width;
					}
					else
						throw QString("sPrimitiveRectangle");
					break;
				}

				default:
				{
					qCritical() << "cOpenClDynamicData::BuildPrimitivesData - invalid object type";
					break;
				}
			}
		}
		catch (const QString &ex)
		{
			qCritical() << QString(
				"cOpenClDynamicData::BuildPrimitivesData - invalid dynamic cast to %1 object");
		}

		data.append(reinterpret_cast<char *>(&primitiveCl), sizeof(primitiveCl));
		totalDataOffset += sizeof(primitiveCl);
	}

	// replace arrayOffset:
	data.replace(arrayOffsetAddress, sizeof(arrayOffset), reinterpret_cast<char *>(&arrayOffset),
		sizeof(arrayOffset));
}

void cOpenClDynamicData::BuildObjectsData(const QVector<cObjectData> *objectData)
{
	/* use __attribute__((aligned(16))) in kernel code for array
	 *
	 * header:
	 * cl_int numberOfObjects
	 * cl_int arrayOffset;
	 *
	 * array (aligned to 16):
	 * 	objectDataCl object1
	 * 	objectDataCl object2
	 *  ...
	 * 	objectDataCl objectN
	 */

	totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);
	objectsOffset = totalDataOffset;

	cl_int numberOfObjects = objectData->size();
	data.append(reinterpret_cast<char *>(&numberOfObjects), sizeof(numberOfObjects));
	totalDataOffset += sizeof(numberOfObjects);

	// reserve bytes for array offset
	cl_int arrayOffset = 0;
	int arrayOffsetAddress = totalDataOffset;
	data.append(reinterpret_cast<char *>(&arrayOffset), sizeof(arrayOffset));
	totalDataOffset += sizeof(arrayOffset);

	// copy objects aligned to 16
	for (int i = 0; i < numberOfObjects; i++)
	{
		// align struct to 16
		totalDataOffset += PutDummyToAlign(totalDataOffset, 16, &data);
		if (i == 0) arrayOffset = totalDataOffset;

		sObjectDataCl objectCl;
		const cObjectData *object = &objectData->at(i);
		objectCl.enable = true; // dummy - only used for primitives data
		objectCl.objectId = i;
		objectCl.position = toClFloat3(object->position);
		objectCl.size = toClFloat3(object->size);
		objectCl.materialId = object->materialId;
		objectCl.objectType = static_cast<enumObjectTypeCl>(object->objectType);
		objectCl.rotationMatrix = toClMatrix33(object->rotationMatrix);

		data.append(reinterpret_cast<char *>(&objectCl), sizeof(objectCl));
		totalDataOffset += sizeof(objectCl);
	}

	// replace arrayOffset:
	data.replace(arrayOffsetAddress, sizeof(arrayOffset), reinterpret_cast<char *>(&arrayOffset),
		sizeof(arrayOffset));
}

#endif // USE_OPENCL
