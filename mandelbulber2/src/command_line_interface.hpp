/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-16 Krzysztof Marczak     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen
 *
 * cCommandLineInterface - CLI Input handler
 */

#ifndef MANDELBULBER2_SRC_COMMAND_LINE_INTERFACE_HPP_
#define MANDELBULBER2_SRC_COMMAND_LINE_INTERFACE_HPP_
#include <QtCore>

class cCommandLineInterface
{
public:
	cCommandLineInterface(QCoreApplication *qapplication);
	~cCommandLineInterface();

	enum cliTODOMode
	{
		modeBootOnly,
		modeNetrender,
		modeKeyframe,
		modeFlight,
		modeStill,
		modeQueue,
		modeVoxel
	};
	enum cliErrors
	{
		cliErrorServerInvalidPort = -10,
		cliErrorClientInvalidPort = -11,
		cliErrorQueueInit = -12,
		cliErrorLoadSettingsFile = -13,
		cliErrorResolutionInvalid = -14,
		cliErrorFPKInvalid = -15,
		cliErrorImageFileFormatInvalid = -16,
		cliErrorSettingsFileNotSpecified = -17,

		cliErrorFlightNoFrames = -30,
		cliErrorFlightStartFrameOutOfRange = -31,
		cliErrorFlightEndFrameSmallerStartFrame = -32,
		cliErrorFlightEndFrameOutOfRange = -33,

		cliErrorKeyframeNoFrames = -40,
		cliErrorKeyframeStartFrameOutOfRange = -41,
		cliErrorKeyframeEndFrameSmallerStartFrame = -42,
		cliErrorKeyframeEndFrameOutOfRange = -43,

		cliErrorVoxelOutputFolderDoesNotExists = -50
	};

	void ReadCLI();
	void ProcessCLI() const;
	bool isNoGUI() const { return cliData.nogui; }

private:
	// ## helper methods for ReadCLI
	// arguments to cause print and exit
	static void printExampleHelpAndExit();
	static void printInputHelpAndExit();
	static void printParametersAndExit();
	void runTestCasesAndExit() const;
	void runBenchmarksAndExit() const;

	// argument handling methods
	void handleServer();
	void handleClient();
	void handleQueue();
	void handleArgs();
	void handleOverrideParameters() const;
	void handleResolution();
	void handleFpk();
	void handleImageFileFormat();
	void handleFlight();
	void handleKeyframe();
	void handleStartFrame();
	void handleEndFrame();
	void handleVoxel();

	struct sCliData
	{
		bool nogui;
		bool server;
		bool listParameters;
		bool showInputHelp;
		bool showExampleHelp;
		bool keyframe;
		bool flight;
		bool silent;
		bool queue;
		bool voxel;
		bool test;
		bool benchmark;
		bool touch;
		QString startFrameText;
		QString endFrameText;
		QString overrideParametersText;
		QString imageFileFormat;
		QString resolution;
		QString fpkText;
		QString host;
		QString portText;
		QString outputText;
	} cliData;

	QCommandLineParser parser;
	QStringList args;
	cliTODOMode cliTODO;
	bool settingsSpecified;
};

#endif /* MANDELBULBER2_SRC_COMMAND_LINE_INTERFACE_HPP_ */
