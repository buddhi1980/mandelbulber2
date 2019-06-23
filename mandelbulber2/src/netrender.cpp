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

#include "netrender.hpp"

#include <QAbstractSocket>
#include <QHostInfo>

#include "error_message.hpp"
#include "fractal_container.hpp"
#include "global_data.hpp"
#include "headless.h"
#include "initparameters.hpp"
#include "interface.hpp"
#include "render_window.hpp"
#include "settings.hpp"
#include "system.hpp"
#include "texture.hpp"

CNetRender *gNetRender = nullptr;

CNetRender::CNetRender() : QObject(nullptr)
{
	deviceType = netRender_UNKNOWN;
	status = netRender_NEW;
	actualId = 0;
	isUsed = false;
	cNetRenderClient = new CNetRenderClient();
	connect(cNetRenderClient, SIGNAL(changeClientStatus(netRenderStatus)), this,
		SLOT(clientStatusChanged(netRenderStatus)));
	connect(cNetRenderClient, SIGNAL(receivedData()), this, SLOT(clientReceiveData()));

	cNetRenderServer = new CNetRenderServer();
	connect(cNetRenderServer, SIGNAL(changeServerStatus(netRenderStatus)), this,
		SLOT(serverStatusChanged(netRenderStatus)));
	connect(cNetRenderServer, SIGNAL(ClientsChanged()), this, SLOT(ClientsHaveChanged()));
	connect(cNetRenderServer, SIGNAL(NewClient(int)), this, SLOT(SendVersionToClient(int)));
	connect(cNetRenderServer, SIGNAL(ClientReceive(int)), this, SLOT(ReceiveFromClient(int)));
}

CNetRender::~CNetRender()
{
	DeleteServer();
	DeleteClient();
}

void CNetRender::SetServer(qint32 _portNo)
{
	DeleteClient();
	DeleteServer();
	CNetRenderTransport::ResetMessage(&msgCurrentJob);
	deviceType = netRender_SERVER;
	cNetRenderServer->SetServer(_portNo);
}

void CNetRender::DeleteServer()
{
	if (deviceType != netRender_SERVER) return;

	deviceType = netRender_UNKNOWN;
	WriteLog("NetRender - Delete Server", 2);
	cNetRenderServer->DeleteServer();
	emit ClientsChanged();
	status = netRender_DISABLED;
	emit NewStatusServer();
}

void CNetRender::DeleteClient()
{
	deviceType = netRender_UNKNOWN;
	cNetRenderClient->DeleteClient();
}

void CNetRender::SetClient(QString _address, int _portNo)
{
	DeleteServer();
	deviceType = netRender_CLIENT;
	status = netRender_NEW;
	CNetRenderTransport::ResetMessage(&msgFromServer);
	cNetRenderClient->SetClient(_address, _portNo);

	if (systemData.noGui)
	{
		QTextStream out(stdout);
		out << "NetRender - Client Setup, link to server: " + cNetRenderClient->getAddress()
						 + ", port: " + QString::number(cNetRenderClient->getPortNo()) + "\n";
	}
}

void CNetRender::ProcessData(QTcpSocket *socket, sMessage *inMsg)
{
	// beware: payload points to char, first cast to target type pointer, then dereference
	// *(qint32*)msg->payload

	//------------------------- CLIENT ------------------------
	if (IsClient())
	{
		switch (netCommandServer(inMsg->command))
		{
			case netRender_VERSION:
			{
			sMessage outMsg;
			QDataStream stream(&inMsg->payload, QIODevice::ReadOnly);
			qint32 serverVersion;
			stream >> serverVersion;
			QByteArray buffer;
			qint32 size;
			stream >> size;
			buffer.resize(size);
			stream.readRawData(buffer.data(), size);
			serverName = QString::fromUtf8(buffer.data(), buffer.size());
			if (CNetRenderTransport::CompareMajorVersion(serverVersion, CNetRenderTransport::version()))
			{
				QString connectionMsg = "NetRender - version matches (" + QString::number(CNetRenderTransport::version()) + ")";
				QString serverInfo = QString("NetRender - Connection established, Server is %1:%2 [%3]")
															 .arg(cNetRenderClient->getAddress(), QString::number(cNetRenderClient->getPortNo()), serverName);
				WriteLog(connectionMsg, 2);
				WriteLog(serverInfo, 2);
				if (systemData.noGui)
				{
					QTextStream out(stdout);
					out << connectionMsg << "\n";
					out << serverInfo << "\n";
				}

				// server version matches, send worker count
				outMsg.command = netRender_WORKER;
				QDataStream outStream(&outMsg.payload, QIODevice::WriteOnly);
				outStream << qint32(systemData.numberOfThreads);
				QString machineName = QHostInfo::localHostName();
				outStream << qint32(machineName.toUtf8().size());
				outStream.writeRawData(machineName.toUtf8().data(), machineName.toUtf8().size());
				status = netRender_READY;
				emit NewStatusClient();
				WriteLog(
					QString("NetRender - ProcessData(), command VERSION, version %1").arg(serverVersion),
					2);
			}
			else
			{
				cErrorMessage::showMessage(tr("NetRender - version mismatch!\n")
																		 + tr("Client version: %1\n").arg(CNetRenderTransport::version())
																		 + tr("Server version: %1").arg(serverVersion),
					cErrorMessage::errorMessage, gMainInterface->mainWindow);

				outMsg.command = netRender_BAD;
			}
			SendData(cNetRenderClient->getSocket(), outMsg);
			break;
			}
			case netRender_STOP:
			{
				// status = netRender_READY;
				gMainInterface->stopRequest = true;
				// NotifyStatus();
				WriteLog("NetRender - ProcessData(), command STOP", 2);
				break;
			}
			case netRender_ASK_STATUS:
			{
				WriteLog("NetRender - ProcessData(), command STATUS", 3);
				NotifyStatus();
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
					NotifyStatus();

					// read settings
					stream >> size;
					buffer.resize(size);
					stream.readRawData(buffer.data(), size);
					QString settingsText = QString::fromUtf8(buffer.data(), buffer.size());
					WriteLog(
						QString("NetRender - ProcessData(), command JOB, settings size: %1").arg(size), 2);
					WriteLog(
						QString("NetRender - ProcessData(), command JOB, settings: %1").arg(settingsText), 3);

					// getting textures from server
					textures.clear();

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

					gInterfaceReadyForSynchronization = false;
					parSettings.LoadFromString(settingsText);
					parSettings.Decode(gPar, gParFractal);

					WriteLog("NetRender - ProcessData(), command JOB, starting rendering", 2);

					gInterfaceReadyForSynchronization = true;
					if (!systemData.noGui)
					{
						gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
						gMainInterface->StartRender(true);
					}
					else
					{
						// in noGui mode it must be started as separate thread to be able to process event loop
						gMainInterface->headless = new cHeadless;

						auto *thread = new QThread; // deleted by deleteLater()
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

			case netRender_KICK_AND_KILL:
			{
				WriteLog("NetRender - ProcessData(), command KICK AND KILL", 2);
				QApplication::quit();
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
			switch (netCommandClient(inMsg->command))
			{
				case netRender_BAD:
				{

					cErrorMessage::showMessage(
						QObject::tr("NetRender - Client version mismatch!\n Client address:")
							+ socket->peerAddress().toString(),
						cErrorMessage::errorMessage, gMainInterface->mainWindow);
					cNetRenderServer->removeClientIndex(index);
					emit ClientsChanged();
					return; // to avoid resetting already deleted message buffer
				}
				case netRender_WORKER:
				{
					QDataStream stream(&inMsg->payload, QIODevice::ReadOnly);
					qint32 clientWorkerCount;
					stream >> clientWorkerCount;
					cNetRenderServer->SetClientWorkerCount(index, clientWorkerCount);
					QByteArray buffer;
					qint32 size;
					stream >> size;
					buffer.resize(size);
					stream.readRawData(buffer.data(), size);
					cNetRenderServer->SetClientWorkerName(
						index, QString::fromUtf8(buffer.data(), buffer.size()));

					if (cNetRenderServer->GetClient(index).status == netRender_NEW)
						cNetRenderServer->SetClientWorkerStatus(index, netRender_READY);
					WriteLog("NetRender - new Client #" + QString::number(index) + "("
										 + cNetRenderServer->GetClient(index).name + " - "
										 + cNetRenderServer->GetClient(index).socket->peerAddress().toString() + ")",
						1);
					emit ClientsChanged(index);

					if (systemData.noGui)
					{
						QTextStream out(stdout);
						out << "NetRender - Client connected: Name: " + cNetRenderServer->GetClient(index).name;
						out << " IP: " + cNetRenderServer->GetClient(index).socket->peerAddress().toString();
						out << " CPUs: " + QString::number(cNetRenderServer->GetClient(index).clientWorkerCount)
										 + "\n";
					}

					// when the client connects while a render is in progress, send the current job to the
					// client
					if (msgCurrentJob.command != netRender_NONE)
					{
						SendData(cNetRenderServer->GetClient(index).socket, msgCurrentJob);
						cNetRenderServer->SetClientWorkerLinesRendered(index, 0);
						WriteLog("CNetRender::ProcessData(): Send data at reconnect", 3);
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
						cNetRenderServer->SetClientWorkerLinesRendered(
							index, cNetRenderServer->GetClient(index).linesRendered + receivedLineNumbers.size());
						emit NewLinesArrived(receivedLineNumbers, receivedRenderedLines);

						// send acknowledge
						sMessage outMsg;
						outMsg.id = actualId;
						outMsg.command = netRender_ACK;
						SendData(cNetRenderServer->GetClient(index).socket, outMsg);
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
					cNetRenderServer->SetClientWorkerStatus(
						index, netRenderStatus(*reinterpret_cast<qint32 *>(inMsg->payload.data())));
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
	CNetRenderTransport::ResetMessage(inMsg);
}

// send rendered lines
void CNetRender::SendRenderedLines(
	const QList<int> &lineNumbers, const QList<QByteArray> &lines) const
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
	SendData(cNetRenderClient->getSocket(), msg);
}

// stop rendering of all clients
void CNetRender::Stop()
{
	sMessage msg;
	msg.command = netRender_STOP;
	for (int i = 0; i < cNetRenderServer->GetClientCount(); i++)
	{
		auto &client = cNetRenderServer->GetClient(i);
		SendData(client.socket, msg);
	}
}

void CNetRender::SetCurrentJob(
	const cParameterContainer &settings, const cFractalContainer &fractal, QStringList listOfTextures)
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
			else
			{
				qCritical() << "Cannot send texture using NetRender. File:" << listOfTextures[i];
			}

			stream << qint32(0); // empty entry
		}

		for (int i = 0; i < cNetRenderServer->GetClientCount(); i++)
		{
			auto &client = cNetRenderServer->GetClient(i);
			SendData(client.socket, msgCurrentJob);
			cNetRenderServer->SetClientWorkerLinesRendered(i, 0);
		}
	}
}

void CNetRender::NotifyStatus()
{
	if (cNetRenderClient->getSocket() != nullptr)
	{
		sMessage outMsg;
		outMsg.command = netRender_STATUS;
		outMsg.payload.append(reinterpret_cast<char *>(&status), sizeof(qint32));
		SendData(cNetRenderClient->getSocket(), outMsg);
	}
	emit NewStatusClient();
}

void CNetRender::SendToDoList(int clientIndex, const QList<int> &done)
{
	if (clientIndex < cNetRenderServer->GetClientCount())
	{
		sMessage msg;
		msg.command = netRender_RENDER;
		QDataStream stream(&msg.payload, QIODevice::WriteOnly);
		stream << qint32(done.size());
		for (int doneFlag : done)
		{
			stream << qint32(doneFlag);
		}
		SendData(cNetRenderServer->GetClient(clientIndex).socket, msg);
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

void CNetRender::SendSetup(int clientIndex, int id, const QList<int> &_startingPositions)
{
	WriteLog("NetRender - send setup to clients", 2);
	if (clientIndex < cNetRenderServer->GetClientCount())
	{
		sMessage msg;
		msg.command = netRender_SETUP;
		QDataStream stream(&msg.payload, QIODevice::WriteOnly);
		stream << qint32(id);
		stream << qint32(_startingPositions.size());
		for (int startingPosition : _startingPositions)
		{
			stream << qint32(startingPosition);
		}
		SendData(cNetRenderServer->GetClient(clientIndex).socket, msg);
		actualId = id;
	}
	else
	{
		qCritical() << "CNetRender::SendSetup(int clientIndex, int id, QList<int> startingPositions): "
									 "Client index out of range:"
								<< clientIndex;
	}
}

void CNetRender::KickAndKillClient(int clientIndex)
{
	WriteLog("NetRender - kick and kill client", 2);
	if (clientIndex < cNetRenderServer->GetClientCount())
	{
		sMessage msg;
		msg.command = netRender_KICK_AND_KILL;
		SendData(cNetRenderServer->GetClient(clientIndex).socket, msg);
	}
	else
	{
		qCritical() << "CNetRender::KickAndKillClient(int clientIndex): "
									 "Client index out of range:"
								<< clientIndex;
	}
}

QString CNetRender::GetStatusText(netRenderStatus displayStatus)
{
	switch (displayStatus)
	{
		case netRender_DISABLED: return tr("DISABLED");
		case netRender_READY: return tr("READY");
		case netRender_WORKING: return tr("WORKING");
		case netRender_NEW: return tr("NEW");
		case netRender_CONNECTING: return tr("(RE-)CONNECTING");
		case netRender_ERROR: return tr("ERROR");
	}
	return tr("UNKNOWN");
}

QString CNetRender::GetStatusColor(netRenderStatus displayStatus)
{
	switch (displayStatus)
	{
		case netRender_DISABLED: return "darkgrey";
		case netRender_READY: return "darkgreen";
		case netRender_WORKING: return "darkblue";
		case netRender_NEW: return "darkpurple";
		case netRender_CONNECTING: return "darkorange";
		case netRender_ERROR: return "darkred";
	}
	return "red";
}

netRenderStatus CNetRender::GetClientStatus(int index)
{
	if (index < GetClientCount())
	{
		return cNetRenderServer->GetClient(index).status;
	}
	else
	{
		return netRender_ERROR;
	}
}

bool CNetRender::WaitForAllClientsReady(double timeout)
{
	return cNetRenderServer->WaitForAllClientsReady(timeout);
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

QByteArray *CNetRender::GetTexture(const QString &textureName, int frameNo)
{
	const QList<QString> keys = textures.keys();
	QString animatedTextureName = AnimatedFileName(textureName, frameNo, &keys);
	return &textures[animatedTextureName];
}

void CNetRender::clientStatusChanged(netRenderStatus _status)
{
	if (IsClient())
	{
		status = _status;
		emit NotifyStatus();
	}
}

void CNetRender::serverStatusChanged(netRenderStatus _status)
{
	if (IsServer())
	{
		status = _status;
		emit NewStatusServer();
	}
}

void CNetRender::clientReceiveData()
{
	WriteLog("NetRender - ReceiveFromServer()", 3);
	ReceiveData(cNetRenderClient->getSocket(), &msgFromServer);
}

bool CNetRender::SendData(QTcpSocket *socket, sMessage msg) const
{
	return CNetRenderTransport::SendData(socket, msg, actualId);
}

void CNetRender::ReceiveData(QTcpSocket *socket, sMessage *msg)
{
	if (socket->bytesAvailable() > 0)
	{
		if (CNetRenderTransport::ReceiveData(socket, msg))
		{
			ProcessData(socket, msg);
			return;
		}
	}
}

void CNetRender::ClientsHaveChanged()
{
	emit ClientsChanged();
}

void CNetRender::SendVersionToClient(int index)
{
	// tell mandelbulber version to client
	sMessage msg;
	msg.command = netRender_VERSION;

	QDataStream stream(&msg.payload, QIODevice::WriteOnly);
	stream << qint32(CNetRenderTransport::version());
	QString machineName = QHostInfo::localHostName();
	stream << qint32(machineName.toUtf8().size());
	stream.writeRawData(machineName.toUtf8().data(), machineName.toUtf8().size());
	SendData(cNetRenderServer->GetClient(index).socket, msg);
}

void CNetRender::ReceiveFromClient(int index)
{
	ReceiveData(
		cNetRenderServer->GetClient(index).socket, cNetRenderServer->GetMessagePointer(index));
}
