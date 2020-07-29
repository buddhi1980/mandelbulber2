/*
 * radiance_hdr.h
 *
 *  Created on: 9 lip 2020
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_RADIANCE_HDR_H_
#define MANDELBULBER2_SRC_RADIANCE_HDR_H_

#include <QString>
#include <vector>
#include "color_structures.hpp"

class QPixmap;

class cRadianceHDR
{
public:
	cRadianceHDR();
	~cRadianceHDR();

	bool Init(const QString filename, int *width, int *height);
	void Load(std::vector<sRGBFloat> *fBitmap);
	void LoadToQPixmap(QPixmap *pixmap);

private:
	QString actualFileName;
	size_t imageWidth = 0;
	size_t imageHeight = 0;
};

#endif /* MANDELBULBER2_SRC_RADIANCE_HDR_H_ */
