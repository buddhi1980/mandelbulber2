/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2018-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * cResourceHttpProvider class - loads a URL resource over the internet,
 * caches it locally and provides the filename
 */

#include "resource_http_provider.hpp"

#include <QNetworkReply>
#include <QNetworkRequest>
#include <QSslConfiguration>

#include "global_data.hpp"
#include "system.hpp"

cResourceHttpProvider::cResourceHttpProvider(QString &_filename)
{
	filename = _filename;
	QCryptographicHash hashCrypt(QCryptographicHash::Md4);
	hashCrypt.addData(filename.toLocal8Bit());
	QByteArray hash = hashCrypt.result();
	cachedFilename = systemData.GetHttpCacheFolder() + QDir::separator() + hash.toHex() + "."
									 + QFileInfo(filename).suffix();
	reply = nullptr;
	outFile = nullptr;
}

bool cResourceHttpProvider::IsUrl()
{
	if (filename.startsWith("http://") || filename.startsWith("https://"))
	{
		return true;
	}
	return false;
}

QString cResourceHttpProvider::cacheAndGetFilename()
{
	if (!QFile(cachedFilename).exists())
	{
		loadOverInternet();
	}
	return cachedFilename;
}

void cResourceHttpProvider::loadOverInternet()
{
	outFile = new QFile(cachedFilename);
	if (!outFile->open(QIODevice::WriteOnly))
	{
		qCritical() << "could not open file for writing!";
	}

	QNetworkAccessManager network;
	QUrl url(filename);
	QNetworkRequest request(url);

	reply = network.get(request);

	QEventLoop loop;
	connect(reply, SIGNAL(readyRead()), this, SLOT(moreData()));
	connect(reply, SIGNAL(finished()), this, SLOT(closeFile()));
	connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
	connect(reply, SIGNAL(error(QNetworkReply::NetworkError)), &loop, SLOT(quit()));

	// ssl error checking is disabled, otherwise throws error"SSL handshake failed"
	// watch out: disabling of ssl errors means that the site identity is not checked,
	// possibly allowing mitm attacks!
	// TODO: find a way to re-enable ssl error checking
	reply->ignoreSslErrors();

	loop.exec();

	outFile->close();
	outFile->flush();

	if (reply->isFinished())
	{
		QNetworkReply::NetworkError error = reply->error();
		if (error != QNetworkReply::NoError)
		{

			qCritical() << "Error when downloaded file!" << reply->errorString() << filename;
		}
	}
	else
	{
		qCritical() << "Error when downloaded file!" << reply->errorString() << filename;
	}
}

void cResourceHttpProvider::moreData()
{
	outFile->write(reply->readAll());
}

void cResourceHttpProvider::closeFile() {}
