/*
 * system.hpp
 *
 *  Created on: Mar 9, 2014
 *      Author: krzysztof
 */

#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#define MANDELBULBER_VERSION 2.0
#define MANDELBULBER_VERSION_STRING "2.0"
#define TO_STRING(a)	#a

#ifdef WIN32 /* WINDOWS */
#include <windows.h>
#define HAVE_BOOLEAN  /* prevent jmorecfg.h from redefining it */
#endif

#ifndef SHARED_DIR
	#ifndef WIN32
		#define SHARED_DIR "/usr/share/mandelbulber"
	#endif
#endif

#include <QtCore>

using namespace std;

struct sSystem
{
	QString homedir;
	QString dataDirectory;
	QString sharedDir;
	QString logfileName;
	QString lastSettingsFile;
	QString lastImageFile;
	QString lastImagePaletteFile;
	unsigned int numberOfThreads;
	bool noGui;
};

struct sActualFileNames
{
	QString actualFilenameSettings;
	QString actualFilenameImage;
	QString actualFilenamePalette;
};


extern sSystem systemData;
extern sActualFileNames actualFileNames;

bool InitSystem(void);
void WriteLog(QString text);
void WriteLogDouble(QString text, double value);
void WriteLogString(QString text, QString value);
int get_cpu_count();
bool CreateDefaultFolders(void);
bool CreateDirectory(QString name);
int fcopy(QString source, QString dest);
void Wait(long int time);

#endif /* SYSTEM_HPP_ */
