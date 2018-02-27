/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * c++ - opencl connector for the DOF OpenCL renderer
 */

#include "opencl_engine_render_dof.h"

#include "cimage.hpp"
#include "common_math.h"
#include "dof.hpp"
#include "fractparams.hpp"
#include "opencl_engine_render_dof_phase1.h"
#include "opencl_engine_render_dof_phase2.h"
#include "opencl_global.h"
#include "opencl_hardware.h"
#include "parameters.hpp"
#include "progress_text.hpp"
#include "render_data.hpp"
#include "system.hpp"

cOpenClEngineRenderDOF::cOpenClEngineRenderDOF(cOpenClHardware *hardware) : QObject(hardware)
{
#ifdef USE_OPENCL
	dofEnginePhase1 = new cOpenClEngineRenderDOFPhase1(hardware);
	dofEnginePhase2 = new cOpenClEngineRenderDOFPhase2(hardware);

	connect(dofEnginePhase1,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
	connect(dofEnginePhase2,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));

#endif
}

cOpenClEngineRenderDOF::~cOpenClEngineRenderDOF()
{
#ifdef USE_OPENCL
	delete dofEnginePhase1;
	delete dofEnginePhase2;
#endif
}

#ifdef USE_OPENCL
void cOpenClEngineRenderDOF::Reset()
{
	dofEnginePhase1->Reset();
	dofEnginePhase2->Reset();
}

bool cOpenClEngineRenderDOF::RenderDOF(const sParamRender *paramRender,
	const cParameterContainer *params, cImage *image, bool *stopRequest, cRegion<int> screenRegion)
{
	int numberOfPasses = paramRender->DOFNumberOfPasses;

	emit updateProgressAndStatus(QObject::tr("OpenCL DOF"), QObject::tr("Initializing Phase 1"), 0.0);

	cProgressText progressText;
	progressText.ResetTimer();

	dofEnginePhase1->Lock();
	dofEnginePhase1->SetParameters(paramRender);
	bool result = false;
	if (dofEnginePhase1->LoadSourcesAndCompile(params))
	{
		dofEnginePhase1->CreateKernel4Program(params);
		qint64 neededMem = dofEnginePhase1->CalcNeededMemory();
		WriteLogDouble("OpenCl render DOF Phase 1 - needed mem:", neededMem / 1048576.0, 2);
		if (neededMem / 1048576 < params->Get<int>("opencl_memory_limit"))
		{
			dofEnginePhase1->PreAllocateBuffers(params);
			dofEnginePhase1->CreateCommandQueue();
			result = dofEnginePhase1->Render(image, stopRequest);
		}
		else
		{
			qCritical() << "Not enough GPU mem!";
			result = false;
		}
	}
	dofEnginePhase1->ReleaseMemory();
	dofEnginePhase1->Unlock();

	if (!result)
	{
		cPostRenderingDOF dof(image);
		connect(&dof, SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
			SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)));
		connect(&dof, SIGNAL(updateImage), this, SIGNAL(updateImage()));

		dof.Render(screenRegion,
			paramRender->DOFRadius * (image->GetWidth() + image->GetHeight()) / 2000.0,
			paramRender->DOFFocus, paramRender->DOFNumberOfPasses, paramRender->DOFBlurOpacity,
			paramRender->DOFMaxRadius, stopRequest);

		// refresh image at end
		WriteLog("image->CompileImage()", 2);
		image->CompileImage();

		if (image->IsPreview())
		{
			WriteLog("image->ConvertTo8bit()", 2);
			image->ConvertTo8bit();
			WriteLog("image->UpdatePreview()", 2);
			image->UpdatePreview();
			WriteLog("image->GetImageWidget()->update()", 2);
			emit updateImage();
		}
		result = true;
		return result;
	}

	emit updateProgressAndStatus(QObject::tr("OpenCL DOF"), QObject::tr("Sorting Z-Buffer"), 0.0);

	quint64 numberOfPixels = quint64(image->GetWidth()) * quint64(image->GetHeight());
	cPostRenderingDOF::sSortZ<float> *tempSort = new cPostRenderingDOF::sSortZ<float>[numberOfPixels];

	for (quint64 index = 0; index < numberOfPixels; index++)
	{
		tempSort[index].z = image->GetZBufferPtr()[index];
		tempSort[index].i = index;
	}

	// sorting z-buffer
	emit updateProgressAndStatus(QObject::tr("OpenCL DOF"), QObject::tr("Sorting Z-Buffer"), 0.0);

	cPostRenderingDOF::QuickSortZBuffer(tempSort, 1, numberOfPixels - 1);

	for (int pass = 0; pass < numberOfPasses; pass++)
	{

		float neutral = paramRender->DOFFocus;
		float deep =
			paramRender->DOFRadius * (paramRender->imageWidth + paramRender->imageHeight) / 2000.0;

		emit updateProgressAndStatus(
			QObject::tr("OpenCL DOF"), QObject::tr("Randomizing Z-Buffer"), 0.0);

// Randomize Z-buffer
#pragma omp parallel for schedule(dynamic, 1)
		for (qint64 i = numberOfPixels - 1; i >= 0; i--)
		{
			cPostRenderingDOF::sSortZ<float> temp;
			temp = tempSort[i];
			float z1 = temp.z;
			float size1 = (z1 - neutral) / z1 * deep;

			qint64 randomStep = i;

			bool done = false;
			qint64 ii;
			do
			{
				ii = i - Random(randomStep);
				if (ii <= 0) ii = 0;
				cPostRenderingDOF::sSortZ<float> temp2 = tempSort[ii];
				float z2 = temp2.z;
				float size2 = (z2 - neutral) / z2 * deep;

				if (size1 * size2 > 0)
				{
					float sizeCompare;
					if (size1 > 0)
					{
						sizeCompare = size2 / size1;
					}
					else
					{
						sizeCompare = size1 / size2;
					}

					int intDiff = int((1.0f - sizeCompare) * 500);
					intDiff *= intDiff;
					if (intDiff < Random(10000))
					{
						done = true;
					}
					else
					{
						done = false;
					}
				}
				else
				{
					done = false;
				}
				randomStep = int(randomStep * 0.7 - 1.0);

				if (randomStep <= 0) done = true;
			} while (!done);
			tempSort[i] = tempSort[ii];
			tempSort[ii] = temp;
		}

		emit updateProgressAndStatus(
			QObject::tr("OpenCL DOF"), QObject::tr("Initializing Phase 2"), 0.0);

		if (!*stopRequest)
		{
			dofEnginePhase2->Lock();
			dofEnginePhase2->SetParameters(paramRender);
			if (dofEnginePhase2->LoadSourcesAndCompile(params))
			{
				dofEnginePhase2->CreateKernel4Program(params);
				qint64 neededMem = dofEnginePhase2->CalcNeededMemory();
				WriteLogDouble("OpenCl render DOF Phase 2 - needed mem:", neededMem / 1048576.0, 2);
				if (neededMem / 1048576 < params->Get<int>("opencl_memory_limit"))
				{
					dofEnginePhase2->PreAllocateBuffers(params);
					dofEnginePhase2->CreateCommandQueue();
					result = dofEnginePhase2->Render(image, tempSort, stopRequest);
				}
				else
				{
					qCritical() << "Not enough GPU mem!";
				}
			}
			dofEnginePhase2->ReleaseMemory();
			dofEnginePhase2->Unlock();
		}
	} // next pass

	emit updateProgressAndStatus(tr("OpenCL DOF finished"), progressText.getText(1.0), 1.0);

	delete[] tempSort;

	return result;
}
#endif
