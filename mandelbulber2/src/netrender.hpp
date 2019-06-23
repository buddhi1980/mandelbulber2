/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Netrender class - Networking class for command and payload communication
 */

#ifndef MANDELBULBER2_SRC_NETRENDER_HPP_
#define MANDELBULBER2_SRC_NETRENDER_HPP_

#include <QTcpServer>
#include <QTcpSocket>
#include <QtCore>

#include "fractal_container.hpp"
#include "netrender_client.hpp"
#include "netrender_server.hpp"
#include "netrender_transport.hpp"
#include "parameters.hpp"

// forward declarations
struct sRenderData;

class CNetRender : public QObject
{
	Q_OBJECT
public:
	explicit CNetRender();
	~CNetRender() override;

	//--------------- enumerations ---------------------
public:
	enum typeOfDevice
	{
		netRender_CLIENT,
		netRender_SERVER,
		netRender_UNKNOWN
	};

	enum enumUiNetRenderMode
	{
		netRenderClient,
		netRenderServer
	};

	//----------------- public methods --------------------------
public:
	// ask if server is established
	bool IsServer() const { return deviceType == netRender_SERVER; }
	// ask if client is connected
	bool IsClient() const { return deviceType == netRender_CLIENT; }
	// initializing server
	void SetServer(qint32 _portNo);
	// deleting server
	void DeleteServer();
	// connecting client
	void SetClient(QString address, qint32 portNo);
	// deleting client
	void DeleteClient();

	// get client
	const sClient &GetClient(int index) { return cNetRenderServer->GetClient(index); }
	// get number of connected clients
	qint32 GetClientCount() const { return cNetRenderServer->GetClientCount(); }
	// get number of CPU cores for selected client
	qint32 GetWorkerCount(qint32 index) { return cNetRenderServer->GetWorkerCount(index); }
	// get total number of available CPUs
	qint32 getTotalWorkerCount() { return cNetRenderServer->getTotalWorkerCount(); }
	// get status
	netRenderStatus GetStatus() const { return status; }
	// get name of the connected server
	QString GetServerName() const { return serverName; }
	// update status
	void SetStatus(netRenderStatus _status) { status = _status; }
	// get status of Client
	netRenderStatus GetClientStatus(int index);

	// stop rendering of all clients
	void Stop();
	// get line numbers which should be rendered first
	QList<int> GetStartingPositions() const { return startingPositions; }
	// get received textures
	QByteArray *GetTexture(const QString &textureName, int frameNo);

	bool WaitForAllClientsReady(double timeout);

	// setting status test
	static QString GetStatusText(netRenderStatus displayStatus);
	// setting status color
	static QString GetStatusColor(netRenderStatus displayStatus);

	bool Block();
	void Release() { isUsed = false; }

private:
	void ReceiveData(QTcpSocket *socket, sMessage *msg);
	bool SendData(QTcpSocket *socket, sMessage msg) const;
	// process received data and send response if needed
	void ProcessData(QTcpSocket *socket, sMessage *inMsg);
	// get client index by given socket pointer
	int GetClientIndexFromSocket(const QTcpSocket *socket)
	{
		return cNetRenderServer->GetClientIndexFromSocket(socket);
	}

	//---------------- private data -----------------
private:
	CNetRenderClient *cNetRenderClient;
	CNetRenderServer *cNetRenderServer;
	netRenderStatus status;
	QString serverName;
	typeOfDevice deviceType;
	sMessage msgFromServer;
	sMessage msgCurrentJob;

	// client data buffers
	qint32 actualId;
	QList<int> startingPositions;
	bool isUsed;
	QMap<QString, QByteArray> textures;

	//------------------- public slots -------------------
public slots:
	// send parameters and textures to all clients and start rendering
	void SetCurrentJob(const cParameterContainer &settings, const cFractalContainer &fractal,
		QStringList listOfTextures);
	// send to server a list of numbers and image data of already rendered lines
	void SendRenderedLines(const QList<int> &lineNumbers, const QList<QByteArray> &lines) const;
	// send list of already rendered lines
	void SendToDoList(int clientIndex, const QList<int> &done); // send list of already rendered lines
	// notify the server about client status change
	void NotifyStatus();
	// send message to all clients to stop rendering
	void StopAll();
	// send client id and list of list of lines to render at the beginning to selected client
	void SendSetup(int clientIndex, int id, const QList<int> &_startingPositions);
	// kicks and kills a client (can be used if client is hanging)
	void KickAndKillClient(int clientIndex);

	//------------------- private slots ------------------
private slots:
	// received client status changed
	void clientStatusChanged(netRenderStatus _status);
	// received server status changed
	void serverStatusChanged(netRenderStatus _status);
	// received data on client connection
	void clientReceiveData();
	void ClientsHaveChanged();
	void SendVersionToClient(int index);
	void ReceiveFromClient(int index);

signals:
	// request to update table of clients
	void ClientsChanged();
	void ClientsChanged(int i);
	void ClientsChanged(int i, int j);
	// send data of newly rendered lines to cRenderer
	void NewLinesArrived(QList<int> lineNumbers, QList<QByteArray> lines);
	// send list of rendered lines to cRenderer
	void ToDoListArrived(QList<int> done);
	// confirmation of data receive
	void AckReceived();

	void NewStatusClient();
	void NewStatusServer();
};

extern CNetRender *gNetRender;

#endif /* MANDELBULBER2_SRC_NETRENDER_HPP_ */
