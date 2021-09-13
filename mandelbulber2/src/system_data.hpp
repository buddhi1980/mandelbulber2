/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2020 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * TODO: description
 */

#ifndef MANDELBULBER2_SRC_SYSTEM_DATA_HPP_
#define MANDELBULBER2_SRC_SYSTEM_DATA_HPP_

#include <QElapsedTimer>
#include <QLocale>
#include <QString>
#include <QThread>

enum enumRenderingThreadPriority
{
	renderingPriorityLowest = 0,
	renderingPriorityLow = 1,
	renderingPriorityNormal = 2,
	renderingPriorityHigh = 3
};

struct sSystem
{
	bool IsUpgraded() const;
	void Upgrade() const;
	QString GetIniFile() const;

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
	QThread::Priority GetQThreadPriority(enumRenderingThreadPriority priority);

	int getPreferredCustomFormulaFontSize() const { return preferredCustomFormulaFontSize; }
	void setPreferredCustomFormulaFontSize(int _preferredCustomFormulaFontSize)
	{
		this->preferredCustomFormulaFontSize = _preferredCustomFormulaFontSize;
	}

	QString logfileName;
	QString lastSettingsFile;
	QString lastImageFile;
	QString lastImagePaletteFile;
	QString lastGradientFile;
	QHash<QString, QString> supportedLanguages;
	int numberOfThreads;
	bool noGui;
	bool silent;
    QString decimalPoint;
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
	int preferredCustomFormulaFontSize;
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

#endif /* MANDELBULBER2_SRC_SYSTEM_DATA_HPP_ */
