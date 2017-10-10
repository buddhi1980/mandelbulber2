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
 * c++ - opencl connector for the DOF Phase 1 OpenCL renderer
 */

#ifndef MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_DOF_PHASE2_H_
#define MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_DOF_PHASE2_H_

#include "dof.hpp"
#include "include_header_wrapper.hpp"
#include "opencl_engine.h"

// custom includes
#ifdef USE_OPENCL
#include "opencl/dof_cl.h"
#endif // USE_OPENCL

struct sParamRender;
class cImage;

class cOpenClEngineRenderDOFPhase2 : public cOpenClEngine
{
	Q_OBJECT

public:
	cOpenClEngineRenderDOFPhase2(cOpenClHardware *_hardware);
	~cOpenClEngineRenderDOFPhase2();

#ifdef USE_OPENCL
	void SetParameters(const sParamRender *paramRender);
	bool LoadSourcesAndCompile(const cParameterContainer *params) override;
	bool PreAllocateBuffers(const cParameterContainer *params) override;
	bool AssignParametersToKernel();
	bool WriteBuffersToQueue();
	bool ProcessQueue(qint64 pixelsLeft, qint64 pixelIndex);
	bool ReadBuffersFromQueue();
	bool Render(cImage *image, cPostRenderingDOF::sSortZ<float> *sortedZBuffer, bool *stopRequest);
	void ReleaseMemory();
	size_t CalcNeededMemory() override;

private:
	QString GetKernelName() override;

	sParamsDOF paramsDOF;

	sSortedZBufferCl *inZBufferSorted;
	cl::Buffer *inCLZBufferSorted;

	cl_float4 *inImageBuffer;
	cl::Buffer *inCLImageBuffer;

	int numberOfPixels;

	cl_float4 *outBuffer;
	cl::Buffer *outCl;

#endif

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
};

#endif /* MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_DOF_PHASE2_H_ */
