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
	cRenderJob(const parameters::container *_params, const parameters::container *_fractal, cImage *_image, QWidget *_qwidget = NULL);
	~cRenderJob();

	enum enumMode
	{
		still, keframeAnim, flightAnim, flightAnimRecord
	};

	bool Init(enumMode _mode);
	bool Execute();
	cImage* GetImagePtr() {return image;}
	int GetNumberOfCPUs() {return totalNumberOfCPUs;};

private:
	bool InitImage(int w, int h);
	parameters::container *paramsContainer;
	parameters::container *fractalContainer;
	cImage *image;
	sRenderData *renderData;
	int width;
	int height;
	enumMode mode;
	bool ready;
	bool inProgress;
	bool hasQWidget;
	QWidget *imageWidget;
	int totalNumberOfCPUs;
	static int id; //global identifier of actual rendering job
};



#endif /* RENDER_JOB_HPP_ */
