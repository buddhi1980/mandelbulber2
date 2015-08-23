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

#ifndef RENDER_JOB_HPP_
#define RENDER_JOB_HPP_

#include "parameters.hpp"
#include "cimage.hpp"
#include "render_image.hpp"
#include <QProgressBar>
#include <QStatusBar>
#include "camera_target.hpp"

class cRenderJob : public QObject
{
	Q_OBJECT
public:
	cRenderJob(const cParameterContainer *_params, const cFractalContainer *_fractal, cImage *_image, bool *_stopRequest, QObject *_parent = NULL, QWidget *_qwidget = NULL);
	~cRenderJob();
	//QWidtet *parent is needed to connect signals for refreshing progress and status bar.
	//If _parent is not NULL then parent has to have slot slotUpdateProgressAndStatus()

	enum enumMode
	{
		still, keyframeAnim, flightAnim, flightAnimRecord
	};

	bool Init(enumMode _mode);
	bool Execute();
	cImage* GetImagePtr() {return image;}
	int GetNumberOfCPUs() {return totalNumberOfCPUs;};
	void UseSizeFromImage(bool mode) {useSizeFromImage = mode;}
	void ChangeCameraTargetPosition(cCameraTarget &cameraTarget);

	void UpdateParameters(const cParameterContainer *_params, const cFractalContainer *_fractal);
	void SetMaxRenderTime(double time);
	static int GetRunningJobCount() {return runningJobs;}
	void ForceNumberOfThreads(int noOfThreads) {totalNumberOfCPUs = noOfThreads; renderData->numberOfThreads = noOfThreads;}
	void BeQuiet() {beQuiet = true;}

public slots:
	void slotExecute();

private:
	QObject *parentObject;
	bool InitImage(int w, int h);
	void ReduceDetail();

	bool hasQWidget;
	bool inProgress;
	bool ready;
	bool useSizeFromImage;
	cImage *image;
	cFractalContainer *fractalContainer;
	cParameterContainer *paramsContainer;

	enumMode mode;
	int height;
	int totalNumberOfCPUs;
	int width;
	QWidget *imageWidget;
	sRenderData *renderData;
	bool *stopRequest;
	bool beQuiet;

	static int id; //global identifier of actual rendering job
	static int runningJobs;

	signals:
	void finished();
	void fullyRendered();
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void updateImage();
	void SendNetRenderJob(cParameterContainer settings, cFractalContainer fractal, sTextures textures);
	void SendNetRenderSetup(int clientIndex, int id, QList<int> startingPositions);
};



#endif /* RENDER_JOB_HPP_ */
