/*
 * render.cpp
 *
 *  Created on: May 3, 2014
 *      Author: krzysztof
 */

#include "render_job.hpp"
#include <QtCore>
#include "system.hpp"
#include "fractparams.hpp"
#include "four_fractals.hpp"
#include "render_image.hpp"

cRenderJob::cRenderJob(const parameters::container *_params, const parameters::container *_fractal, cImage *_image, QWidget *_qwidget)
{
	WriteLog("cRenderJob::cRenderJob");
	image = _image;

	//create new copy of parameter container
	paramsContainer = new parameters::container;
	*paramsContainer = *_params;
	fractalContainer = new parameters::container[4];
	for(int i=0; i < 4; i++)
		fractalContainer[i] = _fractal[i];

	width = 0;
	height = 0;
	mode = still;
	ready = false;
	inProgress = false;
	if(_qwidget)
	{
		imageWidget = _qwidget;
		hasQWidget = true;
	}
	else
	{
		imageWidget = NULL;
		hasQWidget = false;
	}

	totalNumberOfCPUs = systemData.numberOfThreads;
	id++;
}
int cRenderJob::id = 0;

cRenderJob::~cRenderJob()
{
	id--;
	delete paramsContainer;
	delete[] fractalContainer;
	WriteLog("Job finished and closed");
}

bool cRenderJob::Init(enumMode _mode)
{
	WriteLog("cRenderJob::Init");

	mode = _mode;

	width = paramsContainer->Get<int>("image_width");
	height = paramsContainer->Get<int>("image_height");

	if(!InitImage(width, height))
	{
		ready = false;
		return false;
	}

	//here will be total number of CPU cores in the network
	totalNumberOfCPUs = systemData.numberOfThreads;

	//here will be also preparation of random lights, textures and animation keyframe interpolation
	ready = true;
	return true;
}

bool cRenderJob::InitImage(int w, int h)
{
	WriteLog("cRenderJob::InitImage");
	QTextStream out(stdout);

	if(!image->ChangeSize(w, h))
	{
		printf("Cannot allocate memory. Maybe image is too big");
		return false;
	}
	else
	{
		WriteLog("complexImage allocated");
		out << "Memory for image: " << image->GetUsedMB() << " MB" << endl;

		//temporary set preview scale to 1.0
		image->CreatePreview(1.0, imageWidget);

		if(hasQWidget)
		{
			imageWidget->setMinimumSize(image->GetPreviewWidth(),image->GetPreviewHeight());
		}

		return true;
	}
}

bool cRenderJob::Execute(void)
{
	inProgress = true;

	//move parameters from containers to structures
	cParamRender *params = new cParamRender(paramsContainer);
	cFourFractals *fourFractals = new cFourFractals(fractalContainer);

	//aux renderer data
	sRenderData *data = new sRenderData;
	data->rendererID = id;
	data->numberOfThreads = totalNumberOfCPUs;

	//create and execute renderer
	cRenderer *renderer = new cRenderer(params, fourFractals, data, image);
	bool result = renderer->RenderImage();

	delete params;
	delete fourFractals;
	delete data;
	delete renderer;
	inProgress = false;
	return true;
}
