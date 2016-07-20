/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-16 Krzysztof Marczak     §R-==%w["'~5]m%=L.=~5N
 *                                        ,=mm=§M ]=4 yJKA"/-Nsaj  "Bw,==,,
 * This file is part of Mandelbulber.    §R.r= jw",M  Km .mM  FW ",§=ß., ,TN
 *                                     ,4R =%["w[N=7]J '"5=],""]]M,w,-; T=]M
 * Mandelbulber is free software:     §R.ß~-Q/M=,=5"v"]=Qf,'§"M= =,M.§ Rz]M"Kw
 * you can redistribute it and/or     §w "xDY.J ' -"m=====WeC=\ ""%""y=%"]"" §
 * modify it under the terms of the    "§M=M =D=4"N #"%==A%p M§ M6  R' #"=~.4M
 * GNU General Public License as        §W =, ][T"]C  §  § '§ e===~ U  !§[Z ]N
 * published by the                    4M",,Jm=,"=e~  §  §  j]]""N  BmM"py=ßM
 * Free Software Foundation,          ]§ T,M=& 'YmMMpM9MMM%=w=,,=MT]M m§;'§,
 * either version 3 of the License,    TWw [.j"5=~N[=§%=%W,T ]R,"=="Y[LFT ]N
 * or (at your option)                   TW=,-#"%=;[  =Q:["V""  ],,M.m == ]N
 * any later version.                      J§"mr"] ,=,," =="""J]= M"M"]==ß"
 *                                          §= "=C=4 §"eM "=B:m|4"]#F,§~
 * Mandelbulber is distributed in            "9w=,,]w em%wJ '"~" ,=,,ß"
 * the hope that it will be useful,                 . "K=  ,=RMMMßM"""
 * but WITHOUT ANY WARRANTY;                            .'''
 * without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * ###########################################################################
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * cRenderJob class - prepare and coordinate rendering of single or multiple images (animation)
 */

#include "render_job.hpp"
//#include <QtCore>
#include "image_scale.hpp"
#include "netrender.hpp"
#include "render_image.hpp"
#include "system.hpp"
//#include "fractparams.hpp"
//#include "global_data.hpp"
//#include "progress_text.hpp"
//#include "error_message.hpp"
//#include "nine_fractals.hpp"

cRenderJob::cRenderJob(const cParameterContainer *_params, const cFractalContainer *_fractal,
	cImage *_image, bool *_stopRequest, QWidget *_qwidget)
		: QObject()
{
	WriteLog("cRenderJob::cRenderJob", 2);
	image = _image;

	// create new copy of parameter container
	paramsContainer = new cParameterContainer;
	*paramsContainer = *_params;
	fractalContainer = new cFractalContainer;
	*fractalContainer = *_fractal;
	canUseNetRender = false;

	width = 0;
	height = 0;
	mode = still;
	ready = false;
	inProgress = false;
	if (_qwidget)
	{
		imageWidget = _qwidget;
		hasQWidget = true;
		connect(this, SIGNAL(SetMinimumWidgetSize(int, int)), imageWidget,
			SLOT(slotSetMinimumSize(int, int)));
	}
	else
	{
		imageWidget = NULL;
		hasQWidget = false;
	}

	totalNumberOfCPUs = systemData.numberOfThreads;
	renderData = NULL;
	useSizeFromImage = false;
	stopRequest = _stopRequest;

	id++;
	// qDebug() << "Id" << id;
}
int cRenderJob::id = 0;
int cRenderJob::runningJobs = 0;

cRenderJob::~cRenderJob()
{
	id--;
	// qDebug() << "Id" << id;
	delete paramsContainer;
	delete fractalContainer;
	if (renderData) delete renderData;

	if (canUseNetRender) gNetRender->Release();

	WriteLog("Job finished and closed", 2);
}

bool cRenderJob::Init(enumMode _mode, const cRenderingConfiguration &config)
{
	WriteLog("cRenderJob::Init id = " + QString::number(id), 2);

	mode = _mode;

	if (config.UseNetRender()) canUseNetRender = gNetRender->Block();

	// needed when image has to fit in widget
	if (useSizeFromImage)
	{
		paramsContainer->Set("image_width", image->GetWidth());
		paramsContainer->Set("image_height", image->GetHeight());
	}
	width = paramsContainer->Get<int>("image_width");
	height = paramsContainer->Get<int>("image_height");

	sImageOptional imageOptional;
	imageOptional.optionalNormal = paramsContainer->Get<bool>("normal_enabled");

	emit updateProgressAndStatus(
		QObject::tr("Initialization"), QObject::tr("Setting up image buffers"), 0.0);
	// gApplication->processEvents();

	if (!InitImage(width, height, imageOptional))
	{
		ready = false;
		return false;
	}

	if (image->IsMainImage())
	{
		// clear image before start rendering
		if ((gNetRender->IsClient() || gNetRender->IsServer()) && canUseNetRender)
		{
			image->ClearImage();
			image->UpdatePreview();
			if (hasQWidget) imageWidget->update();
		}
	}

	if (config.UseNetRender() && canUseNetRender)
	{
		// connect signals
		if (gNetRender->IsServer())
		{
			connect(this, SIGNAL(SendNetRenderJob(cParameterContainer, cFractalContainer, QStringList)),
				gNetRender, SLOT(SetCurrentJob(cParameterContainer, cFractalContainer, QStringList)));
			connect(this, SIGNAL(SendNetRenderSetup(int, int, QList<int>)), gNetRender,
				SLOT(SendSetup(int, int, QList<int>)));
		}
	}

	totalNumberOfCPUs = systemData.numberOfThreads;
	// totalNumberOfCPUs = 1;
	// systemData.numberOfThreads = 1;

	// aux renderer data
	if (renderData) delete renderData;
	renderData = new sRenderData;

	PrepareData(config);

	ready = true;

	return true;
}

bool cRenderJob::InitImage(int w, int h, sImageOptional optional)
{
	WriteLog("cRenderJob::InitImage", 2);

	if (!image->ChangeSize(w, h, optional))
	{
		printf("Cannot allocate memory. Maybe image is too big");
		return false;
	}
	else
	{
		WriteLog("complexImage allocated", 2);
		if (hasQWidget)
		{
			double scale =
				ImageScaleComboSelection2Double(paramsContainer->Get<int>("image_preview_scale"));
			if (useSizeFromImage) scale = 0.0;
			scale = CalcMainImageScale(
				scale, image->GetPreviewVisibleWidth(), image->GetPreviewVisibleHeight(), image);
			image->CreatePreview(
				scale, image->GetPreviewVisibleWidth(), image->GetPreviewVisibleHeight(), imageWidget);
			image->UpdatePreview();
			emit SetMinimumWidgetSize(image->GetPreviewWidth(), image->GetPreviewHeight());
		}

		// qDebug() << "Memory for image: " << image->GetUsedMB() << " MB" << endl;
		return true;
	}
}

void cRenderJob::PrepareData(const cRenderingConfiguration &config)
{
	WriteLog("Init renderData", 2);
	renderData->rendererID = id;
	renderData->configuration = config;

	if (!canUseNetRender) renderData->configuration.DisableNetRender();

	// set image region to render
	if (paramsContainer->Get<bool>("legacy_coordinate_system"))
	{
		renderData->imageRegion.Set(-0.5, -0.5, 0.5, 0.5);
	}
	else
	{
		renderData->imageRegion.Set(-0.5, 0.5, 0.5, -0.5);
	}
	renderData->screenRegion.Set(0, 0, width, height);

	// textures are deleted with destruction of renderData

	emit updateProgressAndStatus(QObject::tr("Initialization"), QObject::tr("Loading textures"), 0.0);
	// gApplication->processEvents();

	if (gNetRender->IsClient())
	{
		// get received textures from NetRender buffer
		if (paramsContainer->Get<bool>("textured_background"))
			renderData->textures.backgroundTexture.FromQByteArray(
				gNetRender->GetTexture(paramsContainer->Get<QString>("file_background")),
				cTexture::doNotUseMipmaps);

		if (paramsContainer->Get<bool>("env_mapping_enable"))
			renderData->textures.envmapTexture.FromQByteArray(
				gNetRender->GetTexture(paramsContainer->Get<QString>("file_envmap")),
				cTexture::doNotUseMipmaps);

		if (paramsContainer->Get<int>("ambient_occlusion_mode") == params::AOmodeMultipeRays
				&& paramsContainer->Get<bool>("ambient_occlusion_enabled"))
			renderData->textures.lightmapTexture.FromQByteArray(
				gNetRender->GetTexture(paramsContainer->Get<QString>("file_lightmap")),
				cTexture::doNotUseMipmaps);
	}
	else
	{
		if (paramsContainer->Get<bool>("textured_background"))
			renderData->textures.backgroundTexture =
				cTexture(paramsContainer->Get<QString>("file_background"), cTexture::doNotUseMipmaps,
					config.UseIgnoreErrors());

		if (paramsContainer->Get<bool>("env_mapping_enable"))
			renderData->textures.envmapTexture = cTexture(paramsContainer->Get<QString>("file_envmap"),
				cTexture::doNotUseMipmaps, config.UseIgnoreErrors());

		if (paramsContainer->Get<int>("ambient_occlusion_mode") == params::AOmodeMultipeRays
				&& paramsContainer->Get<bool>("ambient_occlusion_enabled"))
			renderData->textures.lightmapTexture =
				cTexture(paramsContainer->Get<QString>("file_lightmap"), cTexture::doNotUseMipmaps,
					config.UseIgnoreErrors());
	}

	// assign stop handler
	renderData->stopRequest = stopRequest;

	CreateMaterialsMap(
		paramsContainer, &renderData->materials, renderData->configuration.UseIgnoreErrors());

	// preparation of lights
	// connect signal for progress bar update
	connect(&renderData->lights,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));

	renderData->lights.Set(paramsContainer, fractalContainer);

	renderData->objectData.resize(NUMBER_OF_FRACTALS); // reserve first items for fractal formulas
}

bool cRenderJob::Execute(void)
{
	// if(image->IsUsed())
	//{
	//	//cErrorMessage::showMessage(QObject::tr("Rendering engine is busy. Stop unfinished rendering
	// before starting new one"), cErrorMessage::errorMessage);
	//	return false;
	//}

	image->BlockImage();

	emit updateProgressAndStatus(
		QObject::tr("Rendering image"), QObject::tr("Starting rendering of image"), 0.0);

	// send settings to all NetRender clients
	if (renderData->configuration.UseNetRender())
	{
		if (gNetRender->IsServer())
		{
			// new id
			qint32 id = rand();

			// calculation of starting positions list and sending id to clients
			renderData->netRenderStartingPositions.clear();

			int clientIndex = 0;
			int clientWorkerIndex = 0;

			int workersCount =
				gNetRender->getTotalWorkerCount() + renderData->configuration.GetNumberOfThreads();

			QList<int> startingPositionsToSend;

			for (int i = 0; i < workersCount; i++)
			{
				if (i < renderData->configuration.GetNumberOfThreads())
				{
					renderData->netRenderStartingPositions.append(i * image->GetHeight() / workersCount);
				}
				else
				{
					startingPositionsToSend.append(i * image->GetHeight() / workersCount);
					clientWorkerIndex++;

					if (clientWorkerIndex >= gNetRender->GetWorkerCount(clientIndex))
					{
						emit SendNetRenderSetup(clientIndex, id, startingPositionsToSend);
						clientIndex++;
						clientWorkerIndex = 0;
						startingPositionsToSend.clear();
					}
				}
			}

			QStringList listOfUsedTextures = CreateListOfUsedTextures();

			// send settings to all clients
			emit SendNetRenderJob(*paramsContainer, *fractalContainer, listOfUsedTextures);
		}

		// get starting positions received from server
		if (gNetRender->IsClient())
		{
			renderData->netRenderStartingPositions = gNetRender->GetStartingPositions();
		}
	}

	runningJobs++;
	// qDebug() << "runningJobs" << runningJobs;

	inProgress = true;
	*renderData->stopRequest = false;

	WriteLog("cRenderJob::Execute(void): running jobs = " + QString::number(runningJobs), 2);

	// move parameters from containers to structures
	cParamRender *params = new cParamRender(paramsContainer, &renderData->objectData);
	cNineFractals *fractals = new cNineFractals(fractalContainer, paramsContainer);

	// recalculation of some parameters;
	params->resolution = 1.0 / image->GetHeight();
	ReduceDetail();

	// initialize histograms
	renderData->statistics.histogramIterations.Resize(paramsContainer->Get<int>("N"));
	renderData->statistics.histogramStepCount.Resize(1000);
	renderData->statistics.Reset();
	renderData->statistics.usedDEType = fractals->GetDETypeString();

	// create and execute renderer
	cRenderer *renderer = new cRenderer(params, fractals, renderData, image);

	// connect signal for progress bar update
	connect(renderer, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
	connect(
		renderer, SIGNAL(updateStatistics(cStatistics)), this, SIGNAL(updateStatistics(cStatistics)));

	if (renderData->configuration.UseNetRender())
	{
		if (gNetRender->IsClient())
		{
			QObject::connect(renderer, SIGNAL(sendRenderedLines(QList<int>, QList<QByteArray>)),
				gNetRender, SLOT(SendRenderedLines(QList<int>, QList<QByteArray>)));
			QObject::connect(gNetRender, SIGNAL(ToDoListArrived(QList<int>)), renderer,
				SLOT(ToDoListArrived(QList<int>)));
			QObject::connect(renderer, SIGNAL(NotifyClientStatus()), gNetRender, SLOT(NotifyStatus()));
			QObject::connect(gNetRender, SIGNAL(AckReceived()), renderer, SLOT(AckReceived()));
		}

		if (gNetRender->IsServer())
		{
			QObject::connect(gNetRender, SIGNAL(NewLinesArrived(QList<int>, QList<QByteArray>)), renderer,
				SLOT(NewLinesArrived(QList<int>, QList<QByteArray>)));
			QObject::connect(renderer, SIGNAL(SendToDoList(int, QList<int>)), gNetRender,
				SLOT(SendToDoList(int, QList<int>)));
			QObject::connect(renderer, SIGNAL(StopAllClients()), gNetRender, SLOT(StopAll()));
		}
	}

	bool result = renderer->RenderImage();

	if (result) emit fullyRendered();

	delete params;
	delete fractals;
	delete renderer;
	inProgress = false;

	WriteLog("cRenderJob::Execute(void): finished", 2);

	image->ReleaseImage();

	runningJobs--;
	// qDebug() << "runningJobs" << runningJobs;

	return result;
}

void cRenderJob::ChangeCameraTargetPosition(cCameraTarget &cameraTarget)
{
	paramsContainer->Set("camera", cameraTarget.GetCamera());
	paramsContainer->Set("target", cameraTarget.GetTarget());
	paramsContainer->Set("camera_top", cameraTarget.GetTopVector());
	paramsContainer->Set("camera_rotation", cameraTarget.GetRotation());
	paramsContainer->Set("camera_distance_to_target", cameraTarget.GetDistance());
}

void cRenderJob::UpdateParameters(
	const cParameterContainer *_params, const cFractalContainer *_fractal)
{
	*paramsContainer = *_params;
	*fractalContainer = *_fractal;

	PrepareData(renderData->configuration);
}

void cRenderJob::ReduceDetail()
{
	if (mode == flightAnimRecord)
	{
		renderData->reduceDetail = sqrt(renderData->lastPercentage);
		if (renderData->reduceDetail < 0.1) renderData->reduceDetail = 0.1;
	}
	else
	{
		renderData->reduceDetail = 1.0;
	}
}

void cRenderJob::slotExecute()
{
	Execute();
	emit finished();
}

QStringList cRenderJob::CreateListOfUsedTextures()
{
	QSet<QString> listOfTextures;
	if (renderData)
	{
		QList<int> keys = renderData->materials.keys();
		for (int i = 0; i < keys.size(); i++)
		{
			int matIndex = keys[i];
			if (renderData->materials[matIndex].colorTexture.IsLoaded())
				listOfTextures.insert(renderData->materials[matIndex].colorTexture.GetFileName());

			if (renderData->materials[matIndex].diffusionTexture.IsLoaded())
				listOfTextures.insert(renderData->materials[matIndex].diffusionTexture.GetFileName());

			if (renderData->materials[matIndex].normalMapTexture.IsLoaded())
				listOfTextures.insert(renderData->materials[matIndex].normalMapTexture.GetFileName());

			if (renderData->materials[matIndex].displacementTexture.IsLoaded())
				listOfTextures.insert(renderData->materials[matIndex].displacementTexture.GetFileName());

			if (renderData->materials[matIndex].luminosityTexture.IsLoaded())
				listOfTextures.insert(renderData->materials[matIndex].luminosityTexture.GetFileName());
		}

		for (int i = 0; i < renderData->textures.textureList.size(); i++)
		{
			if (renderData->textures.textureList[i]->IsLoaded())
				listOfTextures.insert(renderData->textures.textureList[i]->GetFileName());
		}

		return listOfTextures.toList();
	}
	return QStringList();
}
