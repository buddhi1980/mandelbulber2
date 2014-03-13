/*
 * system.cpp
 *
 *  Created on: Mar 9, 2014
 *      Author: krzysztof
 */

#include "system.hpp"

#include <unistd.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>
#include <errno.h>
#include <string.h>

#define CLSUPPORT

sSystem systemData;
sActualFileNames actualFileNames;

bool InitSystem(void)
{
	using namespace std;

	setlocale(LC_ALL, "");

	//read $home env variable
	char *homedir = NULL;
#ifdef WIN32 /* WINDOWS */
	homedir = getenv("USERPROFILE");
#else               /*other unix - try sysconf*/
	homedir = getenv("HOME");
#endif  /* WINDOWS */

	if(homedir)
	{
		systemData.homedir = string(homedir);
	}
	else
	{
		cerr << "Can't get homedir path from environment variables";
	}

#ifdef WIN32 /* WINDOWS */
  char pathCWD[MAXPATHLEN];
  getcwd(pathCWD, MAXPATHLEN);
  systemData.sharedDir = std::string(pathCWD) + "\\";
#else               /*other unix - try sysconf*/
	systemData.sharedDir = string(SHARED_DIR) + "/";
#endif  /* WINDOWS */

	//logfile
#ifdef WIN32 /* WINDOWS */
	systemData.logfileName = "log.txt";
#else
	systemData.logfileName = systemData.homedir + "/.mandelbulber_log.txt";
#endif
	FILE *logfile = fopen(systemData.logfileName.c_str(), "w");
	fclose(logfile);

	printf("Log file: %s\n", systemData.logfileName.c_str());
	WriteLogString("Mandelbulber version", string(MANDELBULBER_VERSION_STRING));
	WriteLogString("Mandelbulber compilation date", string(__DATE__) + " " + string(__TIME__));

	//detecting number of CPU cores
	systemData.numberOfThreads = get_cpu_count();
	//NR_THREADS = 1;

	printf("Detected %d CPUs\n", systemData.numberOfThreads);
	WriteLogDouble("CPUs detected", systemData.numberOfThreads);

#ifdef ONETHREAD //for debbuging
	NR_THREADS = 1;
#endif

	//data directory location
#ifdef WIN32 /* WINDOWS */
	systemData.dataDirectory = systemData.homedir + "/mandelbulber";
#else
	systemData.dataDirectory = systemData.homedir + "/.mandelbulber";
#endif
	printf("Default data directory: %s\n", systemData.dataDirectory.c_str());
	WriteLogString("Default data directory", systemData.dataDirectory);

	return true;
}

int get_cpu_count()
{
	int ret;

#ifdef WIN32 /* WINDOWS */
	SYSTEM_INFO info;

	GetSystemInfo(&info);
	ret = info.dwNumberOfProcessors;
#elif defined(__sgi)
	ret = (int) sysconf(_SC_NPROC_ONLN);
#else               /*other unix - try sysconf*/
	ret = (int) sysconf(_SC_NPROCESSORS_ONLN);
#endif  /* WINDOWS */
	return ret;
}

void WriteLog(string text)
{
	FILE *logfile = fopen(systemData.logfileName.c_str(), "a");
	fprintf(logfile, "%ld: %s\n", (unsigned long int) clock(), text.c_str());
	fclose(logfile);
}

void WriteLogDouble(string text, double value)
{
	FILE *logfile = fopen(systemData.logfileName.c_str(), "a");
	fprintf(logfile, "%ld: %s, value = %g\n", (unsigned long int) clock(), text.c_str(), value);
	fclose(logfile);
}

void WriteLogString(string text, string value)
{
	FILE *logfile = fopen(systemData.logfileName.c_str(), "a");
	fprintf(logfile, "%ld: %s, value = %s\n", (unsigned long int) clock(), text.c_str(), value.c_str());
	fclose(logfile);
}

bool CreateDefaultFolders(void)
{
	//create data directory if not exists
		bool result = true;

		result &= CreateDirectory(systemData.dataDirectory);
		result &= CreateDirectory(systemData.dataDirectory + "/images");
		result &= CreateDirectory(systemData.dataDirectory + "/keyframes");
		result &= CreateDirectory(systemData.dataDirectory + "/paths");
		result &= CreateDirectory(systemData.dataDirectory + "/undo");
		result &= CreateDirectory(systemData.dataDirectory + "/paths");

#ifdef CLSUPPORT
		string oclDir = systemData.dataDirectory + "/custom_ocl_formulas";
		DIR *dir;
		dir = opendir(oclDir.c_str());
		if (dir != NULL) (void) closedir(dir);
		else
		{
			result &= CreateDirectory(oclDir);
		}
#endif

	#ifdef CLSUPPORT
		//create and copy of example custom formulas

		dir = opendir(oclDir.c_str());
		if (dir != NULL) (void) closedir(dir);
	#ifdef WIN32
		else
		{
			mkdir(oclDir.c_str());
	#else
		else
		{
			mkdir(oclDir.c_str(), (S_IRUSR | S_IWUSR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH));
	#endif
			fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example1.c", oclDir + "/cl_example1.c");
			fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example2.c", oclDir + "/cl_example2.c");
			fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example3.c", oclDir + +"/cl_example3.c");
			fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example1Init.c", oclDir + "/cl_example1Init.c");
			fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example2Init.c", oclDir + "/cl_example2Init.c");
			fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example3Init.c", oclDir + "/cl_example3Init.c");
		}
	#endif

		actualFileNames.actualFilenameSettings = "settings/default.fract";
		actualFileNames.actualFilenameImage = "images/image.jpg";
		actualFileNames.actualFilenamePalette = systemData.sharedDir +"textures/colour palette.jpg";

		return result;
}

bool CreateDirectory(string name)
{
	DIR *dir;
	dir = opendir(name.c_str());
	if (dir != NULL) {
		(void) closedir(dir);
		WriteLogString("Directory already exists", name);
		return true;
	}
	else
	{
		bool result;
		errno = 0;
#ifdef WIN32
		result = mkdir(name);
#else
		result = mkdir(name.c_str(), (S_IRUSR | S_IWUSR | S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH));
#endif
		if(result)
		{
			int error = errno;
			cerr << "system error no. " << errno << " occured during creating directory: " << name << endl << "error: " << strerror( error ) << endl;
			WriteLogString("Directory can't be created", name);
			WriteLogDouble("mkdir", error);
			return false;
		}
		else
		{
			WriteLogString("Directory created", name);
			return true;
		}
	}
}

int fcopy(string source, string dest)
{
	// ------ file reading

	FILE * pFile;
	unsigned long lSize;
	char *buffer;
	size_t result;

	pFile = fopen(source.c_str(), "rb");
	if (pFile == NULL)
	{
		cerr << "Can't open source file for copying: " << source << endl;
		WriteLogString("Can't open source file for copying", source);
		return 1;
	}

	// obtain file size:
	fseek(pFile, 0, SEEK_END);
	lSize = ftell(pFile);
	rewind(pFile);

	// allocate memory to contain the whole file:
	buffer = new char[lSize];

	// copy the file into the buffer:
	result = fread(buffer, 1, lSize, pFile);
	if (result != lSize)
	{
		cerr << "Can't read source file for copying: " << source << endl;
		WriteLogString("Can't read source file for copying", source);
		delete[] buffer;
		fclose(pFile);
		return 2;
	}
	fclose(pFile);

	// ----- file writing

	pFile = fopen(dest.c_str(), "wb");
	if (pFile == NULL)
	{
		cerr << "Can't open destination file for copying: " << dest << endl;
		WriteLogString("Can't open destination file for copying", dest);
		delete[] buffer;
		return 3;
	}
	fwrite(buffer, 1, lSize, pFile);
	fclose(pFile);

	delete [] buffer;
	WriteLogString("File copied", dest);
	return 0;
}
