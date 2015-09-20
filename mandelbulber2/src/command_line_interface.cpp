/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cCommandLineInterface - CLI Input handler
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License clias published by the Free Software Foundation, either version 3 of the
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

#include "command_line_interface.hpp"
#include "global_data.hpp"
#include "settings.hpp"
#include "headless.h"
#include "error_message.hpp"

cCommandLineInterface::cCommandLineInterface(QCoreApplication *qapplication)
{
	// text from http://sourceforge.net/projects/mandelbulber/
	parser.setApplicationDescription("Mandelbulber is an easy to use, "
		"handy application designed to help you render 3D Mandelbrot fractals called Mandelbulb "
		"and some other kind of 3D fractals like Mandelbox, Bulbbox, Juliabulb, Menger Sponge");
	parser.addHelpOption();
	parser.addVersionOption();
	QCommandLineOption noguiOption(QStringList() << "n" << "nogui",
		QCoreApplication::translate("main", "Start program without GUI."));
	QCommandLineOption animationOption(QStringList() << "m" << "mode",
		QCoreApplication::translate("main", "Set <MODE> of animation (flight / keyframe), default is flight."),
		QCoreApplication::translate("main", "MODE"));
	QCommandLineOption startOption(QStringList() << "s" << "start",
		QCoreApplication::translate("main", "Start rendering from frame number <N>."),
		QCoreApplication::translate("main", "N"));
	QCommandLineOption endOption(QStringList() << "e" << "end",
		QCoreApplication::translate("main", "Stop rendering on frame number <N>."),
		QCoreApplication::translate("main", "N"));
	QCommandLineOption overrideOption(QStringList() << "O" << "override",
		QCoreApplication::translate("main", "Override item '<KEY>' from settings file with new value '<value>'."),
		QCoreApplication::translate("main", "KEY=VALUE"));
	QCommandLineOption listOption(QStringList() << "L" << "list",
		QCoreApplication::translate("main", "List all possible parameters '<KEY>' with corresponding default value '<value>'."));
	QCommandLineOption formatOption(QStringList() << "f" << "format",
		QCoreApplication::translate("main", "Image output format:\n"
			"jpg - JPEG format\n"
			"png - PNG format\n"
			"png16 - 16-bit PNG format\n"
			"png16alpha - 16-bit PNG with alpha channel format"),
		QCoreApplication::translate("main", "FORMAT"));
	QCommandLineOption resOption(QStringList() << "r" << "res",
		QCoreApplication::translate("main", "Override image resolution."),
		QCoreApplication::translate("main", "WIDTHxHEIGHT"));
	QCommandLineOption fpkOption("fpk",
		QCoreApplication::translate("main", "Override frames per key parameter."),
		QCoreApplication::translate("main", "N"));
	QCommandLineOption serverOption(QStringList() << "S" << "server",
		QCoreApplication::translate("main", "Set application as a server listening for clients."));
	QCommandLineOption hostOption(QStringList() << "H" << "host",
		QCoreApplication::translate("main", "Set application as a client connected to server of given Host address"
			" (Host can be of type IPv4, IPv6 and Domain name address)."),
		QCoreApplication::translate("main", "N.N.N.N"));
	QCommandLineOption portOption(QStringList() << "p" << "port",
		QCoreApplication::translate("main", "Set network port number for Netrender (default 5555)."),
		QCoreApplication::translate("main", "N"));
	QCommandLineOption noColorOption(QStringList() << "C" << "no-cli-color",
		QCoreApplication::translate("main", "Start program without ANSI colors, when execution on CLI."));
	QCommandLineOption outputOption(QStringList() << "o" << "output",
		QCoreApplication::translate("main", "Save rendered image(s) to this file / folder."),
		QCoreApplication::translate("main", "N"));

	parser.addPositionalArgument("settings_file", QCoreApplication::translate("main",
		"file with fractal settings (program also tries\nto find file in ./mandelbulber/settings directory)\n"
		"When settings_file is put as a command line argument then program will start in noGUI mode"
	));

	parser.addOption(noguiOption);
	parser.addOption(outputOption);
	parser.addOption(animationOption);
	parser.addOption(startOption);
	parser.addOption(endOption);
	parser.addOption(overrideOption);
	parser.addOption(listOption);
	parser.addOption(formatOption);
	parser.addOption(resOption);
	parser.addOption(fpkOption);
	parser.addOption(serverOption);
	parser.addOption(hostOption);
	parser.addOption(portOption);
	parser.addOption(noColorOption);

	// Process the actual command line arguments given by the user
	parser.process(*qapplication);
	args = parser.positionalArguments();

	cliData.nogui = parser.isSet(noguiOption);
	cliData.animationMode = parser.value(animationOption);
	cliData.startFrameText = parser.value(startOption);
	cliData.endFrameText = parser.value(endOption);
	cliData.overrideParametersText = parser.value(overrideOption);
	cliData.imageFileFormat = parser.value(formatOption);
	cliData.resolution = parser.value(resOption);
	cliData.fpkText = parser.value(fpkOption);
	cliData.server = parser.isSet(serverOption);
	cliData.host = parser.value(hostOption);
	cliData.portText = parser.value(portOption);
	cliData.outputText = parser.value(outputOption);
	cliData.listParameters = parser.isSet(listOption);

#ifdef WIN32 /* WINDOWS */
	systemData.useColor = false;
#else
	systemData.useColor = !parser.isSet(noColorOption);
#endif  /* WINDOWS */

	cliTODO = modeBootOnly;
}

cCommandLineInterface::~cCommandLineInterface()
{
	// TODO Auto-generated destructor stub
}

void cCommandLineInterface::ReadCLI (void)
{
	bool checkParse = true;
	bool settingsSpecified = false;

	// list parameters only
	if(cliData.listParameters)
	{
		QList<QString> listOfParameters = gPar->GetListOfParameters();
		qDebug() << "list of parameters:";
		qDebug() << "KEY=VALUE";
		for(int i = 0; i < listOfParameters.size(); i++)
		{
			QString parameterName = listOfParameters.at(i);
			QString defaultValue = gPar->GetDefault<QString>(parameterName);
			qDebug() << parameterName + "=" + defaultValue;
		}
		exit(0);
	}

	// check netrender server / client
	if(cliData.server)
	{
		int port = gPar->Get<int>("netrender_server_local_port");

		if(cliData.portText != "")
		{
			port = cliData.portText.toInt(&checkParse);
			if(!checkParse || port <= 0){
				cErrorMessage::showMessage("Specified port is invalid\n", cErrorMessage::errorMessage);
				parser.showHelp(-10);
			}
			gPar->Set("netrender_server_local_port", port);
		}
		cliData.nogui = true; systemData.noGui = true;
		gNetRender->SetServer(gPar->Get<int>("netrender_server_local_port"));
		QElapsedTimer timer;
		timer.start();

		if(systemData.noGui)
		{
			QTextStream out(stdout);
			out << "NetRender - Waiting for clients\n";
		}

		while(timer.elapsed() < 5000)
		{
			gApplication->processEvents();
		}
	}
	else if(cliData.host != "")
	{
		int port = gPar->Get<int>("netrender_client_remote_port");
		gPar->Set("netrender_client_remote_address", cliData.host);
		if(cliData.portText != "")
		{
			port = cliData.portText.toInt(&checkParse);
			if(!checkParse || port <= 0){
				cErrorMessage::showMessage("Specified port is invalid\n", cErrorMessage::errorMessage);
				parser.showHelp(-11);
			}
			gPar->Set("netrender_client_remote_port", port);
		}
		cliData.nogui = true; systemData.noGui = true;
		cliTODO = modeNetrender;
		return;
	}

	if(args.size() > 0){
		// TODO for future development -> load multiple settings file to queue
		// file specified -> load it
		QString filename = args[0];
		if(!QFile::exists(filename))
		{
			// try to find settings in default settings path
			filename = systemData.dataDirectory + "settings" + QDir::separator() + filename;
		}
		if(QFile::exists(filename))
		{
			cSettings parSettings(cSettings::formatFullText);
			parSettings.LoadFromFile(filename);
			parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
			settingsSpecified = true;
			systemData.lastSettingsFile = filename;
		}
		else
		{
			cErrorMessage::showMessage("Cannot load file!\n", cErrorMessage::errorMessage);
			qCritical() << "\nSetting file " << filename << " not found\n";
			parser.showHelp(-12);
		}
	}

	// overwriting parameters
	if(cliData.overrideParametersText != "")
	{
		QStringList overrideParameters = cliData.overrideParametersText.split(QRegExp("\\s"));
		for(int i = 0; i < overrideParameters.size(); i++)
		{
			QStringList overrideParameter = overrideParameters[i].split(QRegExp("\\="));
			if(overrideParameter.size() == 2){
				gPar->Set(overrideParameter[0], overrideParameter[1]);
			}
		}
	}

	// specified resolution
	if(cliData.resolution != "")
	{
		QStringList resolutionParameters = cliData.resolution.split(QRegExp("x"));
		if(resolutionParameters.size() == 2)
		{
			int xRes = resolutionParameters[0].toInt(&checkParse);
			int yRes = resolutionParameters[1].toInt(&checkParse);
			if(!checkParse || xRes <= 0 || yRes <= 0){
				WriteLog("Specified resolution not valid\n"
								 "both dimensions need to be > 0");
				parser.showHelp(-13);
			}
			gPar->Set("image_width", xRes);
			gPar->Set("image_height", yRes);
		}
	}

	// specified frames per keyframe
	if(cliData.fpkText != "")
	{
		int fpk = cliData.fpkText.toInt(&checkParse);
		if(!checkParse || fpk <= 0){
			WriteLog("Specified frames per key not valid\n"
							 "need to be > 0");
			parser.showHelp(-14);
		}
		gPar->Set("frames_per_keyframe", fpk);
	}

	// specified image file format
	if(cliData.imageFileFormat != "")
	{
		QStringList allowedImageFileFormat;
		allowedImageFileFormat << "jpg" << "png" << "png16" << "png16alpha";
		if(!allowedImageFileFormat.contains(cliData.imageFileFormat)){
			WriteLog("Specified imageFileFormat is not valid\n"
							 "allowed formats are: " + allowedImageFileFormat.join(", "));
			parser.showHelp(-15);
		}
	}
	else
	{
		cliData.imageFileFormat = "jpg";
	}

	// animation rendering
	if(cliData.startFrameText != "" || cliData.endFrameText != "")
	{
		int startFrame = cliData.startFrameText.toInt(&checkParse);
		int endFrame = cliData.endFrameText.toInt(&checkParse);
		if(!checkParse || startFrame < 0 || endFrame < startFrame){
			cErrorMessage::showMessage("Specified startframe or endframe not valid\n"
							 "(need to be > 0, endframe > startframe)", cErrorMessage::errorMessage);
			parser.showHelp(-16);
		}

		if(cliData.animationMode == "flight" || cliData.animationMode == "")
		{
			gPar->Set("flight_first_to_render", startFrame);
			gPar->Set("flight_last_to_render", endFrame);
			if(cliData.outputText != "") gPar->Set("anim_flight_dir", cliData.outputText);
			cliData.nogui = true; systemData.noGui = true;
			cliTODO = modeFlight;
			return;
		}
		else if(cliData.animationMode == "keyframe")
		{
			gPar->Set("keyframe_first_to_render", startFrame);
			gPar->Set("keyframe_last_to_render", endFrame);
			if(cliData.outputText != "") gPar->Set("anim_keyframe_dir", cliData.outputText);
			cliData.nogui = true; systemData.noGui = true;
			cliTODO = modeKeyframe;
			return;
		}
		else
		{
			WriteLog("Unknown mode: " + cliData.animationMode + "\n");
			parser.showHelp(-17);
		}
	}

	if(!settingsSpecified && cliData.nogui && cliTODO != modeNetrender)
	{
		WriteLog("You have to specify a settings file, for this configuration!");
		parser.showHelp(-18);
	}

	if(cliData.nogui && cliTODO != modeKeyframe && cliTODO != modeFlight)
	{
		//creating output filename if it's not specified
		if(cliData.outputText == "")
		{
			cliData.outputText = gPar->Get<QString>("default_image_path") + QDir::separator();
			cliData.outputText += QFileInfo(systemData.lastSettingsFile).completeBaseName();
		}
		cliTODO = modeStill;
		return;
	}

	// TODO handle overrideParametersText
	cliTODO = modeBootOnly;
}

void cCommandLineInterface::ProcessCLI (void)
{
	switch(cliTODO)
	{
		case modeNetrender:
		{
			gNetRender->SetClient(gPar->Get<QString>("netrender_client_remote_address"), gPar->Get<int>("netrender_client_remote_port"));
			gApplication->exec();
			break;
		}
		case modeFlight:
		{
			cHeadless headless;
			headless.RenderFlightAnimation();
			break;
		}
		case modeKeyframe:
		{
			return gKeyframeAnimation->slotRenderKeyframes();
			break;
		}
		case modeStill:
		{
			cHeadless headless;
			headless.RenderStillImage(cliData.outputText, cliData.imageFileFormat);
			break;
		}
	}
}
