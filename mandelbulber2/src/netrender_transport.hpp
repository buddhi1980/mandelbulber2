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

// forward declarations
struct sRenderData;

enum netCommand
{
	netRender_NONE,		 // used for invalidating the message buffer after a message has been processed
	netRender_VERSION, // ask for server version (server to clients)
	netRender_WORKER,	// ask for number of client CPU count (client to server)
	netRender_RENDER,	// list of lines needed to be rendered,
										 // and suggestion which lines should be rendered first (server to clients)
	netRender_DATA,		 // data of rendered lines (client to server)
	netRender_BAD,		 // answer about wrong server version (client to server)
	netRender_JOB,		 // sending of settings and textures
										 // Receiving of job will start rendering on client (server to clients)
	netRender_STOP,		 // terminate rendering request (server to clients)
	netRender_STATUS,	// ask for status (server to clients)
	netRender_SETUP,	 // send setup job id and starting positions (server to clients)
	netRender_ACK,		 // acknowledge receiving of rendered lines (server to clients)
	netRender_KICK_AND_KILL // command to kill the client (program exit) (server to clients)
};

enum netRenderStatus
{
	netRender_DISABLED,		// no slot configured - netrendering disabled in the program
	netRender_READY,			// client is ready and able to receive jobs
	netRender_WORKING,		// during rendering
	netRender_NEW,				// just connected
	netRender_CONNECTING, // connecting in progress
	netRender_ERROR				// error occurred
};

struct sMessage
{
	sMessage() : command(netRender_NONE), id(0), size(0) {}
	qint32 command;
	qint32 id;
	qint32 size;
	QByteArray payload;
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
};

#endif /* MANDELBULBER2_SRC_NETRENDER_TRANSPORT_HPP_ */
