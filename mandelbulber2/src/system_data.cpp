/*
 * system_data.cpp
 *
 *  Created on: 13 kwi 2020
 *      Author: krzysztof
 */

#include <QDir>
#include <QDebug>

#include "files.h"
#include "system_data.hpp"
#include "system.hpp"
#include "system_directories.hpp"
#include "write_log.hpp"

QString sSystem::GetIniFile() const
{
	double version = MANDELBULBER_VERSION;
	int versionInt = int(version * 100);

	QString iniFileName = QString("mandelbulber_%1.ini").arg(versionInt);
	QString fullIniFileName = systemDirectories.dataDirectoryHidden + iniFileName;

	// if setting file doesn't exist then look for older files
	if (!QFile::exists(fullIniFileName))
	{
		QString tempFileName;
		for (int ver = versionInt; ver >= 212; ver--)
		{
			if (ver == 212)
			{
				tempFileName = QString("mandelbulber.ini");
			}
			else
			{
				tempFileName = QString("mandelbulber_%1.ini").arg(ver);
			}
			if (QFile::exists(systemDirectories.dataDirectoryHidden + tempFileName))
			{
				fcopy(systemDirectories.dataDirectoryHidden + tempFileName, fullIniFileName);
				WriteLogString(
					"Found older settings file", systemDirectories.dataDirectoryHidden + tempFileName, 1);
				break;
			}
		}
	}
	return fullIniFileName;
}

void sSystem::Upgrade() const
{
	QStringList moveFolders = {systemDirectories.GetSettingsFolder(),
		systemDirectories.GetImagesFolder(), systemDirectories.GetSlicesFolder(),
		systemDirectories.GetMaterialsFolder(), systemDirectories.GetAnimationFolder()};
	for (int i = 0; i < moveFolders.size(); i++)
	{
		QString folderSource = moveFolders.at(i);
		QString folderTarget = folderSource;
		folderTarget.replace(
			systemDirectories.dataDirectoryHidden, systemDirectories.dataDirectoryPublic);
		if (QFileInfo::exists(folderTarget))
		{
			qCritical() << QString("target folder %1 already exists, won't move!").arg(folderTarget);
		}
		else if (!QDir().rename(folderSource, folderTarget))
		{
			qCritical() << QString("cannot move folder %1 to %2!").arg(folderSource, folderTarget);
		}
	}
}

QString sSystem::GetImageFileNameSuggestion()
{
	QString imageBaseName = QFileInfo(lastImageFile).completeBaseName();

	// if the last image file has been saved manually, this is the suggestion for the filename
	if (!lastImageFile.endsWith("image.jpg")) return imageBaseName;

	// otherwise if the settings has been loaded from a proper .fract file, this fileName's basename
	// is the suggestion
	if (lastSettingsFile.endsWith(".fract")) return QFileInfo(lastSettingsFile).completeBaseName();

	// maybe loaded by clipboard, no better suggestion, than the default lastImageFile's baseName
	return imageBaseName;
}

bool sSystem::IsUpgraded() const
{
	return QFileInfo::exists(systemDirectories.dataDirectoryPublic + "settings");
}

QThread::Priority sSystem::GetQThreadPriority(enumRenderingThreadPriority priority)
{
	switch (priority)
	{
		case renderingPriorityLowest: return QThread::LowestPriority;
		case renderingPriorityLow: return QThread::LowPriority;
		case renderingPriorityNormal: return QThread::NormalPriority;
		case renderingPriorityHigh: return QThread::HighPriority;
		default: return QThread::NormalPriority;
	}
}
