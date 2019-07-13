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
 * Netrender Transport class - transport specific Networking class for command and payload
 * communication
 */

#ifndef MANDELBULBER2_SRC_NETRENDER_TRANSPORT_HPP_
#define MANDELBULBER2_SRC_NETRENDER_TRANSPORT_HPP_

#include <QTcpServer>
#include <QTcpSocket>
#include <QtCore>

#include "fractal_container.hpp"
#include "parameters.hpp"
#include "system.hpp"

// forward declarations
struct sRenderData;

enum netCommand
{
	netRender_NONE = 0, // used for invalidating the message buffer after a message has been processed
};

/* these commands are send from the server to the client */
enum netCommandServer
{
	netRender_VERSION = 1,				/* send the program version */
	netRender_RENDER = 3,					/* list of lines needed to be rendered,
																 and suggestion which lines should be rendered first */
	netRender_JOB = 6,						/* sending of settings and textures
																 Receiving of job will start rendering on client */
	netRender_STOP = 7,						/* terminate rendering request */
	netRender_SETUP = 9,					/* send setup job id and starting positions */
	netRender_ACK = 10,						/* acknowledge receiving of rendered lines */
	netRender_KICK_AND_KILL = 11, /* command to kill the client (program exit) */
	netRender_ASK_STATUS = 12			/* ask the client what its statis is */
};

/* these commands are send from the client to the server */
enum netCommandClient
{
	netRender_WORKER = 2, /* send the worker stats */
	netRender_DATA = 4,		/* data of rendered lines */
	netRender_BAD = 5,		/* answer about wrong server version */
	netRender_STATUS = 8, /* send status update */
};

enum netRenderStatus
{
	netRender_DISABLED = 0,		/* no slot configured - netrendering disabled in the program */
	netRender_READY = 1,			/* client is ready and able to receive jobs */
	netRender_WORKING = 2,		/* during rendering */
	netRender_NEW = 3,				/* just connected */
	netRender_CONNECTING = 4, /* connecting in progress */
	netRender_ERROR = 5				/* error occurred */
};

struct sMessage
{
	sMessage() {}
	qint32 command{netRender_NONE};
	qint32 id{0};
	qint32 size{0};
	QByteArray payload;

	static qint64 headerSize() { return qint64(sizeof(command) + sizeof(id) + sizeof(size)); }
	static qint64 crcSize() { return qint64(sizeof(quint16)); }
};

struct sClient
{
	sClient() {}
	QTcpSocket *socket{nullptr};
	sMessage msg;
	netRenderStatus status{netRender_NEW};
	qint32 linesRendered{0};
	qint32 clientWorkerCount{0};
	QString name;
};

class CNetRenderTransport
{
public:
	// send data to communication partner
	static bool SendData(QTcpSocket *socket, sMessage msg, qint32 id);
	// receive data from partner
	static bool ReceiveData(QTcpSocket *socket, sMessage *msg);
	// clearing message buffer
	static void ResetMessage(sMessage *msg);
	// compare major version of software
	static bool CompareMajorVersion(qint32 version1, qint32 version2);
	// the numeric and comparable version of the mandelbulber instance
	static int version() { return 1000L * MANDELBULBER_VERSION; }
};

#endif /* MANDELBULBER2_SRC_NETRENDER_TRANSPORT_HPP_ */
