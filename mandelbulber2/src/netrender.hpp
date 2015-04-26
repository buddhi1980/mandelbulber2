/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * Netrender class - Networking class for command and payload communication
 * between one server and multiple clients
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

#ifndef NETRENDER_HPP_
#define NETRENDER_HPP_

#include <QtCore>
#include <QTcpSocket>
#include <QTcpServer>
#include "parameters.hpp"
#include "fractal_container.hpp"
#include "fractparams.hpp"
#include "scheduler.hpp"

//TODO add different error messages
//TODO check if there are no memory leaks
//TODO add sending job if new client is connected during rendering

class CNetRender : public QObject
{
	Q_OBJECT
public:
	CNetRender(qint32 workerCount);
	~CNetRender();

//--------------- enumerations ---------------------
public:
	enum netCommand { netRender_NONE, netRender_VERSION, netRender_WORKER, netRender_RENDER, netRender_DATA,
										netRender_BAD, netRender_JOB, netRender_STOP, netRender_STATUS, netRender_SETUP};
	//VERSION - ask for server version
	//WORKER - ask for number of client CPU count
	//RENDER - list of lines needed to be rendered (to Client), and suggestion which lines should be rendered first
	//DATA - data of rendered lines (to Server)
	//BAD - answer about wrong server version
	//JOB - settings and textures for clients (to clients). Receiving of job will start rendering
	//STOP - terminate rendering request (to clients)
	//STATUS - ask for status (to client)
	//SETUP - setup job id and starting positions

	enum netRenderStatus { netRender_DISABLED, netRender_READY, netRender_WORKING, netRender_NEW, netRender_CONNECTING, netRender_ERROR };
	//DISABLED - no slot configured
	//READY - ready for receiving jobs
	//WORKING - during rendering
	//NEW - just connected
	//CONNECTING - connecting in progress
	//ERROR - error occurred

	enum typeOfDevice { netRender_CLIENT, netRender_SERVER, netRender_UNKNOWN };

	enum enumUiNetRenderMode {netRenderClient, netRenderServer};

//---------------- internal data structures ----------------
private:
	//general message frame for sending/receiving
	struct sMessage
	{
		sMessage() : command(netRender_NONE), id(0), size(0) {}
		qint32 command;
		qint32 id;
		qint32 size;
		QByteArray payload;
	};

public:
	//all information about connected clients
	struct sClient
	{
		sClient() : socket(NULL), status(netRender_NEW), jobsDone(0), jobsOpen(0), clientWorkerCount(0) {}
		QTcpSocket* socket;
		sMessage msg;
		netRenderStatus status;
		qint32 jobsDone;
		qint32 jobsOpen;
		qint32 clientWorkerCount;
		QString name;
	};

//----------------- public methods --------------------------
public:
	//ask if server is established
	bool IsServer() {return deviceType == netRender_SERVER;}
	//ask if client is connected
	bool IsClient() {return deviceType == netRender_CLIENT;}
	//initializing server
	void SetServer(qint32 portNo);
	//deleting server
	void DeleteServer();
	//connecting client
	void SetClient(QString address, qint32 portNo);
	//deleting client
	void DeleteClient();

	//get client
	const sClient& GetClient(int index);
	//get number of connected clients
	qint32 GetClientCount() {return clients.size();}
	//get number of CPU cores for selected client
	qint32 GetWorkerCount(qint32 index) {return clients[index].clientWorkerCount;}
	//get total number of available CPUs
	qint32 getTotalWorkerCount();
	//get status
	netRenderStatus GetStatus() {return status;}
	//update status
	void SetStatus(netRenderStatus _status) {status = _status;}

	//stop rendering of all clients
	void Stop();
	//get line numbers which should be rendered first
	QList<int> GetStartingPositions() {return startingPositions;}
	//get received textures
	sTextures *GetTextures() {return &textures;}

	//setting status test
	static QString GetStatusText(netRenderStatus displayStatus);
	//setting status color
	static QString GetStatusColor(netRenderStatus displayStatus);

private:
	//send data to communication partner
	bool SendData(QTcpSocket *socket, sMessage msg);
	//receive data from partner
	void ReceiveData(QTcpSocket *socket, sMessage *msg);
	//process received data and send response if needed
	void ProcessData(QTcpSocket *socket, sMessage *inMsg);
	//clearing message buffer
	void ResetMessage(sMessage *msg);
	//get client index by given socket pointer
	int GetClientIndexFromSocket(const QTcpSocket *socket);

//---------------- private data -----------------
private:
	netRenderStatus status;
	QList<sClient> clients;
	sClient nullClient; //dummy client for fail-safe purposes
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
	QTimer *reconnectTimer;

	//client data buffers
	QString settingsText;
	sTextures textures;
	qint32 actualId;
	QList<int> startingPositions;

//------------------- public slots -------------------
public slots:
	//send parameters and textures to all clients and start rendering
  void SendJob(cParameterContainer settings, cFractalContainer fractal, sTextures textures);
	//send to server a list of numbers and image data of already rendered lines
  void SendRenderedLines(QList<int> lineNumbers, QList<QByteArray> lines);
	//send list of already rendered lines
  void SendToDoList(int clientIndex, QList<int> done); //send list of already rendered lines
	//notify the server about client status change
  void NotifyStatus();
  //send message to all clients to stop rendering
	void StopAll();
	//send client id and list of list of lines to render at the beginning to selected client
	void SendSetup(int clientIndex, int id, QList<int> startingPositions);

//------------------- private slots ------------------
private slots:
	//handle new client
	void HandleNewConnection();
	//delete client from list when disconnected
	void ClientDisconnected();
	//received data from client
	void ReceiveFromClient();
	//when client is disconnected from server
	void ServerDisconnected();
	//received data from server
	void ReceiveFromServer();
	//try to connect to server
	void TryServerConnect();

signals:
	//request to update table of clients
	void ClientsChanged();
	void ClientsChanged(int i);
	void ClientsChanged(int i, int j);
	//send data of newly rendered lines to cRenderer
	void NewLinesArrived(QList<int> lineNumbers, QList<QByteArray> lines);
	//send list of rendered lines to cRenderer
	void ToDoListArrived(QList<int> done);

	void NewStatusClient();
	void NewStatusServer();
};

extern CNetRender *netRender;

#endif /* NETRENDER_HPP_ */
