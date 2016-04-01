/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cArchiveDownloader class - downloads an archive to a folder and extracts it
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 */

#include "file_downloader.hpp"
#include "global_data.hpp"
#include <QNetworkRequest>

cFileDownloader::cFileDownloader(QString sourceBaseURL, QString targetDir) : QObject()
{
	this->sourceBaseURL = sourceBaseURL;
	this->targetDir = targetDir;
	network = new QNetworkAccessManager();
	done = true;
	currentThumbNailFinished = true;
}

cFileDownloader::~cFileDownloader()
{

}

void cFileDownloader::startDownload()
{
	done = false;
	currentThumbNailFinished = false;
	QNetworkReply *reply = network->get(QNetworkRequest(QUrl(sourceBaseURL + "/filelist.txt")));
	connect(reply, SIGNAL(finished()),
					this, SLOT(filelistDownloaded()));
	while(!done)
	{
		Wait(100);
		gApplication->processEvents();
	}
}

void cFileDownloader::filelistDownloaded()
{
	// read filelist content
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	QString fileListContent = reply->readAll();
	QStringList tempList = fileListContent.split("\n");
	this->cntFilesAlreadyExists = 0;
	this->cntFilesToDownload = 0;
	this->cntFilesDownloaded = 0;
	for(int i = 0; i < tempList.size(); i++){
		QString temp = tempList.at(i).trimmed();
		if(temp == "" || temp.startsWith("#")) continue;
		if(QFile::exists(this->targetDir + QDir::separator() + temp)){
			 this->cntFilesAlreadyExists++;
			 continue;
		}
		this->cntFilesToDownload++;
		this->filesToDownload.append(temp);
	}

	// process all thumbnails
	for(int i = 0; i < this->filesToDownload.size(); i++){
		QString thumbnail = this->filesToDownload.at(i);
		QNetworkReply *reply = network->get(QNetworkRequest(QUrl(sourceBaseURL + "/" + thumbnail)));
		tempFile = new QFile(this->targetDir + QDir::separator() + thumbnail);
		if(!tempFile->open(QIODevice::WriteOnly)){
			qCritical() << "could not open thumbnail file for writing!";
		}
		else
		{
			connect(reply, SIGNAL(finished()),
							this, SLOT(thumbnailDownloaded()));
			while(!currentThumbNailFinished)
			{
				Wait(10);
				gApplication->processEvents();
			}
		}
		currentThumbNailFinished = false;
	}
	done = true;
}

void cFileDownloader::thumbnailDownloaded()
{
	QNetworkReply *reply = qobject_cast<QNetworkReply*>(sender());
	tempFile->write(reply->readAll());
	tempFile->flush();
	tempFile->close();
	this->cntFilesDownloaded++;
	QFileInfo fileInfo(tempFile->fileName());

	emit updateProgressAndStatus(
		tr("File downloader"),
		tr("file %1 downloaded, %2 of %3").arg(
					fileInfo.fileName(),
					QString::number(this->cntFilesDownloaded),
					QString::number(this->cntFilesToDownload)),
		1.0 * this->cntFilesDownloaded / this->cntFilesToDownload
	);
	currentThumbNailFinished = true;
}
