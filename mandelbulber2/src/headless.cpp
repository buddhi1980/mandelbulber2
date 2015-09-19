/*
 * headless.cpp
 *
 *  Created on: 12-09-2015
 *      Author: krzysztof
 */

#include "headless.h"
#include "cimage.hpp"
#include "files.h"
#include "render_job.hpp"
#include "global_data.hpp"
#include "interface.hpp"

cHeadless::cHeadless() : QObject()
{
	// TODO Auto-generated constructor stub

}

cHeadless::~cHeadless()
{
	// TODO Auto-generated destructor stub
}

void cHeadless::RenderStillImage(bool isNetRenderClient)
{
	cImage *image = new cImage(gPar->Get<int>("image_width"), gPar->Get<int>("image_height"));
	cRenderJob *renderJob = new cRenderJob(gPar, gParFractal, image, &gMainInterface->stopRequest);

	cRenderingConfiguration config;
	config.EnableConsoleOutput();
	config.DisableRefresh();
	config.DisableProgressiveRender();
	config.EnableNetRender();

	renderJob->Init(cRenderJob::still, config);
	renderJob->Execute();

	if(!isNetRenderClient)
	{
		//TODO saving in different image formats
		SaveImage(systemData.dataDirectory + "images/test.jpg", IMAGE_FILE_TYPE_JPG, image);
	}
	delete renderJob;
	delete image;
	qDebug() << "Rendering finished";
	emit finished();
}

void cHeadless::slotNetRender()
{
	RenderStillImage(true);
}

void cHeadless::RenderingProgressOutput(const QString &progressTxt, double percentDone, bool newLine)
{
	QTextStream out(stdout);
	QString text = formatLine(progressTxt) + " ";
	if(systemData.terminalWidth > 0)
	{
		int freeWidth = systemData.terminalWidth - progressTxt.length() - 4;
		int intProgress = freeWidth * percentDone;
		text = "\r" + text;
		text += colorize("[", ansiBlue, noExplicitColor, true);
		text += colorize(QString(intProgress, '#'), ansiMagenta, noExplicitColor, true);
		text += QString(freeWidth - intProgress, ' ');
		text += colorize("]", ansiBlue, noExplicitColor, true);
		if(newLine) text += "\n";
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
