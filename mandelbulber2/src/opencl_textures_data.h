/*
 * opencl_textures_data.h
 *
 *  Created on: 10 sie 2018
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPENCL_TEXTURES_DATA_H_
#define MANDELBULBER2_SRC_OPENCL_TEXTURES_DATA_H_

#ifdef USE_OPENCL

#include "opencl_abstract_dynamic_data.h"

struct sTextures;
class cMaterial;
class cTexture;

class cOpenClTexturesData : public cOpenClAbstractDynamicData
{
public:
	cOpenClTexturesData(int numberOfTextures);
	~cOpenClTexturesData();

	static int CheckNumberOfTextures(
		const sTextures &textures, const QMap<int, cMaterial> &materials);
	void BuildAllTexturesData(const sTextures &textures, const QMap<int, cMaterial> &materials);

private:
	static bool CountTexture(const cTexture *texture, QSet<QString> *listOfTextures, int *counter);
	void BuildTextureData(const cTexture *texture, int textureIndex);

	int numberOfTextures;
};

#endif // USE_OPENCL

#endif /* MANDELBULBER2_SRC_OPENCL_TEXTURES_DATA_H_ */
