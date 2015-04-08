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

class CNetRender : public QObject
{
	Q_OBJECT
public:
	CNetRender(qint32 workerCount);
	~CNetRender();

	enum netCommand { NONE, VERSION, WORKER, RENDER, DATA, BAD, JOB, STOP, STATUS};
	//VERSION - ask for server version
	//WORKER - ask for number of client CPU count
	//RENDER - list of lines needed to be rendered (to Client), and suggestion which lines should be rendered first
	//DATA - data of rendered lines (to Server)
	//BAD - answer about wrong server version
	//JOB - settings and textures for clients (to clients). Receiving of job will start rendering
	//STOP - terminate rendering request (to clients)
	//STATUS - ask for status (to client)

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

	//TODO ------- netrender functions to do ------------
	void SendJob(cParameterContainer *settings, cFractalContainer *fractal, sTextures *textures);
	void GetJob(cParameterContainer *settings, cFractalContainer *fractal, sTextures *textures);
	void Stop(); //stop rendering of all clients
	void GetStatus(); //get status of all clients
	void SendToDoList(cScheduler *scheduler); //send list of lines to render and suggestion which lines should be rendered first
	void GetToDoList(cScheduler *scheduler);
	void GetRenderedLines(QList<int> *lineNumbers, QList<QByteArray> *lines);

private:
	void ReceiveData(QTcpSocket *socket, sMessage *msg);
	void ProcessData(QTcpSocket *socket, sMessage *inMsg);
	void ResetMessage(sMessage *msg);
	int GetClientIndexFromSocket(const QTcpSocket *socket);

public slots:
	void SendRenderedLines(QList<int> lineNumbers, QList<QByteArray> lines);

private slots:
	void HandleNewConnection();
	void ClientDisconnected();
	void ReceiveFromClient();

	void ServerDisconnected();
	void ReceiveFromServer();
	void TryServerConnect();

public:
	QList<sClient> clients;

private:
	QString address;
	qint32 portNo;
	qint32 version;
	qint32 workerCount;
	clientStatus status;
	QTcpServer *server;
	QTcpSocket *clientSocket;
	typeOfDevice deviceType;
	sMessage msgFromServer;
	QTimer *reconnectTimer;

	//server data buffers
	QList<QByteArray> receivedRenderedLines;
	QList<int> receivedLineNumbers;

	//client data buffers
	QString settingsText;
	sTextures textures;

signals:
	// TODO connect signals
	void ThereIsMoreData();
	void RenderRequest(sMessage *msg);
	void RenderResponse(qint32 index, sMessage *msg);
	void ClientsChanged();
	void ClientsChanged(int i);
	void ClientsChanged(int i, int j);

	//TODO new signals
	void NewJobReceived();
	void NewLinesArrived();
	void ToDoListReceived();
	void StopReceived();
};

extern CNetRender *netRender;

#endif /* NETRENDER_HPP_ */
