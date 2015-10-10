/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cQueue - class to manage rendering queue
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen
 */

#include "queue.hpp"
#include "system.hpp"
#include "settings.hpp"
#include "global_data.hpp"

cQueue::cQueue(const QString &_queueListFileName, const QString &_queueFolder)
{
	//initializes queue and create necessary files and folders
	queueListFileName = _queueListFileName;
	queueFolder = _queueFolder;

	if(!QFile::exists(queueFolder) && !CreateDirectory(queueFolder)){
		throw QString("cannot init queueListFileName folder to: " + queueFolder);
	}

	if(!QFile::exists(queueListFileName)){
		QFile file(queueListFileName);
		if (file.open(QIODevice::WriteOnly)) {
			QTextStream stream(&file);
			stream << "#\n# Mandelbulber queue file\n#\n";
			file.close();
		}
		else
			throw QString("cannot init queueListFileName to: " + queueListFileName);
	}

	// watch queue folder and the queue file in the filesystem
	queueFileWatcher.addPath(queueListFileName);
	queueFolderWatcher.addPath(queueFolder);
	QApplication::connect(&queueFileWatcher, SIGNAL(fileChanged(const QString&)), this, SLOT(queueFileChanged(const QString&)));
	QApplication::connect(&queueFolderWatcher, SIGNAL(directoryChanged(const QString&)), this, SLOT(queueFolderChanged(const QString&)));

	UpdateListFromQueueFile();
	UpdateListFromFileSystem();
}

cQueue::~cQueue()
{
	// nothing to be done
}

void cQueue::Append(const QString &filename, enumRenderType renderType)
{
	//add new fractal to queue
	if (QFileInfo(filename).suffix() == QString("fract"))
	{
		cSettings parSettings(cSettings::formatFullText);
		parSettings.BeQuiet(true);
		if (parSettings.LoadFromFile(filename))
		{
			QString filenameQueue = "queue_" + parSettings.GetHashCode() + ".fract";
			QString completeFileName = queueFolder + QDir::separator() + filenameQueue;
			parSettings.SaveToFile(completeFileName);
			AddToList(structQueueItem(completeFileName, renderType));
		}
	}
}

void cQueue::Append(enumRenderType renderType)
{
	//add current settings to queue
	Append(gPar, gParFractal, gAnimFrames, gKeyframes, renderType);
}

void cQueue::Append(cParameterContainer *par, cFractalContainer *fractPar, cAnimationFrames *frames, cKeyframes *keyframes, enumRenderType renderType)
{
	//add settings to queue
	cSettings parSettings(cSettings::formatCondensedText);
	parSettings.CreateText(par, fractPar, frames, keyframes);
	QString filename = "queue_" + parSettings.GetHashCode() + ".fract";
	QString completeFileName = queueFolder + QDir::separator() + filename;
	parSettings.SaveToFile(completeFileName);
	AddToList(structQueueItem(completeFileName, renderType));
}

bool cQueue::Get()
{
	//get next fractal from queue into global scope containers
	return Get(gPar, gParFractal, gAnimFrames, gKeyframes);
}

bool cQueue::Get(cParameterContainer *par, cFractalContainer *fractPar, cAnimationFrames *frames, cKeyframes *keyframes)
{
	//get next fractal from queue
	structQueueItem queueItem = GetNextFromList();
	if(queueItem.filename == "") return false; // reached end of list
	cSettings parSettings(cSettings::formatCondensedText);

	if (parSettings.LoadFromFile(queueItem.filename))
	{
		return parSettings.Decode(par, fractPar, frames, keyframes);
	}
	return false;
}

QStringList cQueue::RemoveOrphanedFiles()
{
	qDebug() << "remove orphaned";
	// find and delete files which are not on the list
	QStringList removeList;
	for(int i = 0; i < queueListFileSystem.size(); i++)
	{
		structQueueItem queueItem = structQueueItem(queueListFileSystem.at(i), queue_STILL);
		if(!queueListFromFile.contains(queueItem))
		{
			removeList << queueListFileSystem[i];
			RemoveFromFileSystem(queueListFileSystem[i]);
		}
	}
	return removeList;
}

QStringList cQueue::AddOrphanedFilesToList()
{
	// add orphaned files from queue folder to the end of the list
	qDebug() << "add orphaned";
	QStringList appendList;
	for(int i = 0; i < queueListFileSystem.size(); i++)
	{
		structQueueItem queueItem = structQueueItem(queueListFileSystem.at(i), queue_STILL);
		if(!queueListFromFile.contains(queueItem))
		{
			appendList << queueListFileSystem.at(i);
			queueListFromFile << queueItem;
		}
	}
	if(appendList.size() > 0)
	{
		StoreList();
	}
	return appendList;
}

cQueue::structQueueItem cQueue::GetNextFromList()
{
	//gives next filename
	if(queueListFromFile.size() > 0)
	{
		RemoveQueueItem(queueListFromFile.at(0));
		return queueListFromFile.at(0);
	}
	return structQueueItem("", queue_STILL);
}

void cQueue::AddToList(const structQueueItem &queueItem)
{
	//add filename to the end of list
	if(queueListFromFile.contains(queueItem))
	{
		qWarning() << "Entry already exists";
		return;
	}
	queueListFromFile << queueItem;
	StoreList();
}

void cQueue::RemoveQueueItem(int i)
{
	RemoveQueueItem(queueListFromFile.at(i));
}

void cQueue::RemoveQueueItem(const structQueueItem queueItem)
{
	//remove queue item from list and filesystem
	RemoveFromList(queueItem);
	// check if fract file is still on the list (one file with different renderTypes)
	for(int i = 0; i < queueListFromFile.size(); i++)
	{
		if(queueListFromFile.at(i).filename == queueItem.filename)
		{
			return;
		}
	}
	RemoveFromFileSystem(queueItem.filename);
}

void cQueue::UpdateQueueItemType(int i, enumRenderType renderType)
{
	queueListFromFile[i].renderType = renderType;
	StoreList();
}

bool cQueue::ValidateEntry(const QString &filename)
{
	//checks if
	//TODO
}

void cQueue::RemoveFromList(const structQueueItem &queueItem)
{
	//remove queue item if it is on the list
	queueListFromFile.removeAll(queueItem);
	StoreList();
}

void cQueue::StoreList()
{
	QFile file(queueListFileName);
	if(file.open(QIODevice::ReadWrite | QIODevice::Text | QIODevice::Truncate))
	{
		QTextStream stream(&file);
		stream << "#\n# Mandelbulber queue file\n#\n";
		for(int i = 0; i < queueListFromFile.size(); i++)
		{
			stream << queueListFromFile.at(i).filename << " " << GetTypeText(queueListFromFile.at(i).renderType) << endl;
		}
	}
	file.close();
}

void cQueue::RemoveFromFileSystem(const QString &filename)
{
	//remove queue file from filesystem

	//TODO only remove files from queue folder (there could be some other files on the list which are located in another folders)
	QFile::remove(filename);
}

cQueue::enumRenderType cQueue::GetTypeEnum(const QString &queueText)
{
	enumRenderType renderType;
	if(queueText == "STILL") renderType = queue_STILL;
	else if(queueText == "KEYFRAME") renderType = queue_KEYFRAME;
	else if(queueText == "FLIGHT") renderType = queue_FLIGHT;
	else renderType = queue_STILL;
	return renderType;
}

QString cQueue::GetTypeText(enumRenderType queueType)
{
	switch(queueType)
	{
		case cQueue::queue_STILL: return tr("STILL");
		case cQueue::queue_FLIGHT: return tr("FLIGHT");
		case cQueue::queue_KEYFRAME: return tr("KEYFRAME");
	}
	return tr("STILL");
}

QString cQueue::GetTypeColor(enumRenderType queueType)
{
	switch(queueType)
	{
		case cQueue::queue_STILL: return "darkgrey";
		case cQueue::queue_FLIGHT: return "green";
		case cQueue::queue_KEYFRAME: return "darkblue";
	}
	return "red";
}

void cQueue::queueFileChanged(const QString &path)
{
	qDebug() << "queueFileChanged";
	if(path == queueListFileName)
	{
		UpdateListFromQueueFile();
	}
}

void cQueue::queueFolderChanged(const QString &path)
{
	qDebug() << "queueFolderChanged";
	UpdateListFromFileSystem();
}

void cQueue::UpdateListFromQueueFile()
{
	queueListFromFile.clear();
	//returns list of fractals to render
	QFile file(queueListFileName);
	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream in(&file);
		while (!in.atEnd())
		{
			QString line = in.readLine().trimmed();
			if(line.startsWith("#") || line == "") continue;
			QRegularExpression reType("^(.*?\.fract) (STILL|KEYFRAME|FLIGHT)?$");
			QRegularExpressionMatch matchType = reType.match(line);
			if (matchType.hasMatch())
			{
				queueListFromFile << structQueueItem(matchType.captured(1), GetTypeEnum(matchType.captured(2)));
			}
			else qWarning() << "wrong format in line: " << line;
		}
		file.close();
	}
	emit queueChanged();
}

void cQueue::UpdateListFromFileSystem()
{
	queueListFileSystem = QDir(queueFolder).entryList(QDir::NoDotAndDotDot | QDir::Files);
}
