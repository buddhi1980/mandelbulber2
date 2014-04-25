/*
 * system.cpp
 *
 *  Created on: Mar 9, 2014
 *      Author: krzysztof
 */

#include "system.hpp"

#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QtGui>
#include <ctime>
#include <QTextStream>

//#define CLSUPPORT

sSystem systemData;
sActualFileNames actualFileNames;

bool InitSystem(void)
{
	QTextStream out(stdout);
	setlocale(LC_ALL, "");

	systemData.homedir = QDir::homePath();

#ifdef WIN32 /* WINDOWS */
    //systemData.sharedDir = (QDir::currentPath() + QDir::separator()).toStdString();
    systemData.sharedDir = (QDir::currentPath() + QDir::separator());
#else               /*other unix - try sysconf*/
	systemData.sharedDir = QString(SHARED_DIR) + "/";
#endif  /* WINDOWS */

	//logfile
#ifdef WIN32 /* WINDOWS */
	systemData.logfileName = "log.txt";
#else
	systemData.logfileName = systemData.homedir + "/.mandelbulber_log.txt";
#endif
	FILE *logfile = fopen(systemData.logfileName.toUtf8().constData(), "w");
	fclose(logfile);

	out << "Log file name: " << systemData.logfileName << endl;
	WriteLogString("Mandelbulber version", QString(MANDELBULBER_VERSION_STRING));
	WriteLogString("Mandelbulber compilation date", QString(__DATE__) + " " + QString(__TIME__));

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
	out << "Default data directory: " << systemData.dataDirectory << endl;
	WriteLogString("Default data directory", systemData.dataDirectory);

	return true;
}

int get_cpu_count()
{
	return QThread::idealThreadCount ();
}

void WriteLog(QString text)
{
	FILE *logfile = fopen(systemData.logfileName.toUtf8().constData(), "a");
	fprintf(logfile, "%ld: %s\n", (unsigned long int) clock(), text.toUtf8().constData());
	fclose(logfile);
}

void WriteLogDouble(QString text, double value)
{
	FILE *logfile = fopen(systemData.logfileName.toUtf8().constData(), "a");
	fprintf(logfile, "%ld: %s, value = %g\n", (unsigned long int) clock(), text.toUtf8().constData(), value);
	fclose(logfile);
}

void WriteLogString(QString text, QString value)
{
	FILE *logfile = fopen(systemData.logfileName.toUtf8().constData(), "a");
	fprintf(logfile, "%ld: %s, value = %s\n", (unsigned long int) clock(), text.toUtf8().constData(), value.toUtf8().constData());
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
		QString qoclDir = QString::fromStdString(oclDir);
		if(!QDir(qoclDir).exists())
		{
			result &= CreateDirectory(oclDir);
			if(result)
			{
				fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example1.c", oclDir + "/cl_example1.c");
				fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example2.c", oclDir + "/cl_example2.c");
				fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example3.c", oclDir + +"/cl_example3.c");
				fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example1Init.c", oclDir + "/cl_example1Init.c");
				fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example2Init.c", oclDir + "/cl_example2Init.c");
				fcopy(systemData.sharedDir + "/exampleOCLformulas/cl_example3Init.c", oclDir + "/cl_example3Init.c");
			}
		}
#endif

		actualFileNames.actualFilenameSettings = "settings/default.fract";
		actualFileNames.actualFilenameImage = "images/image.jpg";
		actualFileNames.actualFilenamePalette = systemData.sharedDir +"textures/colour palette.jpg";

		return result;
}

bool CreateDirectory(QString qname)
{
	if(QDir(qname).exists())
	{
		WriteLogString("Directory already exists", qname);
		return true;
	}
	else
	{
		if(QDir().mkdir(qname))
		{
			WriteLogString("Directory created", qname);
			return true;
		}
		else
		{
			WriteLogString("Directory can't be created", qname);
			qCritical() << "error: directory " << qname << " cannot be created" << endl;
			return false;
		}
	}
}

int fcopy(QString source, QString dest)
{
	// ------ file reading

	FILE * pFile;
	unsigned long lSize;
	char *buffer;
	size_t result;

	pFile = fopen(source.toUtf8().constData(), "rb");
	if (pFile == NULL)
	{
		qCritical() << "Can't open source file for copying: " << source << endl;
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
		qCritical() << "Can't read source file for copying: " << source << endl;
		WriteLogString("Can't read source file for copying", source);
		delete[] buffer;
		fclose(pFile);
		return 2;
	}
	fclose(pFile);

	// ----- file writing

	pFile = fopen(dest.toUtf8().constData(), "wb");
	if (pFile == NULL)
	{
		qCritical() << "Can't open destination file for copying: " << dest << endl;
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
