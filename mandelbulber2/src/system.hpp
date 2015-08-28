/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * system data - place for system functions and definition of default file paths
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */
#ifndef SYSTEM_HPP_
#define SYSTEM_HPP_

#define MANDELBULBER_VERSION 2.05
#define MANDELBULBER_VERSION_STRING "2.05"
#define TO_STRING(a)	#a

#ifdef WIN32 /* WINDOWS */
#include <windows.h>
#define HAVE_BOOLEAN  /* prevent jmorecfg.h from redefining it */
#endif

#ifndef SHARED_DIR
	#ifndef WIN32
		#define SHARED_DIR "/usr/share/mandelbulber2"
	#endif
#endif

#include <QtCore>
#include <stdio.h>
#include <stdlib.h>
#include <qapplication.h>

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
	QString thumbnailDir;
	QString autosaveFile;
	QHash<QString, QString> supportedLanguages;
	int numberOfThreads;
	bool noGui;
	QChar decimalPoint;
	QLocale locale;
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
void DeleteAllFilesFromDirectory(QString folder, QString filterExpression);
int fcopy(QString source, QString dest);
void Wait(long int time);
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
void UpdateDefaultPaths();
void UpdateUIStyle();
void UpdateUISkin();
void UpdateLanguage();
QString ReadCLI();
void ProcessCLI(QString cliTODO);

#endif /* SYSTEM_HPP_ */
