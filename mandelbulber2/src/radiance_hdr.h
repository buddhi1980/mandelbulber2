/*
 * radiance_hdr.h
 *
 *  Created on: 9 lip 2020
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_RADIANCE_HDR_H_
#define MANDELBULBER2_SRC_RADIANCE_HDR_H_

#include <QString>
#include "color_structures.hpp"

class cRadianceHDR
{
public:
	cRadianceHDR();
	~cRadianceHDR();

	bool Init(const QString filename, int *width, int *height);
	void Load(sRGBFloat *fBitmap);

private:
	QString actualFileName;
};

#endif /* MANDELBULBER2_SRC_RADIANCE_HDR_H_ */
