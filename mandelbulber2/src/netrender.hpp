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

//TODO blocking of START / STOP buttons on client side when NetRender is activated
//TODO add different error messages
//TODO clean up al qDebus()s and add WriteLog()s
//TODO check if there are no memory leaks
//TODO lock using NetRender if render job is not in main image
//TODO add displaying of client status (would be nice to have displayed information about server address)
//TODO add sending job if new client is connected during rendering

class CNetRender : public QObject
{
	Q_OBJECT
public:
	CNetRender(qint32 workerCount);
	~CNetRender();

	enum netCommand { NONE, VERSION, WORKER, RENDER, DATA, BAD, JOB, STOP, STATUS, SETUP};
	//VERSION - ask for server version
	//WORKER - ask for number of client CPU count
	//RENDER - list of lines needed to be rendered (to Client), and suggestion which lines should be rendered first
	//DATA - data of rendered lines (to Server)
	//BAD - answer about wrong server version
	//JOB - settings and textures for clients (to clients). Receiving of job will start rendering
	//STOP - terminate rendering request (to clients)
	//STATUS - ask for status (to client)
	//SETUP - setup job id and starting postions

	enum clientStatus { IDLE, WORKING, NEW };
	enum typeOfDevice { CLIENT, SERVER, UNKNOWN };
	enum enumUiNetRenderMode {netRenderClient, netRenderServer};

	struct sMessage
	{
		sMessage() : command(NONE), id(0), size(0) {}
		qint32 command;
		qint32 id;
		qint32 size;
		QByteArray payload;
	};

	struct sClient
	{
		sClient() : socket(NULL), status(NEW), jobsDone(0), jobsOpen(0), clientWorkerCount(0) {}
		QTcpSocket* socket;
		sMessage msg;
		clientStatus status;
		qint32 jobsDone;
		qint32 jobsOpen;
		qint32 clientWorkerCount;
	};

	bool IsServer() {return deviceType == SERVER;}
	bool IsClient() {return deviceType == CLIENT;}

	void SetServer(qint32 portNo);
	void DeleteServer();
	void SetClient(QString address, qint32 portNo);
	void DeleteClient();

	qint32 GetClientCount() {return clients.size();}
	qint32 GetWorkerCount(qint32 index) {return clients[index].clientWorkerCount;}
	qint32 getTotalWorkerCount();

	bool SendData(QTcpSocket *socket, sMessage msg);
	void GetJob(cParameterContainer *settings, cFractalContainer *fractal, sTextures *textures);
	void Stop(); //stop rendering of all clients
	void GetStatus(); //get status of all clients
	QList<int> GetStartingPositions() {return startingPositions;}

private:
	void ReceiveData(QTcpSocket *socket, sMessage *msg);
	void ProcessData(QTcpSocket *socket, sMessage *inMsg);
	void ResetMessage(sMessage *msg);
	int GetClientIndexFromSocket(const QTcpSocket *socket);

public:
	QList<sClient> clients;
	clientStatus status;

private:
	QString address;
	qint32 portNo;
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

public slots:
  void SendJob(cParameterContainer settings, cFractalContainer fractal, sTextures textures);
	void SendRenderedLines(QList<int> lineNumbers, QList<QByteArray> lines);
	void SendToDoList(int clientIndex, QList<int> toDo); //send list of lines to render and suggestion which lines should be rendered first
	void NotifyStatus();
	void StopAll();
	void SendSetup(int clientIndex, int id, QList<int> startingPositions);

private slots:
	void HandleNewConnection();
	void ClientDisconnected();
	void ReceiveFromClient();

	void ServerDisconnected();
	void ReceiveFromServer();
	void TryServerConnect();

signals:
	void ClientsChanged();
	void ClientsChanged(int i);
	void ClientsChanged(int i, int j);
	void NewJobReceived();
	void NewLinesArrived(QList<int> lineNumbers, QList<QByteArray> lines);
	void ToDoListArrived(QList<int> done);
	void StopReceived();
};

extern CNetRender *netRender;

#endif /* NETRENDER_HPP_ */
