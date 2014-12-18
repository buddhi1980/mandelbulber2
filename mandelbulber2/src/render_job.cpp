/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cRenderJob class - prepare and coordinate rendering of single or multiple images (animation)
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */

#include "render_job.hpp"
#include <QtCore>
#include "system.hpp"
#include "fractparams.hpp"
#include "four_fractals.hpp"
#include "global_data.hpp"
#include "progress_text.hpp"

cRenderJob::cRenderJob(const cParameterContainer *_params, const cFractalContainer *_fractal, cImage *_image, bool *_stopRequest, QWidget *_qwidget)
{
	WriteLog("cRenderJob::cRenderJob");
	image = _image;

	//create new copy of parameter container
	paramsContainer = new cParameterContainer;
	*paramsContainer = *_params;
	fractalContainer = new cFractalContainer;
	*fractalContainer = *_fractal;

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

	useSizeFromImage = false;

	statusBar = NULL;
	progressBar = NULL;

	stopRequest = _stopRequest;

	id++;
}
int cRenderJob::id = 0;

cRenderJob::~cRenderJob()
{
	id--;
	delete paramsContainer;
	delete fractalContainer;
	if(renderData) delete renderData;
	WriteLog("Job finished and closed");
}

bool cRenderJob::Init(enumMode _mode)
{
	WriteLog("cRenderJob::Init");

	mode = _mode;

	if(useSizeFromImage)
	{
		paramsContainer->Set("image_width", image->GetWidth());
		paramsContainer->Set("image_height", image->GetHeight());
	}
	width = paramsContainer->Get<int>("image_width");
	height = paramsContainer->Get<int>("image_height");

	ProgressStatusText(QObject::tr("Initialization"), QObject::tr("Setting up image buffers"), 0.0, statusBar, progressBar);
	application->processEvents();

	if(!InitImage(width, height))
	{
		ready = false;
		return false;
	}

	totalNumberOfCPUs = systemData.numberOfThreads;
	//totalNumberOfCPUs = 1;

	WriteLog("Init renderData");
	//aux renderer data
	if(renderData) delete renderData;
	renderData = new sRenderData;
	renderData->rendererID = id;
	renderData->numberOfThreads = totalNumberOfCPUs;

	if(paramsContainer->Get<bool>("legacy_coordinate_system"))
	{
		renderData->imageRegion.Set(-0.5, -0.5, 0.5, 0.5);
	}
	else
	{
		renderData->imageRegion.Set(-0.5, 0.5, 0.5, -0.5);
	}

	renderData->screenRegion.Set(0, 0, width, height);
	renderData->lights.Set(paramsContainer, fractalContainer);
	renderData->palette = paramsContainer->Get<cColorPalette>("surface_color_palette");

	//textures are deleted with destruction of renderData

	ProgressStatusText(QObject::tr("Initialization"), QObject::tr("Loading textures"), 0.0, statusBar, progressBar);
	application->processEvents();

	if(paramsContainer->Get<bool>("textured_background"))
		renderData->textures.backgroundTexture = new cTexture(paramsContainer->Get<QString>("file_background"));

	if(paramsContainer->Get<bool>("env_mapping_enable"))
		renderData->textures.envmapTexture = new cTexture(paramsContainer->Get<QString>("file_envmap"));

	if(paramsContainer->Get<int>("ambient_occlusion_mode") == params::AOmodeMultipeRays && paramsContainer->Get<bool>("ambient_occlusion_enabled"))
		renderData->textures.lightmapTexture = new cTexture(paramsContainer->Get<QString>("file_lightmap"));

	renderData->statusBar = statusBar;
	renderData->progressBar = progressBar;
	renderData->stopRequest = stopRequest;

	if(mode == flightAnimRecord) renderData->doNotRefresh = true;
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
			double scale = ImageScaleComboSelection2Double(paramsContainer->Get<int>("image_preview_scale"));
			scale = CalcMainImageScale(scale, image->GetPreviewVisibleWidth(), image->GetPreviewVisibleHeight(), image);
			image->CreatePreview(scale, image->GetPreviewVisibleWidth(), image->GetPreviewVisibleHeight(), imageWidget);
			image->UpdatePreview();
			imageWidget->setMinimumSize(image->GetPreviewWidth(), image->GetPreviewHeight());
		}

		out << "Memory for image: " << image->GetUsedMB() << " MB" << endl;
		return true;
	}
}

bool cRenderJob::Execute(void)
{
	inProgress = true;
	*renderData->stopRequest = false;

	WriteLog("cRenderJob::Execute(void)");

	//move parameters from containers to structures
	cParamRender *params = new cParamRender(paramsContainer);
	cFourFractals *fourFractals = new cFourFractals(fractalContainer, paramsContainer);

	//recalculation of some parameters;
	params->resolution = 1.0/image->GetHeight();

	//create and execute renderer
	cRenderer *renderer = new cRenderer(params, fourFractals, renderData, image);
	image->BlockImage();
	renderer->RenderImage();
	image->ReleaseImage();

	delete params;
	delete fourFractals;
	delete renderer;
	inProgress = false;

	WriteLog("cRenderJob::Execute(void): finished");

	return true;
}

void cRenderJob::ChangeCameraTargetPosition(cCameraTarget cameraTarget)
{
	paramsContainer->Set("camera", cameraTarget.GetCamera());
	paramsContainer->Set("target", cameraTarget.GetTarget());
	paramsContainer->Set("camera_top", cameraTarget.GetTopVector());
	paramsContainer->Set("camera_rotation", cameraTarget.GetRotation());
	paramsContainer->Set("camera_distance_to_target", cameraTarget.GetDistance());
}

void cRenderJob::SetMaxRenderTime(double time)
{
	if(renderData)
	{
		renderData->maxRenderTime = time;
	}
	else
	{
		qCritical() << "cRenderJob::SetMaxRenderTime(double time): renderData not set";
	}
}
