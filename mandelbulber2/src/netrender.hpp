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

class CNetRender : public QObject
{
	Q_OBJECT
public:
	CNetRender(qint32 workerCount);
	~CNetRender();

	enum netCommand { VERSION, WORKER, RENDER, DATA, BAD };
	enum clientStatus { IDLE, WORKING, NEW };
	enum typeOfDevice { CLIENT, SERVER, UNKNOWN };

	struct sMessage
	{
		sMessage() : command(-1), id(0), size(0), payload(NULL) {}
		qint32 command;
		qint32 id;
		qint32 size;
		char* payload;
		//QByteArray payload - will be better
	};

	struct sClient
	{
		sClient() : socket(NULL), msg(NULL), status(NEW), clientWorkerCount(0) {}
		QTcpSocket* socket;
		sMessage* msg;
		clientStatus status;
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

private:
	void ReceiveData(QTcpSocket *socket, sMessage *msg);
	void ProcessData(QTcpSocket *socket, sMessage *inMsg);
	void ResetMessage(sMessage *msg);
	int GetClientIndexFromSocket(const QTcpSocket *socket);

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
	QTcpServer *server;
	QTcpSocket *clientSocket;
	typeOfDevice deviceType;
	sMessage msgFromServer;
	QTimer *reconnectTimer;

signals:
	// TODO connect signals
	void RenderRequest(sMessage *msg);
	void RenderResponse(qint32 index, sMessage *msg);
	void ClientsChanged();
};

extern CNetRender *netRender;

#endif /* NETRENDER_HPP_ */
