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

#ifndef FILE_DOWNLOADER_HPP_
#define FILE_DOWNLOADER_HPP_

#include <QUrl>
#include <QDir>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QFile>

class cFileDownloader: public QObject
{
Q_OBJECT

public:
	cFileDownloader(QString sourceBaseURL, QString targetDir);
	~cFileDownloader();
	void startDownload();

private slots:
	void filelistDownloaded();
	void thumbnailDownloaded();

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);

private:
	QString sourceBaseURL;
	QString targetDir;
	QNetworkAccessManager* network;
	QFile* tempFile;
	QStringList filesToDownload;
	int cntFilesAlreadyExists;
	int cntFilesToDownload;
	int cntFilesDownloaded;
	bool done;
	bool currentThumbNailFinished;
};

#endif /* FILE_DOWNLOADER_HPP_ */
