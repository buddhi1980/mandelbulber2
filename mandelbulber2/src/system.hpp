/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#define MANDELBULBER_VERSION 2.23
#define MANDELBULBER_VERSION_STRING "2.23"

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

#include <cstdio>
#include <cstdlib>
#include <utility>

#include <QApplication>
#include <QElapsedTimer>
#include <QFileInfo>
#include <QThread>

// compatibility with qt version < 5.5
#ifndef qInfo
#define qInfo qDebug
#endif

using namespace std;

bool InitSystem();
void handle_winch(int sig);
int get_cpu_count();
bool CreateDefaultFolders();
void ClearNetRenderCache();
void DeleteOldChache(const QString &directoryPath, int days);
bool CreateFolder(const QString &name);
void DeleteAllFilesFromDirectory(const QString &folder, const QString &filterExpression,
	QRegExp::PatternSyntax pattern = QRegExp::Wildcard);
void myMessageOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg);
void UpdateDefaultPaths();
void UpdateUIStyle();
void UpdateUISkin();
void UpdateLanguage();
void RetrieveToolbarPresets(bool force);
void RetrieveExampleMaterials(bool force);
void CalcPreferredFontSize(bool noGui);
bool IsOutputTty();
bool PutClangFormatFileToDataDirectoryHidden();

#endif /* MANDELBULBER2_SRC_SYSTEM_HPP_ */
