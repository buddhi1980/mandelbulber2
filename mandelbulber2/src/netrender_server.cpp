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

#include "netrender_server.hpp"

#include <QAbstractSocket>
#include <QHostInfo>

#include "error_message.hpp"
#include "fractal_container.hpp"
#include "global_data.hpp"
#include "headless.h"
#include "initparameters.hpp"
#include "interface.hpp"
#include "keyframes.hpp"
#include "render_window.hpp"
#include "settings.hpp"
#include "system.hpp"
#include "texture.hpp"

cNetRenderServer::cNetRenderServer()
{
	actualId = 0;
	server = nullptr;
	portNo = 0;
	connect(this, SIGNAL(NewClient(int)), this, SLOT(SendVersionToClient(int)));
}

cNetRenderServer::~cNetRenderServer()
{
	DeleteServer();
}

void cNetRenderServer::SetServer(int _portNo)
{
	server = new QTcpServer(this);
	portNo = _portNo;
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
	}
	else
	{
		connect(server, SIGNAL(newConnection()), this, SLOT(HandleNewConnection()));
		WriteLog("NetRender - Server Setup on localhost, port: " + QString::number(portNo), 2);

		if (systemData.noGui)
		{
			QTextStream out(stdout);
			out << "NetRender - Server Setup on localhost, port: " + QString::number(portNo) + "\n";
		}

		emit changeServerStatus(netRenderSts_NEW);
	}
}

int cNetRenderServer::getTotalWorkerCount()
{
	int totalCount = 0;
	for (int i = 0; i < clients.count(); i++)
	{
		totalCount += clients[i].clientWorkerCount;
	}
	return totalCount;
}

void cNetRenderServer::DeleteServer()
{
	if (server)
	{
		server->close();
		delete server;
		server = nullptr;
	}
	clients.clear();
	cNetRenderTransport::ResetMessage(&msgCurrentJob);
	emit Deleted();
}

void cNetRenderServer::HandleNewConnection()
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
		emit NewClient(clients.size() - 1);
		emit ClientsChanged();
	}
}

void cNetRenderServer::ClientDisconnected()
{
	// get client by signal emitter
	auto *socket = qobject_cast<QTcpSocket *>(sender());
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

int cNetRenderServer::GetClientIndexFromSocket(const QTcpSocket *socket) const
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

void cNetRenderServer::ReceiveFromClient()
{
	// get client by signal emitter
	auto *socket = qobject_cast<QTcpSocket *>(sender());
	int index = GetClientIndexFromSocket(socket);
	if (index != -1)
	{
		WriteLog("NetRender - ReceiveFromClient()", 3);
		ClientReceive(index);
	}
	else
	{
		qCritical() << "Unknown client for socket ";
	}
}

void cNetRenderServer::ClientReceive(int index)
{
	WriteLog("NetRender - ClientReceive()", 3);

	if (clients.at(index).socket->bytesAvailable() > 0)
	{
		if (cNetRenderTransport::ReceiveData(clients.at(index).socket, &clients[index].msg))
		{
			ProcessData(clients.at(index).socket, &clients[index].msg);
			cNetRenderTransport::ResetMessage(&clients[index].msg);

			// try to get the next message, if available
			ClientReceive(index); // FIXME convert recursion to while()
		}
	}
}

void cNetRenderServer::ProcessData(QTcpSocket *socket, sMessage *inMsg)
{
	int index = GetClientIndexFromSocket(socket);
	if (index > -1)
	{
		switch (netCommandClient(inMsg->command))
		{
			case netRenderCmd_WORKER: ProcessRequestWorker(inMsg, index, socket); break;
			case netRenderCmd_DATA: ProcessRequestData(inMsg, index, socket); break;
			case netRenderCmd_STATUS: ProcessRequestStatus(inMsg, index, socket); break;
			case netRenderCmd_BAD: ProcessRequestBad(inMsg, index, socket); return;
			case netRenderCmd_FRAME_DONE: ProcessRequestFrameDone(inMsg, index, socket); return;
			default:
				qWarning() << "NetRender - command unknown: " + QString::number(inMsg->command);
				break;
		}
	}
	else
	{
		qWarning() << "NetRender - client unknown, address: " + socket->peerAddress().toString();
	}
}

const sClient &cNetRenderServer::GetClient(int index)
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

bool cNetRenderServer::WaitForAllClientsReady(double timeout)
{
	QElapsedTimer timer;
	timer.start();
	while (timer.elapsed() < timeout * 1000)
	{
		bool allReady = true;
		for (int i = 0; i < GetClientCount(); i++)
		{
			if (clients.at(i).status != netRenderSts_READY)
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

		QApplication::processEvents();
	}
	return false;
}

void cNetRenderServer::SetCurrentJob(
	const cParameterContainer &settings, const cFractalContainer &fractal, QStringList listOfTextures)
{
	WriteLog(QString("NetRender - Sending job to %1 client(s)").arg(clients.size()), 2);
	cSettings settingsData(cSettings::formatNetRender);
	size_t dataSize = settingsData.CreateText(&settings, &fractal);
	if (dataSize > 0)
	{
		QString settingsText = settingsData.GetSettingsText();
		msgCurrentJob.command = netRenderCmd_JOB;
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
			else
			{
				qCritical() << "Cannot send texture using NetRender. File:" << listOfTextures[i];
			}

			stream << qint32(0); // empty entry
		}

		for (int i = 0; i < GetClientCount(); i++)
		{
			auto &client = GetClient(i);
			cNetRenderTransport::SendData(client.socket, msgCurrentJob, actualId);
			clients[i].linesRendered = 0;
		}
	}
	else
	{
		qCritical() << "NetRender - Sending job data size is 0";
	}
}

void cNetRenderServer::SetCurrentAnimation(
	const cParameterContainer &settings, const cFractalContainer &fractal, bool isFlight)
{
	WriteLog(QString("NetRender - Sending animation to %1 client(s)").arg(clients.size()), 2);
	cSettings settingsData(cSettings::formatCondensedText);
	size_t dataSize;

	if (isFlight)
	{
		dataSize = settingsData.CreateText(&settings, &fractal, gAnimFrames, nullptr);
	}
	else
	{
		dataSize = settingsData.CreateText(&settings, &fractal, nullptr, gKeyframes);
	}

	if (dataSize > 0)
	{
		QString settingsText = settingsData.GetSettingsText();

		if (isFlight)
			msgCurrentJob.command = netRenderCmd_ANIM_FLIGHT;
		else
			msgCurrentJob.command = netRenderCmd_ANIM_KEY;

		QDataStream stream(&msgCurrentJob.payload, QIODevice::WriteOnly);

		// write settings
		stream << qint32(settingsText.toUtf8().size());
		stream.writeRawData(settingsText.toUtf8().data(), settingsText.toUtf8().size());
	}

	for (int i = 0; i < GetClientCount(); i++)
	{
		auto &client = GetClient(i);
		cNetRenderTransport::SendData(client.socket, msgCurrentJob, actualId);
		clients[i].linesRendered = 0;
	}
}

void cNetRenderServer::ProcessRequestBad(sMessage *inMsg, int index, QTcpSocket *socket)
{
	Q_UNUSED(inMsg);
	cErrorMessage::showMessage(QObject::tr("NetRender - Client version mismatch!\n Client address:")
															 + socket->peerAddress().toString(),
		cErrorMessage::errorMessage, gMainInterface->mainWindow);
	clients.removeAt(index);
	emit ClientsChanged();
	return; // to avoid resetting already deleted message buffer
}

void cNetRenderServer::ProcessRequestFrameDone(sMessage *inMsg, int index, QTcpSocket *socket)
{
	Q_UNUSED(inMsg);
	Q_UNUSED(socket);

	WriteLog("NetRender - ProcessData(), command DATA", 3);
	if (inMsg->id == actualId)
	{
		QDataStream stream(&inMsg->payload, QIODevice::ReadOnly);
		qint32 frameIndex;
		qint32 sizeOfListToDo;

		while (!stream.atEnd())
		{
			stream >> frameIndex;
			stream >> sizeOfListToDo;

			WriteLog(QString("NetRender - ProcessData(), command FRAME_DONE, frame %1, toDoLength %2")
								 .arg(frameIndex)
								 .arg(sizeOfListToDo),
				3);
		}

		emit FinishedFrame(frameIndex, sizeOfListToDo);
	}
	else
	{
		WriteLog("NetRender - received DATA message with wrong id", 1);
	}
}

void cNetRenderServer::ProcessRequestWorker(sMessage *inMsg, int index, QTcpSocket *socket)
{
	Q_UNUSED(inMsg);
	Q_UNUSED(socket);

	QDataStream stream(&inMsg->payload, QIODevice::ReadOnly);
	qint32 clientWorkerCount;
	stream >> clientWorkerCount;
	clients[index].clientWorkerCount = clientWorkerCount;
	QByteArray buffer;
	qint32 size;
	stream >> size;
	buffer.resize(size);
	stream.readRawData(buffer.data(), size);
	clients[index].name = QString::fromUtf8(buffer.data(), buffer.size());

	if (GetClient(index).status == netRenderSts_NEW) clients[index].status = netRenderSts_READY;
	WriteLog("NetRender - new Client #" + QString::number(index) + "(" + GetClient(index).name + " - "
						 + GetClient(index).socket->peerAddress().toString() + ")",
		1);
	emit ClientsChanged(index);

	if (systemData.noGui)
	{
		QTextStream out(stdout);
		out << "NetRender - Client connected: Name: " + GetClient(index).name;
		out << " IP: " + GetClient(index).socket->peerAddress().toString();
		out << " CPUs: " + QString::number(GetClient(index).clientWorkerCount) + "\n";
	}

	// when the client connects while a render is in progress, send the current job to the
	// client
	if (msgCurrentJob.command != netRender_NONE)
	{
		cNetRenderTransport::SendData(GetClient(index).socket, msgCurrentJob, actualId);
		clients[index].linesRendered = 0;
		WriteLog("CNetRender::ProcessData(): Send data at reconnect", 3);
	}
}

void cNetRenderServer::ProcessRequestData(sMessage *inMsg, int index, QTcpSocket *socket)
{
	Q_UNUSED(inMsg);
	Q_UNUSED(socket);

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
			WriteLog(QString("NetRender - ProcessData(), command DATA, line %1, lineDataLength %2")
								 .arg(line)
								 .arg(lineLength),
				3);
		}
		clients[index].linesRendered += receivedLineNumbers.size();
		emit NewLinesArrived(receivedLineNumbers, receivedRenderedLines);

		// send acknowledge
		sMessage outMsg;
		outMsg.id = actualId;
		outMsg.command = netRenderCmd_ACK;
		cNetRenderTransport::SendData(GetClient(index).socket, outMsg, actualId);
	}
	else
	{
		WriteLog("NetRender - received DATA message with wrong id", 1);
	}
}

void cNetRenderServer::ProcessRequestStatus(sMessage *inMsg, int index, QTcpSocket *socket)
{
	Q_UNUSED(inMsg);
	Q_UNUSED(socket);

	WriteLog("NetRender - ProcessData(), command STATUS", 3);
	netRenderStatus clientStatus =
		netRenderStatus(*reinterpret_cast<qint32 *>(inMsg->payload.data()));
	clients[index].status = clientStatus;
	emit ClientsChanged(index);
}

void cNetRenderServer::SendToDoList(int clientIndex, const QList<int> &done)
{
	if (clientIndex < GetClientCount())
	{
		sMessage msg;
		msg.command = netRenderCmd_RENDER;
		QDataStream stream(&msg.payload, QIODevice::WriteOnly);
		stream << qint32(done.size());
		for (int doneFlag : done)
		{
			stream << qint32(doneFlag);
		}
		cNetRenderTransport::SendData(GetClient(clientIndex).socket, msg, actualId);
	}
	else
	{
		qCritical() << "CNetRender::SendToDoList(int clientIndex, QList<int> done, QList<int> "
									 "startPositions): Client index out of range:"
								<< clientIndex;
	}
}

void cNetRenderServer::SendSetup(int clientIndex, const QList<int> &_startingPositions)
{
	WriteLog("NetRender - send setup to clients", 2);
	if (clientIndex < GetClientCount())
	{
		sMessage msg;
		msg.command = netRenderCmd_SETUP;
		QDataStream stream(&msg.payload, QIODevice::WriteOnly);
		stream << actualId;
		stream << qint32(_startingPositions.size());
		for (int startingPosition : _startingPositions)
		{
			stream << qint32(startingPosition);
		}
		cNetRenderTransport::SendData(GetClient(clientIndex).socket, msg, actualId);
	}
	else
	{
		qCritical() << "CNetRender::SendSetup(int clientIndex, int id, QList<int> startingPositions): "
									 "Client index out of range:"
								<< clientIndex;
	}
}

void cNetRenderServer::KickAndKillClient(int clientIndex)
{
	WriteLog("NetRender - kick and kill client", 2);
	if (clientIndex < GetClientCount())
	{
		sMessage msg;
		msg.command = netRenderCmd_KICK_AND_KILL;
		cNetRenderTransport::SendData(GetClient(clientIndex).socket, msg, actualId);
	}
	else
	{
		qCritical() << "CNetRender::KickAndKillClient(int clientIndex): "
									 "Client index out of range:"
								<< clientIndex;
	}
}

// stop rendering of all clients
void cNetRenderServer::StopAllClients()
{
	sMessage msg;
	msg.command = netRenderCmd_STOP;
	for (int i = 0; i < GetClientCount(); i++)
	{
		auto &client = GetClient(i);
		cNetRenderTransport::SendData(client.socket, msg, actualId);
	}
}

void cNetRenderServer::SendVersionToClient(int index)
{
	// tell mandelbulber version to client
	sMessage msg;
	msg.command = netRenderCmd_VERSION;

	QDataStream stream(&msg.payload, QIODevice::WriteOnly);
	stream << qint32(cNetRenderTransport::version());
	QString machineName = QHostInfo::localHostName();
	stream << qint32(machineName.toUtf8().size());
	stream.writeRawData(machineName.toUtf8().data(), machineName.toUtf8().size());
	cNetRenderTransport::SendData(GetClient(index).socket, msg, actualId);
}
