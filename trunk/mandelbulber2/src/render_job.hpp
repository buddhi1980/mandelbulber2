/*
 * render_job.hpp
 *
 *  Created on: May 3, 2014
 *      Author: krzysztof
 */

#ifndef RENDER_JOB_HPP_
#define RENDER_JOB_HPP_

#include "parameters.hpp"
#include "cimage.hpp"
#include "render_image.hpp"

class cRenderJob
{
public:
	cRenderJob(const cParameterContainer *_params, const cParameterContainer *_fractal, cImage *_image, QWidget *_qwidget = NULL);
	~cRenderJob();

	enum enumMode
	{
		still, keframeAnim, flightAnim, flightAnimRecord
	};

	bool Init(enumMode _mode);
	bool Execute();
	cImage* GetImagePtr() {return image;}
	int GetNumberOfCPUs() {return totalNumberOfCPUs;};
	void UseSizeFromImage(bool mode) {useSizeFromImage = mode;}

private:
	bool hasQWidget;
	bool InitImage(int w, int h);
	bool inProgress;
	bool ready;
	bool useSizeFromImage;
	cImage *image;
	cParameterContainer *fractalContainer;
	cParameterContainer *paramsContainer;
	enumMode mode;
	int height;
	int totalNumberOfCPUs;
	int width;
	QWidget *imageWidget;
	sRenderData *renderData;
	static int id; //global identifier of actual rendering job
};



#endif /* RENDER_JOB_HPP_ */
