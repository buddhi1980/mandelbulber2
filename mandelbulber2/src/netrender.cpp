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

#include "netrender.hpp"
#include "system.hpp"
#include <QAbstractSocket>
#include <QHostInfo>
#include "settings.hpp"
#include "global_data.hpp"
#include "interface.hpp"

CNetRender::CNetRender(qint32 workerCount) : QObject(NULL)
{
	this->workerCount = workerCount;
	deviceType = UNKNOWN;
	version = 1000 * MANDELBULBER_VERSION;
	clientSocket = NULL;
	server = NULL;
	portNo = 0;
	status = NEW;
	reconnectTimer = NULL;
	actualId = 0;
	totalReceivedUncompressed = 0;
	totalReceived = 0;
}

CNetRender::~CNetRender()
{
	DeleteServer();
	DeleteClient();
}

void CNetRender::SetServer(qint32 portNo)
{
	DeleteClient();
	DeleteServer();
	this->portNo = portNo;
	server = new QTcpServer(this);
	WriteLog("NetRender - starting server.");
	if(!server->listen(QHostAddress::Any, portNo))
	{
		if(server->serverError() == QAbstractSocket::AddressInUseError)
		{
			qWarning() << "NetRender - address already in use. Is there already a mandelbulber server instance running on this port?";
		}
		else
		{
			qCritical() << "NetRender - SetServer Error: " << server->errorString();
		}
		deviceType = UNKNOWN;
	}
	else
	{
		connect(server, SIGNAL(newConnection()), this, SLOT(HandleNewConnection()));
		deviceType = SERVER;
		WriteLog("NetRender - Server Setup on localhost, port: " + QString::number(portNo));
		status = NEW;
		emit NewStatusServer();
	}
}

void CNetRender::DeleteServer()
{
	if(deviceType != SERVER) return;

	deviceType = UNKNOWN;
	WriteLog("NetRender - Delete Server");
	if(server)
	{
		server->close();
		delete server;
		server = NULL;
	}
	clients.clear();
	status = DISABLED;
	emit NewStatusServer();
}

void CNetRender::DeleteClient()
{
	if (deviceType != CLIENT) return;
	deviceType = UNKNOWN;
	WriteLog("NetRender - Delete Client");
	if(reconnectTimer->isActive()) reconnectTimer->stop();
	if(clientSocket)
	{
		clientSocket->close();
		delete clientSocket;
		clientSocket = NULL;
	}
	status = DISABLED;
	emit NotifyStatus();
}

int CNetRender::getTotalWorkerCount()
{
	int totalCount = 0;
	for(int i = 0; i < clients.count(); i++)
	{
		totalCount += clients[i].clientWorkerCount;
	}
	return totalCount;
}

void CNetRender::HandleNewConnection()
{
	while (server->hasPendingConnections())
	{
		WriteLog("NetRender - new client connected");
		// push new socket to list
		sClient client;
		client.socket = server->nextPendingConnection();
		clients.append(client);

		connect(client.socket, SIGNAL(disconnected()), this, SLOT(ClientDisconnected()));
		connect(client.socket, SIGNAL(readyRead()), this, SLOT(ReceiveFromClient()));

		// tell mandelbulber version to client
		sMessage msg;
		msg.command = VERSION;
		msg.payload.append((char*)&version, sizeof(qint32));
		SendData(client.socket, msg);
		emit ClientsChanged();
	}
}

void CNetRender::ClientDisconnected()
{
	// get client by signal emitter
	QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
	if(socket)
	{
		int index = GetClientIndexFromSocket(socket);
		WriteLog("NetRender - Client disconnected #" + QString::number(index) + " (" + socket->peerAddress().toString() + ")");
		if(index > -1){
			clients.removeAt(index);
		}
		socket->close();
		socket->deleteLater();
		emit ClientsChanged();
	}
}

int CNetRender::GetClientIndexFromSocket(const QTcpSocket *socket)
{
	for(int i = 0; i < clients.size(); i++)
	{
		if(clients.at(i).socket->socketDescriptor() == socket->socketDescriptor())
		{
			return i;
		}
	}
	return -1;
}

void CNetRender::ReceiveFromClient()
{
	// get client by signal emitter
	QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
	int index = GetClientIndexFromSocket(socket);
	if(index != -1)
	{
		ReceiveData(socket, &clients[index].msg);
	}
}

void CNetRender::SetClient(QString address, int portNo)
{
	DeleteServer();
	deviceType = CLIENT;
	status = NEW;
	this->address = address;
	this->portNo = portNo;
	ResetMessage(&msgFromServer);
	clientSocket = new QTcpSocket(this);

	reconnectTimer = new QTimer;
	reconnectTimer->setInterval(1000);
	connect(reconnectTimer, SIGNAL(timeout()), this, SLOT(TryServerConnect()));
	connect(clientSocket, SIGNAL(disconnected()), this, SLOT(ServerDisconnected()));
	connect(clientSocket, SIGNAL(readyRead()), this, SLOT(ReceiveFromServer()));

	reconnectTimer->start();
	WriteLog("NetRender - Client Setup, link to server: " + address + ", port: " + QString::number(portNo));
	emit NotifyStatus();
}

void CNetRender::ServerDisconnected()
{
	reconnectTimer->start();
}

void CNetRender::TryServerConnect()
{
	if(clientSocket)
	{
		if(clientSocket->state() == QAbstractSocket::ConnectedState)
		{
			reconnectTimer->stop();
		}
		else
		{
			clientSocket->close();
			clientSocket->connectToHost(address, portNo);
		}
	}
}

void CNetRender::ReceiveFromServer()
{
	ReceiveData(clientSocket, &msgFromServer);
}

bool CNetRender::SendData(QTcpSocket *socket, sMessage msg)
{
	//			NetRender Message format         (optional)
	// | qint32		| qint32	| qint32	|( 0 - ???	| quint16  |)
	// | command	| id			| size		|( payload	| checksum |)

	if (!socket) return false;
	if(socket->state() != QAbstractSocket::ConnectedState) return false;

	QByteArray byteArray;
	QDataStream socketWriteStream(&byteArray, QIODevice::ReadWrite);

	if(msg.payload.size() > 0){
		msg.payload = qCompress(msg.payload);
	}

	msg.size = msg.payload.size();
	msg.id = actualId;

	// append header
	socketWriteStream << msg.command << msg.id << msg.size;

	// append payload
	if(msg.size > 0)
	{
		socketWriteStream.writeRawData(msg.payload.data(), msg.size);
		// append checksum
		qint16 checksum = qChecksum(msg.payload.data(), msg.size);
		socketWriteStream << checksum;
	}

	// write to socket
	socket->write(byteArray);
	socket->waitForBytesWritten();

	return true;
}

void CNetRender::ResetMessage(sMessage *msg)
{
	if(msg == NULL)
	{
		qCritical() << "CNetRender::ResetMessage(sMessage *msg): message has NULL pointer!";
	}
	else
	{
		msg->command = NONE;
		msg->id = 0;
		msg->size = 0;
		msg->payload.clear();
	}
}

void CNetRender::ReceiveData(QTcpSocket *socket, sMessage *msg)
{
	QDataStream socketReadStream(socket);
	qint64 bytesAvailable = socket->bytesAvailable();

	while(bytesAvailable > 0)
	{
		if (msg->command == NONE)
		{
			if (socket->bytesAvailable() < (sizeof(msg->command) + sizeof(msg->id) + sizeof(msg->size)))
			{
				return;
			}
			// meta data available
			socketReadStream >> msg->command;
			socketReadStream >> msg->id;
			socketReadStream >> msg->size;
		}

		bytesAvailable = socket->bytesAvailable();

		if(msg->size > 0)
		{
			if (bytesAvailable < sizeof(quint16) + msg->size)
			{
				return;
			}
			// full payload available
			char* buffer = new char[msg->size];
			socketReadStream.readRawData(buffer, msg->size);
			msg->payload.append(buffer, msg->size);

			quint16 crcCalculated = qChecksum(buffer, msg->size);
			quint16 crcReceived;
			socketReadStream >> crcReceived;

			delete[] buffer;
			if(crcCalculated != crcReceived)
			{
				//ResetMessage(msg);
				//socketReadStream.atEnd();
				//socketReadStream.skipRawData(socket->bytesAvailable());
				return;
			}
			msg->payload = qUncompress(msg->payload);
			msg->size = msg->payload.size();
		}
		ProcessData(socket, msg);

		bytesAvailable = socket->bytesAvailable();
	}
}

void CNetRender::ProcessData(QTcpSocket *socket, sMessage *inMsg)
{
	// beware: payload points to char, first cast to target type pointer, then dereference
	// *(qint32*)msg->payload

	//------------------------- CLIENT ------------------------
	if(IsClient())
	{
		switch ((netCommand)inMsg->command)
		{
		case VERSION:
		{
			sMessage outMsg;
			if(*(qint32*)inMsg->payload.data() == version)
			{
				WriteLog("NetRender - version matches (" + QString::number(version) + "), connection established");
				// server version matches, send worker count
				outMsg.command = WORKER;
				QDataStream stream(&outMsg.payload, QIODevice::WriteOnly);
				stream << workerCount;
				QString machineName = QHostInfo::localHostName();
				stream << (qint32)machineName.toUtf8().size();
				stream.writeRawData(machineName.toUtf8().data(), machineName.toUtf8().size());
				status = READY;
				emit NewStatusClient();
			}
			else
			{
				qWarning() << "CNetRender - version mismatch! client version: " << version << ", server: " << *(qint32*)inMsg->payload.data();
				outMsg.command = BAD;
			}
			SendData(clientSocket, outMsg);
			break;
			}
			case STOP:
			{
				status = READY;
				gMainInterface->stopRequest = true;
				emit NotifyStatus();
				emit StopReceived();
				break;
			}
			case STATUS:
			{
				emit NotifyStatus();
				break;
			}
			case JOB:
			{
				if (inMsg->id == actualId)
				{
					WriteLog("NetRender - received new job");
					QDataStream stream(&inMsg->payload, QIODevice::ReadOnly);
					QByteArray buffer;
					qint32 size;
					status = WORKING;
					emit NotifyStatus();

					// read settings
					stream >> size;
					buffer.resize(size);
					stream.readRawData(buffer.data(), size);
					settingsText = QString::fromUtf8(buffer.data(), buffer.size());

					// read textures
					QList<cTexture*> textureList;
					textureList.append(textures.backgroundTexture);
					textureList.append(textures.envmapTexture);
					textureList.append(textures.lightmapTexture);
					for (int i = 0; i < textureList.size(); i++)
					{
						stream >> size;
						if (size > 0)
						{
							if (textureList[i] == NULL)
							{
								textureList[i] = new cTexture();
							}
							buffer.resize(size);
							stream.readRawData(buffer.data(), size);
							textureList[i]->FromQByteArray(buffer);
						}
					}

					cSettings parSettings(cSettings::formatCondensedText);
					parSettings.BeQuiet(true);
					parSettings.LoadFromString(settingsText);
					parSettings.Decode(gPar, gParFractal);

					gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
					gMainInterface->StartRender();

					emit NewJobReceived();
				}
				else
				{
					WriteLog("NetRender - received JOB message with wrong id");
				}
				break;
			}
			case RENDER:
			{
				if (inMsg->id == actualId)
				{
					QDataStream stream(&inMsg->payload, QIODevice::ReadOnly);
					qint32 doneSize;
					stream >> doneSize;
					QList<int> done;
					for (int i = 0; i < doneSize; i++)
					{
						qint32 line;
						stream >> line;
						done.append(line);
					}

					emit ToDoListArrived(done);
				}
				else
				{
					WriteLog("NetRender - received RENDER message with wrong id");
				}
				break;
			}

			case SETUP:
			{
				QDataStream stream(&inMsg->payload, QIODevice::ReadOnly);

				stream >> actualId;

				qint32 startingPositionsSize;
				stream >> startingPositionsSize;
				startingPositions.clear();
				for (int i = 0; i < startingPositionsSize; i++)
				{
					qint32 line;
					stream >> line;
					startingPositions.append(line);
				}
				break;
			}
			default:
				break;
		}
	}

	//----------------------------- SERVER ----------------------
	else if(IsServer())
	{
		int index = GetClientIndexFromSocket(socket);
		if(index > -1)
		{
			switch ((netCommand)inMsg->command)
			{
			case BAD:
			{
				clients.removeAt(index);
				emit ClientsChanged();
				break;
			}
			case WORKER:
			{
				QDataStream stream(&inMsg->payload, QIODevice::ReadOnly);
				stream >> clients[index].clientWorkerCount;
				QByteArray buffer;
				qint32 size;
				stream >> size;
				buffer.resize(size);
				stream.readRawData(buffer.data(), size);
				clients[index].name = QString::fromUtf8(buffer.data(), buffer.size());

				if(clients[index].status == NEW) clients[index].status = READY;
				WriteLog("NetRender - new Client #" + QString::number(index) + "(" + clients[index].name + " - " + clients[index].socket->peerAddress().toString() + ")");
				emit ClientsChanged(index);
				break;
			}
			case DATA:
			{
				QDataStream stream(&inMsg->payload, QIODevice::ReadOnly);
				qint32 line;
				qint32 lineLength;
				QByteArray lineData;

				QList<QByteArray> receivedRenderedLines;
				QList<int> receivedLineNumbers;

				while(!stream.atEnd())
				{
					stream >> line;
					stream >> lineLength;
					lineData.resize(lineLength);
					stream.readRawData(lineData.data(), lineData.size());
					receivedLineNumbers.append(line);
					receivedRenderedLines.append(lineData);
				}

				emit NewLinesArrived(receivedLineNumbers, receivedRenderedLines);
				break;
			}
			case STATUS:
			{
				clients[index].status = (netRenderStatus)*(qint32*)inMsg->payload.data();
				emit ClientsChanged(index);
				break;
			}

			default:
				break;
			}
		}
		else
		{
			qWarning() << "NetRender - client unknown, address: " + socket->peerAddress().toString();
		}
	}
	ResetMessage(inMsg);
}

//send rendered lines
void CNetRender::SendRenderedLines(QList<int> lineNumbers, QList<QByteArray> lines)
{
	sMessage msg;
	msg.command = DATA;
	QDataStream stream(&msg.payload, QIODevice::WriteOnly);
	for(int i = 0; i < lineNumbers.size(); i++)
	{
		stream << (qint32) lineNumbers.at(i);
		stream << (qint32) lines.at(i).size();
		stream.writeRawData(lines.at(i).data(), lines.at(i).size());
	}
	SendData(clientSocket, msg);
}

// stop rendering of all clients
void CNetRender::Stop()
{
	sMessage msg;
	msg.command = STOP;
	for(int i = 0; i < clients.size(); i++)
	{
		SendData(clients[i].socket, msg);
	}
}

// get status of all clients
void CNetRender::GetStatus()
{
	sMessage msg;
	msg.command = STATUS;
	for(int i = 0; i < clients.size(); i++)
	{
		SendData(clients[i].socket, msg);
	}
}

void CNetRender::SendJob(cParameterContainer settings, cFractalContainer fractal, sTextures textures)
{
	WriteLog("NetRender - Sending job");
	cSettings settingsData(cSettings::formatCondensedText);
	size_t dataSize = settingsData.CreateText(&settings, &fractal);
	if(dataSize > 0)
	{
		QString settingsText = settingsData.GetSettingsText();
		sMessage msg;
		msg.command = JOB;
		QDataStream stream(&msg.payload, QIODevice::WriteOnly);

		// write settings
		stream << (qint32)settingsText.toUtf8().size();
		stream.writeRawData(settingsText.toUtf8().data(), settingsText.toUtf8().size());

		// write textures (from files)
		QList< cTexture* > textureList;
		textureList.append(textures.backgroundTexture);
		textureList.append(textures.envmapTexture);
		textureList.append(textures.lightmapTexture);
		for(int i = 0; i < textureList.size(); i++)
		{
			if(textureList[i] != NULL)
			{
				QFile file(textureList[i]->GetFileName());
				if (file.open(QIODevice::ReadOnly))
				{
					QByteArray buffer = file.readAll();
					stream << (qint32)buffer.size();
					stream.writeRawData(buffer.data(), buffer.size());
					continue;
				}
			}
			stream << (qint32) 0; // empty entry
		}

		for(int i = 0; i < clients.size(); i++)
		{
			SendData(clients[i].socket, msg);
		}
	}
}

void CNetRender::NotifyStatus()
{
	if(clientSocket != NULL)
	{
		sMessage outMsg;
		outMsg.command = STATUS;
		outMsg.payload.append((char*)&status, sizeof(qint32));
		SendData(clientSocket, outMsg);
	}
	emit NewStatusClient();
}

void CNetRender::SendToDoList(int clientIndex, QList<int> done)
{
	if(clientIndex < clients.size())
	{
		sMessage msg;
		msg.command = RENDER;
		QDataStream stream(&msg.payload, QIODevice::WriteOnly);
		stream << (qint32)done.size();
		for(int i = 0; i < done.size(); i++)
		{
			stream << (qint32)done.at(i);
		}
		SendData(clients[clientIndex].socket, msg);
	}
	else
	{
		qCritical() << "CNetRender::SendToDoList(int clientIndex, QList<int> done, QList<int> startPositions): Client index out of range:" << clientIndex;
	}
}

void CNetRender::StopAll()
{
	Stop();
}

void CNetRender::SendSetup(int clientIndex, int id, QList<int> startingPositions)
{
	WriteLog("NetRender - send setup to clients");
	if (clientIndex < clients.size())
	{
		sMessage msg;
		msg.command = SETUP;
		QDataStream stream(&msg.payload, QIODevice::WriteOnly);
		stream << (qint32) id;
		stream << (qint32) startingPositions.size();
		for (int i = 0; i < startingPositions.size(); i++)
		{
			stream << (qint32) startingPositions.at(i);
		}
		SendData(clients[clientIndex].socket, msg);
		actualId = id;
	}
	else
	{
		qCritical() << "CNetRender::SendToDoList(int clientIndex, QList<int> done, QList<int> startPositions): Client index out of range:" << clientIndex;
	}
}

