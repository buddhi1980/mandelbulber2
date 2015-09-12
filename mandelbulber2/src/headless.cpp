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

cHeadless::cHeadless()
{
	// TODO Auto-generated constructor stub

}

cHeadless::~cHeadless()
{
	// TODO Auto-generated destructor stub
}

void cHeadless::RenderStillImage(void)
{
	bool stopRequest;
	cImage *image = new cImage(gPar->Get<int>("image_width"), gPar->Get<int>("image_height"));
	cRenderJob *renderJob = new cRenderJob(gPar, gParFractal, image, &stopRequest);
	renderJob->EnableConsoleOutput();
	renderJob->Init(cRenderJob::still);
	renderJob->Execute();
	//TODO saving in different image formats
	SaveImage(systemData.dataDirectory + "images/test.jpg", IMAGE_FILE_TYPE_JPG, image);
	delete renderJob;
	delete image;
	qDebug() << "Rendering finished";
}

void cHeadless::RenderingProgressOutput(const QString &progressTxt, double percentDone)
{
	//TODO some nicer output
	QTextStream out(stdout);
	QString text = progressTxt;
	if(systemData.terminalWidth > 0)
	{
		int freeWidth = systemData.terminalWidth - text.length() - 2;
		int intProgress = freeWidth * percentDone;
		text += "[";
		text += QString(intProgress, '*');
		text += QString(freeWidth - intProgress, '-');
		text += QString("]\r");
	}
	else
	{
		text += QString("\n");
	}
	out << text;
	out.flush();
}
