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
	netRenderCmd_VERSION = 1,				 /* send the program version */
	netRenderCmd_RENDER = 3,				 /* list of lines (frames) needed to be rendered,
														and suggestion which lines should be rendered first */
	netRenderCmd_JOB = 6,						 /* sending of settings and textures
																	Receiving of job will start rendering on client */
	netRenderCmd_STOP = 7,					 /* terminate rendering request */
	netRenderCmd_SETUP = 9,					 /* send setup job id and starting positions */
	netRenderCmd_ACK = 10,					 /* acknowledge receiving of rendered lines */
	netRenderCmd_KICK_AND_KILL = 11, /* command to kill the client (program exit) */
	netRenderCmd_ASK_STATUS = 12,		 /* ask the client what its status is */
	netRenderCmd_ANIM_KEY = 13,		 /* sending of settings and start rendering of keyframe animation */
	netRenderCmd_ANIM_FLIGHT = 14, /* sending of settings and start rendering of flight animation */
	netRenderCmd_SEND_REQ_FILE = 18, /* send file requested by client (e.g. texture)*/
	netRenderCmd_FRAMES_TODO = 20		 /* send list of frames to do next */
};

/* these commands are send from the client to the server */
enum netCommandClient
{
	netRenderCmd_WORKER = 2,						/* send the worker stats */
	netRenderCmd_DATA = 4,							/* data of rendered lines */
	netRenderCmd_BAD = 5,								/* answer about wrong server version */
	netRenderCmd_STATUS = 8,						/* send status update */
	netRenderCmd_SEND_FILE_HEADER = 15, /* send file data header */
	netRenderCmd_SEND_FILE_DATA = 16,		/* send chunk of file data */
	netRenderCmd_REQ_FILE = 17,					/* ask server of a file (e.g. texture) */
	netRenderCmd_FRAME_DONE = 19				/* confirmation of finished rendering frame */
};

enum netRenderStatus
{
	netRenderSts_DISABLED = 0,	 /* no slot configured - netrendering disabled in the program */
	netRenderSts_READY = 1,			 /* client is ready and able to receive jobs */
	netRenderSts_WORKING = 2,		 /* during rendering */
	netRenderSts_NEW = 3,				 /* just connected */
	netRenderSts_CONNECTING = 4, /* connecting in progress */
	netRenderSts_ERROR = 5			 /* error occurred */
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
	netRenderStatus status{netRenderSts_NEW};
	qint32 itemsRendered{0};
	qint32 clientWorkerCount{0};
	QString name;
};

class cNetRenderTransport
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
