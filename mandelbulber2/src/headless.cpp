/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cHeadless - class to handle CLI instructions without GUI manipulation
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen
 */

#include "headless.h"
#include "cimage.hpp"
#include "files.h"
#include "render_job.hpp"
#include "global_data.hpp"
#include "interface.hpp"
#include "progress_text.hpp"
#include "error_message.hpp"

cHeadless::cHeadless() : QObject()
{
	// TODO Auto-generated constructor stub
}

cHeadless::~cHeadless()
{
	// TODO Auto-generated destructor stub
	//qDebug() << "cHeadless::~cHeadless()";
}

void cHeadless::RenderStillImage(QString filename, QString imageFileFormat)
{
	cImage *image = new cImage(gPar->Get<int>("image_width"), gPar->Get<int>("image_height"));
	cRenderJob *renderJob = new cRenderJob(gPar, gParFractal, image, &gMainInterface->stopRequest);

	QObject::connect(renderJob, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double)),
									 this, SLOT(slotUpdateProgressAndStatus(const QString&, const QString&, double)));
	QObject::connect(renderJob, SIGNAL(updateStatistics(cStatistics)), this, SLOT(slotUpdateStatistics(cStatistics)));

	cRenderingConfiguration config;
	config.DisableRefresh();
	config.DisableProgressiveRender();
	config.EnableNetRender();

	renderJob->Init(cRenderJob::still, config);
	renderJob->Execute();

	QFileInfo fi(filename);
	filename = fi.path() + QDir::separator() + fi.baseName();

	QString ext;

	if(imageFileFormat == "jpg")
	{
		ext = ".jpg";
		SaveImage(filename + ext, IMAGE_FILE_TYPE_JPG, image);
	}
	else if(imageFileFormat == "png16")
	{
		ext = ".png";
		SaveImage(filename + ext, IMAGE_FILE_TYPE_PNG, image);
	}
	else if(imageFileFormat == "png16")
	{
		ext = ".png";
		structSaveImageChannel saveImageChannel(IMAGE_CONTENT_COLOR, IMAGE_CHANNEL_QUALITY_16, "");
		SavePNG(filename + ext, image, saveImageChannel, false);
	}
	else if(imageFileFormat == "png16alpha")
	{
		ext = ".png";
		structSaveImageChannel saveImageChannel(IMAGE_CONTENT_COLOR, IMAGE_CHANNEL_QUALITY_16, "");
		SavePNG(filename + ext, image, saveImageChannel, true);
	}
	else if(imageFileFormat == "exr")
	{
		ext = ".exr";
		SaveImage(filename + ext, IMAGE_FILE_TYPE_EXR, image);
	}

	QTextStream out(stdout);
	out << "Image saved to: " << filename << ext << "\n";

	delete renderJob;
	delete image;
	emit finished();
}

void cHeadless::RenderQueue()
{
	gQueue->slotQueueRender();

	while(true)
	{
		do
		{
			gApplication->processEvents();
			Wait(100); // TODO substitute with proper handling
		} while (gQueue->GetQueueSize() > 0);

		cErrorMessage::showMessage("Queue is empty. Waiting for something to do.", cErrorMessage::infoMessage);
		do
		{
			gApplication->processEvents();
			Wait(100);
		} while (gQueue->GetQueueSize() == 0);

		gQueue->slotQueueRender();
	}
}

void cHeadless::RenderFlightAnimation()
{
	cImage *image = new cImage(gPar->Get<int>("image_width"), gPar->Get<int>("image_height"));
	gFlightAnimation = new cFlightAnimation(gMainInterface, gAnimFrames, image, NULL, gPar, gParFractal, NULL);
	QObject::connect(gFlightAnimation, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double, cProgressText::enumProgressType)),
									 this, SLOT(slotUpdateProgressAndStatus(const QString&, const QString&, double, cProgressText::enumProgressType)));
	// QObject::connect(gFlightAnimation, SIGNAL(updateProgressHide(cProgressText::enumProgressType)), unused
	//								 this, SLOT(slotUpdateProgressHide(cProgressText::enumProgressType)));
	QObject::connect(gFlightAnimation, SIGNAL(updateStatistics(cStatistics)),
									 this, SLOT(slotUpdateStatistics(cStatistics)));
	gFlightAnimation->slotRenderFlight();
	delete image;
	delete gFlightAnimation;
	gFlightAnimation = NULL;
	return;
}

void cHeadless::RenderKeyframeAnimation()
{
	cImage *image = new cImage(gPar->Get<int>("image_width"), gPar->Get<int>("image_height"));
	gKeyframeAnimation = new cKeyframeAnimation(gMainInterface, gKeyframes, image, NULL, gPar, gParFractal, NULL);
	QObject::connect(gKeyframeAnimation, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double, cProgressText::enumProgressType)),
									 this, SLOT(slotUpdateProgressAndStatus(const QString&, const QString&, double, cProgressText::enumProgressType)));
	// QObject::connect(gKeyframeAnimation, SIGNAL(updateProgressHide(cProgressText::enumProgressType)), unused
	// 								 this, SLOT(slotUpdateProgressHide(cProgressText::enumProgressType)));
	QObject::connect(gKeyframeAnimation, SIGNAL(updateStatistics(cStatistics)),
									 this, SLOT(slotUpdateStatistics(cStatistics)));
	gKeyframeAnimation->slotRenderKeyframes();
	delete image;
	delete gKeyframeAnimation;
	gKeyframeAnimation = NULL;
	return;
}


void cHeadless::slotNetRender()
{
	gMainInterface->stopRequest = true;
	cImage *image = new cImage(gPar->Get<int>("image_width"), gPar->Get<int>("image_height"));
	cRenderJob *renderJob = new cRenderJob(gPar, gParFractal, image, &gMainInterface->stopRequest);
	QObject::connect(renderJob, SIGNAL(updateProgressAndStatus(const QString&, const QString&, double)), this, SLOT(slotUpdateProgressAndStatus(const QString&, const QString&, double)));
	QObject::connect(renderJob, SIGNAL(updateStatistics(cStatistics)), this, SLOT(slotUpdateStatistics(cStatistics)));

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

void cHeadless::slotUpdateProgressAndStatus(const QString &text, const QString &progressText, double progress, cProgressText::enumProgressType progressType)
{
	static bool firstCallProgressUpdate = true;
	if(firstCallProgressUpdate)
	{
		firstCallProgressUpdate = false;
		QTextStream out(stdout);
		out << "\n\n\n";
		out.flush();
	}

	if(gMainInterface->headless)
	{
		switch(progressType)
		{
			case cProgressText::progress_IMAGE:
				MoveCursor(0, -1); break;
			case cProgressText::progress_ANIMATION:
				MoveCursor(0, -2); break;
			case cProgressText::progress_QUEUE: MoveCursor(0, -3); break;
		}

		// not enough space to display info in animation bar
		QString displayText = (progressType == cProgressText::progress_ANIMATION ? "" : text);

		RenderingProgressOutput(displayText, progressText, progress);

		switch(progressType)
		{
			case cProgressText::progress_QUEUE:
				MoveCursor(0, 1);
				EraseLine();
			case cProgressText::progress_ANIMATION:
				MoveCursor(0, 1);
				EraseLine();
			case cProgressText::progress_IMAGE:
				MoveCursor(0, 1);
		}
	}
}

void cHeadless::slotUpdateStatistics(cStatistics stat)
{
	// TODO: maybe display this info on cli, when some cli flag is set
	/*ui->label_histogram_de->SetBarcolor(QColor(0, 255, 0));
	ui->label_histogram_de->UpdateHistogram(stat.histogramStepCount);
	ui->label_histogram_iter->UpdateHistogram(stat.histogramIterations);
	*/

	QTextStream out(stdout);
	QString statsText = "";
	statsText += tr("Total number of iterations") + ": " + QString::number(stat.GetTotalNumberOfIterations()) + ", ";
	statsText += tr("Number of iterations per pixel") + ": " + QString::number(stat.GetNumberOfIterationsPerPixel() )+ "\n";
	statsText += tr("Number of iterations per second") + ": " + QString::number(stat.GetNumberOfIterationsPerSecond()) + ", ";
	statsText += tr("Percentage of wrong distance estimations") + ": " + QString::number(stat.GetMissedDEPercentage());
	out << statsText;
	out.flush();
}


void cHeadless::RenderingProgressOutput(const QString &header, const QString &progressTxt, double percentDone)
{
	QTextStream out(stdout);
	QString formatedText = formatLine(progressTxt) + " ";
	QString useHeader = header;
	QString text;
	if(systemData.terminalWidth > 0)
	{
		if(useHeader != "") useHeader += ": ";
		int freeWidth = systemData.terminalWidth - progressTxt.length() - useHeader.length() - 4;
		int intProgress = freeWidth * percentDone;
		text = "\r";
		text += colorize(useHeader, ansiYellow, noExplicitColor, true);
		text += formatedText;
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

QString cHeadless::colorize(QString text, ansiColor foregroundcolor, ansiColor backgroundColor, bool bold)
{
	// more info on ANSI escape codes here: https://en.wikipedia.org/wiki/ANSI_escape_code
#ifdef WIN32 /* WINDOWS */
	return text;
#else
	if(!systemData.useColor) return text;

	QStringList ansiSequence;
	if(foregroundcolor != noExplicitColor) ansiSequence << QString::number(foregroundcolor + 30);
	if(backgroundColor != noExplicitColor) ansiSequence << QString::number(backgroundColor + 40);
	if(bold) ansiSequence << "1";

	if(ansiSequence.size() == 0) return text;

	QString colorizedString = "\033["; // start ANSI escape sequence
	colorizedString += ansiSequence.join(";");
	colorizedString += "m"; // end ANSI escape sequence
	colorizedString += text;
	colorizedString += "\033[0m"; // reset color and bold after string
	return colorizedString;
#endif
}

QString cHeadless::formatLine(const QString& text)
{
#ifdef WIN32 /* WINDOWS */
	return text;
#else
	if(!systemData.useColor) return text;
	QList<QRegularExpression> reType;
	reType.append(QRegularExpression("^(Done )(.*?)(, )(elapsed: )(.*?)(, )(estimated to end: )(.*)"));
	reType.append(QRegularExpression("^(Gotowe )(.*?)(, )(upłynęło: )(.*?)(, )(do końca: )(.*)"));
	reType.append(QRegularExpression("^(Fortschritt )(.*?)(, )(vergangen: )(.*?)(, )(voraussichtlich noch: )(.*)"));

	reType.append(QRegularExpression("^(.*?)( Done)(, )(total time: )(.*)"));
	reType.append(QRegularExpression("^(.*?)( gotowe)(, )(całkowity czas: )(.*)"));
	reType.append(QRegularExpression("^(.*?)( Fertig)(, )(Gesamtzeit: )(.*)"));

	// animation
	reType.append(QRegularExpression("^(Frame .*? of .*? Done )(.*?)(, )(elapsed: )(.*?)(, )(estimated to end: )(.*)"));
	reType.append(QRegularExpression("^(Klatka .*? z .*? Gotowe )(.*?)(, )(upłynęło: )(.*?)(, )(do końca: )(.*)"));
	reType.append(QRegularExpression("^(Frame .*? von .*? Fortschritt )(.*?)(, )(vergangen: )(.*?)(, )(voraussichtlich noch: )(.*)"));

	QRegularExpressionMatch matchType;
	for(int i = 0; i < reType.size(); i++){
		matchType = reType.at(i).match(text);
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

	if(matchType.lastCapturedIndex() == 8)
	{
		out += colorize(matchType.captured(6), noExplicitColor, noExplicitColor, false);
		out += colorize(matchType.captured(7), ansiCyan, noExplicitColor, false);
		out += colorize(matchType.captured(8), ansiCyan, noExplicitColor, true);
	}

	return out;
#endif
}

bool cHeadless::ConfirmMessage(QString message)
{
	QTextStream out(stdout);
	QTextStream in(stdin);
	out << message << " y/n\n";
	out.flush();
	QString response = in.readLine().toLower();
	return (response  == "y");
}

void cHeadless::EraseLine()
{
#ifdef WIN32 /* WINDOWS */
	return;
#else
	QTextStream out(stdout);
	out << "\033[2K";
	out.flush();
#endif
}

void cHeadless::MoveCursor(int leftRight, int downUp)
{
#ifdef WIN32 /* WINDOWS */
	return;
#else
	QTextStream out(stdout);
	if(leftRight != 0)
	{
		QString code = "\033[";
		code += (leftRight > 0) ? QString::number(leftRight) : QString::number(leftRight * -1);
		code += (leftRight > 0) ? "C" : "D";
		out << code;
	}
	if(downUp != 0)
	{
		QString code = "\033[";
		code += (downUp > 0) ? QString::number(downUp) : QString::number(downUp * -1);
		code += (downUp > 0) ? "B" : "A";
		out << code;
	}
	out.flush();
#endif
}
