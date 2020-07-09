/*
 * radiance_hdr.cpp
 *
 *  Created on: 9 lip 2020
 *      Author: krzysztof
 */

#include "radiance_hdr.h"

#define STB_IMAGE_IMPLEMENTATION
#include "third-party/stb/stb_image.h"

cRadianceHDR::cRadianceHDR()
{
	// Just for testing
	int x, y, n;
	unsigned char *data = stbi_load("test.hdr", &x, &y, &n, 0);
	//    // ... process data if not NULL ...
	//    // ... x = width, y = height, n = # 8-bit components per pixel ...
	//    // ... replace '0' with '1'..'4' to force that many components per pixel
	//    // ... but 'n' will always be the number that it would have been if you said 0
	stbi_image_free(data);
}

cRadianceHDR::~cRadianceHDR()
{
	// TODO Auto-generated destructor stub
}
