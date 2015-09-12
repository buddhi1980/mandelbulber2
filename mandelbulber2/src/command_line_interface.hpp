/*
 * cCommandLineInterface.hpp
 *
 *  Created on: 11-09-2015
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_CCOMMANDLINEINTERFACE_HPP_
#define MANDELBULBER2_SRC_CCOMMANDLINEINTERFACE_HPP_
#include <QtCore>
#include <qapplication.h>

class cCommandLineInterface
{
public:
	cCommandLineInterface(QCoreApplication *qapplication);
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
  QString cliTODO;
};

#endif /* MANDELBULBER2_SRC_CCOMMANDLINEINTERFACE_HPP_ */
