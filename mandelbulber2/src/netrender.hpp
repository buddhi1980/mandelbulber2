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
 * Netrender class - Networking class for command and payload communication
 */

#ifndef MANDELBULBER2_SRC_NETRENDER_HPP_
#define MANDELBULBER2_SRC_NETRENDER_HPP_

#include <QTcpServer>
#include <QTcpSocket>
#include <QtCore>

#include "parameters.hpp"
#include "fractal_container.hpp"

// forward declarations
struct sRenderData;

class CNetRender : public QObject
{
	Q_OBJECT
public:
	CNetRender(qint32 workerCount);
	~CNetRender();

	//--------------- enumerations ---------------------
public:
	enum netCommand
	{
		netRender_NONE,
		netRender_VERSION,
		netRender_WORKER,
		netRender_RENDER,
		netRender_DATA,
		netRender_BAD,
		netRender_JOB,
		netRender_STOP,
		netRender_STATUS,
		netRender_SETUP,
		netRender_ACK
	};
	// VERSION - ask for server version
	// WORKER - ask for number of client CPU count
	// RENDER - list of lines needed to be rendered (to Client), and suggestion which lines should be
	// rendered first
	// DATA - data of rendered lines (to Server)
	// BAD - answer about wrong server version
	// JOB - settings and textures for clients (to clients). Receiving of job will start rendering
	// STOP - terminate rendering request (to clients)
	// STATUS - ask for status (to client)
	// SETUP - setup job id and starting positions
	// ACK - acknowledge after receive rendered lines

	enum netRenderStatus
	{
		netRender_DISABLED,
		netRender_READY,
		netRender_WORKING,
		netRender_NEW,
		netRender_CONNECTING,
		netRender_ERROR
	};
	// DISABLED - no slot configured
	// READY - ready for receiving jobs
	// WORKING - during rendering
	// NEW - just connected
	// CONNECTING - connecting in progress
	// ERROR - error occurred

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

	//---------------- internal data structures ----------------
private:
	// general message frame for sending/receiving
	struct sMessage
	{
		sMessage() : command(netRender_NONE), id(0), size(0) {}
		qint32 command;
		qint32 id;
		qint32 size;
		QByteArray payload;
	};

public:
	// all information about connected clients
	struct sClient
	{
		sClient() : socket(nullptr), status(netRender_NEW), linesRendered(0), clientWorkerCount(0) {}
		QTcpSocket *socket;
		sMessage msg;
		netRenderStatus status;
		qint32 linesRendered;
		qint32 clientWorkerCount;
		QString name;
	};

	//----------------- public methods --------------------------
public:
	// ask if server is established
	bool IsServer() const { return deviceType == netRender_SERVER; }
	// ask if client is connected
	bool IsClient() const { return deviceType == netRender_CLIENT; }
	// initializing server
	void SetServer(qint32 portNo);
	// deleting server
	void DeleteServer();
	// connecting client
	void SetClient(QString address, qint32 portNo);
	// deleting client
	void DeleteClient();

	// get client
	const sClient &GetClient(int index);
	// get number of connected clients
	qint32 GetClientCount() const { return clients.size(); }
	// get number of CPU cores for selected client
	qint32 GetWorkerCount(qint32 index) { return clients[index].clientWorkerCount; }
	// get total number of available CPUs
	qint32 getTotalWorkerCount();
	// get status
	netRenderStatus GetStatus() const { return status; }
	// update status
	void SetStatus(netRenderStatus _status) { status = _status; }
	// get status of Client
	netRenderStatus GetClientStatus(int index);

	// stop rendering of all clients
	void Stop();
	// get line numbers which should be rendered first
	QList<int> GetStartingPositions() const { return startingPositions; }
	// get received textures
	QByteArray *GetTexture(QString textureName);

	bool WaitForAllClientsReady(double timeout);

	// setting status test
	static QString GetStatusText(netRenderStatus displayStatus);
	// setting status color
	static QString GetStatusColor(netRenderStatus displayStatus);

	bool Block();
	void Release() { isUsed = false; }

private:
	// send data to communication partner
	bool SendData(QTcpSocket *socket, sMessage msg) const;
	// receive data from partner
	void ReceiveData(QTcpSocket *socket, sMessage *msg);
	// process received data and send response if needed
	void ProcessData(QTcpSocket *socket, sMessage *inMsg);
	// clearing message buffer
	static void ResetMessage(sMessage *msg);
	// get client index by given socket pointer
	int GetClientIndexFromSocket(const QTcpSocket *socket) const;
	// compare major version of software
	static bool CompareMajorVersion(qint32 version1, qint32 version2);

	//---------------- private data -----------------
private:
	netRenderStatus status;
	QList<sClient> clients;
	sClient nullClient; // dummy client for fail-safe purposes
	QString address;
	qint32 portNo;
	qint32 totalReceived;
	qint32 totalReceivedUncompressed;
	qint32 version;
	qint32 workerCount;
	QTcpServer *server;
	QTcpSocket *clientSocket;
	typeOfDevice deviceType;
	sMessage msgFromServer;
	sMessage msgCurrentJob;
	QTimer *reconnectTimer;

	// client data buffers
	QString settingsText;
	qint32 actualId;
	QList<int> startingPositions;
	bool isUsed;
	QMap<QString, QByteArray> textures;

	//------------------- public slots -------------------
public slots:
	// send parameters and textures to all clients and start rendering
	void SetCurrentJob(
		cParameterContainer settings, cFractalContainer fractal, QStringList listOfTextures);
	// send to server a list of numbers and image data of already rendered lines
	void SendRenderedLines(QList<int> lineNumbers, QList<QByteArray> lines) const;
	// send list of already rendered lines
	void SendToDoList(int clientIndex, QList<int> done); // send list of already rendered lines
	// notify the server about client status change
	void NotifyStatus();
	// send message to all clients to stop rendering
	void StopAll();
	// send client id and list of list of lines to render at the beginning to selected client
	void SendSetup(int clientIndex, int id, QList<int> startingPositions);

	//------------------- private slots ------------------
private slots:
	// handle new client
	void HandleNewConnection();
	// delete client from list when disconnected
	void ClientDisconnected();
	// received data from client
	void ReceiveFromClient();
	// when client is disconnected from server
	void ServerDisconnected();
	// received data from server
	void ReceiveFromServer();
	// try to connect to server
	void TryServerConnect();

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
