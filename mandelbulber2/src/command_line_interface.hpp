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

#ifndef MANDELBULBER2_SRC_CCOMMANDLINEINTERFACE_HPP_
#define MANDELBULBER2_SRC_CCOMMANDLINEINTERFACE_HPP_
#include <QtCore>

class cCommandLineInterface
{
public:
	cCommandLineInterface(QCoreApplication *qapplication);
	~cCommandLineInterface();

	enum cliTODOMode
	{
		modeBootOnly, modeNetrender, modeKeyframe, modeFlight, modeStill, modeQueue
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
		cliErrorKeyframeEndFrameOutOfRange = -43
	};

	void ReadCLI(void);
	void ProcessCLI(void);
	bool isNoGUI(void)
	{
		return cliData.nogui;
	}
private:
	struct sCliData
	{
		bool nogui;
		bool server;
		bool listParameters;
		bool showInputHelp;
		bool keyframe;
		bool flight;
		bool queue;
		bool test;
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
};

#endif /* MANDELBULBER2_SRC_CCOMMANDLINEINTERFACE_HPP_ */
