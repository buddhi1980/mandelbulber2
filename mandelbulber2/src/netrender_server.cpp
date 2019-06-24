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
#include "render_window.hpp"
#include "settings.hpp"
#include "system.hpp"
#include "texture.hpp"

CNetRenderServer::CNetRenderServer()
{
	server = nullptr;
}

CNetRenderServer::~CNetRenderServer()
{
	DeleteServer();
}

void CNetRenderServer::SetServer(int _portNo)
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

		emit changeServerStatus(netRender_NEW);
	}
}

int CNetRenderServer::getTotalWorkerCount()
{
	int totalCount = 0;
	for (int i = 0; i < clients.count(); i++)
	{
		totalCount += clients[i].clientWorkerCount;
	}
	return totalCount;
}

void CNetRenderServer::DeleteServer()
{
	if (server)
	{
		server->close();
		delete server;
		server = nullptr;
	}
	clients.clear();
	emit Deleted();
}

void CNetRenderServer::HandleNewConnection()
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

void CNetRenderServer::ClientDisconnected()
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

int CNetRenderServer::GetClientIndexFromSocket(const QTcpSocket *socket) const
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

void CNetRenderServer::ReceiveFromClient()
{
	// get client by signal emitter
	auto *socket = qobject_cast<QTcpSocket *>(sender());
	int index = GetClientIndexFromSocket(socket);
	if (index != -1)
	{
		WriteLog("NetRender - ReceiveFromClient()", 3);
		emit ClientReceive(index);
	}
}

const sClient &CNetRenderServer::GetClient(int index)
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
bool CNetRenderServer::WaitForAllClientsReady(double timeout)
{
	QElapsedTimer timer;
	timer.start();
	while (timer.elapsed() < timeout * 1000)
	{
		bool allReady = true;
		for (int i = 0; i < GetClientCount(); i++)
		{
			if (clients.at(i).status != netRender_READY)
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
