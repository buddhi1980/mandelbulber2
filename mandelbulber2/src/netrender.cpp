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
#include "error_message.hpp"
#include "headless.h"

CNetRender::CNetRender(qint32 workerCount) : QObject(NULL)
{
	this->workerCount = workerCount;
	deviceType = netRender_UNKNOWN;
	version = 1000 * MANDELBULBER_VERSION;
	clientSocket = NULL;
	server = NULL;
	portNo = 0;
	status = netRender_NEW;
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
			cErrorMessage::showMessage(
				QObject::tr("NetRender - address already in use.\n\nIs there already a mandelbulber server instance running on this port?"),
				cErrorMessage::errorMessage, gMainInterface->mainWindow);
		}
		else
		{
			cErrorMessage::showMessage(
				QObject::tr("NetRender - SetServer Error:\n\n") + server->errorString(),
				cErrorMessage::errorMessage, gMainInterface->mainWindow);
		}
		deviceType = netRender_UNKNOWN;
	}
	else
	{
		connect(server, SIGNAL(newConnection()), this, SLOT(HandleNewConnection()));
		deviceType = netRender_SERVER;
		WriteLog("NetRender - Server Setup on localhost, port: " + QString::number(portNo));

		if(systemData.noGui)
		{
			QTextStream out(stdout);
			out << "NetRender - Server Setup on localhost, port: " + QString::number(portNo) + "\n";
		}

		status = netRender_NEW;
		emit NewStatusServer();
	}
}

void CNetRender::DeleteServer()
{
	if(deviceType != netRender_SERVER) return;

	deviceType = netRender_UNKNOWN;
	WriteLog("NetRender - Delete Server");
	if(server)
	{
		server->close();
		delete server;
		server = NULL;
	}
	clients.clear();
	status = netRender_DISABLED;
	emit NewStatusServer();
}

void CNetRender::DeleteClient()
{
	if (deviceType != netRender_CLIENT) return;
	deviceType = netRender_UNKNOWN;
	WriteLog("NetRender - Delete Client");
	if(reconnectTimer->isActive()) reconnectTimer->stop();
	if(clientSocket)
	{
		clientSocket->close();
		delete clientSocket;
		clientSocket = NULL;
	}
	status = netRender_DISABLED;
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
		msg.command = netRender_VERSION;
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
	deviceType = netRender_CLIENT;
	status = netRender_NEW;
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

	if(systemData.noGui)
	{
		QTextStream out(stdout);
		out << "NetRender - Client Setup, link to server: " + address + ", port: " + QString::number(portNo) + "\n";
	}
}

void CNetRender::ServerDisconnected()
{
	status = netRender_ERROR;
	emit NotifyStatus();

	gMainInterface->stopRequest = true;

	reconnectTimer->start();

	if(systemData.noGui)
	{
		QTextStream out(stdout);
		cErrorMessage::showMessage(QString("Connection lost"), cErrorMessage::errorMessage);
	}
}

void CNetRender::TryServerConnect()
{
	if(clientSocket)
	{
		if(clientSocket->state() == QAbstractSocket::ConnectedState)
		{
			reconnectTimer->stop();
		}
		else if(clientSocket->state() == QAbstractSocket::ConnectingState)
		{
			return; // wait for result
		}
		else
		{
			status = netRender_CONNECTING;
			emit NotifyStatus();
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
	//socket->waitForBytesWritten();

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
		msg->command = netRender_NONE;
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
		if (msg->command == netRender_NONE)
		{
			if (socket->bytesAvailable() < (qint64)((sizeof(msg->command) + sizeof(msg->id) + sizeof(msg->size))))
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
			if (bytesAvailable < (qint64)(sizeof(quint16) + msg->size))
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
		case netRender_VERSION:
		{
			sMessage outMsg;
			if(*(qint32*)inMsg->payload.data() == version)
			{
				WriteLog("NetRender - version matches (" + QString::number(version) + "), connection established");

				if(systemData.noGui)
				{
					QTextStream out(stdout);
					out << "NetRender - version matches (" + QString::number(version) + "), connection established\n";
				}

				// server version matches, send worker count
				outMsg.command = netRender_WORKER;
				QDataStream stream(&outMsg.payload, QIODevice::WriteOnly);
				stream << workerCount;
				QString machineName = QHostInfo::localHostName();
				stream << (qint32)machineName.toUtf8().size();
				stream.writeRawData(machineName.toUtf8().data(), machineName.toUtf8().size());
				status = netRender_READY;
				emit NewStatusClient();
			}
			else
			{
				qWarning() << "CNetRender - version mismatch! client version: " << version << ", server: " << *(qint32*)inMsg->payload.data();
				outMsg.command = netRender_BAD;
			}
			SendData(clientSocket, outMsg);
			break;
			}
			case netRender_STOP:
			{
				status = netRender_READY;
				gMainInterface->stopRequest = true;
				emit NotifyStatus();
				WriteLog("CNetRender - STOP");
				break;
			}
			case netRender_STATUS:
			{
				emit NotifyStatus();
				break;
			}
			case netRender_JOB:
			{
				if (inMsg->id == actualId)
				{
					WriteLog("NetRender - received new job");
					QDataStream stream(&inMsg->payload, QIODevice::ReadOnly);
					QByteArray buffer;
					qint32 size;
					status = netRender_WORKING;
					emit NotifyStatus();

					// read settings
					stream >> size;
					buffer.resize(size);
					stream.readRawData(buffer.data(), size);
					settingsText = QString::fromUtf8(buffer.data(), buffer.size());

					// read textures
					for (int i = 0; i < textures.textureList.size(); i++)
					{
						stream >> size;
						if (size > 0)
						{
							buffer.resize(size);
							stream.readRawData(buffer.data(), size);
							textures.textureList[i]->FromQByteArray(buffer);
						}
					}

					cSettings parSettings(cSettings::formatCondensedText);
					parSettings.BeQuiet(true);
					parSettings.LoadFromString(settingsText);
					parSettings.Decode(gPar, gParFractal);

					if(!systemData.noGui)
					{
						gMainInterface->SynchronizeInterface(gPar, gParFractal, cInterface::write);
						gMainInterface->StartRender();
					}
					else
					{
						//in noGui mode it must be started as separate thread to be able to process event loop
						cHeadless *headless = new cHeadless;

						QThread *thread = new QThread; //deleted by deleteLater()
						headless->moveToThread(thread);
						QObject::connect(thread, SIGNAL(started()), headless, SLOT(slotNetRender()));
						thread->setObjectName("RenderJob");
						thread->start();

						QObject::connect(headless, SIGNAL(finished()), headless, SLOT(deleteLater()));
						QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
					}

				}
				else
				{
					WriteLog("NetRender - received JOB message with wrong id");
				}
				break;
			}
			case netRender_RENDER:
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

			case netRender_SETUP:
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

			case netRender_ACK:
			{
				if (inMsg->id == actualId)
				{
					emit AckReceived();
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
			case netRender_BAD:
			{
				clients.removeAt(index);
				emit ClientsChanged();
				break;
			}
			case netRender_WORKER:
			{
				QDataStream stream(&inMsg->payload, QIODevice::ReadOnly);
				stream >> clients[index].clientWorkerCount;
				QByteArray buffer;
				qint32 size;
				stream >> size;
				buffer.resize(size);
				stream.readRawData(buffer.data(), size);
				clients[index].name = QString::fromUtf8(buffer.data(), buffer.size());

				if(clients[index].status == netRender_NEW) clients[index].status = netRender_READY;
				WriteLog("NetRender - new Client #" + QString::number(index) + "(" + clients[index].name + " - " + clients[index].socket->peerAddress().toString() + ")");
				emit ClientsChanged(index);

				if(systemData.noGui)
				{
					QTextStream out(stdout);
					out << "NetRender - Client connected: Name: " + clients[index].name;
					out << " IP: " + clients[index].socket->peerAddress().toString();
					out << " CPUs: " + QString::number(clients[index].clientWorkerCount) +  "\n";
				}

				break;
			}
			case netRender_DATA:
			{
				if (inMsg->id == actualId)
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
					clients[index].linesRendered += receivedLineNumbers.size();
					emit NewLinesArrived(receivedLineNumbers, receivedRenderedLines);

					//send acknowledge
					sMessage outMsg;
					outMsg.id = actualId;
					outMsg.command = netRender_ACK;
					SendData(clients[index].socket, outMsg);
				}
				else
				{
					WriteLog("NetRender - received DATA message with wrong id");
				}
				break;
			}
			case netRender_STATUS:
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
	msg.command = netRender_DATA;
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
	msg.command = netRender_STOP;
	for(int i = 0; i < clients.size(); i++)
	{
		SendData(clients[i].socket, msg);
	}
}

void CNetRender::SendJob(cParameterContainer settings, cFractalContainer fractal, sTextures textures)
{
	WriteLog("NetRender - Sending job");
	cSettings settingsData(cSettings::formatNetRender);
	size_t dataSize = settingsData.CreateText(&settings, &fractal);
	if(dataSize > 0)
	{
		QString settingsText = settingsData.GetSettingsText();
		sMessage msg;
		msg.command = netRender_JOB;
		QDataStream stream(&msg.payload, QIODevice::WriteOnly);

		// write settings
		stream << (qint32)settingsText.toUtf8().size();
		stream.writeRawData(settingsText.toUtf8().data(), settingsText.toUtf8().size());

		// write textures (from files)
		for(int i = 0; i < textures.textureList.size(); i++)
		{
			if(textures.textureList[i]->IsLoaded())
			{
				QFile file(textures.textureList[i]->GetFileName());
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
			clients[i].linesRendered = 0;
		}
	}
}

void CNetRender::NotifyStatus()
{
	if(clientSocket != NULL)
	{
		sMessage outMsg;
		outMsg.command = netRender_STATUS;
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
		msg.command = netRender_RENDER;
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
		msg.command = netRender_SETUP;
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
		qCritical() << "CNetRender::SendSetup(int clientIndex, int id, QList<int> startingPositions): Client index out of range:" << clientIndex;
	}
}

QString CNetRender::GetStatusText(netRenderStatus displayStatus)
{
	switch(displayStatus)
	{
		case CNetRender::netRender_DISABLED: return tr("DISABLED");
		case CNetRender::netRender_READY: return tr("READY");
		case CNetRender::netRender_WORKING: return tr("WORKING");
		case CNetRender::netRender_NEW: return tr("NEW");
		case CNetRender::netRender_CONNECTING: return tr("(RE-)CONNECTING");
		case CNetRender::netRender_ERROR: return tr("ERROR");
	}
	return tr("UNKNOWN");
}

QString CNetRender::GetStatusColor(netRenderStatus displayStatus)
{
	switch(displayStatus)
	{
		case CNetRender::netRender_DISABLED: return "darkgrey";
		case CNetRender::netRender_READY: return "green";
		case CNetRender::netRender_WORKING: return "darkblue";
		case CNetRender::netRender_NEW: return "purple";
		case CNetRender::netRender_CONNECTING: return "orange";
		case CNetRender::netRender_ERROR: return "red";
	}
	return "red";
}

const CNetRender::sClient& CNetRender::GetClient(int index)
{
	if(index >=0 && index < GetClientCount())
	{
		return clients.at(index);
	}
	else
	{
		qCritical() << "CNetRender::sClient& CNetRender::getClient(int index): client " << index << " doesn't exist";
		return nullClient;
	}
}

CNetRender::netRenderStatus CNetRender::GetClientStatus(int index)
{
	if(index < GetClientCount())
	{
		return clients[index].status;
	}
	else
	{
		return netRender_ERROR;
	}
}

bool CNetRender::WaitForAllClientsReady(double timeout)
{
	QElapsedTimer timer;
	timer.start();
	while(timer.elapsed() < timeout * 1000)
	{
		bool allReady = true;
		for(int i=0; i < GetClientCount(); i++)
		{
			if(GetClientStatus(i) != netRender_READY)
			{
				allReady = false;
				WriteLog(QString("Client # %1 is not ready yet").arg(i));
				break;
			}
		}

		if(allReady)
		{
			return true;
		}
		else
		{
			Wait(200);
		}

		gApplication->processEvents();
	}
	return false;
}
