/*
 * render_ssao.h
 *
 *  Created on: Aug 3, 2014
 *      Author: krzysztof
 */

#ifndef RENDER_SSAO_H_
#define RENDER_SSAO_H_

#include "fractparams.hpp"
#include "render_data.hpp"

class cRenderSSAO
{
public:
	cRenderSSAO(const cParamRender *_params, const sRenderData *_renderData, cImage *_image);
	~cRenderSSAO();

	void RenderSSAO(QList<int> *list = NULL);
	void setQuiet() {quiet = true;}
	void setProgressive(double step) {progressive = step;}

private:
	const cParamRender *params;
	const sRenderData *data;
	cImage *image;
	bool quiet;
	double qualityFactor;
	int progressive;
};

#endif /* RENDER_SSAO_H_ */
