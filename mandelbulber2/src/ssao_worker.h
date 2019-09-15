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
 * cSSAOWorker class - worker for multi thread SSAO effect
 *
 * SSAO shades inner edges and corners. For each pixel the surrounding
 * z-buffer (depth from camera) is scanned and based on the angle of the surface
 * each pixel will be shaded. This class gets used in render_ssao.cpp as multiple threads.
 */

#ifndef MANDELBULBER2_SRC_SSAO_WORKER_H_
#define MANDELBULBER2_SRC_SSAO_WORKER_H_

#include <qobject.h>

#include <QList>
#include <QThread>

#include "color_structures.hpp"
#include "region.hpp"

// forward declarations
struct sParamRender;
struct sRenderData;
class cImage;

class cSSAOWorker : public QObject
{
	Q_OBJECT
public:
	struct sThreadData
	{
		int startLine;
		int noOfThreads;
		int quality;
		int progressive;
		sRGBFloat color;
		unsigned int done;
		bool stopRequest;
		QList<int> *list;
		cRegion<int> region;
	};

	cSSAOWorker(const sParamRender *_params, sThreadData *_threadData, const sRenderData *_data,
		cImage *_image);
	~cSSAOWorker() override;

	QThread workerThread;

	// data got from main thread
	const sParamRender *params;
	const sRenderData *data;
	sThreadData *threadData;
	cImage *image;

public slots:
	void doWork();

signals:
	void finished();
};

#endif /* MANDELBULBER2_SRC_SSAO_WORKER_H_ */
