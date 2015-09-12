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

#ifndef MANDELBULBER2_SRC_CCOMMANDLINEINTERFACE_HPP_
#define MANDELBULBER2_SRC_CCOMMANDLINEINTERFACE_HPP_
#include <QtCore>
#include <qapplication.h>

namespace cli
{
	enum cliTODOMode
	{
		bootOnly, netrender, keyframe, flight, still
	};
}

class cCommandLineInterface
{
public:
	cCommandLineInterface(QApplication *qapplication);
	~cCommandLineInterface();

	void ReadCLI(void);
	void ProcessCLI(void);
	bool isNoGUI(void) {return cliData.nogui;}
private:
	struct sCliData
	{
		bool nogui;
		QString animationMode;
		QString startFrameText;
		QString endFrameText;
		QString overrideParametersText;
		QString imageFileFormat;
		QString resolution;
		QString fpkText;
		QString host;
		QString portText;
	} cliData;

	QCommandLineParser parser;
	QStringList args;
	cli::cliTODOMode cliTODO;
};

#endif /* MANDELBULBER2_SRC_CCOMMANDLINEINTERFACE_HPP_ */
