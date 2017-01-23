/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * cCommandLineInterface - CLI Input handler
 */

#include "command_line_interface.hpp"

#include "../src/interface.hpp"
#include "animation_frames.hpp"
#include "error_message.hpp"
#include "fractal_container.hpp"
#include "global_data.hpp"
#include "headless.h"
#include "initparameters.hpp"
#include "keyframes.hpp"
#include "netrender.hpp"
#include "queue.hpp"
#include "settings.hpp"
#include "system.hpp"
#include "test.hpp"

cCommandLineInterface::cCommandLineInterface(QCoreApplication *qapplication)
		: settingsSpecified(false)
{
	// text from http://sourceforge.net/projects/mandelbulber/
	parser.setApplicationDescription(QCoreApplication::translate("main",
		"Mandelbulber is an easy to use, "
		"handy application designed to help you render 3D Mandelbrot fractals called Mandelbulb "
		"and some other kind of 3D fractals like Mandelbox, Bulbbox, Juliabulb, Menger Sponge"));

	parser.addHelpOption();
	parser.addVersionOption();

	QCommandLineOption noguiOption(QStringList({"n", "nogui"}),
		QCoreApplication::translate("main", "Starts the program without a GUI."));

	QCommandLineOption keyframeOption(QStringList({"K", "keyframe"}),
		QCoreApplication::translate("main", "Renders keyframe animation."));

	QCommandLineOption flightOption(
		QStringList({"F", "flight"}), QCoreApplication::translate("main", "Renders flight animation."));

	QCommandLineOption silentOption(QStringList({"X", "never-delete"}),
		QCoreApplication::translate("main", "Never delete data, instead Exit CLI application."));

	QCommandLineOption startOption(QStringList({"s", "start"}),
		QCoreApplication::translate("main", "Starts rendering from frame number <N>."),
		QCoreApplication::translate("main", "N"));

	QCommandLineOption endOption(QStringList({"e", "end"}),
		QCoreApplication::translate("main", "Stops rendering on frame number <N>."),
		QCoreApplication::translate("main", "N"));

	QCommandLineOption overrideOption(
		QStringList({"O", "override"}),
		QCoreApplication::translate("main",
			"<KEY=VALUE> overrides item '<KEY>' from settings file with new value '<VALUE>'.\n"
			"Specify multiple KEY=VALUE pairs by separating with a '#': <KEY1=VALUE1#KEY2=VALUE2>. Quote "
			"whole expression to avoid whitespace parsing issues\n"
			"Override fractal parameter in the form 'fractal<N>_KEY=VALUE' with <N> being index of "
			"fractal"),
		QCoreApplication::translate("main", "..."));

	QCommandLineOption listOption(
		QStringList({"L", "list"}),
		QCoreApplication::translate(
			"main", "Lists all possible parameters '<KEY>' with corresponding default value '<VALUE>'."));

	QCommandLineOption formatOption(QStringList({"f", "format"}),
		QCoreApplication::translate("main",
																		"Image output format:\n"
																		"  jpg - JPEG format (default)\n"
																		"  png - PNG format\n"
																		"  png16 - 16-bit PNG format\n"
																		"  png16alpha - 16-bit PNG with alpha channel format\n"
																		"  exr - EXR format\n"
																		"  tiff - TIFF format"),
		QCoreApplication::translate("main", "FORMAT"));

	QCommandLineOption resOption(
		QStringList({"r", "res"}),
		QCoreApplication::translate(
			"main", "Overrides image resolution. Specify as width and height separated by 'x'"),
		QCoreApplication::translate("main", "WxH"));

	QCommandLineOption fpkOption("fpk",
		QCoreApplication::translate("main", "Overrides frames per key parameter."),
		QCoreApplication::translate("main", "N"));

	QCommandLineOption serverOption(QStringList({"S", "server"}),
		QCoreApplication::translate("main", "Sets application as a server listening for clients."));

	QCommandLineOption hostOption(
		QStringList({"H", "host"}),
		QCoreApplication::translate("main",
			"Sets application as a client connected to server of given host address"
			" (Host can be of type IPv4, IPv6 and Domain name address)."),
		QCoreApplication::translate("main", "N.N.N.N"));

	QCommandLineOption portOption(QStringList({"p", "port"}),
		QCoreApplication::translate("main", "Sets network port number for netrender (default 5555)."),
		QCoreApplication::translate("main", "N"));

	QCommandLineOption noColorOption(QStringList({"C", "no-cli-color"}),
		QCoreApplication::translate("main",
																		 "Starts program without ANSI colors, when execution on CLI."));

	QCommandLineOption outputOption(QStringList({"o", "output"}),
		QCoreApplication::translate("main", "Saves rendered image(s) to this file / folder."),
		QCoreApplication::translate("main", "N"));

	QCommandLineOption queueOption(QStringList({"q", "queue"}),
		QCoreApplication::translate("main", "Renders all images from common queue."));

	QCommandLineOption testOption(QStringList({"t", "test"}),
		QCoreApplication::translate("main", "Runs testcases on the mandelbulber instance"));

	QCommandLineOption benchmarkOption(QStringList({"b", "benchmark"}),
		QCoreApplication::translate("main", "Runs benchmarks on the mandelbulber instance"));

	QCommandLineOption touchOption(
		QStringList({"T", "touch"}),
		QCoreApplication::translate(
			"main", "Resaves a settings file (can be used to update a settings file)"));

	QCommandLineOption voxelOption(QStringList({"V", "voxel"}),
		QCoreApplication::translate("main", "Renders the voxel volume in a stack of images."));

	QCommandLineOption statsOption(QStringList({"stats"}),
		QCoreApplication::translate("main", "Shows statistics while rendering in CLI mode."));

	QCommandLineOption helpInputOption(
		QStringList({"help-input"}), QCoreApplication::translate("main", "Shows help about input."));
	QCommandLineOption helpExamplesOption(
		QStringList({"help-examples"}), QCoreApplication::translate("main", "Shows example commands."));

	parser.addPositionalArgument(
		"settings_file",
		QCoreApplication::translate("main",
			"file with fractal settings (program also tries\nto find file in ./mandelbulber/settings "
			"directory)\n"
			"When settings_file is put as a command line argument then program will start in noGUI mode"
			"<settings_file> can also be specified as a list, see all options with --help-input"));

	parser.addOption(noguiOption);
	parser.addOption(outputOption);
	parser.addOption(keyframeOption);
	parser.addOption(flightOption);
	parser.addOption(silentOption);
	parser.addOption(startOption);
	parser.addOption(endOption);
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
	parser.addOption(benchmarkOption);
	parser.addOption(touchOption);
	parser.addOption(voxelOption);
	parser.addOption(overrideOption);
	parser.addOption(statsOption);
	parser.addOption(helpInputOption);
	parser.addOption(helpExamplesOption);

	// Process the actual command line arguments given by the user
	parser.process(*qapplication);
	args = parser.positionalArguments();
	if (!args.empty() && args[0] == "-")
	{
		// "-" marks stdin to be used as the input for the program (filename or content)
		// use the input as first argument
		QTextStream qin(stdin);
		args[0] = qin.readAll();
	}

	cliData.nogui = parser.isSet(noguiOption);
	cliData.keyframe = parser.isSet(keyframeOption);
	cliData.flight = parser.isSet(flightOption);
	cliData.silent = parser.isSet(silentOption);
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
	cliData.benchmark = parser.isSet(benchmarkOption);
	cliData.touch = parser.isSet(touchOption);
	cliData.showInputHelp = parser.isSet(helpInputOption);
	cliData.showExampleHelp = parser.isSet(helpExamplesOption);
	systemData.statsOnCLI = parser.isSet(statsOption);

#ifdef WIN32 /* WINDOWS */
	systemData.useColor = false;
#else
	systemData.useColor = !parser.isSet(noColorOption);
#endif /* WINDOWS */

	if (cliData.listParameters) cliData.nogui = true;
	if (cliData.queue) cliData.nogui = true;
	if (cliData.test) cliData.nogui = true;
	if (cliData.benchmark) cliData.nogui = true;
	cliTODO = modeBootOnly;
}

cCommandLineInterface::~cCommandLineInterface()
{
}

void cCommandLineInterface::ReadCLI()
{
	settingsSpecified = false;

	// show example help only
	if (cliData.showExampleHelp) printExampleHelpAndExit();
	// show input help only
	if (cliData.showInputHelp) printInputHelpAndExit();
	// list parameters only
	if (cliData.listParameters) printParametersAndExit();

	// run test cases
	if (cliData.test) runTestCasesAndExit();
	// run benchmarks
	if (cliData.benchmark) runBenchmarksAndExit();

	// check netrender server / client
	if (cliData.server)
		handleServer();
	else if (cliData.host != "")
	{
		handleClient();
		return;
	}

	if (cliData.queue)
		handleQueue();
	else
		handleArgs();

	// overwriting parameters
	if (cliData.overrideParametersText != "") handleOverrideParameters();

	// specified resolution
	if (cliData.resolution != "") handleResolution();

	// specified frames per keyframe
	if (cliData.fpkText != "") handleFpk();

	// specified image file format
	if (cliData.imageFileFormat != "")
		handleImageFileFormat();
	else
		cliData.imageFileFormat = "jpg";

	// silent mode
	if (cliData.silent) systemData.silent = true;

	// flight animation
	if (cliData.flight) handleFlight();

	// keyframe animation
	if (cliData.keyframe) handleKeyframe();

	// start frame of animation
	if (cliData.startFrameText != "") handleStartFrame();

	// end frame of animation
	if (cliData.endFrameText != "") handleEndFrame();

	// voxel export
	if (cliData.voxel) handleVoxel();

	// folder for animation frames
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
		cErrorMessage::showMessage(
			QObject::tr("You have to specify a settings file, for this configuration!"),
			cErrorMessage::errorMessage);
		parser.showHelp(cliErrorSettingsFileNotSpecified);
	}

	if (cliData.nogui && cliTODO != modeKeyframe && cliTODO != modeFlight && cliTODO != modeQueue
			&& cliTODO != modeVoxel)
	{
		// creating output filename if it's not specified
		if (cliData.outputText == "")
		{
			cliData.outputText = gPar->Get<QString>("default_image_path") + QDir::separator();
			cliData.outputText += QFileInfo(systemData.lastSettingsFile).completeBaseName();
		}
		cliTODO = modeStill;
		return;
	}
}

void cCommandLineInterface::ProcessCLI() const
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

void cCommandLineInterface::printExampleHelpAndExit()
{
	QTextStream out(stdout);
	out << cHeadless::colorize(QObject::tr("Some useful example commands:"), cHeadless::ansiRed)
			<< "\n\n";

	out << cHeadless::colorize(QObject::tr("Simple render"), cHeadless::ansiBlue) << "\n";
	out << cHeadless::colorize("mandelbulber2 -n path/to/fractal.fract", cHeadless::ansiYellow)
			<< "\n";
	out << QObject::tr("Renders the file on the cli (no window required).") << "\n\n";

	out << cHeadless::colorize(QObject::tr("Animation render"), cHeadless::ansiBlue) << "\n";
	out << cHeadless::colorize("mandelbulber2 -n -K -s 200 -e 300 path/to/keyframe_fractal.fract",
					 cHeadless::ansiYellow)
			<< "\n";
	out << QObject::tr(
					 "Renders the keyframe animation of the file keyframe_fractal.fract "
					 "within frames 200 till 300.")
			<< "\n\n";

	out << cHeadless::colorize(QObject::tr("Network render"), cHeadless::ansiBlue) << "\n";
	out << cHeadless::colorize("mandelbulber2 -n --host 192.168.100.1", cHeadless::ansiYellow)
			<< cHeadless::colorize(" # (1) client", cHeadless::ansiGreen) << "\n";
	out << cHeadless::colorize(
					 "mandelbulber2 -n --server path/to/fractal.fract", cHeadless::ansiYellow)
			<< cHeadless::colorize(" # (2) server", cHeadless::ansiGreen) << "\n";
	out << QObject::tr(
					 "In a network you can render on multiple machines. One is a server (2) and multiple "
					 "clients (1) can connect to help rendering.\n"
					 "On each client run (1), 192.168.100.1 should be substituted with the IP address of "
					 "the server.\nOn the server run (2) with the settings required for the render and "
					 "additionally '--server'.\nThe server will start and wait a short time for the "
					 "clients to connect. Then the whole system will start rendering.")
			<< "\n\n";

	out << cHeadless::colorize(QObject::tr("Voxel volume render"), cHeadless::ansiBlue) << "\n";
	out << cHeadless::colorize(
					 "mandelbulber2 --voxel -n path/to/voxel_fractal.fract"
					 " -O 'voxel_custom_limit_enabled=1#voxel_limit_min=-1 -1 -1#voxel_limit_max=1 1 "
					 "1#voxel_samples_x=10#voxel_samples_y=10#voxel_samples_z=10'",
					 cHeadless::ansiYellow)
			<< "\n";
	out << QObject::tr(
					 "Renders the voxel volume in the bounding box of [x(-1 - 1); y(-1 - 1); z(-1 - 1)] "
					 "with a resolution of 10x10x10.\nThis will produce 10 slices (z) with a resolution "
					 "of 10(x) times 10(y) and save as black and white images to working folder/slices.")
			<< "\n\n";

	out << cHeadless::colorize(QObject::tr("Queue render"), cHeadless::ansiBlue) << "\n";
	out << cHeadless::colorize(
					 "nohup mandelbulber2 -q > /tmp/queue.log 2>&1 &", cHeadless::ansiYellow)
			<< "\n";
	out << QObject::tr(
					 "Runs the mandelbulber instance in queue mode and daemonizes it.\n"
					 "Mandelbulber runs in background and waits for jobs.\n"
					 "The output will be written to /tmp/queue.log.\n"
					 "(will not work under Windows)")
			<< "\n\n";

	out.flush();
	exit(0);
}

void cCommandLineInterface::printInputHelpAndExit()
{
	QTextStream out(stdout);
	out << QObject::tr(
		"Mandelbulber also accepts an arbitrary number of input files\n"
		"These files can be of type:\n"
		"  .fract File - An ordinary fractal file\n"
		"  .fractlist File - A queue file, all entries inside the queue file will be added to the "
		"current queue\n"
		"  Folder - if the specified argument is a folder all .fract files inside the folder will be "
		"added to the queue\n"
		"You can also use \"-\" as a special argument to indicate that the filename, or the whole "
		"file contents are specified in stdin, example: cat example.fract | mandelbulber2 -\n");
	out.flush();
	exit(0);
}

void cCommandLineInterface::printParametersAndExit()
{
	QTextStream out(stdout);
	InitMaterialParams(1, gPar);
	QList<QString> listOfParameters = gPar->GetListOfParameters();
	out << cHeadless::colorize(
		"\nList of main parameters:\n", cHeadless::ansiYellow, cHeadless::noExplicitColor, true);
	out << "KEY=VALUE\n";
	for (int i = 0; i < listOfParameters.size(); i++)
	{
		QString parameterName = listOfParameters.at(i);
		QString defaultValue = gPar->GetDefault<QString>(parameterName);
		out << parameterName + "=" + defaultValue + "\n";
	}

	QList<QString> listOfFractalParameters = gParFractal->at(0).GetListOfParameters();
	out << cHeadless::colorize(QObject::tr("\nList of fractal parameters:\n"), cHeadless::ansiYellow,
		cHeadless::noExplicitColor, true);

	for (int i = 0; i < listOfFractalParameters.size(); i++)
	{
		QString parameterName = listOfFractalParameters.at(i);
		QString defaultValue = gParFractal->at(0).GetDefault<QString>(parameterName);
		out << parameterName + "=" + defaultValue + "\n";
	}

	out.flush();
	exit(0);
}

void cCommandLineInterface::runTestCasesAndExit() const
{
	QStringList arguments = gApplication->arguments();
	arguments.removeOne(QString("--test"));
	arguments.removeOne(QString("-t"));

	int status = 0;
	Test test(Test::simpleTestMode);
	status |= QTest::qExec(&test, arguments);
	exit(status);
}

void cCommandLineInterface::runBenchmarksAndExit() const
{
	QStringList arguments = gApplication->arguments();
	arguments.removeOne(QString("--benchmark"));
	arguments.removeOne(QString("-b"));

	int status = 0;
	Test test(Test::benchmarkTestMode);
	status |= QTest::qExec(&test, arguments);
	exit(status);
}

void cCommandLineInterface::handleServer()
{
	QTextStream out(stdout);
	bool checkParse = true;
	if (cliData.portText != "")
	{
		int port = cliData.portText.toInt(&checkParse);
		if (!checkParse || port <= 0)
		{
			cErrorMessage::showMessage(
				QObject::tr("Specified server port is invalid\n"), cErrorMessage::errorMessage);
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

void cCommandLineInterface::handleClient()
{
	bool checkParse = true;
	gPar->Set("netrender_client_remote_address", cliData.host);
	if (cliData.portText != "")
	{
		int port = cliData.portText.toInt(&checkParse);
		if (!checkParse || port <= 0)
		{
			cErrorMessage::showMessage(
				QObject::tr("Specified client port is invalid\n"), cErrorMessage::errorMessage);
			parser.showHelp(cliErrorClientInvalidPort);
		}
		gPar->Set("netrender_client_remote_port", port);
	}
	cliData.nogui = true;
	systemData.noGui = true;
	cliTODO = modeNetrender;
}

void cCommandLineInterface::handleQueue()
{
	cliTODO = modeQueue;
	settingsSpecified = true;
	cliData.nogui = true;
	systemData.noGui = true;
	try
	{
		gQueue = new cQueue(
			gMainInterface, systemData.GetQueueFractlistFile(), systemData.GetQueueFolder(), nullptr);
	}
	catch (QString &ex)
	{
		cErrorMessage::showMessage(
			QObject::tr("Cannot init queue: ") + ex, cErrorMessage::errorMessage);
		parser.showHelp(cliErrorQueueInit);
	}
}

void cCommandLineInterface::handleArgs()
{
	if (args.size() > 0)
	{
		// file specified -> load it
		if (args.size() == 1 && QFileInfo(args[0]).suffix() != QString("fractlist")
				&& !QDir(args[0]).exists())
		{
			QString filename = args[0];
			cSettings parSettings(cSettings::formatFullText);
			if (!QFile::exists(filename))
			{
				// try to find settings in default settings path
				filename = systemData.GetSettingsFolder() + QDir::separator() + filename;
			}
			if (QFile::exists(filename))
			{
				parSettings.LoadFromFile(filename);
				parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
				settingsSpecified = true;
				systemData.lastSettingsFile = filename;
				systemData.settingsLoadedFromCLI = true;
				if (cliData.touch)
				{
					parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
					parSettings.SaveToFile(filename);
					qDebug() << "touched file: " << filename;
					exit(0);
				}
			}
			else if (parSettings.LoadFromString(args[0]))
			{
				// the whole settings file is specified as an argument from stdin
				parSettings.Decode(gPar, gParFractal, gAnimFrames, gKeyframes);
				settingsSpecified = true;
				systemData.lastSettingsFile = "from stdin";
				systemData.settingsLoadedFromCLI = true;
			}
			else
			{
				cErrorMessage::showMessage(QObject::tr("Cannot load file!\n"), cErrorMessage::errorMessage);
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
				gQueue = new cQueue(
					gMainInterface, systemData.GetQueueFractlistFile(), systemData.GetQueueFolder(), nullptr);
			}
			catch (QString &ex)
			{
				cErrorMessage::showMessage(
					QObject::tr("Cannot init queue: ") + ex, cErrorMessage::errorMessage);
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

void cCommandLineInterface::handleOverrideParameters() const
{
	QStringList overrideParameters =
		cliData.overrideParametersText.split("#", QString::SkipEmptyParts);
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
				gParFractal->at(fractalIndex)
					.Set(overrideParameter[0].trimmed(), overrideParameter[1].trimmed());
			}
			else
			{
				gPar->Set(overrideParameter[0].trimmed(), overrideParameter[1].trimmed());
			}
		}
	}
}

void cCommandLineInterface::handleResolution()
{
	bool checkParse = true;
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

void cCommandLineInterface::handleFpk()
{
	bool checkParse = true;
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

void cCommandLineInterface::handleImageFileFormat()
{
	QStringList allowedImageFileFormat({"jpg", "png", "png16", "png16alpha", "exr", "tiff"});
	if (!allowedImageFileFormat.contains(cliData.imageFileFormat))
	{
		cErrorMessage::showMessage(QObject::tr("Specified imageFileFormat is not valid\n"
																					 "allowed formats are: ")
																 + allowedImageFileFormat.join(", "),
			cErrorMessage::errorMessage);
		parser.showHelp(cliErrorImageFileFormatInvalid);
	}
}

void cCommandLineInterface::handleFlight()
{
	if (gAnimFrames->GetNumberOfFrames() > 0)
	{
		cliTODO = modeFlight;
		cliData.nogui = true;
		systemData.noGui = true;
	}
	else
	{
		cErrorMessage::showMessage(
			QObject::tr("There are no flight animation frames in specified settings file"),
			cErrorMessage::errorMessage);
		parser.showHelp(cliErrorFlightNoFrames);
	}
}

void cCommandLineInterface::handleKeyframe()
{
	if (cliTODO == modeFlight)
	{
		cErrorMessage::showMessage(
			QObject::tr("You cannot render keyframe animation at the same time as flight animation"),
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

void cCommandLineInterface::handleStartFrame()
{
	bool checkParse = true;
	int startFrame = cliData.startFrameText.toInt(&checkParse);
	if (cliTODO == modeFlight)
	{
		if (startFrame <= gAnimFrames->GetNumberOfFrames())
		{
			gPar->Set("flight_first_to_render", startFrame);
		}
		else
		{
			cErrorMessage::showMessage(
				QObject::tr("Animation has only %1 frames").arg(gAnimFrames->GetNumberOfFrames()),
				cErrorMessage::errorMessage);
			parser.showHelp(cliErrorFlightStartFrameOutOfRange);
		}
	}

	if (cliTODO == modeKeyframe)
	{
		int numberOfFrames =
			(gKeyframes->GetNumberOfFrames() - 1) * gPar->Get<int>("frames_per_keyframe");
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

void cCommandLineInterface::handleEndFrame()
{
	bool checkParse = true;
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
				cErrorMessage::showMessage(
					QObject::tr("End frame has to be greater than start frame which is %1")
						.arg(gPar->Get<int>("flight_first_to_render")),
					cErrorMessage::errorMessage);
				parser.showHelp(cliErrorFlightEndFrameSmallerStartFrame);
			}
		}
		else
		{
			cErrorMessage::showMessage(
				QObject::tr("Animation has only %1 frames").arg(gAnimFrames->GetNumberOfFrames()),
				cErrorMessage::errorMessage);
			parser.showHelp(cliErrorFlightEndFrameOutOfRange);
		}
	}

	if (cliTODO == modeKeyframe)
	{
		int numberOfFrames =
			(gKeyframes->GetNumberOfFrames() - 1) * gPar->Get<int>("frames_per_keyframe");
		if (numberOfFrames < 0) numberOfFrames = 0;

		if (endFrame <= numberOfFrames)
		{
			if (endFrame > gPar->Get<int>("keyframe_first_to_render"))
			{
				gPar->Set("keyframe_last_to_render", endFrame);
			}
			else
			{
				cErrorMessage::showMessage(
					QObject::tr("End frame has to be greater than start frame which is %1")
						.arg(gPar->Get<int>("keyframe_first_to_render")),
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

void cCommandLineInterface::handleVoxel()
{
	QString folderString = gPar->Get<QString>("voxel_image_path");
	QDir folder(folderString);
	if (!folder.exists())
	{
		cErrorMessage::showMessage(
			QObject::tr("Cannot start voxel export. Specified folder (%1) does not exist.")
				.arg(folderString),
			cErrorMessage::errorMessage);
		parser.showHelp(cliErrorVoxelOutputFolderDoesNotExists);
	}
	cliTODO = modeVoxel;
	cliData.nogui = true;
	systemData.noGui = true;
}
