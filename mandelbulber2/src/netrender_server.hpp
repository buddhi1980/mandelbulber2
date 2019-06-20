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
 * NetrenderClient class - Networking class for server specific command and payload communication
 */

#ifndef MANDELBULBER2_SRC_NETRENDER_SERVER_HPP_
#define MANDELBULBER2_SRC_NETRENDER_SERVER_HPP_

#include <QTcpServer>
#include <QTcpSocket>
#include <QtCore>

#include "netrender_transport.hpp"

// forward declarations
struct sRenderData;

class CNetRenderServer : public QObject
{
	Q_OBJECT
public:
	explicit CNetRenderServer();
	~CNetRenderServer() override;
	// get client index by given socket pointer
	int GetClientIndexFromSocket(const QTcpSocket *socket) const;
	// get number of connected clients
	qint32 GetClientCount() const { return clients.size(); }
	// get number of CPU cores for selected client
	qint32 GetWorkerCount(qint32 index) { return clients[index].clientWorkerCount; }
	int getTotalWorkerCount();
	void SetServer(int _portNo);
	void DeleteServer();
	// get client
	const sClient &GetClient(int index);
	void removeClientIndex(int index) { clients.removeAt(index); }
	void SetClientWorkerCount(int index, qint32 clientWorkerCount)
	{
		clients[index].clientWorkerCount = clientWorkerCount;
	}
	void SetClientWorkerName(int index, QString name) { clients[index].name = name; }
	void SetClientWorkerStatus(int index, netRenderStatus status) { clients[index].status = status; }
	void SetClientWorkerLinesRendered(int index, int linesRendered)
	{
		clients[index].linesRendered = linesRendered;
	}
	sMessage *GetMessagePointer(int index) { return &clients[index].msg; }
	bool WaitForAllClientsReady(double timeout);

private slots:
	// received data from client
	void ClientDisconnected();
	void ReceiveFromClient();
	void HandleNewConnection();

signals:
	void changeServerStatus(netRenderStatus status);
	void ClientsChanged();
	void NewClient(int index);
	void ClientReceive(int index);

private:
	QList<sClient> clients;
	sClient nullClient; // dummy client for fail-safe purposes
	qint32 portNo;
	QTcpServer *server;
};

#endif /* MANDELBULBER2_SRC_NETRENDER_SERVER_HPP_ */
