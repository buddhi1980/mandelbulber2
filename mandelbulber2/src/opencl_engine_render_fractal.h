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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Robert Pancoast (RobertPancoast77@gmail.com),
 *  Sebastian Jennen (jenzebas@gmail.com)
 *
 * cOpenClEngineRenderFractal - prepares and executes fractal rendering on opencl
 */

#ifndef MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_FRACTAL_H_
#define MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_FRACTAL_H_

#include "fractal_enums.h"
#include "include_header_wrapper.hpp"
#include "opencl_engine.h"
#include "statistics.h"

// custom includes
#ifdef USE_OPENCL
#include "opencl/input_data_structures.h"
#include "opencl/mesh_export_data_cl.h"
#endif // USE_OPENCL

class cImage;
class cFractalContainer;
class cOpenClDynamicData;
class cOpenClTexturesData;
struct sParamRender;
class cNineFractals;
struct sRenderData;

class cOpenClEngineRenderFractal : public cOpenClEngine
{
	Q_OBJECT

public:
	enum enumClRenderEngineMode
	{
		clRenderEngineTypeNone,
		clRenderEngineTypeFast,
		clRenderEngineTypeLimited,
		clRenderEngineTypeFull
	};

	cOpenClEngineRenderFractal(cOpenClHardware *hardware);
	~cOpenClEngineRenderFractal() override;

#ifdef USE_OPENCL
	bool LoadSourcesAndCompile(const cParameterContainer *params) override;
	void SetParameters(const cParameterContainer *paramContainer,
		const cFractalContainer *fractalContainer, sParamRender *paramRender, cNineFractals *fractals,
		sRenderData *renderData, bool meshExportModeEnable);
	void RegisterInputOutputBuffers(const cParameterContainer *params) override;
	bool PreAllocateBuffers(const cParameterContainer *params) override;
	bool PrepareBufferForBackground(sRenderData *renderData);
	bool AssignParametersToKernelAdditional(int argIterator) override;
	bool WriteBuffersToQueue();
	bool ProcessQueue(size_t jobX, size_t jobY, size_t pixelsLeftX, size_t pixelsLeftY);
	bool ReadBuffersFromQueue();

	// render 3D fractal
	bool Render(cImage *image, bool *stopRequest, sRenderData *renderData);

	// render 2D slice with fractal
	bool Render(double *distances, double *colors, int sliceIndex, bool *stopRequest,
		sRenderData *renderData, size_t dataOffset);

	QList<QPoint> calculateOptimalTileSequence(int gridWidth, int gridHeight);
	static bool sortByCenterDistanceAsc(
		const QPoint &v1, const QPoint &v2, int gridWidth, int gridHeight);

	void MarkCurrentPendingTile(cImage *image, QRect corners);
	void ReleaseMemory();
	size_t CalcNeededMemory() override;
	void SetMeshExportParameters(const sClMeshExport *meshParams);

private:
	const int outputIndex = 0;
	const int outputMeshDistancesIndex = 0;
	const int outputMeshColorsIndex = 1;

	QString GetKernelName() override;

	static QString toCamelCase(const QString &s);

	QScopedPointer<sClInConstants> constantInBuffer;
	QScopedPointer<cl::Buffer> inCLConstBuffer;

	QScopedPointer<sClMeshExport> constantInMeshExportBuffer;
	QScopedPointer<cl::Buffer> inCLConstMeshExportBuffer;

	QByteArray inBuffer;
	QScopedPointer<cl::Buffer> inCLBuffer;

	QByteArray inTextureBuffer;
	QScopedPointer<cl::Buffer> inCLTextureBuffer;

	QScopedPointer<cl::Image2D> backgroundImage2D;
	QScopedArrayPointer<cl_uchar4> backgroungImageBuffer;

	QScopedPointer<cOpenClDynamicData> dynamicData;
	QScopedPointer<cOpenClTexturesData> texturesData;

	QStringList listOfUsedFormulas;

	enumClRenderEngineMode renderEngineMode;

	bool autoRefreshMode;
	bool monteCarlo;
	bool meshExportMode;

#endif

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void updateImage();
	void updateStatistics(cStatistics);
};

#endif /* MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_FRACTAL_H_ */
