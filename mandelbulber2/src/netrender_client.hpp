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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com), Sebastian Jennen (jenzebas@gmail.com)
 *
 * NetrenderClient class - Networking class for client specific command and payload communication
 */

#ifndef MANDELBULBER2_SRC_NETRENDER_CLIENT_HPP_
#define MANDELBULBER2_SRC_NETRENDER_CLIENT_HPP_

#include <QTcpServer>
#include <QTcpSocket>
#include <QtCore>

#include "netrender_transport.hpp"

// forward declarations
struct sRenderData;

class CNetRenderClient : public QObject
{
	Q_OBJECT
public:
	explicit CNetRenderClient();
	~CNetRenderClient() override;
	void SetClient(QString address, qint32 portNo);
	void DeleteClient();

	// notify the server about the current client status
	void SendStatusToServer(netRenderStatus status);
	// get received textures
	QByteArray *GetTexture(const QString &textureName, int frameNo);
	// get line numbers which should be rendered first
	QList<int> GetStartingPositions() { return startingPositions; }
	// send to server a list of numbers and image data of already rendered lines
	void SendRenderedLines(const QList<int> &lineNumbers, const QList<QByteArray> &lines);
	// get name of the connected server
	QString GetServerName() const { return serverName; }

private slots:
	// try to connect to server
	void TryServerConnect();
	// when client is disconnected from server
	void ServerDisconnected();
	// received data from server
	void ReceiveFromServer();

signals:
	// The client has been deleted
	void Deleted();
	// send list of rendered lines to cRenderer
	void ToDoListArrived(QList<int> done);
	// confirmation of data receive
	void AckReceived();
	// the status of the client has changed to this new status
	void changeClientStatus(netRenderStatus status);
	// notify about the current status
	void NotifyStatus();

private:
	void ProcessData();

	// Process methods
	void ProcessRequestVersion(sMessage *inMsg);
	void ProcessRequestStop(sMessage *inMsg);
	void ProcessRequestAskStatus(sMessage *inMsg);
	void ProcessRequestJob(sMessage *inMsg);
	void ProcessRequestRender(sMessage *inMsg);
	void ProcessRequestSetup(sMessage *inMsg);
	void ProcessRequestAck(sMessage *inMsg);
	void ProcessRequestKickAndKill(sMessage *inMsg);

	QTcpSocket *clientSocket;
	QTimer *reconnectTimer;
	QString address;
	QString serverName;
	qint32 portNo;
	sMessage msgFromServer;

	qint32 actualId;
	QList<int> startingPositions;
	QMap<QString, QByteArray> textures;
};

#endif /* MANDELBULBER2_SRC_NETRENDER_CLIENT_HPP_ */
