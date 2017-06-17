/*
 * opencl_dynamic_data.hpp
 *
 *  Created on: 17 cze 2017
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_OPENCL_DYNAMIC_DATA_HPP_
#define MANDELBULBER2_SRC_OPENCL_DYNAMIC_DATA_HPP_

#ifdef USE_OPENCL

#include <QtCore>

class cMaterial;

class cOpenClDynamicData
{
public:
	cOpenClDynamicData();
	~cOpenClDynamicData();

	void Clear();
	static int PutDummyToAlign(int dataLength, int alignmentSize, QByteArray *array);
	void BuildMaterialsData(const QMap<int, cMaterial> &materials);
	QByteArray &GetData(void);

private:
	QByteArray data;
};

#endif // USE_OPENCL

#endif /* MANDELBULBER2_SRC_OPENCL_DYNAMIC_DATA_HPP_ */
