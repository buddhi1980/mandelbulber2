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

#include <string>

using namespace std;

struct sSystem
{
	string homedir;
	string dataDirectory;
	string sharedDir;
	string logfileName;
	unsigned int numberOfThreads;
};

struct sActualFileNames
{
	string actualFilenameSettings;
	string actualFilenameImage;
	string actualFilenamePalette;
};


extern sSystem systemData;
extern sActualFileNames actualFileNames;

bool InitSystem(void);
void WriteLog(string text);
void WriteLogDouble(string text, double value);
void WriteLogString(string text, string value);
int get_cpu_count();
bool CreateDefaultFolders(void);
bool CreateDirectory(string name);
int fcopy(string source, string dest);

#endif /* SYSTEM_HPP_ */
