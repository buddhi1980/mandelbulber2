#include <utility>

#include <utility>

/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * system data - place for system functions and definition of default file paths
 */

#ifndef MANDELBULBER2_SRC_SYSTEM_HPP_
#define MANDELBULBER2_SRC_SYSTEM_HPP_

#define MANDELBULBER_VERSION 2.20
#define MANDELBULBER_VERSION_STRING "2.20-dev"

#define DO_QUOTE(X) #X
#define QUOTE(X) DO_QUOTE(X)

#ifndef SHARED_DIR
#ifdef CUSTOM_PREFIX
#define SHARED_DIR QUOTE(CUSTOM_PREFIX) "/share/mandelbulber2"
#endif
#endif

#ifndef SHARED_DOC_DIR
#ifdef CUSTOM_PREFIX
#define SHARED_DOC_DIR QUOTE(CUSTOM_PREFIX) "/share/doc/mandelbulber2"
#endif
#endif

#ifdef _WIN32 /* WINDOWS */
// #include <windows.h>
#define HAVE_BOOLEAN /* prevent jmorecfg.h from redefining it */
#endif

#ifndef SHARED_DIR
#ifdef _WIN32
#elif __APPLE__
#define SHARED_DIR QDir::homePath() + "/share/mandelbulber2"
#endif
#endif

#ifndef SHARED_DOC_DIR
#ifdef _WIN32
#elif __APPLE__
#define SHARED_DOC_DIR QDir::homePath() + "/share/doc/mandelbulber2"
#endif
#endif

#include <qapplication.h>

#include <cstdio>
#include <cstdlib>

#include <QtCore>

// compatibility with qt version < 5.5
#ifndef qInfo
#define qInfo qDebug
#endif

enum enumRenderingThreadPriority
{
	renderingPriorityLowest = 0,
	renderingPriorityLow = 1,
	renderingPriorityNormal = 2,
	renderingPriorityHigh = 3
};

using namespace std;

struct sSystem
{
private:
	QString dataDirectoryPublic;
	QString dataDirectoryHidden;

public:
	bool IsUpgraded() const { return QFileInfo::exists(dataDirectoryPublic + "settings"); }
	void Upgrade() const;
	void SetDataDirectoryHidden(QString target) { dataDirectoryHidden = std::move(target); }
	void SetDataDirectoryPublic(QString target) { dataDirectoryPublic = std::move(target); }
	QString GetDataDirectoryPublic() const { return dataDirectoryPublic; }
	QString GetDataDirectoryHidden() const { return dataDirectoryHidden; }
	QString GetDataDirectoryUsed() const
	{
		return IsUpgraded() ? dataDirectoryPublic : dataDirectoryHidden;
	}

	QString GetSettingsFolder() const { return GetDataDirectoryUsed() + "settings"; }
	QString GetImagesFolder() const { return GetDataDirectoryUsed() + "images"; }
	QString GetSlicesFolder() const { return GetDataDirectoryUsed() + "slices"; }
	QString GetMaterialsFolder() const { return GetDataDirectoryUsed() + "materials"; }
	QString GetAnimationFolder() const { return GetDataDirectoryUsed() + "animation"; }
	QString GetGradientsFolder() const { return GetDataDirectoryUsed() + "gradients"; }

	QString GetQueueFolder() const { return dataDirectoryHidden + "queue"; }
	QString GetToolbarFolder() const { return dataDirectoryHidden + "toolbar"; }
	QString GetHttpCacheFolder() const { return dataDirectoryHidden + "httpCache"; }
	QString GetCustomWindowStateFolder() const { return dataDirectoryHidden + "customWindowState"; }
	QString GetQueueFractlistFile() const { return dataDirectoryHidden + "queue.fractlist"; }
	QString GetThumbnailsFolder() const { return dataDirectoryHidden + "thumbnails"; }
	QString GetAutosaveFile() const { return dataDirectoryHidden + ".autosave.fract"; }
	QString GetIniFile() const;
	QString GetRecentFilesListFile() const { return dataDirectoryHidden + "files.recent"; }
	QString GetResolutionPresetsFile() const { return dataDirectoryHidden + "resolutionPresets.ini"; }
	QString GetNetrenderFolder() const { return dataDirectoryHidden + "netrender"; }

	QString GetImageFileNameSuggestion();

	int GetPreferredFontSize() const { return preferredFontSize; }
	void SetPreferredFontSize(int preferredFontSizeInput)
	{
		preferredFontSize = preferredFontSizeInput;
	}
	int GetPreferredThumbnailSize() const { return preferredThumbnailSize; }
	void SetPreferredThumbnailSize(int preferredThumbnailSizeInput)
	{
		preferredThumbnailSize = preferredThumbnailSizeInput;
	}
	int GetPreferredFontPointSize() const { return preferredFontPointSize; }
	void SetPreferredFontPointSize(int preferredFontPointSizeInput)
	{
		preferredFontPointSize = preferredFontPointSizeInput;
	}
	void SetLogfileName(QString logfileNameInput) { logfileName = std::move(logfileNameInput); }

	QString homeDir;
	QString sharedDir;
	QString docDir;
	QString logfileName;
	QString lastSettingsFile;
	QString lastImageFile;
	QString lastImagePaletteFile;
	QString lastGradientFile;
	QHash<QString, QString> supportedLanguages;
	int numberOfThreads;
	bool noGui;
	bool silent;
	QChar decimalPoint;
	QLocale locale;
	int terminalWidth;
	bool statsOnCLI;
	bool useColor;
	int loggingVerbosity;
	bool settingsLoadedFromCLI;
	QElapsedTimer globalTimer;
	bool globalStopRequest;
	enumRenderingThreadPriority threadsPriority;
	int preferredFontSize;
	int preferredFontPointSize;
	int preferredThumbnailSize;
	bool isOutputTty;
};

struct sActualFileNames
{
	QString actualFilenameSettings;
	QString actualFilenameImage;
	QString actualFilenamePalette;
};

extern sSystem systemData;
extern sActualFileNames actualFileNames;

bool InitSystem();
void WriteLog(const QString &text, int verbosityLevel);
void WriteLogCout(const QString &text, int verbosityLevel);
void WriteLogDouble(const QString &text, double value, int verbosityLevel);
void WriteLogInt(const QString &text, int value, int verbosityLevel);
void WriteLogSizeT(const QString &text, size_t value, int verbosityLevel);
void WriteLogString(const QString &text, const QString &value, int verbosityLevel);
void handle_winch(int sig);
int get_cpu_count();
bool CreateDefaultFolders();
bool CreateFolder(const QString &name);
void DeleteAllFilesFromDirectory(const QString &folder, const QString &filterExpression,
	QRegExp::PatternSyntax pattern = QRegExp::Wildcard);
int fcopy(const QString &source, const QString &dest);
void Wait(long int time);
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
void UpdateDefaultPaths();
void UpdateUIStyle();
void UpdateUISkin();
void UpdateLanguage();
void RetrieveToolbarPresets(bool force);
void RetrieveExampleMaterials(bool force);
QThread::Priority GetQThreadPriority(enumRenderingThreadPriority priority);
void CalcPreferredFontSize(bool noGui);
bool IsOutputTty();

#endif /* MANDELBULBER2_SRC_SYSTEM_HPP_ */
