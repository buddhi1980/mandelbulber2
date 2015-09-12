/*
 * headless.h
 *
 *  Created on: 12-09-2015
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_HEADLESS_H_
#define MANDELBULBER2_SRC_HEADLESS_H_

#include <QtCore>

class cHeadless
{
public:
	cHeadless();
	~cHeadless();

	void RenderStillImage(void);
	static void RenderingProgressOutput(const QString &progressTxt, double percentDone);

};

#endif /* MANDELBULBER2_SRC_HEADLESS_H_ */
