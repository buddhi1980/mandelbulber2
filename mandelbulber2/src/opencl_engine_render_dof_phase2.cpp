/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * c++ - opencl connector for the DOF Phase1 OpenCL renderer
 */

#include "opencl_engine_render_dof_phase2.h"

#include "cimage.hpp"
#include "dof.hpp"
#include "files.h"
#include "fractparams.hpp"
#include "global_data.hpp"
#include "opencl_hardware.h"
#include "parameters.hpp"
#include "progress_text.hpp"
#include "system_data.hpp"
#include "system_directories.hpp"
#include "write_log.hpp"

cOpenClEngineRenderDOFPhase2::cOpenClEngineRenderDOFPhase2(cOpenClHardware *_hardware)
		: cOpenClEngine(_hardware)
{
#ifdef USE_OPENCL
	paramsDOF.height = 0;
	paramsDOF.width = 0;
	paramsDOF.height = 0;
	paramsDOF.radius = 0.0;
	paramsDOF.blurOpacity = 0.0;
	numberOfPixels = 0;
	tilesX = 0;
	tilesY = 0;
	numberOfTiles = 0;
	maxBatchEntries = 0;
	tileListCapacity = 0;
	optimalJob.sizeOfPixel = 0; // memory usage doens't depend on job size
	optimalJob.optimalProcessingCycle = 0.5;
#endif
}

cOpenClEngineRenderDOFPhase2::~cOpenClEngineRenderDOFPhase2()
{
#ifdef USE_OPENCL
	ReleaseMemory();
#endif
}

#ifdef USE_OPENCL

QString cOpenClEngineRenderDOFPhase2::GetKernelName()
{
	return QString("DOFPhase2");
}

void cOpenClEngineRenderDOFPhase2::SetParameters(
	const sParamRender *paramRender, const cRegion<int> &region)
{
	imageRegion = region;
	paramsDOF.width = region.width;
	paramsDOF.height = region.height;
	paramsDOF.deep = paramRender->DOFRadius * (paramsDOF.width + paramsDOF.height) / 2000.0;
	paramsDOF.neutral = paramRender->DOFFocus;
	paramsDOF.blurOpacity = paramRender->DOFBlurOpacity;
	paramsDOF.maxRadius = paramRender->DOFMaxRadius;

	numberOfPixels = quint64(paramsDOF.width) * quint64(paramsDOF.height);

	tilesX = (paramsDOF.width + DOF_PHASE2_TILE_SIZE - 1) / DOF_PHASE2_TILE_SIZE;
	tilesY = (paramsDOF.height + DOF_PHASE2_TILE_SIZE - 1) / DOF_PHASE2_TILE_SIZE;
	numberOfTiles = quint64(tilesX) * quint64(tilesY);
	// every entry adds at least one item to the tile lists
	tileListCapacity = numberOfTiles * maxTileListAverage;
	maxBatchEntries = qMin(numberOfPixels, tileListCapacity);

	definesCollector.clear();
}

bool cOpenClEngineRenderDOFPhase2::LoadSourcesAndCompile(
	std::shared_ptr<const cParameterContainer> params, QString *compilerErrorOutput)
{
	programsLoaded = false;
	readyForRendering = false;
	emit updateProgressAndStatus(
		tr("OpenCL DOF - initializing"), tr("Compiling sources for DOF phase 2"), 0.0);

	QString openclPath = systemDirectories.sharedDir + "opencl" + QDir::separator();
	QString openclEnginePath = openclPath + "engines" + QDir::separator();

	QByteArray programEngine;
	// pass through define constants
	programEngine.append("#define USE_OPENCL 1\n");

	QStringList clHeaderFiles;
	clHeaderFiles.append("opencl_typedefs.h"); // definitions of common opencl types
	clHeaderFiles.append("dof_cl.h");					 // main data structures

	for (int i = 0; i < clHeaderFiles.size(); i++)
	{
		programEngine.append(StripNonAscii(LoadUtf8TextFromFile(openclPath + clHeaderFiles.at(i))));
		programEngine.append("\n");
	}

	QString engineFileName = "dof_phase2.cl";
	QString engineFullFileName = openclEnginePath + engineFileName;
	programEngine.append(LoadUtf8TextFromFile(engineFullFileName));

	SetUseFastRelaxedMath(params->Get<bool>("opencl_use_fast_relaxed_math"));

	// building OpenCl kernel
	QString errorString;

	QElapsedTimer timer;
	timer.start();
	if (Build(programEngine, &errorString, false))
	{
		programsLoaded = true;
	}
	else
	{
		programsLoaded = false;
		WriteLog(errorString, 0);
	}

	if (compilerErrorOutput) *compilerErrorOutput = errorString;

	WriteLogDouble(
		"cOpenClEngineRenderDOFPhase2: Opencl DOF build time [s]", timer.nsecsElapsed() / 1.0e9, 2);

	return programsLoaded;
}

void cOpenClEngineRenderDOFPhase2::RegisterInputOutputBuffers(
	std::shared_ptr<const cParameterContainer> params)
{
	Q_UNUSED(params);
	inputBuffers[0] << sClInputOutputBuffer(sizeof(sDOFEntryCl), maxBatchEntries, "DOF entries");
	inputBuffers[0] << sClInputOutputBuffer(sizeof(cl_int), numberOfTiles + 1, "tile list starts");
	inputBuffers[0] << sClInputOutputBuffer(sizeof(cl_int), tileListCapacity, "tile lists");
	inputAndOutputBuffers[0] << sClInputOutputBuffer(
		sizeof(cl_float4), numberOfPixels, "image buffer");
}

bool cOpenClEngineRenderDOFPhase2::AssignParametersToKernelAdditional(
	uint argIterator, int deviceIndex)
{
	int err = clKernels.at(deviceIndex)->setArg(argIterator++, paramsDOF); // pixel offset
	if (!checkErr(err, "kernel->setArg(2, pixelIndex)"))
	{
		emit EmitErrorMessage(
			QObject::tr("Cannot set OpenCL argument for %1").arg(QObject::tr("DOF params")),
			cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

bool cOpenClEngineRenderDOFPhase2::WriteBatchToQueue(quint64 numberOfEntries, quint64 listLength)
{
	const quint64 sizes[3] = {numberOfEntries * sizeof(sDOFEntryCl),
		(numberOfTiles + 1) * sizeof(cl_int), listLength * sizeof(cl_int)};
	const int indexes[3] = {entriesIndex, tileStartIndex, tileListIndex};

	for (int n = 0; n < 3; n++)
	{
		sClInputOutputBuffer &buffer = inputBuffers[0][indexes[n]];
		if (sizes[n] == 0) continue;
		cl_int err =
			clQueues.at(0)->enqueueWriteBuffer(*buffer.clPtr, CL_TRUE, 0, sizes[n], buffer.ptr.get());
		if (!checkErr(err, "CommandQueue::enqueueWriteBuffer(...) for " + buffer.name))
		{
			emit EmitErrorMessage(QObject::tr("Cannot enqueue writing OpenCL %1").arg(buffer.name),
				cErrorMessage::errorMessage, nullptr);
			return false;
		}
	}
	return true;
}

bool cOpenClEngineRenderDOFPhase2::ProcessQueue()
{
	// one work-item per pixel, work groups cover whole tiles when the device allows it
	cl::NDRange globalRange(
		quint64(tilesX) * DOF_PHASE2_TILE_SIZE, quint64(tilesY) * DOF_PHASE2_TILE_SIZE);
	cl::NDRange localRange = cl::NullRange;
	if (optimalJob.workGroupSize >= DOF_PHASE2_TILE_SIZE * DOF_PHASE2_TILE_SIZE)
		localRange = cl::NDRange(DOF_PHASE2_TILE_SIZE, DOF_PHASE2_TILE_SIZE);
	else if (optimalJob.workGroupSize >= 64)
		localRange = cl::NDRange(8, 8);

	cl_int err =
		clQueues.at(0)->enqueueNDRangeKernel(*clKernels.at(0), cl::NullRange, globalRange, localRange);
	if (!checkErr(err, "CommandQueue::enqueueNDRangeKernel()"))
	{
		emit EmitErrorMessage(
			QObject::tr("Cannot enqueue OpenCL rendering jobs"), cErrorMessage::errorMessage, nullptr);
		return false;
	}

	err = clQueues.at(0)->finish();
	if (!checkErr(err, "CommandQueue::finish() - enqueueNDRangeKernel"))
	{
		emit EmitErrorMessage(
			QObject::tr("Cannot finish rendering DOF"), cErrorMessage::errorMessage, nullptr);
		return false;
	}

	return true;
}

bool cOpenClEngineRenderDOFPhase2::Render(
	std::shared_ptr<cImage> image, cPostRenderingDOF::sSortZ<float> *sortedZBuffer, bool *stopRequest)
{
	if (programsLoaded)
	{
		// The image resolution determines the total amount of work
		quint64 width = imageRegion.width;
		quint64 height = imageRegion.height;

		cProgressText progressText;
		progressText.ResetTimer();

		emit updateProgressAndStatus(
			tr("OpenCL - rendering DOF - phase 1"), progressText.getText(0.0), 0.0);

		QElapsedTimer timer;
		timer.start();

		// copy image to the output buffer
		for (quint64 y = 0; y < height; y++)
		{
			for (quint64 x = 0; x < width; x++)
			{
				quint64 i = x + y * width;
				sRGBFloat imagePixel = image->GetPixelPostImage(imageRegion.x1 + x, imageRegion.y1 + y);
				float alpha = image->GetPixelAlpha(imageRegion.x1 + x, imageRegion.y1 + y) / 65535.0f;
				reinterpret_cast<cl_float4 *>(inputAndOutputBuffers[0][outputIndex].ptr.get())[i] =
					cl_float4{{imagePixel.R, imagePixel.G, imagePixel.B, alpha}};
			}
		}

		// writing data to queue
		if (!WriteBuffersToQueue()) return false;

		sDOFEntryCl *entries = reinterpret_cast<sDOFEntryCl *>(inputBuffers[0][entriesIndex].ptr.get());
		cl_int *tileStart = reinterpret_cast<cl_int *>(inputBuffers[0][tileStartIndex].ptr.get());
		cl_int *tileList = reinterpret_cast<cl_int *>(inputBuffers[0][tileListIndex].ptr.get());
		std::vector<cl_int> tileCount(numberOfTiles);

		// tiles covered by the bounding box of an entry's disc
		auto entryTiles = [&](const sDOFEntryCl &entry, int &tx1, int &tx2, int &ty1, int &ty2)
		{
			tx1 = qMax(entry.x - entry.size, 0) / DOF_PHASE2_TILE_SIZE;
			tx2 = qMin(entry.x + entry.size, int(width) - 1) / DOF_PHASE2_TILE_SIZE;
			ty1 = qMax(entry.y - entry.size, 0) / DOF_PHASE2_TILE_SIZE;
			ty2 = qMin(entry.y + entry.size, int(height) - 1) / DOF_PHASE2_TILE_SIZE;
		};

		// The entries are applied in their sorted order, in batches. Each batch gets for every tile
		// the list of its entries that can reach the tile, in the same order, and every pixel applies
		// them in that order. So every pixel is blended in the same order as in
		// cPostRenderingDOF::Render() and in every render. The batch size limits the work per pixel
		// and kernel run.
		quint64 entryIndex = 0;
		while (entryIndex < numberOfPixels)
		{
			quint64 batchEntries = 0;
			quint64 listLength = 0;
			std::fill(tileCount.begin(), tileCount.end(), 0);

			for (; entryIndex < numberOfPixels && batchEntries < maxBatchEntries; entryIndex++)
			{
				const cPostRenderingDOF::sSortZ<float> &sorted =
					sortedZBuffer[numberOfPixels - entryIndex - 1];
				float z = sorted.z;
				if (!(z > 1e-8f)) continue;

				float blur = fabsf(z - paramsDOF.neutral) / z * paramsDOF.deep + 1.0f;
				if (blur > paramsDOF.maxRadius) blur = paramsDOF.maxRadius;

				sDOFEntryCl entry;
				entry.x = int(sorted.i % width);
				entry.y = int(sorted.i / width);
				entry.size = int(blur);
				entry.blur = blur;
				entry.factor = (3.14f * (blur * blur - blur) + 1.0f) / paramsDOF.blurOpacity;

				int tx1, tx2, ty1, ty2;
				entryTiles(entry, tx1, tx2, ty1, ty2);
				quint64 tilesCovered = quint64(tx2 - tx1 + 1) * quint64(ty2 - ty1 + 1);
				bool full = listLength + tilesCovered > tileListCapacity;
				for (int ty = ty1; ty <= ty2 && !full; ty++)
					for (int tx = tx1; tx <= tx2 && !full; tx++)
						if (tileCount[tx + ty * tilesX] >= maxTileListLength) full = true;
				if (full && batchEntries > 0) break;

				for (int ty = ty1; ty <= ty2; ty++)
					for (int tx = tx1; tx <= tx2; tx++)
						tileCount[tx + ty * tilesX]++;
				listLength += tilesCovered;

				sRGBFloat imagePixel =
					image->GetPixelPostImage(imageRegion.x1 + entry.x, imageRegion.y1 + entry.y);
				entry.colorR = imagePixel.R;
				entry.colorG = imagePixel.G;
				entry.colorB = imagePixel.B;
				entry.alpha =
					image->GetPixelAlpha(imageRegion.x1 + entry.x, imageRegion.y1 + entry.y) / 65535.0f;
				entries[batchEntries++] = entry;
			}
			if (batchEntries == 0) break;

			// tile lists: start offsets, then the entries in their order
			tileStart[0] = 0;
			for (quint64 t = 0; t < numberOfTiles; t++)
			{
				tileStart[t + 1] = tileStart[t] + tileCount[t];
				tileCount[t] = tileStart[t];
			}
			for (quint64 e = 0; e < batchEntries; e++)
			{
				int tx1, tx2, ty1, ty2;
				entryTiles(entries[e], tx1, tx2, ty1, ty2);
				for (int ty = ty1; ty <= ty2; ty++)
					for (int tx = tx1; tx <= tx2; tx++)
						tileList[tileCount[tx + ty * tilesX]++] = cl_int(e);
			}

			if (!WriteBatchToQueue(batchEntries, listLength)) return false;

			// assign parameters to kernel
			if (!AssignParametersToKernel(0)) return false;

			// processing queue
			if (!ProcessQueue()) return false;

			double percentDone = double(entryIndex) / numberOfPixels;
			emit updateProgressAndStatus(
				tr("OpenCL - rendering DOF - phase 2"), progressText.getText(percentDone), percentDone);
			gApplication->processEvents();

			if (*stopRequest || systemData.globalStopRequest)
			{
				return false;
			}
		}

		if (!*stopRequest || systemData.globalStopRequest)
		{
			if (!ReadBuffersFromQueue(0)) return false;

			for (quint64 y = 0; y < height; y++)
			{
				for (quint64 x = 0; x < width; x++)
				{
					quint64 xx = x + imageRegion.x1;
					quint64 yy = y + imageRegion.y1;

					cl_float4 imagePixelCl = reinterpret_cast<cl_float4 *>(
						inputAndOutputBuffers[0][outputIndex].ptr.get())[x + y * width];

					sRGBFloat pixel(imagePixelCl.s[0], imagePixelCl.s[1], imagePixelCl.s[2]);
					unsigned short alpha = ushort(imagePixelCl.s[3] * 65535.0f);
					image->PutPixelPostImage(xx, yy, pixel);
					image->PutPixelAlpha(xx, yy, alpha);
				}
			}

			WriteLogDouble(
				"cOpenClEngineRenderDOFPhase2: OpenCL Rendering time [s]", timer.nsecsElapsed() / 1.0e9, 2);

			WriteLog("image->CompileImage()", 2);
			image->CompileImage();

			if (image->IsPreview())
			{
				WriteLog("image->ConvertTo8bit()", 2);
				image->ConvertTo8bitChar();
				WriteLog("image->UpdatePreview()", 2);
				image->UpdatePreview();
				WriteLog("image->GetImageWidget()->update()", 2);
				emit updateImage();
			}
		}

		emit updateProgressAndStatus(
			tr("OpenCL - rendering DOF phase 2 finished"), progressText.getText(1.0), 1.0);

		return true;
	}
	else
	{
		return false;
	}
}

size_t cOpenClEngineRenderDOFPhase2::CalcNeededMemory()
{
	return numberOfPixels * sizeof(cl_float4) + maxBatchEntries * sizeof(sDOFEntryCl)
				 + (numberOfTiles + 1 + tileListCapacity) * sizeof(cl_int);
}

#endif // USE_OPENCL
