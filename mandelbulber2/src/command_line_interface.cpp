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
#include "system.hpp"
#include "initparameters.hpp"
#include "fractal_container.hpp"
#include "headless.h"
#include "error_message.hpp"
#include "global_data.hpp"
#include "queue.hpp"
#include "netrender.hpp"
#include "animation_frames.hpp"
#include "keyframes.hpp"
#include "settings.hpp"
#include "test.hpp"

cCommandLineInterface::cCommandLineInterface(QCoreApplication *qapplication)
{
	// text from http://sourceforge.net/projects/mandelbulber/
    parser.setApplicationDescription(QCoreApplication::translate("main", "Mandelbulber is an easy to use, "
		"handy application designed to help you render 3D Mandelbrot fractals called Mandelbulb "
        "and some other kind of 3D fractals like Mandelbox, Bulbbox, Juliabulb, Menger Sponge"));

	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption noguiOption(QStringList() << "n" << "nogui",
		QCoreApplication::translate("main", "Starts the program without a GUI."));

	QCommandLineOption keyframeOption(QStringList() << "K" << "keyframe",
		QCoreApplication::translate("main", "Renders keyframe animation."));

	QCommandLineOption flightOption(QStringList() << "F" << "flight",
		QCoreApplication::translate("main", "Renders flight animation."));

	QCommandLineOption startOption(QStringList() << "s" << "start",
		QCoreApplication::translate("main", "Starts rendering from frame number <N>."),
		QCoreApplication::translate("main", "N"));

	QCommandLineOption endOption(QStringList() << "e" << "end",
		QCoreApplication::translate("main", "Stops rendering on frame number <N>."),
		QCoreApplication::translate("main", "N"));

	QCommandLineOption overrideOption(QStringList() << "O" << "override",
		QCoreApplication::translate("main", "Overrides item '<KEY>' from settings file with new value '<VALUE>'.\n"
			"Specify multiple KEY=VALUE pairs by separating with a '#' (KEY1=VALUE1#KEY2=VALUE2). Quote whole expression to avoid whitespace parsing issues\n"
			"Override fractal parameter in the form 'fractal<N>_KEY=VALUE' with <N> as index of fractal"),
		QCoreApplication::translate("main", "KEY=VALUE"));

	QCommandLineOption listOption(QStringList() << "L" << "list",
				QCoreApplication::translate("main", "Lists all possible parameters '<KEY>' with corresponding default value '<VALUE>'."));

	QCommandLineOption formatOption(QStringList() << "f" << "format",
		QCoreApplication::translate("main", "Image output format:\n"
			"jpg - JPEG format (default)\n"
			"png - PNG format\n"
			"png16 - 16-bit PNG format\n"
			"png16alpha - 16-bit PNG with alpha channel format\n"
			"exr - EXR format\n"
			"tiff - TIFF format"),
		QCoreApplication::translate("main", "FORMAT"));

	QCommandLineOption resOption(QStringList() << "r" << "res",
		QCoreApplication::translate("main", "Overrides image resolution."),
		QCoreApplication::translate("main", "WIDTHxHEIGHT"));

	QCommandLineOption fpkOption("fpk",
		QCoreApplication::translate("main", "Overrides frames per key parameter."),
		QCoreApplication::translate("main", "N"));

	QCommandLineOption serverOption(QStringList() << "S" << "server",
		QCoreApplication::translate("main", "Sets application as a server listening for clients."));

	QCommandLineOption hostOption(QStringList() << "H" << "host",
		QCoreApplication::translate("main", "Sets application as a client connected to server of given host address"
			" (Host can be of type IPv4, IPv6 and Domain name address)."),
		QCoreApplication::translate("main", "N.N.N.N"));

	QCommandLineOption portOption(QStringList() << "p" << "port",
		QCoreApplication::translate("main", "Sets network port number for netrender (default 5555)."),
		QCoreApplication::translate("main", "N"));

	QCommandLineOption noColorOption(QStringList() << "C" << "no-cli-color",
		QCoreApplication::translate("main", "Starts program without ANSI colors, when execution on CLI."));

	QCommandLineOption outputOption(QStringList() << "o" << "output",
		QCoreApplication::translate("main", "Saves rendered image(s) to this file / folder."),
		QCoreApplication::translate("main", "N"));

	QCommandLineOption queueOption(QStringList() << "q" << "queue",
								   QCoreApplication::translate("main", "Renders all images from common queue."));

	QCommandLineOption testOption(QStringList() << "t" << "test",
								   QCoreApplication::translate("main", "This will run testcases on the mandelbulber instance"));

	QCommandLineOption voxelOption(QStringList() << "V" << "voxel",
									 QCoreApplication::translate("main", "Renders the voxel volume in a stack of images."));

	QCommandLineOption statsOption(QStringList() << "stats",
		QCoreApplication::translate("main", "Shows statistics while rendering in CLI mode."));

	QCommandLineOption helpInputOption(QStringList() << "help-input",
		QCoreApplication::translate("main", "Shows help about input."));

	parser.addPositionalArgument("settings_file", QCoreApplication::translate("main",
		"file with fractal settings (program also tries\nto find file in ./mandelbulber/settings directory)\n"
		"When settings_file is put as a command line argument then program will start in noGUI mode"
		"<settings_file> can also be specified as a list, see all options with --help-input"
	));

	parser.addOption(noguiOption);
	parser.addOption(outputOption);
	parser.addOption(keyframeOption);
	parser.addOption(flightOption);
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
	parser.addOption(queueOption);
	parser.addOption(testOption);
	parser.addOption(voxelOption);
	parser.addOption(statsOption);
	parser.addOption(helpInputOption);

	// Process the actual command line arguments given by the user
	parser.process(*qapplication);
	args = parser.positionalArguments();

	cliData.nogui = parser.isSet(noguiOption);
	cliData.keyframe = parser.isSet(keyframeOption);
	cliData.flight = parser.isSet(flightOption);
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
	cliData.queue = parser.isSet(queueOption);
	cliData.voxel = parser.isSet(voxelOption);
	cliData.test = parser.isSet(testOption);
	cliData.showInputHelp = parser.isSet(helpInputOption);
	systemData.statsOnCLI = parser.isSet(statsOption);

#ifdef WIN32 /* WINDOWS */
	systemData.useColor = false;
#else
	systemData.useColor = !parser.isSet(noColorOption);
#endif  /* WINDOWS */

	if (cliData.listParameters) cliData.nogui = true;
	if (cliData.queue) cliData.nogui = true;
	if (cliData.test) cliData.nogui = true;
	cliTODO = modeBootOnly;
}

cCommandLineInterface::~cCommandLineInterface()
{

}

void cCommandLineInterface::ReadCLI()
{
	bool checkParse = true;
	bool settingsSpecified = false;
	QTextStream out(stdout);

	// show input help only
	if (cliData.showInputHelp)
	{
		out
				<< QObject::tr("Mandelbulber also accepts an arbitrary number of input files\n"
											 "These files can be of type:\n"
											 ".fract File - An ordinary fractal file\n"
											 ".fractlist File - A queue file, all entries inside the queue file will be added to the current queue\n"
											 "Folder - if the specified argument is a folder all .fract files inside the folder will be added to the queue\n");
		out.flush();
		exit(0);
	}

	// list parameters only
	if (cliData.listParameters)
	{
		QList<QString> listOfParameters = gPar->GetListOfParameters();
		out
				<< cHeadless::colorize("\nList of main parameters:\n",
															 cHeadless::ansiYellow,
															 cHeadless::noExplicitColor,
															 true);
		out << "KEY=VALUE\n";
		for (int i = 0; i < listOfParameters.size(); i++)
		{
			QString parameterName = listOfParameters.at(i);
			QString defaultValue = gPar->GetDefault<QString>(parameterName);
			out << parameterName + "=" + defaultValue + "\n";
		}

		QList<QString> listOfFractalParameters = gParFractal->at(0).GetListOfParameters();
		out
				<< cHeadless::colorize(QObject::tr("\nList of fractal parameters:\n"),
															 cHeadless::ansiYellow,
															 cHeadless::noExplicitColor,
															 true);

		for (int i = 0; i < listOfFractalParameters.size(); i++)
		{
			QString parameterName = listOfFractalParameters.at(i);
			QString defaultValue = gParFractal->at(0).GetDefault<QString>(parameterName);
			out << parameterName + "=" + defaultValue + "\n";
		}

		out.flush();
		exit(0);
	}

	// run test cases
	if (cliData.test) {
		QStringList arguments = gApplication->arguments();
		arguments.removeOne(QString("--test"));
		arguments.removeOne(QString("t"));

		int status = 0;
		Test test;
		status |= QTest::qExec(&test, arguments);
		exit(status);
	}

	// check netrender server / client
	if (cliData.server)
	{
		int port = gPar->Get<int>("netrender_server_local_port");

		if (cliData.portText != "")
		{
			port = cliData.portText.toInt(&checkParse);
			if (!checkParse || port <= 0)
			{
				cErrorMessage::showMessage(QObject::tr("Specified server port is invalid\n"),
																	 cErrorMessage::errorMessage);
				parser.showHelp(cliErrorServerInvalidPort);
			}
			gPar->Set("netrender_server_local_port", port);
		}
		cliData.nogui = true;
		systemData.noGui = true;
		gNetRender->SetServer(gPar->Get<int>("netrender_server_local_port"));
		QElapsedTimer timer;
		timer.start();

		if (systemData.noGui)
		{
			out << QObject::tr("NetRender - Waiting for clients\n");
			out.flush();
		}

		while (timer.elapsed() < 5000)
		{
			gApplication->processEvents();
		}
	}
	else if (cliData.host != "")
	{
		int port = gPar->Get<int>("netrender_client_remote_port");
		gPar->Set("netrender_client_remote_address", cliData.host);
		if (cliData.portText != "")
		{
			port = cliData.portText.toInt(&checkParse);
			if (!checkParse || port <= 0)
			{
				cErrorMessage::showMessage(QObject::tr("Specified client port is invalid\n"),
																	 cErrorMessage::errorMessage);
				parser.showHelp(cliErrorClientInvalidPort);
			}
			gPar->Set("netrender_client_remote_port", port);
		}
		cliData.nogui = true;
		systemData.noGui = true;
		cliTODO = modeNetrender;
		return;
	}

	if (cliData.queue)
	{
		cliTODO = modeQueue;
		settingsSpecified = true;
		cliData.nogui = true;
		systemData.noGui = true;
		try
		{
			gQueue = new cQueue(gMainInterface,
													systemData.dataDirectory + "queue.fractlist",
													systemData.dataDirectory + "queue",
													NULL);
		} catch (QString &ex)
		{
			cErrorMessage::showMessage(QObject::tr("Cannot init queue: ") + ex,
																 cErrorMessage::errorMessage);
			parser.showHelp(cliErrorQueueInit);
		}
	}
	else
	{
		if (args.size() > 0)
		{
			// file specified -> load it
			if (args.size() == 1 && QFileInfo(args[0]).suffix() != QString("fractlist")
					&& !QDir(args[0]).exists())
			{
				QString filename = args[0];
				if (!QFile::exists(filename))
				{
					// try to find settings in default settings path
					filename = systemData.dataDirectory + "settings" + QDir::separator() + filename;
				}
				if (QFile::exists(filename))
				{
					cSettings parSettings(cSettings::formatFullText);
					parSettings.LoadFromFile(filename);
					parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
					settingsSpecified = true;
					systemData.lastSettingsFile = filename;
				}
				else
				{
					cErrorMessage::showMessage(QObject::tr("Cannot load file!\n"),
																		 cErrorMessage::errorMessage);
					qCritical() << "\nSetting file " << filename << " not found\n";
					parser.showHelp(cliErrorLoadSettingsFile);
				}
			}
			else
			{
				// queue render
				cliTODO = modeQueue;
				cliData.nogui = true;
				systemData.noGui = true;
				try
				{
					gQueue = new cQueue(gMainInterface,
															systemData.dataDirectory + "queue.fractlist",
															systemData.dataDirectory + "queue",
															NULL);
				} catch (QString &ex)
				{
					cErrorMessage::showMessage(QObject::tr("Cannot init queue: ") + ex,
																		 cErrorMessage::errorMessage);
					parser.showHelp(cliErrorQueueInit);
				}
				for (int i = 0; i < args.size(); i++)
				{
					QString filename = args[i];
					if (QDir(args[i]).exists())
					{
						// specified input is a folder, load all fractal files contained in this folder
						gQueue->AppendFolder(filename);
						settingsSpecified = true;
					}
					else if (QFileInfo(filename).suffix() == QString("fractlist"))
					{
						// specified input is a queue list file, append all entries to the current queue
						gQueue->AppendList(filename);
						settingsSpecified = true;
					}
					else
					{
						// specified input can only be an ordinary fract file, try to append to queuelist
						gQueue->Append(filename);
						settingsSpecified = true;
					}
				}
			}
		}
	}

	// overwriting parameters
	if (cliData.overrideParametersText != "")
	{
		QStringList overrideParameters = cliData.overrideParametersText.split("#",
																																					QString::SkipEmptyParts);
		for (int i = 0; i < overrideParameters.size(); i++)
		{
			int fractalIndex = -1;
			QRegularExpression reType("^fractal([0-9]+)_(.*)$");
			QRegularExpressionMatch matchType = reType.match(overrideParameters[i]);
			if (matchType.hasMatch())
			{
				fractalIndex = matchType.captured(1).toInt() - 1;
				overrideParameters[i] = matchType.captured(2);
			}
			QStringList overrideParameter = overrideParameters[i].split(QRegExp("\\="));
			if (overrideParameter.size() == 2)
			{
				if (fractalIndex >= 0 && fractalIndex < NUMBER_OF_FRACTALS)
				{
					gParFractal->at(fractalIndex).Set(overrideParameter[0].trimmed(),
																						overrideParameter[1].trimmed());
				}
				else
				{
					gPar->Set(overrideParameter[0].trimmed(), overrideParameter[1].trimmed());
				}
			}
		}
	}

	// specified resolution
	if (cliData.resolution != "")
	{
		QStringList resolutionParameters = cliData.resolution.split(QRegExp("x"));
		if (resolutionParameters.size() == 2)
		{
			int xRes = resolutionParameters[0].toInt(&checkParse);
			int yRes = resolutionParameters[1].toInt(&checkParse);
			if (!checkParse || xRes <= 0 || yRes <= 0)
			{
				cErrorMessage::showMessage(QObject::tr("Specified resolution not valid\n"
																							 "both dimensions need to be > 0"),
																	 cErrorMessage::errorMessage);
				parser.showHelp(cliErrorResolutionInvalid);
			}
			gPar->Set("image_width", xRes);
			gPar->Set("image_height", yRes);
		}
		else
		{
			cErrorMessage::showMessage(QObject::tr("Specified resolution not valid\n"
																						 "resolution has to be in the form WIDTHxHEIGHT"),
																 cErrorMessage::errorMessage);
			parser.showHelp(cliErrorResolutionInvalid);
		}
	}

	// specified frames per keyframe
	if (cliData.fpkText != "")
	{
		int fpk = cliData.fpkText.toInt(&checkParse);
		if (!checkParse || fpk <= 0)
		{
			cErrorMessage::showMessage(QObject::tr("Specified frames per key not valid\n"
																						 "need to be > 0"),
																 cErrorMessage::errorMessage);
			parser.showHelp(cliErrorFPKInvalid);
		}
		gPar->Set("frames_per_keyframe", fpk);
	}

	// specified image file format
	if (cliData.imageFileFormat != "")
	{
		QStringList allowedImageFileFormat;
		allowedImageFileFormat << "jpg" << "png" << "png16" << "png16alpha" << "exr" << "tiff";
		if (!allowedImageFileFormat.contains(cliData.imageFileFormat))
		{
			cErrorMessage::showMessage(QObject::tr("Specified imageFileFormat is not valid\n"
																						 "allowed formats are: ")
																		 + allowedImageFileFormat.join(", "),
																 cErrorMessage::errorMessage);
			parser.showHelp(cliErrorImageFileFormatInvalid);
		}
	}
	else
	{
		cliData.imageFileFormat = "jpg";
	}

	//flight animation
	if (cliData.flight)
	{
		if (gAnimFrames->GetNumberOfFrames() > 0)
		{
			cliTODO = modeFlight;
			cliData.nogui = true;
			systemData.noGui = true;
		}
		else
		{
			cErrorMessage::showMessage(QObject::tr("There are no flight animation frames in specified settings file"),
																 cErrorMessage::errorMessage);
			parser.showHelp(cliErrorFlightNoFrames);
		}
	}

	//keyframe animation
	if (cliData.keyframe)
	{
		if (cliTODO == modeFlight)
		{
			cErrorMessage::showMessage(QObject::tr("You cannot render keyframe animation at the same time as flight animation"),
																 cErrorMessage::errorMessage);
		}
		else
		{
			if (gKeyframes->GetNumberOfFrames() > 0)
			{
				cliTODO = modeKeyframe;
				cliData.nogui = true;
				systemData.noGui = true;
			}
			else
			{
				cErrorMessage::showMessage(QObject::tr("There are no keyframes in specified settings file"),
																	 cErrorMessage::errorMessage);
				parser.showHelp(cliErrorKeyframeNoFrames);
			}
		}
	}

	// start frame of animation
	if (cliData.startFrameText != "")
	{
		int startFrame = cliData.startFrameText.toInt(&checkParse);
		if (cliTODO == modeFlight)
		{
			if (startFrame <= gAnimFrames->GetNumberOfFrames())
			{
				gPar->Set("flight_first_to_render", startFrame);
			}
			else
			{
				cErrorMessage::showMessage(QObject::tr("Animation has only %1 frames").arg(gAnimFrames->GetNumberOfFrames()),
																	 cErrorMessage::errorMessage);
				parser.showHelp(cliErrorFlightStartFrameOutOfRange);
			}
		}

		if (cliTODO == modeKeyframe)
		{
			int numberOfFrames = (gKeyframes->GetNumberOfFrames() - 1)
					* gPar->Get<int>("frames_per_keyframe");
			if (numberOfFrames < 0) numberOfFrames = 0;

			if (startFrame <= numberOfFrames)
			{
				gPar->Set("keyframe_first_to_render", startFrame);
			}
			else
			{
				cErrorMessage::showMessage(QObject::tr("Animation has only %1 frames").arg(numberOfFrames),
																	 cErrorMessage::errorMessage);
				parser.showHelp(cliErrorKeyframeStartFrameOutOfRange);
			}
		}
	}

	// end frame of animation
	if (cliData.endFrameText != "")
	{
		int endFrame = cliData.endFrameText.toInt(&checkParse);
		if (cliTODO == modeFlight)
		{
			if (endFrame <= gAnimFrames->GetNumberOfFrames())
			{
				if (endFrame > gPar->Get<int>("flight_first_to_render"))
				{
					gPar->Set("flight_last_to_render", endFrame);
				}
				else
				{
					cErrorMessage::showMessage(QObject::tr("End frame has to be greater than start frame which is %1").arg(gPar->Get<
																				 int>("flight_first_to_render")),
																		 cErrorMessage::errorMessage);
					parser.showHelp(cliErrorFlightEndFrameSmallerStartFrame);
				}
			}
			else
			{
				cErrorMessage::showMessage(QObject::tr("Animation has only %1 frames").arg(gAnimFrames->GetNumberOfFrames()),
																	 cErrorMessage::errorMessage);
				parser.showHelp(cliErrorFlightEndFrameOutOfRange);
			}
		}

		if (cliTODO == modeKeyframe)
		{
			int numberOfFrames = (gKeyframes->GetNumberOfFrames() - 1)
					* gPar->Get<int>("frames_per_keyframe");
			if (numberOfFrames < 0) numberOfFrames = 0;

			if (endFrame <= numberOfFrames)
			{
				if (endFrame > gPar->Get<int>("keyframe_first_to_render"))
				{
					gPar->Set("keyframe_last_to_render", endFrame);
				}
				else
				{
					cErrorMessage::showMessage(QObject::tr("End frame has to be greater than start frame which is %1").arg(gPar->Get<
																				 int>("keyframe_first_to_render")),
																		 cErrorMessage::errorMessage);
					parser.showHelp(cliErrorKeyframeEndFrameSmallerStartFrame);
				}
			}
			else
			{
				cErrorMessage::showMessage(QObject::tr("Animation has only %1 frames").arg(numberOfFrames),
																	 cErrorMessage::errorMessage);
				parser.showHelp(cliErrorKeyframeEndFrameOutOfRange);
			}
		}
	}

	//voxel export
	if (cliData.voxel)
	{
		cliTODO = modeVoxel;
		cliData.nogui = true;
		systemData.noGui = true;
	}

	//folder for animation frames
	if (cliData.outputText != "" && cliTODO == modeFlight)
	{
		gPar->Set("anim_flight_dir", cliData.outputText);
	}
	if (cliData.outputText != "" && cliTODO == modeKeyframe)
	{
		gPar->Set("anim_keyframe_dir", cliData.outputText);
	}

	if (!settingsSpecified && cliData.nogui && cliTODO != modeNetrender)
	{
		cErrorMessage::showMessage(QObject::tr("You have to specify a settings file, for this configuration!"),
															 cErrorMessage::errorMessage);
		parser.showHelp(cliErrorSettingsFileNotSpecified);
	}

	if (cliData.nogui && cliTODO != modeKeyframe && cliTODO != modeFlight && cliTODO != modeQueue && cliTODO != modeVoxel)
	{
		//creating output filename if it's not specified
		if (cliData.outputText == "")
		{
			cliData.outputText = gPar->Get<QString>("default_image_path") + QDir::separator();
			cliData.outputText += QFileInfo(systemData.lastSettingsFile).completeBaseName();
		}
		cliTODO = modeStill;
		return;
	}
}

void cCommandLineInterface::ProcessCLI(void)
{
	switch (cliTODO)
	{
		case modeNetrender:
		{
			gNetRender->SetClient(gPar->Get<QString>("netrender_client_remote_address"),
														gPar->Get<int>("netrender_client_remote_port"));
			gApplication->exec();
			break;
		}
		case modeFlight:
		{
			gMainInterface->headless = new cHeadless();
			gMainInterface->headless->RenderFlightAnimation();
			break;
		}
		case modeKeyframe:
		{
			gMainInterface->headless = new cHeadless();
			gMainInterface->headless->RenderKeyframeAnimation();
			break;
		}
		case modeStill:
		{
			gMainInterface->headless = new cHeadless();
			gMainInterface->headless->RenderStillImage(cliData.outputText, cliData.imageFileFormat);
			break;
		}
		case modeQueue:
		{
			gMainInterface->headless = new cHeadless();
			gMainInterface->headless->RenderQueue();
			break;
		}
		case modeVoxel:
		{
			gMainInterface->headless = new cHeadless();
			gMainInterface->headless->RenderVoxel();
			break;
		}
		case modeBootOnly:
		{
			// nothing to be done
			break;
		}
	}
}
