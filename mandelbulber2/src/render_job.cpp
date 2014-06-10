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
#include "interface.hpp"

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
	renderData = NULL;

	id++;
}
int cRenderJob::id = 0;

cRenderJob::~cRenderJob()
{
	id--;
	delete paramsContainer;
	delete[] fractalContainer;
	if(renderData) delete renderData;
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
	//totalNumberOfCPUs = 1;

	//aux renderer data
	if(renderData) delete renderData;
	renderData = new sRenderData;
	renderData->rendererID = id;
	renderData->numberOfThreads = totalNumberOfCPUs;
	renderData->imageRegion.Set(-1.0, -1.0, 1.0, 1.0);
	renderData->screenRegion.Set(0, 0, width, height);
	renderData->lights.Set(paramsContainer, fractalContainer);
	renderData->palette = cColorPalette(12345, 1.0);

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
		if(hasQWidget)
		{
			double scale = mainInterface->ImageScaleComboSelection2Double(paramsContainer->Get<int>("image_preview_scale"));
			scale = mainInterface->CalcMainImageScale(scale, image->GetPreviewVisibleWidth(), image->GetPreviewVisibleHeight(), image);
			image->CreatePreview(scale, image->GetPreviewVisibleWidth(), image->GetPreviewVisibleHeight(), imageWidget);
			image->UpdatePreview();
			imageWidget->setMinimumSize(mainInterface->mainImage->GetPreviewWidth(), mainInterface->mainImage->GetPreviewHeight());
		}

		out << "Memory for image: " << image->GetUsedMB() << " MB" << endl;
		return true;
	}
}

bool cRenderJob::Execute(void)
{
	inProgress = true;

	//move parameters from containers to structures
	cParamRender *params = new cParamRender(paramsContainer);
	cFourFractals *fourFractals = new cFourFractals(fractalContainer);

	//recalculation of some parameters;
	params->resolution = 1.0/image->GetWidth();

	//create and execute renderer
	cRenderer *renderer = new cRenderer(params, fourFractals, renderData, image);
	image->BlockImage();
	renderer->RenderImage();
	image->ReleaseImage();

	delete params;
	delete fourFractals;
	delete renderer;
	inProgress = false;
	mainInterface->stopRequest = false;

	return true;
}
