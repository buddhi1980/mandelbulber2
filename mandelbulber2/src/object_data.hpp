/*
 * object_data.hpp
 *
 *  Created on: 3 kwi 2016
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OBJECT_DATA_HPP_
#define MANDELBULBER2_SRC_OBJECT_DATA_HPP_

#include "object_types.hpp"

class cObjectData
{
public:
	cObjectData()
	{
		materialId = 0;
		objectType = fractal::objNone;
	}

	CVector3 position;
	CVector3 size;
	int materialId;
	fractal::enumObjectType objectType;
	CRotationMatrix rotationMatrix;
	void SetRotation(const CVector3 &rot)
	{
		rotationMatrix.SetRotation2(rot * M_PI / 180.0);
	}

private:
	CVector3 rotation;
};




#endif /* MANDELBULBER2_SRC_OBJECT_DATA_HPP_ */
