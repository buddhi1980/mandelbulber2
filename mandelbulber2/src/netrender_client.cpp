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

#include "netrender_client.hpp"

#include <QAbstractSocket>
#include <QHostInfo>

#include "animation_keyframes.hpp"
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

CNetRenderClient::CNetRenderClient()
{
	clientSocket = nullptr;
	reconnectTimer = new QTimer;
	reconnectTimer->setInterval(1000);
	connect(reconnectTimer, SIGNAL(timeout()), this, SLOT(TryServerConnect()));
	actualId = 0;
	portNo = 0;
}

CNetRenderClient::~CNetRenderClient()
{
	DeleteClient();
	if (reconnectTimer)
	{
		if (reconnectTimer->isActive()) reconnectTimer->stop();
		delete reconnectTimer;
		reconnectTimer = nullptr;
	}
}

void CNetRenderClient::DeleteClient()
{
	WriteLog("NetRender - Delete Client", 2);
	if (reconnectTimer)
	{
		if (reconnectTimer->isActive()) reconnectTimer->stop();
	}
	if (clientSocket)
	{
		clientSocket->close();
		delete clientSocket;
		clientSocket = nullptr;
	}
	emit changeClientStatus(netRenderSts_DISABLED);
	emit Deleted();
}

void CNetRenderClient::TryServerConnect()
{
	if (clientSocket)
	{
		WriteLog("NetRender - TryServerConnect", 3);
		switch (clientSocket->state())
		{
			case QAbstractSocket::ConnectedState: reconnectTimer->stop(); break;
			case QAbstractSocket::ConnectingState:
			case QAbstractSocket::HostLookupState: return; // wait for result
			default:
				emit changeClientStatus(netRenderSts_CONNECTING);
				clientSocket->close();
				clientSocket->connectToHost(address, portNo);
				break;
		}
	}
}

void CNetRenderClient::SetClient(QString _address, int _portNo)
{
	address = _address;
	portNo = _portNo;
	clientSocket = new QTcpSocket(this);
	connect(clientSocket, SIGNAL(disconnected()), this, SLOT(ServerDisconnected()));
	connect(clientSocket, SIGNAL(readyRead()), this, SLOT(ReceiveFromServer()));

	reconnectTimer->start();
	QTimer::singleShot(50, this, SLOT(TryServerConnect()));
	WriteLog(
		"NetRender - Client Setup, link to server: " + address + ", port: " + QString::number(portNo),
		2);
	emit changeClientStatus(netRenderSts_CONNECTING);
}

void CNetRenderClient::ReceiveFromServer()
{
	WriteLog("NetRender - ReceiveFromServer()", 3);
	if (clientSocket->bytesAvailable() > 0)
	{
		if (cNetRenderTransport::ReceiveData(clientSocket, &msgFromServer))
		{
			ProcessData();
			cNetRenderTransport::ResetMessage(&msgFromServer);

			// try to get the next message, if available
			ReceiveFromServer();
		}
	}
}

void CNetRenderClient::ServerDisconnected()
{
	emit changeClientStatus(netRenderSts_ERROR);

	gMainInterface->stopRequest = true;

	// if reconnect timer is null, the client has been disabled
	if (reconnectTimer) reconnectTimer->start();

	WriteLog("NetRender - server disconnected", 2);

	if (systemData.noGui)
	{
		QTextStream out(stdout);
		out << QString("Connection lost") + "\n";
		out.flush();
	}
	else
	{
		qCritical() << "Connection lost";
	}
}

void CNetRenderClient::SendStatusToServer(netRenderStatus status)
{
	if (clientSocket != nullptr)
	{
		sMessage outMsg;
		outMsg.command = netRenderCmd_STATUS;
		outMsg.payload.append(reinterpret_cast<char *>(&status), sizeof(qint32));
		cNetRenderTransport::SendData(clientSocket, outMsg, actualId);
	}
}

QByteArray *CNetRenderClient::GetTexture(const QString &textureName, int frameNo)
{
	const QList<QString> keys = textures.keys();
	QString animatedTextureName = AnimatedFileName(textureName, frameNo, &keys);
	return &textures[animatedTextureName];
}

// send rendered lines
void CNetRenderClient::SendRenderedLines(
	const QList<int> &lineNumbers, const QList<QByteArray> &lines)
{
	sMessage msg;
	msg.command = netRenderCmd_DATA;
	QDataStream stream(&msg.payload, QIODevice::WriteOnly);
	for (int i = 0; i < lineNumbers.size(); i++)
	{
		stream << qint32(lineNumbers.at(i));
		stream << qint32(lines.at(i).size());
		stream.writeRawData(lines.at(i).data(), lines.at(i).size());
	}
	WriteLog(QString("NetRender - SendRenderedLines(), %1 lines").arg(lineNumbers.size()), 3);
	cNetRenderTransport::SendData(clientSocket, msg, actualId);
}

void CNetRenderClient::ProcessData()
{
	sMessage *inMsg = &msgFromServer;
	switch (netCommandServer(inMsg->command))
	{
		case netRenderCmd_VERSION: ProcessRequestVersion(inMsg); break;
		case netRenderCmd_STOP: ProcessRequestStop(inMsg); break;
		case netRenderCmd_ASK_STATUS: ProcessRequestAskStatus(inMsg); break;
		case netRenderCmd_JOB: ProcessRequestJob(inMsg); break;
		case netRenderCmd_RENDER: ProcessRequestRender(inMsg); break;
		case netRenderCmd_SETUP: ProcessRequestSetup(inMsg); break;
		case netRenderCmd_ACK: ProcessRequestAck(inMsg); break;
		case netRenderCmd_KICK_AND_KILL: ProcessRequestKickAndKill(inMsg); break;
		case netRenderCmd_ANIM_FLIGHT: ProcessRequestRenderAnimation(inMsg); break;
		case netRenderCmd_ANIM_KEY: ProcessRequestRenderAnimation(inMsg); break;
		default: qWarning() << "NetRender - command unknown: " + QString::number(inMsg->command); break;
	}
}

void CNetRenderClient::ProcessRequestVersion(sMessage *inMsg)
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
	if (cNetRenderTransport::CompareMajorVersion(serverVersion, cNetRenderTransport::version()))
	{
		QString connectionMsg =
			"NetRender - version matches (" + QString::number(cNetRenderTransport::version()) + ")";
		QString serverInfo = QString("NetRender - Connection established, Server is %1:%2 [%3]")
													 .arg(address, QString::number(portNo), serverName);
		WriteLog(connectionMsg, 2);
		WriteLog(serverInfo, 2);
		if (systemData.noGui)
		{
			QTextStream out(stdout);
			out << connectionMsg << "\n";
			out << serverInfo << "\n";
		}

		// server version matches, send worker count
		outMsg.command = netRenderCmd_WORKER;
		QDataStream outStream(&outMsg.payload, QIODevice::WriteOnly);
		outStream << qint32(systemData.numberOfThreads);
		QString machineName = QHostInfo::localHostName();
		outStream << qint32(machineName.toUtf8().size());
		outStream.writeRawData(machineName.toUtf8().data(), machineName.toUtf8().size());
		emit changeClientStatus(netRenderSts_READY);
		WriteLog(
			QString("NetRender - ProcessData(), command VERSION, version %1").arg(serverVersion), 2);
	}
	else
	{
		cErrorMessage::showMessage(tr("NetRender - version mismatch!\n")
																 + tr("Client version: %1\n").arg(cNetRenderTransport::version())
																 + tr("Server version: %1").arg(serverVersion),
			cErrorMessage::errorMessage, gMainInterface->mainWindow);

		outMsg.command = netRenderCmd_BAD;
	}

	cNetRenderTransport::SendData(clientSocket, outMsg, actualId);
}

void CNetRenderClient::ProcessRequestStop(sMessage *inMsg)
{
	Q_UNUSED(inMsg);
	// status = netRender_READY;
	gMainInterface->stopRequest = true;
	// NotifyStatus();
	WriteLog("NetRender - ProcessData(), command STOP", 2);
}

void CNetRenderClient::ProcessRequestAskStatus(sMessage *inMsg)
{
	Q_UNUSED(inMsg);
	WriteLog("NetRender - ProcessData(), command STATUS", 3);
	NotifyStatus();
}

void CNetRenderClient::ProcessRequestJob(sMessage *inMsg)
{
	if (inMsg->id == actualId)
	{
		WriteLog("NetRender - ProcessData(), command JOB", 2);
		QDataStream stream(&inMsg->payload, QIODevice::ReadOnly);
		QByteArray buffer;
		qint32 size;
		emit changeClientStatus(netRenderSts_WORKING);

		// read settings
		stream >> size;
		buffer.resize(size);
		stream.readRawData(buffer.data(), size);
		QString settingsText = QString::fromUtf8(buffer.data(), buffer.size());
		WriteLog(QString("NetRender - ProcessData(), command JOB, settings size: %1").arg(size), 2);
		WriteLog(QString("NetRender - ProcessData(), command JOB, settings: %1").arg(settingsText), 3);

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
				WriteLog(
					QString("NetRender - ProcessData(), command JOB, texture name: %1").arg(textureName), 2);
			}

			stream >> size;
			WriteLog(QString("NetRender - ProcessData(), command JOB, texture size: %1").arg(size), 2);
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
			QObject::connect(thread, SIGNAL(started()), gMainInterface->headless, SLOT(slotNetRender()));
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
		WriteLog(QString("NetRender - received JOB message with wrong id. Local %1 vs Remote %2")
							 .arg(QString::number(actualId), QString::number(inMsg->id)),
			1);
	}
}

void CNetRenderClient::ProcessRequestRender(sMessage *inMsg)
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
		WriteLog(
			QString("NetRender - ProcessData(), command RENDER, done list size: %1").arg(done.size()), 3);
		emit ToDoListArrived(done);
	}
	else
	{
		WriteLog(QString("NetRender - received RENDER message with wrong id. Local %1 vs Remote %2")
							 .arg(QString::number(actualId), QString::number(inMsg->id)),
			1);
	}
}

void CNetRenderClient::ProcessRequestSetup(sMessage *inMsg)
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
		WriteLog(
			QString("NetRender - ProcessData(), command SETUP, start line %1 = %2").arg(i).arg(line), 3);
	}
}

void CNetRenderClient::ProcessRequestAck(sMessage *inMsg)
{
	WriteLog("NetRender - ProcessData(), command ACK", 3);
	if (inMsg->id == actualId)
	{
		emit AckReceived();
	}
}

void CNetRenderClient::ProcessRequestKickAndKill(sMessage *inMsg)
{
	Q_UNUSED(inMsg);
	WriteLog("NetRender - ProcessData(), command KICK AND KILL", 2);
	QApplication::quit();
}

void CNetRenderClient::ProcessRequestRenderAnimation(sMessage *inMsg)
{
	WriteLog("NetRender - ProcessRequestRenderAnimation()", 2);
	if (inMsg->id == actualId)
	{
		if (inMsg->command == netRenderCmd_ANIM_FLIGHT || inMsg->command == netRenderCmd_ANIM_KEY)
		{
			if (inMsg->command == netRenderCmd_ANIM_KEY)
			{
				WriteLog("NetRender - ProcessData(), command ANIM_KEY", 2);
				gKeyframeAnimation->SetNetRenderStartingFrames(startingPositions);
			}

			QDataStream stream(&inMsg->payload, QIODevice::ReadOnly);
			QByteArray buffer;
			qint32 size;
			emit changeClientStatus(netRenderSts_WORKING);

			// read settings
			stream >> size;
			buffer.resize(size);
			stream.readRawData(buffer.data(), size);
			QString settingsText = QString::fromUtf8(buffer.data(), buffer.size());
			WriteLog(QString("NetRender - ProcessData(), command JOB, settings size: %1").arg(size), 2);
			WriteLog(
				QString("NetRender - ProcessData(), command JOB, settings: %1").arg(settingsText), 3);

			cSettings parSettings(cSettings::formatCondensedText);
			parSettings.BeQuiet(true);

			gInterfaceReadyForSynchronization = false;
			parSettings.LoadFromString(settingsText);

			if (inMsg->command == netRenderCmd_ANIM_KEY)
			{
				parSettings.Decode(gPar, gParFractal, nullptr, gKeyframes);
			}
			if (inMsg->command == netRenderCmd_ANIM_FLIGHT)
			{
				parSettings.Decode(gPar, gParFractal, gAnimFrames, nullptr);
			}

			WriteLog("NetRender - ProcessData(), command ANIM_KEY, starting rendering", 2);

			gInterfaceReadyForSynchronization = true;
			if (!systemData.noGui)
			{
				gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);
				emit KeyframeAnimationRender();
			}
			else
			{
				// in noGui mode it must be started as separate thread to be able to process event loop

				// TODO: headless code for animation

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
	}
}

void CNetRenderClient::ConfirmRenderedFrame(int frameIndex, int sizeOfToDoList)
{
	sMessage msg;
	msg.command = netRenderCmd_FRAME_DONE;
	QDataStream stream(&msg.payload, QIODevice::WriteOnly);

	stream << qint32(frameIndex);
	stream << qint32(sizeOfToDoList);

	WriteLog(QString("NetRender - ConfirmRenderedFrame(), frame %1").arg(frameIndex), 3);
	cNetRenderTransport::SendData(clientSocket, msg, actualId);
}
