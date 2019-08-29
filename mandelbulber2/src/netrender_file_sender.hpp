/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2019 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * NetrenderFileSender class - watches for transmissable files on the client to
 * send to the server
 */

#ifndef MANDELBULBER2_SRC_NETRENDER_FILE_SENDER_HPP_
#define MANDELBULBER2_SRC_NETRENDER_FILE_SENDER_HPP_

#include <QtCore>

class cNetRenderFileSender : public QObject
{
	Q_OBJECT
	const qint64 CHUNK_SIZE = 1024 * 1024;

public:
	cNetRenderFileSender(QObject *parent = nullptr);
	~cNetRenderFileSender() override;
	void ClearState();

public slots:
	void AddFileToQueue(QString filename);
	void AcknowledgeReceived();

private:
	void sendFileOverNetrender(const QString &file);
	void SendDataChunk(); // after acknowledge

	QQueue<QString> fileQueue;
	QString actualFileName;
	qint64 actualFileSize;
	qint64 actualNumberOfChunks;
	qint64 actualChunkIndex;
	bool sendingInProgress;
	bool lastChunk;
	QFile actualFile;

signals:
	void NetRenderSendHeader(qint64 size, QString filename);
	void NetRenderSendChunk(int chunkIndex, QByteArray data);
};

#endif /* MANDELBULBER2_SRC_NETRENDER_FILE_SENDER_HPP_ */
