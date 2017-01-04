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

#include "netrender.hpp"
#include "error_message.hpp"
#include "fractal_container.hpp"
#include "global_data.hpp"
#include "headless.h"
#include "initparameters.hpp"
#include "interface.hpp"
#include "settings.hpp"
#include "system.hpp"
#include <QAbstractSocket>
#include <QHostInfo>
#include "render_window.hpp"
#include "texture.hpp"

CNetRender *gNetRender = nullptr;

CNetRender::CNetRender(qint32 workerCount) : QObject(nullptr)
{
	this->workerCount = workerCount;
	deviceType = netRender_UNKNOWN;
	version = 1000 * MANDELBULBER_VERSION;
	clientSocket = nullptr;
	server = nullptr;
	portNo = 0;
	status = netRender_NEW;
	reconnectTimer = nullptr;
	actualId = 0;
	totalReceivedUncompressed = 0;
	totalReceived = 0;
	isUsed = false;
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
	ResetMessage(&msgCurrentJob);
	server = new QTcpServer(this);

	WriteLog("NetRender - starting server.", 2);
	if (!server->listen(QHostAddress::Any, portNo))
	{
		if (server->serverError() == QAbstractSocket::AddressInUseError)
		{
			cErrorMessage::showMessage(
				QObject::tr("NetRender - address already in use.\n\nIs there already a mandelbulber server "
										"instance running on this port?"),
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
		WriteLog("NetRender - Server Setup on localhost, port: " + QString::number(portNo), 2);

		if (systemData.noGui)
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
	if (deviceType != netRender_SERVER) return;

	deviceType = netRender_UNKNOWN;
	WriteLog("NetRender - Delete Server", 2);
	if (server)
	{
		server->close();
		delete server;
		server = nullptr;
	}
	clients.clear();
	emit ClientsChanged();
	status = netRender_DISABLED;
	emit NewStatusServer();
}

void CNetRender::DeleteClient()
{
	if (deviceType != netRender_CLIENT) return;
	deviceType = netRender_UNKNOWN;
	WriteLog("NetRender - Delete Client", 2);
	if (reconnectTimer)
	{
		if (reconnectTimer->isActive()) reconnectTimer->stop();
		delete reconnectTimer;
		reconnectTimer = nullptr;
	}
	if (clientSocket)
	{
		clientSocket->close();
		delete clientSocket;
		clientSocket = nullptr;
	}
	status = netRender_DISABLED;
	emit NotifyStatus();
}

int CNetRender::getTotalWorkerCount()
{
	int totalCount = 0;
	for (int i = 0; i < clients.count(); i++)
	{
		totalCount += clients[i].clientWorkerCount;
	}
	return totalCount;
}

void CNetRender::HandleNewConnection()
{
	while (server->hasPendingConnections())
	{
		WriteLog("NetRender - new client connected", 2);
		// push new socket to list
		sClient client;
		client.socket = server->nextPendingConnection();
		clients.append(client);

		connect(client.socket, SIGNAL(disconnected()), this, SLOT(ClientDisconnected()));
		connect(client.socket, SIGNAL(readyRead()), this, SLOT(ReceiveFromClient()));

		// tell mandelbulber version to client
		sMessage msg;
		msg.command = netRender_VERSION;
		msg.payload.append(reinterpret_cast<char *>(&version), sizeof(qint32));
		SendData(client.socket, msg);
		emit ClientsChanged();
	}
}

void CNetRender::ClientDisconnected()
{
	// get client by signal emitter
	QTcpSocket *socket = qobject_cast<QTcpSocket *>(sender());
	if (socket)
	{
		int index = GetClientIndexFromSocket(socket);
		WriteLog("NetRender - Client disconnected #" + QString::number(index) + " ("
							 + socket->peerAddress().toString() + ")",
			2);
		if (index > -1)
		{
			clients.removeAt(index);
		}
		socket->close();
		socket->deleteLater();
		emit ClientsChanged();
	}
}

int CNetRender::GetClientIndexFromSocket(const QTcpSocket *socket) const
{
	for (int i = 0; i < clients.size(); i++)
	{
		if (clients.at(i).socket->socketDescriptor() == socket->socketDescriptor())
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
	if (index != -1)
	{
		WriteLog("NetRender - ReceiveFromClient()", 3);
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
	QTimer::singleShot(50, this, SLOT(TryServerConnect()));
	WriteLog(
		"NetRender - Client Setup, link to server: " + address + ", port: " + QString::number(portNo),
		2);
	emit NotifyStatus();

	if (systemData.noGui)
	{
		QTextStream out(stdout);
		out << "NetRender - Client Setup, link to server: " + address + ", port: "
						 + QString::number(portNo) + "\n";
	}
}

void CNetRender::ServerDisconnected()
{
	if (deviceType != netRender_CLIENT) return;
	status = netRender_ERROR;
	emit NotifyStatus();

	gMainInterface->stopRequest = true;

	reconnectTimer->start();

	WriteLog("NetRender - server disconnected", 2);

	if (systemData.noGui)
	{
		QTextStream out(stdout);
		out << QString("Connection lost") + "\n";
		out.flush();
	}
	else
	{
		cErrorMessage::showMessage(QString("Connection lost"), cErrorMessage::errorMessage);
	}
}

void CNetRender::TryServerConnect()
{
	if (clientSocket)
	{
		WriteLog("NetRender - TryServerConnect", 3);
		if (clientSocket->state() == QAbstractSocket::ConnectedState)
		{
			reconnectTimer->stop();
		}
		else if (clientSocket->state() == QAbstractSocket::ConnectingState)
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
	WriteLog("NetRender - ReceiveFromServer()", 3);
	ReceiveData(clientSocket, &msgFromServer);
}

bool CNetRender::SendData(QTcpSocket *socket, sMessage msg) const
{
	//			NetRender Message format         (optional)
	// | qint32		| qint32	| qint32	|( 0 - ???	| quint16  |)
	// | command	| id			| size		|( payload	| checksum |)

	if (!socket) return false;
	if (socket->state() != QAbstractSocket::ConnectedState) return false;

	QByteArray byteArray;
	QDataStream socketWriteStream(&byteArray, QIODevice::ReadWrite);

	msg.size = msg.payload.size();
	msg.id = actualId;

	WriteLog(QString("NetRender - send data, command %1, bytes %2, id %3")
						 .arg(msg.command)
						 .arg(msg.size)
						 .arg(msg.id),
		3);

	// append header
	socketWriteStream << msg.command << msg.id << msg.size;

	// append payload
	if (msg.size > 0)
	{
		socketWriteStream.writeRawData(msg.payload.data(), msg.size);
		// append checksum
		qint16 checksum = qChecksum(msg.payload.data(), msg.size);
		socketWriteStream << checksum;
	}

	// write to socket
	socket->write(byteArray);
	// socket->waitForBytesWritten();

	return true;
}

void CNetRender::ResetMessage(sMessage *msg)
{
	if (msg == nullptr)
	{
		qCritical() << "CNetRender::ResetMessage(sMessage *msg): message is nullptr!";
	}
	else
	{
		msg->command = netRender_NONE;
		msg->id = 0;
		msg->size = 0;
		if (!msg->payload.isEmpty()) msg->payload.clear();
	}
}

void CNetRender::ReceiveData(QTcpSocket *socket, sMessage *msg)
{
	QDataStream socketReadStream(socket);
	qint64 bytesAvailable = socket->bytesAvailable();

	while (bytesAvailable > 0)
	{
		if (msg->command == netRender_NONE)
		{
			if (socket->bytesAvailable()
					< qint64((sizeof(msg->command) + sizeof(msg->id) + sizeof(msg->size))))
			{
				return;
			}
			// meta data available
			socketReadStream >> msg->command;
			socketReadStream >> msg->id;
			socketReadStream >> msg->size;
			WriteLog(QString("NetRender - ReceiveData(), command %1, bytes %2, id %3")
								 .arg(msg->command)
								 .arg(msg->size)
								 .arg(msg->id),
				3);
		}

		bytesAvailable = socket->bytesAvailable();

		if (msg->size > 0)
		{
			if (bytesAvailable < qint64(sizeof(quint16) + msg->size))
			{
				return;
			}
			// full payload available
			char *buffer = new char[msg->size];
			socketReadStream.readRawData(buffer, msg->size);
			msg->payload.append(buffer, msg->size);

			quint16 crcCalculated = qChecksum(buffer, msg->size);
			quint16 crcReceived;
			socketReadStream >> crcReceived;

			delete[] buffer;
			if (crcCalculated != crcReceived)
			{
				// ResetMessage(msg);
				// socketReadStream.atEnd();
				// socketReadStream.skipRawData(socket->bytesAvailable());
				WriteLog("NetRender - ReceiveData() : crc error", 2);

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
	if (IsClient())
	{
		switch (netCommand(inMsg->command))
		{
			case netRender_VERSION:
			{
				sMessage outMsg;
				qint32 serverVersion = *reinterpret_cast<qint32 *>(inMsg->payload.data());

				if (CompareMajorVersion(serverVersion, version))
				{
					WriteLog("NetRender - version matches (" + QString::number(version)
										 + "), connection established",
						2);

					if (systemData.noGui)
					{
						QTextStream out(stdout);
						out << "NetRender - version matches (" + QString::number(version)
										 + "), connection established\n";
					}

					// server version matches, send worker count
					outMsg.command = netRender_WORKER;
					QDataStream stream(&outMsg.payload, QIODevice::WriteOnly);
					stream << workerCount;
					QString machineName = QHostInfo::localHostName();
					stream << qint32(machineName.toUtf8().size());
					stream.writeRawData(machineName.toUtf8().data(), machineName.toUtf8().size());
					status = netRender_READY;
					emit NewStatusClient();
					WriteLog(
						QString("NetRender - ProcessData(), command VERSION, version %1").arg(serverVersion),
						2);
				}
				else
				{
					cErrorMessage::showMessage(tr("NetRender - version mismatch!\n")
																			 + tr("Client version: %1\n").arg(version)
																			 + tr("Server version: %1").arg(serverVersion),
						cErrorMessage::errorMessage, gMainInterface->mainWindow);

					outMsg.command = netRender_BAD;
				}
				SendData(clientSocket, outMsg);
				break;
			}
			case netRender_STOP:
			{
				// status = netRender_READY;
				gMainInterface->stopRequest = true;
				// emit NotifyStatus();
				WriteLog("NetRender - ProcessData(), command STOP", 2);
				break;
			}
			case netRender_STATUS:
			{
				WriteLog("NetRender - ProcessData(), command STATUS", 3);
				emit NotifyStatus();
				break;
			}
			case netRender_JOB:
			{
				if (inMsg->id == actualId)
				{
					WriteLog("NetRender - ProcessData(), command JOB", 2);
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
					WriteLog(
						QString("NetRender - ProcessData(), command JOB, settings size: %1").arg(size), 2);
					WriteLog(
						QString("NetRender - ProcessData(), command JOB, settings: %1").arg(settingsText), 3);

					qint32 numberOfTextures;
					stream >> numberOfTextures;

					WriteLog(QString("NetRender - ProcessData(), command JOB, number of textures: %1")
										 .arg(numberOfTextures),
						2);

					// read textures
					for (int i = 0; i < numberOfTextures; i++)
					{
						qint32 sizeOfName;
						stream >> sizeOfName;

						QString textureName;
						if (sizeOfName > 0)
						{
							QByteArray bufferForName;
							bufferForName.resize(sizeOfName);
							stream.readRawData(bufferForName.data(), sizeOfName);
							textureName = QString::fromUtf8(bufferForName);
							WriteLog(QString("NetRender - ProcessData(), command JOB, texture name: %1")
												 .arg(textureName),
								2);
						}

						cTexture texture;
						stream >> size;
						WriteLog(
							QString("NetRender - ProcessData(), command JOB, texture size: %1").arg(size), 2);
						if (size > 0)
						{
							buffer.resize(size);
							stream.readRawData(buffer.data(), size);
						}
						textures.insert(textureName, buffer);
					}

					cSettings parSettings(cSettings::formatCondensedText);
					parSettings.BeQuiet(true);
					parSettings.LoadFromString(settingsText);
					parSettings.Decode(gPar, gParFractal);

					WriteLog("NetRender - ProcessData(), command JOB, starting rendering", 2);

					if (!systemData.noGui)
					{
						gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
						gMainInterface->StartRender(true);
					}
					else
					{
						// in noGui mode it must be started as separate thread to be able to process event loop
						gMainInterface->headless = new cHeadless;

						QThread *thread = new QThread; // deleted by deleteLater()
						gMainInterface->headless->moveToThread(thread);
						QObject::connect(
							thread, SIGNAL(started()), gMainInterface->headless, SLOT(slotNetRender()));
						thread->setObjectName("RenderJob");
						thread->start();

						QObject::connect(gMainInterface->headless, SIGNAL(finished()), gMainInterface->headless,
							SLOT(deleteLater()));
						QObject::connect(gMainInterface->headless, SIGNAL(finished()), thread, SLOT(quit()));
						QObject::connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
					}
				}
				else
				{
					WriteLog("NetRender - received JOB message with wrong id", 1);
				}
				break;
			}
			case netRender_RENDER:
			{
				WriteLog("NetRender - ProcessData(), command RENDER", 3);
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
					WriteLog(QString("NetRender - ProcessData(), command RENDER, done list size: %1")
										 .arg(done.size()),
						3);
					emit ToDoListArrived(done);
				}
				else
				{
					WriteLog("NetRender - received RENDER message with wrong id", 1);
				}
				break;
			}

			case netRender_SETUP:
			{
				WriteLog("NetRender - ProcessData(), command SETUP", 2);
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
					WriteLog(QString("NetRender - ProcessData(), command SETUP, start line %1 = %2")
										 .arg(i)
										 .arg(line),
						3);
				}
				break;
			}

			case netRender_ACK:
			{
				WriteLog("NetRender - ProcessData(), command ACK", 3);
				if (inMsg->id == actualId)
				{
					emit AckReceived();
				}
				break;
			}

			default: break;
		}
	}

	//----------------------------- SERVER ----------------------
	else if (IsServer())
	{
		int index = GetClientIndexFromSocket(socket);
		if (index > -1)
		{
			switch (netCommand(inMsg->command))
			{
				case netRender_BAD:
				{

					cErrorMessage::showMessage(
						QObject::tr("NetRender - Client version mismatch!\n Client address:")
							+ socket->peerAddress().toString(),
						cErrorMessage::errorMessage, gMainInterface->mainWindow);

					clients.removeAt(index);
					emit ClientsChanged();
					return; // to avoid reseting already deleted message buffer
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

					if (clients[index].status == netRender_NEW) clients[index].status = netRender_READY;
					WriteLog("NetRender - new Client #" + QString::number(index) + "(" + clients[index].name
										 + " - " + clients[index].socket->peerAddress().toString() + ")",
						1);
					emit ClientsChanged(index);

					if (systemData.noGui)
					{
						QTextStream out(stdout);
						out << "NetRender - Client connected: Name: " + clients[index].name;
						out << " IP: " + clients[index].socket->peerAddress().toString();
						out << " CPUs: " + QString::number(clients[index].clientWorkerCount) + "\n";
					}

					// when the client connects while a render is in progress, send the current job to the
					// client
					if (msgCurrentJob.command != netRender_NONE)
					{
						SendData(clients[index].socket, msgCurrentJob);
						clients[index].linesRendered = 0;
					}
					break;
				}
				case netRender_DATA:
				{
					WriteLog("NetRender - ProcessData(), command DATA", 3);
					if (inMsg->id == actualId)
					{
						QDataStream stream(&inMsg->payload, QIODevice::ReadOnly);
						qint32 line;
						qint32 lineLength;
						QByteArray lineData;

						QList<QByteArray> receivedRenderedLines;
						QList<int> receivedLineNumbers;

						while (!stream.atEnd())
						{
							stream >> line;
							stream >> lineLength;
							lineData.resize(lineLength);
							stream.readRawData(lineData.data(), lineData.size());
							receivedLineNumbers.append(line);
							receivedRenderedLines.append(lineData);
							WriteLog(
								QString("NetRender - ProcessData(), command DATA, line %1, lineDataLength %2")
									.arg(line)
									.arg(lineLength),
								3);
						}
						clients[index].linesRendered += receivedLineNumbers.size();
						emit NewLinesArrived(receivedLineNumbers, receivedRenderedLines);

						// send acknowledge
						sMessage outMsg;
						outMsg.id = actualId;
						outMsg.command = netRender_ACK;
						SendData(clients[index].socket, outMsg);
					}
					else
					{
						WriteLog("NetRender - received DATA message with wrong id", 1);
					}
					break;
				}
				case netRender_STATUS:
				{
					WriteLog("NetRender - ProcessData(), command STATUS", 3);
					clients[index].status =
						netRenderStatus(*reinterpret_cast<qint32 *>(inMsg->payload.data()));
					emit ClientsChanged(index);
					break;
				}

				default: break;
			}
		}
		else
		{
			qWarning() << "NetRender - client unknown, address: " + socket->peerAddress().toString();
		}
	}
	ResetMessage(inMsg);
}

// send rendered lines
void CNetRender::SendRenderedLines(QList<int> lineNumbers, QList<QByteArray> lines) const
{
	sMessage msg;
	msg.command = netRender_DATA;
	QDataStream stream(&msg.payload, QIODevice::WriteOnly);
	for (int i = 0; i < lineNumbers.size(); i++)
	{
		stream << qint32(lineNumbers.at(i));
		stream << qint32(lines.at(i).size());
		stream.writeRawData(lines.at(i).data(), lines.at(i).size());
	}
	SendData(clientSocket, msg);
}

// stop rendering of all clients
void CNetRender::Stop()
{
	sMessage msg;
	msg.command = netRender_STOP;
	for (int i = 0; i < clients.size(); i++)
	{
		SendData(clients[i].socket, msg);
	}
}

void CNetRender::SetCurrentJob(
	cParameterContainer settings, cFractalContainer fractal, QStringList listOfTextures)
{
	WriteLog("NetRender - Sending job", 2);
	cSettings settingsData(cSettings::formatNetRender);
	size_t dataSize = settingsData.CreateText(&settings, &fractal);
	if (dataSize > 0)
	{
		QString settingsText = settingsData.GetSettingsText();
		msgCurrentJob.command = netRender_JOB;
		QDataStream stream(&msgCurrentJob.payload, QIODevice::WriteOnly);

		// write settings
		stream << qint32(settingsText.toUtf8().size());
		stream.writeRawData(settingsText.toUtf8().data(), settingsText.toUtf8().size());

		// send number of textures
		stream << qint32(listOfTextures.size());

		// write textures (from files)
		for (int i = 0; i < listOfTextures.size(); i++)
		{
			// send length of texture name
			stream << qint32(listOfTextures[i].toUtf8().size());

			// send texture name
			stream.writeRawData(listOfTextures[i].toUtf8(), listOfTextures[i].length());

			QFile file(listOfTextures[i]);
			if (file.open(QIODevice::ReadOnly))
			{
				QByteArray buffer = file.readAll();
				// send size of file
				stream << qint32(buffer.size());
				// send file content
				stream.writeRawData(buffer.data(), buffer.size());
				continue;
			}

			stream << qint32(0); // empty entry
		}

		for (int i = 0; i < clients.size(); i++)
		{
			SendData(clients[i].socket, msgCurrentJob);
			clients[i].linesRendered = 0;
		}
	}
}

void CNetRender::NotifyStatus()
{
	if (clientSocket != nullptr)
	{
		sMessage outMsg;
		outMsg.command = netRender_STATUS;
		outMsg.payload.append(reinterpret_cast<char *>(&status), sizeof(qint32));
		SendData(clientSocket, outMsg);
	}
	emit NewStatusClient();
}

void CNetRender::SendToDoList(int clientIndex, QList<int> done)
{
	if (clientIndex < clients.size())
	{
		sMessage msg;
		msg.command = netRender_RENDER;
		QDataStream stream(&msg.payload, QIODevice::WriteOnly);
		stream << qint32(done.size());
		for (int i = 0; i < done.size(); i++)
		{
			stream << qint32(done.at(i));
		}
		SendData(clients[clientIndex].socket, msg);
	}
	else
	{
		qCritical() << "CNetRender::SendToDoList(int clientIndex, QList<int> done, QList<int> "
									 "startPositions): Client index out of range:"
								<< clientIndex;
	}
}

void CNetRender::StopAll()
{
	Stop();
}

void CNetRender::SendSetup(int clientIndex, int id, QList<int> startingPositions)
{
	WriteLog("NetRender - send setup to clients", 2);
	if (clientIndex < clients.size())
	{
		sMessage msg;
		msg.command = netRender_SETUP;
		QDataStream stream(&msg.payload, QIODevice::WriteOnly);
		stream << qint32(id);
		stream << qint32(startingPositions.size());
		for (int i = 0; i < startingPositions.size(); i++)
		{
			stream << qint32(startingPositions.at(i));
		}
		SendData(clients[clientIndex].socket, msg);
		actualId = id;
	}
	else
	{
		qCritical() << "CNetRender::SendSetup(int clientIndex, int id, QList<int> startingPositions): "
									 "Client index out of range:"
								<< clientIndex;
	}
}

QString CNetRender::GetStatusText(netRenderStatus displayStatus)
{
	switch (displayStatus)
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
	switch (displayStatus)
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

const CNetRender::sClient &CNetRender::GetClient(int index)
{
	if (index >= 0 && index < GetClientCount())
	{
		return clients.at(index);
	}
	else
	{
		qCritical() << "CNetRender::sClient& CNetRender::getClient(int index): client " << index
								<< " doesn't exist";
		return nullClient;
	}
}

CNetRender::netRenderStatus CNetRender::GetClientStatus(int index)
{
	if (index < GetClientCount())
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
	while (timer.elapsed() < timeout * 1000)
	{
		bool allReady = true;
		for (int i = 0; i < GetClientCount(); i++)
		{
			if (GetClientStatus(i) != netRender_READY)
			{
				allReady = false;
				WriteLog(QString("Client # %1 is not ready yet").arg(i), 1);
				break;
			}
		}

		if (allReady)
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

bool CNetRender::Block()
{
	if (isUsed)
	{
		return false;
	}
	else
	{
		isUsed = true;
		return true;
	}
}

bool CNetRender::CompareMajorVersion(qint32 version1, qint32 version2)
{
	qint32 majorVersion1 = version1 / 10;
	qint32 majorVersion2 = version2 / 10;
	return majorVersion1 == majorVersion2;
}

QByteArray *CNetRender::GetTexture(QString textureName)
{
	return &textures[textureName];
}
