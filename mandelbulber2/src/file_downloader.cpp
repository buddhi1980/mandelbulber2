/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cFileDownloader class - downloads specified filelist
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
	currentFileFinished = true;
	cntFilesAlreadyExists = 0;
	cntFilesToDownload = 0;
	cntFilesDownloaded = 0;
	tempFile = NULL;
}

cFileDownloader::~cFileDownloader()
{
	if(tempFile)
	{
		delete tempFile;
		tempFile = NULL;
	}
}

void cFileDownloader::downloadFilelist()
{
	emit updateProgressAndStatus(
				tr("File downloader"), tr("retrieving filelist"), 0.0
	);

	done = false;
	currentFileFinished = false;
	QNetworkReply *reply = network->get(
				QNetworkRequest(QUrl(sourceBaseURL + "/filelist.txt")));
	connect(reply, SIGNAL(finished()),
					this, SLOT(filelistDownloaded()));

	while(!done)
	{
		Wait(100);
		gApplication->processEvents();
	}

	emit updateProgressAndStatus(
		tr("File downloader"),
		tr("finished, downloaded %1 files").arg(this->cntFilesDownloaded),
		1.0
	);
}

void cFileDownloader::filelistDownloaded()
{
	// read filelist content and determine files to download
	QNetworkReply *replyList = qobject_cast<QNetworkReply*>(sender());
	QString fileListContent = replyList->readAll();
	QStringList tempList = fileListContent.split("\n");

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

	// process all files to download
	for(int i = 0; i < this->filesToDownload.size(); i++){
		QString file = this->filesToDownload.at(i);
		QNetworkReply *replyFile = network->get(
					QNetworkRequest(QUrl(sourceBaseURL + "/" + file)));
		if(tempFile)
		{
			delete tempFile;
			tempFile = NULL;
		}
		tempFile = new QFile(this->targetDir + QDir::separator() + file);
		if(!tempFile->open(QIODevice::WriteOnly)){
			qCritical() << "could not open file for writing!";
		}
		else
		{
			connect(replyFile, SIGNAL(finished()),
							this, SLOT(fileDownloaded()));
			while(!currentFileFinished)
			{
				Wait(10);
				gApplication->processEvents();
			}
		}
		currentFileFinished = false;
	}
	done = true;
}

void cFileDownloader::fileDownloaded()
{
	// write downloaded data to file and emit progress
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
	currentFileFinished = true;
}
