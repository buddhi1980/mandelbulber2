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


#ifndef MANDELBULBER2_SRC_QUEUE_HPP_
#define MANDELBULBER2_SRC_QUEUE_HPP_

#include "parameters.hpp"
#include "fractal_container.hpp"
#include <QtCore>

class cQueue
{
public:
	enum enumRenderType {
		still, flight, keyframe
	};

	struct structQueueItem {
		structQueueItem(
			enumRenderType _renderType,
			QString _filename):
			renderType(_renderType), filename(_filename) {}

		enumRenderType renderType;
		QString filename;
	};

	cQueue(const QString &_queueListFileName, const QString &_queueFolder); //initializes queue and create necessary files and folders
	~cQueue();

	void Append(const QString &filename, enumRenderType renderType = still); //add new fractal to queue
	// void Append(const cParameterContainer &par, const cFractalContainer &fract); //add new fractal to queue
	void Append(enumRenderType renderType = still); //add current settings to queue
	bool Get(const cParameterContainer &par, const cFractalContainer &fract); //get next fractal from queue

	QList<structQueueItem> GetListFromQueueFile(); //returns list of fractals to render from queue file
	QStringList GetListFromFileSystem(); //returns list of fractals to render from file system

	QStringList DeleteOrphanedFiles(); //find and delete files which are not on the list
	QStringList AddOrphanedFilesToList(); //add orphaned files from queue folder to the end of the list

private:
	void SaveToQueueFolder(const QString &filename, const cParameterContainer &par, const cFractalContainer &fract);

	structQueueItem GetNextFromList(); //gives next filename
	void EraseFirstLineFromList(); //erases first line from list when fractal is taken
	void AddToList(const QString &filename, enumRenderType renderType = still); //add filename to the end of list

	bool ValidateEntry(const QString &filename); //checks if

	void DeleteFileFromQueue(const QString &filename); //delete file from queue folder

	QString queueListFileName;
	QString queueFolder;
};

#endif /* MANDELBULBER2_SRC_QUEUE_HPP_ */
