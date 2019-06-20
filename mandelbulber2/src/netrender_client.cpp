/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2015-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
	reconnectTimer = nullptr;
	clientSocket = nullptr;
}


CNetRenderClient::~CNetRenderClient()
{
	DeleteClient();
}

void CNetRenderClient::DeleteClient()
{
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
	emit changeClientStatus(netRender_DISABLED);
}

void CNetRenderClient::TryServerConnect()
{
	if (clientSocket)
	{
		WriteLog("NetRender - TryServerConnect", 3);
		if (clientSocket->state() == QAbstractSocket::ConnectedState)
		{
			reconnectTimer->stop();
		}
		else if (clientSocket->state() == QAbstractSocket::ConnectingState
						 || clientSocket->state() == QAbstractSocket::HostLookupState)
		{
			return; // wait for result
		}
		else
		{
			emit changeClientStatus(netRender_CONNECTING);
			clientSocket->close();
			clientSocket->connectToHost(address, portNo);
		}
	}
}

void CNetRenderClient::SetClient(QString _address, int _portNo)
{
	address = _address;
	portNo = _portNo;
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
	emit changeClientStatus(netRender_CONNECTING);
}

void CNetRenderClient::ReceiveFromServer()
{
	emit receivedData();
}

void CNetRenderClient::ServerDisconnected()
{
	emit changeClientStatus(netRender_ERROR);

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
		qCritical() << "Connection lost";
	}
}
