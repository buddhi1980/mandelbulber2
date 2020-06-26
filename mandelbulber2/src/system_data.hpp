/*
 * system_data.hpp
 *
 *  Created on: 13 kwi 2020
 *      Author: krzysztof
 */

#ifndef MANDELBULBER2_SRC_SYSTEM_DATA_HPP_
#define MANDELBULBER2_SRC_SYSTEM_DATA_HPP_

#include <QString>
#include <QLocale>
#include <QElapsedTimer>
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
