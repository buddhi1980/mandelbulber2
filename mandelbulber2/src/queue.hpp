/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen (jenzebas@gmail.com)
 *
 * cQueue - class to manage rendering queue
 */

#ifndef MANDELBULBER2_SRC_QUEUE_HPP_
#define MANDELBULBER2_SRC_QUEUE_HPP_

#include <QtCore>

// forward declarations
class cImage;
class cParameterContainer;
class cFractalContainer;
class cAnimationFrames;
class cKeyframes;
class cInterface;
class RenderedImage;

namespace Ui
{
class cDockQueue;
}

class cQueue : public QObject
{
	Q_OBJECT
public:
	enum enumRenderType
	{
		queue_STILL,
		queue_FLIGHT,
		queue_KEYFRAME
	};

	struct structQueueItem
	{
		structQueueItem(QString _filename, enumRenderType _renderType)
				: filename(_filename), renderType(_renderType)
		{
		}
		bool operator==(const structQueueItem &other) const
		{
			return (filename == other.filename && renderType == other.renderType);
		}
		QString filename;
		enumRenderType renderType;
	};

	// initializes queue and create necessary files and folders
	cQueue(cInterface *_interface, const QString &_queueListFileName, const QString &_queueFolder,
		QObject *parent = nullptr);
	~cQueue();

	// add settings to queue
	void Append(const QString &filename, enumRenderType renderType = queue_STILL);
	void Append(enumRenderType renderType = queue_STILL);
	void Append(cParameterContainer *par, cFractalContainer *fractPar, cAnimationFrames *frames,
		cKeyframes *keyframes, enumRenderType renderType = queue_STILL);
	void AppendList(const QString &filename);
	void AppendFolder(const QString &filename);

	// get next queue element into given containers
	bool Get();
	bool Get(cParameterContainer *par, cFractalContainer *fractPar, cAnimationFrames *frames,
		cKeyframes *keyframes);

	// syncing methods
	// find and delete files which are not on the list
	QStringList RemoveOrphanedFiles();
	// add orphaned files from queue folder to the end of the list
	QStringList AddOrphanedFilesToList();

	// returns list of fractals to render from queue file
	QList<structQueueItem> GetListFromQueueFile() const { return queueListFromFile; }
	// returns list of fractals to render from file system
	QStringList GetListFromFileSystem() const { return queueListFileSystem; }
	QString GetQueueFolder() const { return queueFolder; }

	// get the queue type enum from qstring value
	static enumRenderType GetTypeEnum(const QString &queueText);
	// get the queue type QString from enum value
	static QString GetTypeText(enumRenderType queueType);
	static QString GetTypeName(enumRenderType queueType);
	// get a color for enum value
	static QString GetTypeColor(enumRenderType queueType);

	// list manipulation methods
	// swap queueItem at i with queueItem at j
	void SwapQueueItem(int i, int j);
	// remove queue item which is i'th element of list
	void RemoveQueueItem(int i);
	// remove queue item from list and filesystem
	void RemoveQueueItem(const structQueueItem queueItem);
	void UpdateQueueItemType(int i, enumRenderType renderType);
	// gives next filename
	structQueueItem GetNextFromList();
	// remove queue item if it is on the list
	void RemoveFromList(const structQueueItem &queueItem);
	int GetQueueSize();

	bool stopRequest;

signals:
	// request to update table of queue items
	void queueChanged();
	void queueChanged(int i);
	void queueChanged(int i, int j);

public slots:
	void slotQueueRender() const;

private slots:
	void queueFileChanged(const QString &path);
	void queueFolderChanged(const QString &path);
	void RenderQueue() const;

	// UI
	void slotQueueAddCurrentSettings();
	void slotQueueAddFromFile();
	void slotQueueAddOrphaned();
	void slotQueueRemoveOrphaned();
	void slotQueueRemoveItem();
	void slotQueueMoveItemUp();
	void slotQueueMoveItemDown();
	void slotQueueTypeChanged(int index);
	void slotQueueListUpdate();
	void slotQueueListUpdate(int i);
	void slotQueueListUpdate(int i, int j);
	void slotStopRequest();
	void slotShowQueueThumbsChanges(int state);

private:
	// add filename to the end of list
	void AddToList(const structQueueItem &queueItem);
	// remove queue file from filesystem
	void RemoveFromFileSystem(const QString &filename) const;
	// store queueListFromFile to filesystem
	void StoreList();
	// checks if file exists and it is a proper fractal file
	static bool ValidateEntry(const QString &filename);
	// updates the list of fractals to render from queue file
	void UpdateListFromQueueFile();
	// updates the list of fractals to render from file system
	void UpdateListFromFileSystem();

	cInterface *mainInterface;
	Ui::cDockQueue *ui;
	RenderedImage *renderedImageWidget;

	cImage *image;

	QFileSystemWatcher queueFileWatcher;
	QFileSystemWatcher queueFolderWatcher;

	QList<structQueueItem> queueListFromFile;
	QStringList queueListFileSystem;

	QString queueListFileName;
	QString queueFolder;

	QMutex mutex;
};

extern cQueue *gQueue;

#endif /* MANDELBULBER2_SRC_QUEUE_HPP_ */
