

/*
 * test_engine.c
 *
 *  Created on: 5 maj 2017
 *      Author: krzysztof
 */

typedef float3 cl_float3;
typedef float cl_float;
typedef int cl_int;
typedef unsigned int cl_uint;
typedef unsigned short cl_ushort;
typedef unsigned char cl_uchar;

#include INCLUDE_PATH_CL_DATA

//------------------ MAIN RENDER FUNCTION --------------------
kernel void fractal3D(__global sClPixel *out, __global sClInBuff *inBuff, __constant sClInConstants *consts, int Gcl_offset)
{
	int cl_offset = Gcl_offset;
	
	const int id = get_global_id(0) + cl_offset;
	const int imageX = id % consts->params.width;
	const int imageYtemp = id / consts->params.width;
	const int buffIndex = (id - cl_offset);
	
	const int imageY = clamp(imageYtemp, 0, consts->params.height-1);

	float2 screenPoint = (float2)
	{ convert_float(imageX), convert_float(imageY)};
	float width = convert_float(consts->params.width);
	float height = convert_float(consts->params.height);

	float aspectRatio = width / height;
	float2 screenPointNorm;
	screenPointNorm.x = (screenPoint.x / width - 0.5f) * aspectRatio;
	screenPointNorm.y = (screenPoint.y / height - 0.5f);

	screenPointNorm *= 4.0f;
	
	float2 z = screenPointNorm;
	float2 c = z;
	
	int i;
	
	for(int i = 0; i < consts->params.N; i++)
	{
		float tempX = z.x + z.x - z.y * z.y;
		z.y = 2.0f * z.x * z.y + c.y;
		z.x = tempX + c.x;
		
		if(length(z) > 2.0) break;
	}
	
	sClPixel pixel;

	pixel.R = i / 256.0f;
	pixel.G = i / 256.0f;
	pixel.B = i / 256.0f;
	pixel.zBuffer = 0.0f;
	pixel.colR = 0.0f;
	pixel.colG = 0.0f;
	pixel.colB = 0.0f;
	pixel.opacity = 0;
	pixel.alpha = 65535;
	
	out[buffIndex] = pixel;
}



