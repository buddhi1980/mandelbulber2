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
	isUsed = false;
	cNetRenderClient = new CNetRenderClient();
	connect(cNetRenderClient, SIGNAL(changeClientStatus(netRenderStatus)), this,
		SLOT(clientStatusChanged(netRenderStatus)));
	connect(cNetRenderClient, SIGNAL(Deleted()), this, SLOT(ResetDeviceType()));
	connect(cNetRenderClient, SIGNAL(ToDoListArrived(QList<int>)), this,
		SIGNAL(ToDoListArrived(QList<int>)));
	connect(cNetRenderClient, SIGNAL(AckReceived()), this, SIGNAL(AckReceived()));
	connect(cNetRenderClient, SIGNAL(NotifyStatus()), this, SLOT(NotifyStatus()));

	cNetRenderServer = new CNetRenderServer();
	connect(cNetRenderServer, SIGNAL(changeServerStatus(netRenderStatus)), this,
		SLOT(serverStatusChanged(netRenderStatus)));
	connect(cNetRenderServer, SIGNAL(ClientsChanged()), this, SLOT(ClientsHaveChanged()));
	connect(cNetRenderServer, SIGNAL(ClientsChanged(int)), this, SIGNAL(ClientsChanged(int)));
	connect(
		cNetRenderServer, SIGNAL(ClientsChanged(int, int)), this, SIGNAL(ClientsChanged(int, int)));
	connect(cNetRenderServer, SIGNAL(Deleted()), this, SLOT(ResetDeviceType()));
	connect(cNetRenderServer, SIGNAL(NewLinesArrived(QList<int>, QList<QByteArray>)), this,
		SIGNAL(NewLinesArrived(QList<int>, QList<QByteArray>)));
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
	deviceType = netRender_SERVER;
	cNetRenderServer->SetServer(_portNo);
}

void CNetRender::DeleteServer()
{
	if (deviceType != netRender_SERVER) return;
	WriteLog("NetRender - Delete Server", 2);
	cNetRenderServer->DeleteServer();
	emit ClientsChanged();
	status = netRender_DISABLED;
	emit NewStatusServer();
}

void CNetRender::SetClient(QString _address, int _portNo)
{
	DeleteServer();
	deviceType = netRender_CLIENT;
	status = netRender_NEW;
	cNetRenderClient->SetClient(_address, _portNo);

	if (systemData.noGui)
	{
		QTextStream out(stdout);
		out << QString("NetRender - Client Setup, link to server: %1, port: %2 \n")
						 .arg(_address, QString::number(_portNo));
	}
}

void CNetRender::DeleteClient()
{
	cNetRenderClient->DeleteClient();
}

void CNetRender::SetCurrentJob(
	const cParameterContainer &settings, const cFractalContainer &fractal, QStringList listOfTextures)
{
	cNetRenderServer->SetCurrentJob(settings, fractal, listOfTextures);
}

void CNetRender::SendToDoList(int clientIndex, const QList<int> &done)
{
	cNetRenderServer->SendToDoList(clientIndex, done);
}

void CNetRender::StopAllClients()
{
	cNetRenderServer->StopAllClients();
}

void CNetRender::SetCurrentRenderId(qint32 actualId)
{
	cNetRenderServer->SetActualId(actualId);
}

void CNetRender::SendSetup(int clientIndex, const QList<int> &_startingPositions)
{
	cNetRenderServer->SendSetup(clientIndex, _startingPositions);
}

void CNetRender::KickAndKillClient(int clientIndex)
{
	cNetRenderServer->KickAndKillClient(clientIndex);
}

// send rendered lines
void CNetRender::SendRenderedLines(const QList<int> &lineNumbers, const QList<QByteArray> &lines)
{
	cNetRenderClient->SendRenderedLines(lineNumbers, lines);
}

void CNetRender::NotifyStatus()
{
	emit NewStatusClient();
	cNetRenderClient->SendStatusToServer(status);
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

void CNetRender::ClientsHaveChanged()
{
	emit ClientsChanged();
}

void CNetRender::ResetDeviceType()
{
	deviceType = netRender_UNKNOWN;
	emit NotifyStatus();
}
