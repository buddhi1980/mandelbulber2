/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-21 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <memory>

#include "fractal_enums.h"
#include "include_header_wrapper.hpp"
#include "opencl_engine.h"
#include "opencl_worker_output_queue.h"
#include "rendered_tile_data.hpp"
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
class cOpenClScheduler;
class cOpenCLWorkerOutputQueue;
class cOpenClWorkerThread;
class cDenoiser;

class cOpenClEngineRenderFractal : public cOpenClEngine
{
	Q_OBJECT

public:
	enum enumClRenderEngineMode
	{
		clRenderEngineTypeNone = 0,
		clRenderEngineTypeFast = 1,
		clRenderEngineTypeLimited = 2,
		clRenderEngineTypeFull = 3
	};

	struct sConcurentTileProcessSharedData
	{
		bool firstBlurcalculated;
		int tilesRenderedCounter;
		int lastMonteCarloLoop;
		qint64 pixelsRendered;
		quint64 maskedPixelsCounter;
		qint64 doneMCpixels;
		double doneMC;
		std::vector<bool> donePixelsMask;
		std::vector<float> pixelNoiseBuffer;
		std::vector<float> noiseTable;

		// list of latest rendered tiles - needed for image refreshing
		QList<sRenderedTileData> listOfRenderedTilesData;
		QList<QRect> lastRenderedRects;

		// common mutex for concurent tasks
		QMutex mutex;
	};

	struct sConcurentTileProcessInputData
	{
		bool useAntiAliasing;
		bool pixelLevelOptimization;
		bool useDenoiser;
		bool monteCarlo;
		bool useOptionalImageChannels;
		int minNumberOfSamples;
		int outputIndex;
		float noiseTarget;
		double noiseFilterFactor;
		quint64 width;
		quint64 height;
		quint64 gridWidth;
		quint64 gridHeight;
		quint64 numberOfPixels;
		QVector<int> aaSampleNumberTable;
		cOpenClEngine::sOptimalJob *optimalJob;
	};

#ifdef USE_OPENCL
	struct sConcurentTileProcess
	{
		cOpenCLWorkerOutputQueue::sClSingleOutput tile;
		sConcurentTileProcessSharedData *inOut;
		sConcurentTileProcessInputData *in;
		std::shared_ptr<cImage> image;
		std::shared_ptr<cOpenClScheduler> scheduler;
		std::shared_ptr<cDenoiser> denoiser;
		sRenderData *renderData;
		sParamRenderCl *params;
		std::vector<bool> *pixelMask;
	};
#endif

	cOpenClEngineRenderFractal(cOpenClHardware *_hardware);
	~cOpenClEngineRenderFractal() override;

#ifdef USE_OPENCL
	bool LoadSourcesAndCompile(std::shared_ptr<const cParameterContainer> params,
		QString *compilerErrorOutput = nullptr) override;
	void SetParameters(std::shared_ptr<const cParameterContainer> paramContainer,
		std::shared_ptr<const cFractalContainer> fractalContainer,
		std::shared_ptr<sParamRender> paramRender, std::shared_ptr<cNineFractals> fractals,
		std::shared_ptr<sRenderData> renderData, bool meshExportModeEnable);
	void SetDistanceMode() { distanceMode = true; }
	void RegisterInputOutputBuffers(std::shared_ptr<const cParameterContainer> params) override;
	bool PreAllocateBuffers(std::shared_ptr<const cParameterContainer> params) override;
	bool PrepareBufferForBackground(sRenderData *renderData);
	bool AssignParametersToKernelAdditional(uint argIterator, int deviceIndex) override;
	bool WriteBuffersToQueue();
	bool ProcessQueue(quint64 jobX, quint64 jobY, quint64 pixelsLeftX, quint64 pixelsLeftY);
	bool ReadBuffersFromQueue();

	// render 3D fractal
	bool Render(std::shared_ptr<cImage> image, bool *stopRequest, sRenderData *renderData);
	// render 3D fractal
	bool RenderMulti(std::shared_ptr<cImage> image, bool *stopRequest, sRenderData *renderData);
	// calculate distance using OpenCL
	float CalculateDistance(CVector3 point);

	// render 2D slice with fractal
	bool Render(std::vector<double> *distances, std::vector<double> *colors,
		std::vector<int> *iterations, int sliceIndex, bool *stopRequest, sRenderData *renderData,
		size_t dataOffset);

	QList<QPoint> calculateOptimalTileSequence(int gridWidth, int gridHeight);
	static bool sortByCenterDistanceAsc(
		const QPoint &v1, const QPoint &v2, int gridWidth, int gridHeight);

	void ReleaseMemory();
	size_t CalcNeededMemory() override;
	void SetMeshExportParameters(const sClMeshExport *meshParams);

	static inline sRGBFloat clFloat3TosRGBFloat(const cl_float3 &clPixel)
	{
		return sRGBFloat(clPixel.s0, clPixel.s1, clPixel.s2);
	}

private:
	const int outputIndex = 0;
	const int outputMeshDistancesIndex = 0;
	const int outputMeshColorsIndex = 1;
	const int outputMeshIterationsIndex = 2;

	QString GetKernelName() override;

	static QString toCamelCase(const QString &s);
	void CreateListOfHeaderFiles(QStringList &clHeaderFiles);
	void CreateListOfIncludes(const QStringList &clHeaderFiles, const QString &openclPathSlash,
		std::shared_ptr<const cParameterContainer> params, const QString &openclEnginePath,
		QByteArray &programEngine);
	void LoadSourceWithMainEngine(const QString &openclEnginePath, QByteArray &programEngine);
	void SetParametersForDistanceEstimationMethod(cNineFractals *fractals, sParamRender *paramRender);
	void CreateListOfUsedFormulas(
		cNineFractals *fractals, std::shared_ptr<const cFractalContainer> fractalContainer);
	void SetParametersForPerspectiveProjection(sParamRender *paramRender);
	void SetParametersForShaders(sParamRender *paramRender, sRenderData *renderData);
	void SetParametersForStereoscopic(sRenderData *renderData);
	QMap<QString, int> SetParametersAndDataForTextures(sRenderData *renderData);
	void SetParametersAndDataForMaterials(
		const QMap<QString, int> &textureIndexes, sRenderData *renderData, sParamRender *paramRender);
	void DynamicDataForAOVectors(std::shared_ptr<const sParamRender> paramRender,
		std::shared_ptr<const cNineFractals> fractals, std::shared_ptr<sRenderData> renderData);
	void SetParametersForIterationWeight(cNineFractals *fractals);
	void CreateThreadsForOpenCLWorkers(int numberOfOpenCLWorkers,
		const std::shared_ptr<cOpenClScheduler> &scheduler, quint64 width, quint64 height,
		const std::shared_ptr<cOpenCLWorkerOutputQueue> &outputQueue, int numberOfSamples,
		int antiAliasingDepth, QList<std::shared_ptr<QThread>> &threads,
		QList<std::shared_ptr<cOpenClWorkerThread>> &workers, bool *stopRequest);

	static sRGBFloat MCMixColor(const cOpenCLWorkerOutputQueue::sClSingleOutput &output,
		const sRGBFloat &pixel, const sRGBFloat &oldPixel);
	static void PutMultiPixel(quint64 xx, quint64 yy, const sRGBFloat &newPixel,
		unsigned short newAlpha, const sRGB8 &color, float zDepth, const sRGBFloat &normalWorld,
		unsigned short opacity, cImage *image);
	static void PutMultiPixelOptional(quint64 xx, quint64 yy, sRGB8 color, const sRGBFloat &normal,
		const sRGBFloat &specular, const sRGBFloat &world, const sRGBFloat &shadows,
		const sRGBFloat &gi, const sRGBFloat &notDenoised, std::shared_ptr<cImage> &image);

	int PeriodicRefreshOfTiles(int lastRefreshTime, QElapsedTimer &timerImageRefresh,
		std::shared_ptr<cImage> image, QList<QRect> &lastRenderedRects,
		QList<sRenderedTileData> &listOfRenderedTilesData);
	void FinallRefreshOfImage(QList<QRect> lastRenderedRects, std::shared_ptr<cImage> image);
	static void ConcurentProcessTile(sConcurentTileProcess &data);

	std::unique_ptr<sClInConstants> constantInBuffer;
	QList<std::shared_ptr<cl::Buffer>> inCLConstBuffer;

	std::unique_ptr<sClMeshExport> constantInMeshExportBuffer;
	QList<std::shared_ptr<cl::Buffer>> inCLConstMeshExportBuffer;

	// FIXME: replace QByteArray with std::vector
	QByteArray inBuffer;
	QList<std::shared_ptr<cl::Buffer>> inCLBuffer;

	QByteArray inTextureBuffer;
	QList<std::shared_ptr<cl::Buffer>> inCLTextureBuffer;

	QList<std::shared_ptr<cl::Image2D>> backgroundImage2D;
	std::vector<cl_float4> backgroundImageBuffer;

	std::vector<cl_char> perlinNoiseSeeds;
	QList<std::shared_ptr<cl::Buffer>> inCLPerlinNoiseSeedsBuffer;
	const int perlinNoiseArraySize = 512;

	std::unique_ptr<cOpenClDynamicData> dynamicData;
	std::unique_ptr<cOpenClTexturesData> texturesData;

	std::vector<bool> pixelMask;

	QStringList listOfUsedFormulas;
	QStringList customFormulaCodes;

	enumClRenderEngineMode renderEngineMode;

	bool autoRefreshMode;
	bool monteCarlo;
	bool meshExportMode;
	cl_float3 pointToCalculateDistance;
	bool distanceMode;
	bool useOptionalImageChannels;
	double reservedGpuTime;

#endif

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
	void updateImage();
	void updateStatistics(cStatistics);
	void sendRenderedTilesList(QList<sRenderedTileData>);
	void signalSmallPartRendered(double time);
};

#endif /* MANDELBULBER2_SRC_OPENCL_ENGINE_RENDER_FRACTAL_H_ */
