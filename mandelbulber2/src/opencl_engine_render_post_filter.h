/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2021 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * c++ - render post filers with OpenCL
 */

#ifndef MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_POST_FILTER_H_
#define MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_POST_FILTER_H_

#include <memory>

#include "color_structures.hpp"
#include "include_header_wrapper.hpp"
#include "opencl_engine.h"
#include "region.hpp"

// custom includes
#ifdef USE_OPENCL
#include "opencl/hdr_blur_cl.h"
#include "opencl/chromatic_aberration_cl.h"
#endif // USE_OPENCL

struct sParamRender;
class cImage;

class cOpenClEngineRenderPostFilter : public cOpenClEngine
{
	Q_OBJECT

public:
	cOpenClEngineRenderPostFilter(cOpenClHardware *hardware);
	~cOpenClEngineRenderPostFilter() override;

#ifdef USE_OPENCL
	enum enumPostEffectType
	{
		hdrBlur = 0,
		chromaticAberration = 1
	};

	void SetParameters(
		const sParamRender *paramRender, const cRegion<int> &region, enumPostEffectType _effectType);
	bool LoadSourcesAndCompile(std::shared_ptr<const cParameterContainer> params,
		QString *compilerErrorOutput = nullptr) override;
	void RegisterInputOutputBuffers(std::shared_ptr<const cParameterContainer> params) override;
	bool AssignParametersToKernelAdditional(uint argIterator, int deviceIndex) override;
	bool ProcessQueue(quint64 pixelsLeft, quint64 pixelIndex);
	bool Render(std::shared_ptr<cImage> image, bool *stopRequest);
	size_t CalcNeededMemory() override;

private:
	const int inputImageIndex = 0;
	const int outputIndex = 0;

	QString GetKernelName() override;

	enumPostEffectType effectType;
	QString effectName;

	sParamsHDRBlurCl paramsHDRBlur;
	sParamsChromaticAberrationCl paramsChromaticAberration;
	cRegion<int> imageRegion;
	sRGBFloat aoColor;
	quint64 numberOfPixels;
#endif

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void updateImage();
};

#endif /* MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_POST_FILTER_H_ */
