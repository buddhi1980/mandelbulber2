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

#include "system_data.hpp"

#include <QDebug>
#include <QDir>

#include "files.h"
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
