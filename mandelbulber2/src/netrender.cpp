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

cNetRender *gNetRender = nullptr;

// TODO: to check why cliect hangs after loading mp3 file (second rendering)
// TODO: to check headless mode
// TODO: to add sending of rendering preview
// TODO: to modify NetRender status table
// TODO: to implement NetRender for flight animation rendering
// FIXME: animated textures are not transfered

cNetRender::cNetRender() : QObject(nullptr)
{
	deviceType = netRenderDevuceType_UNKNOWN;
	status = netRenderSts_NEW;
	isUsed = false;
	isAnimation = false;
	netRenderClient = new CNetRenderClient();

	// client signals
	connect(
		netRenderClient, &CNetRenderClient::changeClientStatus, this, &cNetRender::clientStatusChanged);
	connect(netRenderClient, &CNetRenderClient::Deleted, this, &cNetRender::ResetDeviceType);
	connect(netRenderClient, &CNetRenderClient::ToDoListArrived, this, &cNetRender::ToDoListArrived);
	connect(netRenderClient, &CNetRenderClient::AckReceived, this, &cNetRender::AckReceived);
	connect(netRenderClient, &CNetRenderClient::NotifyStatus, this, &cNetRender::NotifyStatus);
	connect(netRenderClient, &CNetRenderClient::KeyframeAnimationRender, this,
		&cNetRender::KeyframeAnimationRender);
	connect(
		netRenderClient, &CNetRenderClient::UpdateFramesToDo, this, &cNetRender::UpdateFramesToDo);
	connect(netRenderClient, &CNetRenderClient::animationStopRequest, this,
		&cNetRender::animationStopRequest);
	connect(this, &cNetRender::AddFileToSender, netRenderClient, &CNetRenderClient::AddFileToSender);

	// server signals
	netRenderServer = new cNetRenderServer();
	connect(
		netRenderServer, &cNetRenderServer::changeServerStatus, this, &cNetRender::serverStatusChanged);
	connect(
		netRenderServer, &cNetRenderServer::ClientsChanged, this, &cNetRender::ClientsHaveChanged);
	connect(
		netRenderServer, &cNetRenderServer::ClientsChangedRow, this, &cNetRender::ClientsChangedRow);
	connect(
		netRenderServer, &cNetRenderServer::ClientsChangedCell, this, &cNetRender::ClientsChangedCell);
	connect(netRenderServer, &cNetRenderServer::Deleted, this, &cNetRender::ResetDeviceType);
	connect(netRenderServer, &cNetRenderServer::NewLinesArrived, this, &cNetRender::NewLinesArrived);
	connect(netRenderServer, &cNetRenderServer::FinishedFrame, this, &cNetRender::FinishedFrame);
}

cNetRender::~cNetRender()
{
	DeleteServer();
	DeleteClient();
}

void cNetRender::SetServer(qint32 _portNo)
{
	DeleteClient();
	DeleteServer();
	deviceType = netRenderDeviceType_SERVER;
	netRenderServer->SetServer(_portNo);
}

void cNetRender::DeleteServer()
{
	if (deviceType != netRenderDeviceType_SERVER) return;
	WriteLog("NetRender - Delete Server", 2);
	netRenderServer->DeleteServer();
	emit ClientsChanged();
	status = netRenderSts_DISABLED;
	emit NewStatusServer();
}

void cNetRender::SetClient(QString _address, int _portNo)
{
	DeleteServer();
	deviceType = netRenderDeviceType_CLIENT;
	status = netRenderSts_NEW;
	netRenderClient->SetClient(_address, _portNo);

	if (systemData.noGui)
	{
		QTextStream out(stdout);
		out << QString("NetRender - Client Setup, link to server: %1, port: %2 \n")
						 .arg(_address, QString::number(_portNo));
	}
}

void cNetRender::DeleteClient()
{
	netRenderClient->DeleteClient();
}

void cNetRender::SetCurrentJob(
	const cParameterContainer &settings, const cFractalContainer &fractal, QStringList listOfTextures)
{
	netRenderServer->SetCurrentJob(settings, fractal, listOfTextures);
}

void cNetRender::SetCurrentAnimation(
	const cParameterContainer &settings, const cFractalContainer &fractal, bool isFlight)
{
	netRenderServer->SetCurrentAnimation(settings, fractal, isFlight);
}

void cNetRender::SendToDoList(int clientIndex, const QList<int> &done)
{
	netRenderServer->SendToDoList(clientIndex, done);
}

void cNetRender::StopAllClients()
{
	netRenderServer->StopAllClients();
}

void cNetRender::SetCurrentRenderId(qint32 actualId)
{
	netRenderServer->SetActualId(actualId);
}

void cNetRender::SendSetup(int clientIndex, const QList<int> &_startingPositions)
{
	netRenderServer->SendSetup(clientIndex, _startingPositions);
}

void cNetRender::KickAndKillClient(int clientIndex)
{
	netRenderServer->KickAndKillClient(clientIndex);
}

void cNetRender::SendFramesToDoList(int clientIndex, QList<int> frameNumbers)
{
	netRenderServer->SendFramesToDoList(clientIndex, frameNumbers);
}

// send rendered lines
void cNetRender::SendRenderedLines(const QList<int> &lineNumbers, const QList<QByteArray> &lines)
{
	netRenderClient->SendRenderedLines(lineNumbers, lines);
}

void cNetRender::NotifyStatus()
{
	emit NewStatusClient();
	netRenderClient->SendStatusToServer(status);
}

void cNetRender::ConfirmRenderedFrame(int frameIndex, int sizeOfToDoList)
{
	netRenderClient->ConfirmRenderedFrame(frameIndex, sizeOfToDoList);
}

QString cNetRender::GetStatusText(netRenderStatus displayStatus)
{
	switch (displayStatus)
	{
		case netRenderSts_DISABLED: return tr("DISABLED");
		case netRenderSts_READY: return tr("READY");
		case netRenderSts_WORKING: return tr("WORKING");
		case netRenderSts_NEW: return tr("NEW");
		case netRenderSts_CONNECTING: return tr("(RE-)CONNECTING");
		case netRenderSts_ERROR: return tr("ERROR");
	}
	return tr("UNKNOWN");
}

QString cNetRender::GetStatusColor(netRenderStatus displayStatus)
{
	switch (displayStatus)
	{
		case netRenderSts_DISABLED: return "darkgrey";
		case netRenderSts_READY: return "darkgreen";
		case netRenderSts_WORKING: return "darkblue";
		case netRenderSts_NEW: return "darkpurple";
		case netRenderSts_CONNECTING: return "darkorange";
		case netRenderSts_ERROR: return "darkred";
	}
	return "red";
}

netRenderStatus cNetRender::GetClientStatus(int index)
{
	if (index < GetClientCount())
	{
		return netRenderServer->GetClient(index).status;
	}
	else
	{
		return netRenderSts_ERROR;
	}
}

bool cNetRender::Block()
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

void cNetRender::clientStatusChanged(netRenderStatus _status)
{
	if (IsClient())
	{
		status = _status;
		emit NotifyStatus();
	}
}

void cNetRender::serverStatusChanged(netRenderStatus _status)
{
	if (IsServer())
	{
		status = _status;
		emit NewStatusServer();
	}
}

void cNetRender::ClientsHaveChanged()
{
	emit ClientsChanged();
}

void cNetRender::ResetDeviceType()
{
	deviceType = netRenderDevuceType_UNKNOWN;
	emit NotifyStatus();
}

QString cNetRender::GetFileFromNetRender(QString requiredFileName, int frameIndex)
{
	// this method need to be thread safe!

	QCryptographicHash hashCrypt(QCryptographicHash::Md4);
	hashCrypt.addData(requiredFileName.toLocal8Bit());
	if (requiredFileName.contains('%'))
	{
		QString stringFrameNumber = QString::number(frameIndex);
		hashCrypt.addData(stringFrameNumber.toLocal8Bit());
	}
	else
	{
		frameIndex = -1;
	}

	QByteArray hash = hashCrypt.result();
	QString hashString = hash.toHex();
	QString fileInCache = systemData.GetNetrenderFolder() + QDir::separator() + hashString + "."
												+ QFileInfo(requiredFileName).suffix();
	if (QFile::exists(fileInCache))
	{
		return fileInCache;
	}
	else
	{
		netRenderClient->RequestFileFromServer(requiredFileName, frameIndex);
		return fileInCache;
	}
}
