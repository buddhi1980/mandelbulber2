/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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

cOpenClEngineRenderDOF::cOpenClEngineRenderDOF(cOpenClHardware *hardware) : QObject(hardware)
{
#ifdef USE_OPENCL
	dofEnginePhase1 = new cOpenClEngineRenderDOFPhase1(hardware);
	dofEnginePhase2 = new cOpenClEngineRenderDOFPhase2(hardware);
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
bool cOpenClEngineRenderDOF::RenderDOF(const sParamRender *paramRender,
	const cParameterContainer *params, cImage *image, bool *stopRequest)
{
	int numberOfPasses = paramRender->DOFNumberOfPasses;

	dofEnginePhase1->Lock();
	dofEnginePhase1->SetParameters(paramRender);
	bool result = false;
	if (dofEnginePhase1->LoadSourcesAndCompile(params))
	{
		dofEnginePhase1->CreateKernel4Program(params);
		dofEnginePhase1->PreAllocateBuffers(params);
		dofEnginePhase1->CreateCommandQueue();
		result = dofEnginePhase1->Render(image, stopRequest);
	}
	dofEnginePhase1->ReleaseMemory();
	dofEnginePhase1->Unlock();

	quint64 numberOfPixels = quint64(image->GetWidth()) * quint64(image->GetHeight());
	cPostRenderingDOF::sSortZ<float> *tempSort = new cPostRenderingDOF::sSortZ<float>[numberOfPixels];

	for (quint64 index = 0; index < numberOfPixels; index++)
	{
		tempSort[index].z = image->GetZBufferPtr()[index];
		tempSort[index].i = index;
	}

	// sorting z-buffer
	cPostRenderingDOF::QuickSortZBuffer(tempSort, 1, numberOfPixels - 1);

	for (int pass = 0; pass < numberOfPasses; pass++)
	{

		float neutral = paramRender->DOFFocus;
		float deep =
			paramRender->DOFRadius * (paramRender->imageWidth + paramRender->imageHeight) / 2000.0;

		// Randomize Z-buffer
		for (qint64 i = numberOfPixels - 1; i >= 0; i--)
		{
			if (*stopRequest) throw tr("DOF terminated");
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

		dofEnginePhase2->Lock();
		dofEnginePhase2->SetParameters(paramRender);
		if (dofEnginePhase2->LoadSourcesAndCompile(params))
		{
			dofEnginePhase2->CreateKernel4Program(params);
			dofEnginePhase2->PreAllocateBuffers(params);
			dofEnginePhase2->CreateCommandQueue();
			result = dofEnginePhase2->Render(image, tempSort, stopRequest);
		}
		dofEnginePhase2->ReleaseMemory();
		dofEnginePhase2->Unlock();

	} // next pass

	delete[] tempSort;

	return result;
}
#endif
