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
			parSettings.SaveToFile(queueFolder + "/" + filenameQueue);
			AddToList(filenameQueue, renderType);
			emit queueChanged();
		}
	}
}

void cQueue::Append(enumRenderType renderType)
{
	//add current settings to queue
	cSettings parSettings(cSettings::formatCondensedText);
	parSettings.CreateText(gPar, gParFractal, gAnimFrames, gKeyframes);
	QString filename = "queue_" + parSettings.GetHashCode() + ".fract";
	parSettings.SaveToFile(queueFolder + "/" + filename);
	AddToList(filename, renderType);
	emit queueChanged();
}

bool cQueue::Get(cParameterContainer *par, cFractalContainer *fractPar, cAnimationFrames *frames, cKeyframes *keyframes)
{
	//get next fractal from queue
	structQueueItem queueItem = GetNextFromList();
	if(queueItem.filename == "") return false; // reached end of list
	cSettings parSettings(cSettings::formatCondensedText);

	if (parSettings.LoadFromFile(queueItem.filename))
	{
		parSettings.Decode(par, fractPar, frames, keyframes);
	}
	return false;
}

QList<cQueue::structQueueItem> cQueue::GetListFromQueueFile()
{
	//returns list of fractals to render
	QList<structQueueItem> queueList;
	QFile file(queueListFileName);
	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream in(&file);
		while (!in.atEnd())
		{
			QString line = in.readLine().trimmed();
			if(line.startsWith("#") || line == "") continue;
			QRegularExpression reType("^(.*?\.fract) (still|keyframe|flight)?$");
			QRegularExpressionMatch matchType = reType.match(line);
			if (matchType.hasMatch())
			{
				enumRenderType renderType;
				if(matchType.captured(2) == "still") renderType = queue_STILL;
				else if(matchType.captured(2) == "keyframe") renderType = queue_KEYFRAME;
				else if(matchType.captured(2) == "flight") renderType = queue_FLIGHT;
				else renderType = queue_STILL;
				queueList << structQueueItem(renderType, matchType.captured(1));
			}
			else qWarning() << "wrong format in line: " << line;
		}
		file.close();
	}
	return queueList;
}

QStringList cQueue::GetListFromFileSystem()
{
	return QDir(queueFolder).entryList(QDir::NoDotAndDotDot | QDir::Files);
}

QStringList cQueue::DeleteOrphanedFiles()
{
	// find and delete files which are not on the list
	QList<structQueueItem> queueListFromQueueFile = GetListFromQueueFile();
	QStringList queueListFromFileSystem = GetListFromFileSystem();
	QStringList removeList;
	for(int i = 0; i < queueListFromFileSystem.size(); i++)
	{
		bool inList = false;
		for(int j = 0; j < queueListFromQueueFile.size(); j++)
		{
			if(queueListFromQueueFile.at(j).filename == queueListFromFileSystem.at(i)){
				inList = true;
				break;
			}
		}
		if(!inList)
		{
			removeList << queueListFromFileSystem[i];
			QFile::remove(queueListFromFileSystem[i]);
		}
	}
	return removeList;
}

QStringList cQueue::AddOrphanedFilesToList()
{
	// add orphaned files from queue folder to the end of the list
	QList<structQueueItem> queueListFromQueueFile = GetListFromQueueFile();
	QStringList queueListFromFileSystem = QDir(queueFolder).entryList(QDir::NoDotAndDotDot | QDir::Files);
	QStringList appendList;
	for(int i = 0; i < queueListFromFileSystem.size(); i++)
	{
		bool inList = false;
		for(int j = 0; j < queueListFromQueueFile.size(); j++)
		{
			if(queueListFromQueueFile.at(j).filename == queueListFromFileSystem.at(i)){
				inList = true;
				break;
			}
		}
		if(!inList)
		{
			appendList << queueListFromFileSystem[i] + " still";
		}
	}
	if(appendList.size() > 0)
	{
		QFile file(queueListFileName);
		if (file.open(QIODevice::WriteOnly | QIODevice::Append))
		{
			QTextStream stream(&file);
			stream << "\n" << appendList.join("\n");
			file.close();
		}
		else
			throw QString("cannot open queueListFileName: " + queueListFileName);
	}
	return appendList;
}

void cQueue::SaveToQueueFolder(const QString &filename, const cParameterContainer &par, const cFractalContainer &fract)
{
	//TODO
}

cQueue::structQueueItem cQueue::GetNextFromList()
{
	//gives next filename
	QFile file(queueListFileName);
	if (file.open(QIODevice::ReadOnly))
	{
		QTextStream in(&file);
		while (!in.atEnd())
		{
			QString line = in.readLine().trimmed();
			if(line.startsWith("#") || line == "") continue;
			QRegularExpression reType("^(.*?\.fract) (still|keyframe|flight)?$");
			QRegularExpressionMatch matchType = reType.match(line);
			if (matchType.hasMatch())
			{
				enumRenderType renderType;
				if(matchType.captured(2) == "still") renderType = queue_STILL;
				else if(matchType.captured(2) == "keyframe") renderType = queue_KEYFRAME;
				else if(matchType.captured(2) == "flight") renderType = queue_FLIGHT;
				else renderType = queue_STILL;
				return structQueueItem(renderType, matchType.captured(1));
			}
			else
				qWarning() << "wrong format in line: " << line;
		}
		file.close();
	}
	return structQueueItem(queue_STILL, "");
}

void cQueue::EraseFirstLineFromList()
{
	//erases first line from list when fractal is taken
	//TODO
}

void cQueue::AddToList(const QString &filename, enumRenderType renderType)
{
	//add filename to the end of list
	QFile file(queueListFileName);
	if (file.open(QIODevice::WriteOnly | QIODevice::Append))
	{
		QTextStream stream(&file);
		QString renderTypeString = "";
		switch(renderType){
			case queue_STILL: renderTypeString = "still"; break;
			case queue_KEYFRAME: renderTypeString = "keyframe"; break;
			case queue_FLIGHT: renderTypeString = "flight"; break;
			default: renderTypeString = "still"; break;
		}
		stream << "\n" << filename << " " << renderTypeString;
		file.close();
	}
	else
		throw QString("cannot open queueListFileName: " + queueListFileName);
}

bool cQueue::ValidateEntry(const QString &filename)
{
	//checks if
	//TODO
}

void cQueue::DeleteFileFromQueue(const QString &filename)
{
	//delete file from queue folder
	//TODO
}

QString cQueue::GetTypeText(enumRenderType displayStatus)
{
	switch(displayStatus)
	{
		case cQueue::queue_STILL: return tr("STILL");
		case cQueue::queue_FLIGHT: return tr("FLIGHT");
		case cQueue::queue_KEYFRAME: return tr("KEYFRAME");
	}
	return tr("UNKNOWN");
}

QString cQueue::GetTypeColor(enumRenderType displayStatus)
{
	switch(displayStatus)
	{
		case cQueue::queue_STILL: return "darkgrey";
		case cQueue::queue_FLIGHT: return "green";
		case cQueue::queue_KEYFRAME: return "darkblue";
	}
	return "red";
}
