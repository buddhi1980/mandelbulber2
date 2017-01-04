/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#ifndef MANDELBULBER2_SRC_RENDER_JOB_HPP_
#define MANDELBULBER2_SRC_RENDER_JOB_HPP_

#ifndef _USE_MATH_DEFINES
#define _USE_MATH_DEFINES
#endif

#include <QObject>

#include "camera_target.hpp"
#include "statistics.h"
#include "parameters.hpp"
#include "fractal_container.hpp"

// forward declarations
class cImage;
struct sRenderData;
class cRenderingConfiguration;
struct sImageOptional;

class cRenderJob : public QObject
{
	Q_OBJECT
public:
	cRenderJob(const cParameterContainer *_params, const cFractalContainer *_fractal, cImage *_image,
		bool *_stopRequest, QWidget *_qwidget = nullptr);
	~cRenderJob();
	// QWidtet *parent is needed to connect signals for refreshing progress and status bar.
	// If _parent is not nullptr then parent has to have slot slotUpdateProgressAndStatus()

	enum enumMode
	{
		still,
		keyframeAnim,
		flightAnim,
		flightAnimRecord
	};

	bool Init(enumMode _mode, const cRenderingConfiguration &config);
	bool Execute();
	cImage *GetImagePtr() const { return image; }
	int GetNumberOfCPUs() const { return totalNumberOfCPUs; }
	void UseSizeFromImage(bool mode) { useSizeFromImage = mode; }
	void ChangeCameraTargetPosition(cCameraTarget &cameraTarget) const;

	void UpdateParameters(const cParameterContainer *_params, const cFractalContainer *_fractal);
	void UpdateConfig(const cRenderingConfiguration &config) const;
	static int GetRunningJobCount() { return runningJobs; }
	cStatistics GetStatistics() const;

public slots:
	void slotExecute();

private:
	bool InitImage(int w, int h, const sImageOptional &optional);
	void PrepareData(const cRenderingConfiguration &config);
	void ReduceDetail() const;
	QStringList CreateListOfUsedTextures() const;

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
	bool canUseNetRender;

	static int id; // global identifier of actual rendering job
	static int runningJobs;

signals:
	void finished();
	void fullyRendered(const QString &text, const QString &progressText);
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void updateStatistics(cStatistics statistics);
	void updateImage();
	void SendNetRenderJob(
		cParameterContainer settings, cFractalContainer fractal, QStringList listOfTextures);
	void SendNetRenderSetup(int clientIndex, int id, QList<int> startingPositions);
	void SetMinimumWidgetSize(int width, int height);
};

#endif /* MANDELBULBER2_SRC_RENDER_JOB_HPP_ */
