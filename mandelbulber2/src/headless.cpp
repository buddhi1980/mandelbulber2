/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen (jenzebas@gmail.com)
 *
 * cHeadless - class to handle CLI instructions without GUI manipulation
 */

#include "headless.h"

#include "animation_flight.hpp"
#include "animation_keyframes.hpp"
#include "cimage.hpp"
#include "error_message.hpp"
#include "file_image.hpp"
#include "files.h"
#include "fractal_container.hpp"
#include "global_data.hpp"
#include "initparameters.hpp"
#include "interface.hpp"
#include "mesh_export.hpp"
#include "opencl_engine_render_fractal.h"
#include "opencl_engine_render_ssao.h"
#include "opencl_global.h"
#include "queue.hpp"
#include "render_job.hpp"
#include "rendering_configuration.hpp"
#include "voxel_export.hpp"

cHeadless::cHeadless() : QObject() {}

cHeadless::~cHeadless() = default;

void cHeadless::RenderStillImage(QString filename, QString imageFileFormat)
{
	cImage *image = new cImage(gPar->Get<int>("image_width"), gPar->Get<int>("image_height"));
	cRenderJob *renderJob = new cRenderJob(gPar, gParFractal, image, &gMainInterface->stopRequest);

	QObject::connect(renderJob,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
		SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
	QObject::connect(renderJob, SIGNAL(updateStatistics(cStatistics)), this,
		SLOT(slotUpdateStatistics(cStatistics)));

#ifdef USE_OPENCL
	// connect signal for progress bar update
	connect(gOpenCl->openClEngineRenderFractal,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
		SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
	connect(gOpenCl->openClEngineRenderSSAO,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
		SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
#endif

	cRenderingConfiguration config;
	config.DisableRefresh();
	config.DisableProgressiveRender();
	config.EnableNetRender();

	renderJob->Init(cRenderJob::still, config);
	renderJob->Execute();

	QString filenameWithoutExtension = ImageFileSave::ImageNameWithoutExtension(filename);

	QString ext;
	if (imageFileFormat == "png16" || imageFileFormat == "png16alpha")
	{
		ext = ".png";
		ImageFileSave::structSaveImageChannel saveImageChannel(
			ImageFileSave::IMAGE_CONTENT_COLOR, ImageFileSave::IMAGE_CHANNEL_QUALITY_16, "");
		ImageFileSave::ImageConfig imageConfig;
		imageConfig.insert(ImageFileSave::IMAGE_CONTENT_COLOR, saveImageChannel);
		bool appendAlpha = (imageFileFormat == "png16alpha");
		ImageFileSavePNG imageSaver(filenameWithoutExtension, image, imageConfig);
		imageSaver.SetAppendAlphaCustom(appendAlpha);
		imageSaver.SaveImage();
	}
	else
	{
		ext = "." + imageFileFormat;
		ImageFileSave::enumImageFileType imageFileType = ImageFileSave::ImageFileType(imageFileFormat);
		SaveImage(filenameWithoutExtension + ext, imageFileType, image, this);
	}

	QTextStream out(stdout);
	out << tr("Image saved to: %1\n").arg(filenameWithoutExtension + ext);

	delete renderJob;
	delete image;
	emit finished();
}

void cHeadless::RenderQueue()
{
	gQueue->slotQueueRender();

	while (true)
	{
		do
		{
			gApplication->processEvents();
			Wait(100);
		} while (gQueue->GetQueueSize() > 0);

		cErrorMessage::showMessage(
			"Queue is empty. Waiting for something to do.", cErrorMessage::infoMessage);
		int intProgress = 0;
		QTextStream out(stdout);
		do
		{
			gApplication->processEvents();
			Wait(100);
			if (!systemData.isOutputTty) continue;

			QString progressChars = "|\\-/";
			intProgress = (intProgress + 1) % progressChars.length();
			QString text = colorize(progressChars.mid(intProgress, 1), ansiBlue, noExplicitColor, true)
										 + QString(" Waiting ...") + "\r";
			out << text;
			out.flush();
		} while (gQueue->GetQueueSize() == 0);

		gQueue->slotQueueRender();
	}
}

void cHeadless::RenderVoxel(QString voxelFormat)
{
	CVector3 limitMin;
	CVector3 limitMax;
	if (gPar->Get<bool>("voxel_custom_limit_enabled"))
	{
		limitMin = gPar->Get<CVector3>("voxel_limit_min");
		limitMax = gPar->Get<CVector3>("voxel_limit_max");
	}
	else
	{
		limitMin = gPar->Get<CVector3>("limit_min");
		limitMax = gPar->Get<CVector3>("limit_max");
	}
	int maxIter = gPar->Get<int>("voxel_max_iter");
	int samplesX = gPar->Get<int>("voxel_samples_x");
	int samplesY = gPar->Get<int>("voxel_samples_y");
	int samplesZ = gPar->Get<int>("voxel_samples_z");

	if (voxelFormat == "slice")
	{
		QString folderString = gPar->Get<QString>("voxel_image_path");
		QDir folder(folderString);
		cVoxelExport *voxelExport =
			new cVoxelExport(samplesX, samplesY, samplesZ, limitMin, limitMax, folder, maxIter);
		QObject::connect(voxelExport,
			SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
			SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
		voxelExport->ProcessVolume();
		delete voxelExport;
	}
	else if (voxelFormat == "ply")
	{
		QString fileString = gPar->Get<QString>("mesh_output_filename");
		QList<MeshFileSave::enumMeshContentType> meshContent({MeshFileSave::MESH_CONTENT_GEOMETRY});
		if (gPar->Get<bool>("mesh_color")) meshContent << MeshFileSave::MESH_CONTENT_COLOR;
		MeshFileSave::structSaveMeshConfig meshConfig(MeshFileSave::MESH_FILE_TYPE_PLY, meshContent,
			MeshFileSave::enumMeshFileModeType(gPar->Get<int>("mesh_file_mode")));

		cMeshExport *meshExport = new cMeshExport(
			samplesX, samplesY, samplesZ, limitMin, limitMax, fileString, maxIter, meshConfig);
		QObject::connect(meshExport,
			SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
			SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
		meshExport->ProcessVolume();
		delete meshExport;
	}
	emit finished();
}

void cHeadless::RenderFlightAnimation() const
{
	cImage *image = new cImage(gPar->Get<int>("image_width"), gPar->Get<int>("image_height"));
	gFlightAnimation =
		new cFlightAnimation(gMainInterface, gAnimFrames, image, nullptr, gPar, gParFractal, nullptr);
	QObject::connect(gFlightAnimation,
		SIGNAL(updateProgressAndStatus(
			const QString &, const QString &, double, cProgressText::enumProgressType)),
		this,
		SLOT(slotUpdateProgressAndStatus(
			const QString &, const QString &, double, cProgressText::enumProgressType)));
	// QObject::connect(gFlightAnimation, SIGNAL(updateProgressHide(cProgressText::enumProgressType)),
	// unused
	//								 this, SLOT(slotUpdateProgressHide(cProgressText::enumProgressType)));
	QObject::connect(gFlightAnimation, SIGNAL(updateStatistics(cStatistics)), this,
		SLOT(slotUpdateStatistics(cStatistics)));
	gFlightAnimation->slotRenderFlight();
	delete image;
	delete gFlightAnimation;
	gFlightAnimation = nullptr;
	return;
}

void cHeadless::RenderKeyframeAnimation() const
{
	cImage *image = new cImage(gPar->Get<int>("image_width"), gPar->Get<int>("image_height"));
	gKeyframeAnimation =
		new cKeyframeAnimation(gMainInterface, gKeyframes, image, nullptr, gPar, gParFractal, nullptr);
	QObject::connect(gKeyframeAnimation,
		SIGNAL(updateProgressAndStatus(
			const QString &, const QString &, double, cProgressText::enumProgressType)),
		this,
		SLOT(slotUpdateProgressAndStatus(
			const QString &, const QString &, double, cProgressText::enumProgressType)));
	// QObject::connect(gKeyframeAnimation,
	// SIGNAL(updateProgressHide(cProgressText::enumProgressType)), unused
	// 								 this, SLOT(slotUpdateProgressHide(cProgressText::enumProgressType)));
	QObject::connect(gKeyframeAnimation, SIGNAL(updateStatistics(cStatistics)), this,
		SLOT(slotUpdateStatistics(cStatistics)));
	gKeyframeAnimation->slotRenderKeyframes();
	delete image;
	delete gKeyframeAnimation;
	gKeyframeAnimation = nullptr;
	return;
}

void cHeadless::slotNetRender()
{
	gMainInterface->stopRequest = true;
	cImage *image = new cImage(gPar->Get<int>("image_width"), gPar->Get<int>("image_height"));
	cRenderJob *renderJob = new cRenderJob(gPar, gParFractal, image, &gMainInterface->stopRequest);
	QObject::connect(renderJob,
		SIGNAL(updateProgressAndStatus(const QString &, const QString &, double)), this,
		SLOT(slotUpdateProgressAndStatus(const QString &, const QString &, double)));
	QObject::connect(renderJob, SIGNAL(updateStatistics(cStatistics)), this,
		SLOT(slotUpdateStatistics(cStatistics)));

	cRenderingConfiguration config;
	config.DisableRefresh();
	config.DisableProgressiveRender();
	config.EnableNetRender();

	renderJob->Init(cRenderJob::still, config);
	renderJob->Execute();

	delete renderJob;
	delete image;
	emit finished();
}

void cHeadless::slotUpdateProgressAndStatus(const QString &text, const QString &progressText,
	double progress, cProgressText::enumProgressType progressType)
{
	static bool firstCallProgressUpdate = true;
	if (firstCallProgressUpdate)
	{
		firstCallProgressUpdate = false;
		QTextStream out(stdout);
		out << "\n\n\n";
		if (systemData.statsOnCLI) out << "\n\n";
		out.flush();
	}
	if (systemData.statsOnCLI) MoveCursor(0, -2);
	if (gMainInterface->headless)
	{
		switch (progressType)
		{
			case cProgressText::progress_IMAGE: MoveCursor(0, -1); break;
			case cProgressText::progress_ANIMATION: MoveCursor(0, -2); break;
			case cProgressText::progress_QUEUE: MoveCursor(0, -3); break;
		}

		// not enough space to display information in animation bar
		QString displayText = (progressType == cProgressText::progress_ANIMATION ? "" : text);

		RenderingProgressOutput(displayText, progressText, progress);

		switch (progressType)
		{
			case cProgressText::progress_QUEUE:
				MoveCursor(0, 1);
				EraseLine();
				MoveCursor(0, 1);
				EraseLine();
				MoveCursor(0, 1);
				EraseLine();
				break;
			case cProgressText::progress_ANIMATION:
				MoveCursor(0, 1);
				EraseLine();
				MoveCursor(0, 1);
				EraseLine();
				break;
			case cProgressText::progress_IMAGE: MoveCursor(0, 1); break;
		}
		if (systemData.statsOnCLI) MoveCursor(0, 2);
	}
}

void cHeadless::slotUpdateStatistics(const cStatistics &stat) const
{
	if (!systemData.statsOnCLI) return;
	/*ui->label_histogram_de->SetBarColor(QColor(0, 255, 0));
	 ui->label_histogram_de->UpdateHistogram(stat.histogramStepCount);
	 ui->label_histogram_iter->UpdateHistogram(stat.histogramIterations);
	 */
	if (systemData.statsOnCLI) MoveCursor(0, -2);
	QTextStream out(stdout);
	QString statsText = "";
	statsText += tr("Total number of iters").leftJustified(25, ' ') + ": ";
	statsText += colorize(QString::number(stat.GetTotalNumberOfIterations()).rightJustified(12, ' '),
								 ansiBlue, noExplicitColor, true)
							 + ", ";
	statsText += tr("Number of iters / pixel").leftJustified(25, ' ') + ": ";
	statsText +=
		colorize(QString::number(stat.GetNumberOfIterationsPerPixel()).rightJustified(12, ' '),
			ansiBlue, noExplicitColor, true)
		+ "\n";
	statsText += tr("Number of iters / second").leftJustified(25, ' ') + ": ";
	statsText +=
		colorize(QString::number(stat.GetNumberOfIterationsPerSecond()).rightJustified(12, ' '),
			ansiBlue, noExplicitColor, true)
		+ ", ";
	statsText += tr("Percentage of wrong DE").leftJustified(25, ' ') + ": ";
	statsText += colorize(QString::number(stat.GetMissedDEPercentage()).rightJustified(12, ' '),
								 ansiBlue, noExplicitColor, true)
							 + "\n";
	out << statsText;
	out.flush();
}

void cHeadless::RenderingProgressOutput(
	const QString &header, const QString &progressTxt, double percentDone)
{
	QTextStream out(stdout);
	QString formattedText = formatLine(progressTxt) + " ";
	QString useHeader = header;
	QString text;
	if (systemData.terminalWidth > 0)
	{
		if (useHeader != "") useHeader += ": ";
		int freeWidth = systemData.terminalWidth - progressTxt.length() - useHeader.length() - 4;
		int intProgress = freeWidth * percentDone;
		if (systemData.isOutputTty) text = "\r";
		text += colorize(useHeader, ansiYellow, noExplicitColor, true);
		text += formattedText;
		text += colorize("[", ansiBlue, noExplicitColor, true);
		text += colorize(QString(intProgress, '#'), ansiMagenta, noExplicitColor, true);
		text += QString(freeWidth - intProgress, ' ');
		text += colorize("]", ansiBlue, noExplicitColor, true);
		text += "\n";
	}
	else
	{
		text += QString("\n");
	}
	out << text;
	out.flush();
}

QString cHeadless::colorize(
	QString text, ansiColor foregroundColor, ansiColor backgroundColor, bool bold)
{
	// more information on ANSI escape codes here: https://en.wikipedia.org/wiki/ANSI_escape_code
	if (!systemData.isOutputTty) return text;
	if (!systemData.useColor) return text;

	QStringList ansiSequence;
	if (foregroundColor != noExplicitColor) ansiSequence << QString::number(foregroundColor + 30);
	if (backgroundColor != noExplicitColor) ansiSequence << QString::number(backgroundColor + 40);
	if (bold) ansiSequence << "1";

	if (ansiSequence.size() == 0) return text;

	QString colorizedString = "\033["; // start ANSI escape sequence
	colorizedString += ansiSequence.join(";");
	colorizedString += "m"; // end ANSI escape sequence
	colorizedString += text;
	colorizedString += "\033[0m"; // reset color and bold after string
	return colorizedString;
}

QString cHeadless::formatLine(const QString &text)
{
	if (!systemData.isOutputTty) return text;
	if (!systemData.useColor) return text;
	QList<QRegularExpression> reType;
	reType.append(
		QRegularExpression("^(Done )(.*?)(, )(elapsed: )(.*?)(, )(estimated to end: )(.*)"));
	reType.append(QRegularExpression("^(Gotowe )(.*?)(, )(upłynęło: )(.*?)(, )(do końca: )(.*)"));
	reType.append(QRegularExpression(
		"^(Fortschritt )(.*?)(, )(vergangen: )(.*?)(, )(voraussichtlich noch: )(.*)"));

	reType.append(QRegularExpression("^(.*?)( Done)(, )(total time: )(.*)"));
	reType.append(QRegularExpression("^(.*?)( gotowe)(, )(całkowity czas: )(.*)"));
	reType.append(QRegularExpression("^(.*?)( Fertig)(, )(Gesamtzeit: )(.*)"));

	// animation
	reType.append(QRegularExpression(
		"^(Frame .*? of .*? Done )(.*?)(, )(elapsed: )(.*?)(, )(estimated to end: )(.*)"));
	reType.append(QRegularExpression(
		"^(Klatka .*? z .*? Gotowe )(.*?)(, )(upłynęło: )(.*?)(, )(do końca: )(.*)"));
	reType.append(
		QRegularExpression("^(Frame .*? von .*? Fortschritt )(.*?)(, )(vergangen: )(.*?)(, "
											 ")(voraussichtlich noch: )(.*)"));

	QRegularExpressionMatch matchType;
	for (const auto &i : reType)
	{
		matchType = i.match(text);
		if (matchType.hasMatch()) break;
	}

	if (!matchType.hasMatch())
	{
		return text;
	}

	QString out = "";
	out += colorize(matchType.captured(1), noExplicitColor, noExplicitColor, false);
	out += colorize(matchType.captured(2), noExplicitColor, noExplicitColor, true);
	out += colorize(matchType.captured(3), noExplicitColor, noExplicitColor, false);

	out += colorize(matchType.captured(4), ansiGreen, noExplicitColor, false);
	out += colorize(matchType.captured(5), ansiGreen, noExplicitColor, true);

	if (matchType.lastCapturedIndex() == 8)
	{
		out += colorize(matchType.captured(6), noExplicitColor, noExplicitColor, false);
		out += colorize(matchType.captured(7), ansiCyan, noExplicitColor, false);
		out += colorize(matchType.captured(8), ansiCyan, noExplicitColor, true);
	}

	return out;
}

bool cHeadless::ConfirmMessage(QString message)
{
	QTextStream out(stdout);
	QTextStream in(stdin);
	out << message << " y/n\n";
	out.flush();
	QString response = in.readLine().toLower();
	return (response == "y");
}

void cHeadless::EraseLine()
{
	if (!systemData.isOutputTty) return;
	QTextStream out(stdout);
	out << "\033[2K";
	out.flush();
}

void cHeadless::MoveCursor(int leftRight, int downUp)
{
	if (!systemData.isOutputTty) return;
	QTextStream out(stdout);
	if (leftRight != 0)
	{
		QString code = "\033[";
		code += (leftRight > 0) ? QString::number(leftRight) : QString::number(leftRight * -1);
		code += (leftRight > 0) ? "C" : "D";
		out << code;
	}
	if (downUp != 0)
	{
		QString code = "\033[";
		code += (downUp > 0) ? QString::number(downUp) : QString::number(downUp * -1);
		code += (downUp > 0) ? "B" : "A";
		out << code;
	}
	out.flush();
}
