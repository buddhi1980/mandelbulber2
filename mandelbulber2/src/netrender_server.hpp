/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2019-22 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include "netrender_transport.hpp"

// forward declarations
struct sRenderData;
class cNetRenderFileReceiver;

class cNetRenderServer : public QObject
{
	Q_OBJECT
public:
	explicit cNetRenderServer(QObject *parent);
	~cNetRenderServer() override;
	void SetServer(int _portNo);
	void DeleteServer();

	// get client index by given socket pointer
	int GetClientIndexFromSocket(const QTcpSocket *socket) const;
	// set the current render job id
	void SetActualId(qint32 _actualId) { actualId = _actualId; }
	// send list of already rendered lines
	void SendToDoList(int clientIndex, const QList<int> &done); // send list of already rendered lines
	// send client id and list of list of lines to render at the beginning to selected client
	void SendSetup(int clientIndex, const QList<int> &_startingPositions);
	// kicks and kills a client (can be used if client is hanging)
	void KickAndKillClient(int clientIndex);
	// stop rendering of all clients
	void StopAllClients();
	// get number of connected clients
	qint32 GetClientCount() const { return clients.size(); }
	// get number of CPU cores for selected client
	qint32 GetWorkerCount(qint32 index) { return clients[index].clientWorkerCount; }
	// get number of CPU cores for all clients
	int getTotalWorkerCount();
	// get client
	const sClient &GetClient(int index);
	// in cli mode this method enables waiting for the clients before start of rendering
	bool WaitForAllClientsReady(double timeout);
	// send parameters and textures to all clients and start rendering
	void SetCurrentJob(std::shared_ptr<const cParameterContainer> settings,
		std::shared_ptr<const cFractalContainer> fractal, QStringList listOfTextures);
	// send parameters and start rendering animation from frame n
	void SetCurrentAnimation(std::shared_ptr<const cParameterContainer> settings,
		std::shared_ptr<const cFractalContainer> fractal, bool isFlight);
	// send list of frames to render next
	void SendFramesToDoList(int clientIndex, const QList<int> &frameNumbers);

private slots:
	// received data from client
	void ClientDisconnected();
	void ReceiveFromClient();
	void HandleNewConnection();
	void SendVersionToClient(int index);

signals:
	void changeServerStatus(netRenderStatus status);
	void NewClient(int index);
	void Deleted();
	// request to update table of clients
	void ClientsChanged();
	void ClientsChangedRow(int i);
	void ClientsChangedCell(int i, int j);
	// send data of newly rendered lines to cRenderer
	void NewLinesArrived(QList<int> lineNumbers, QList<QByteArray> lines);
	void FinishedFrame(int clientIndex, int frameIndex, int sizeOfDoDoList);
	void ReceivedFileHeader(int index, qint64 fileSize, QString fileName);
	void ReceivedFileData(int index, int chunkIndex, QByteArray chunkData);

private:
	// process received data and send response if needed
	void ProcessData(QTcpSocket *socket, sMessage *inMsg);

	void ClientReceive(int index);

	// Process methods
	void ProcessRequestBad(sMessage *inMsg, int index, QTcpSocket *socket);
	void ProcessRequestWorker(sMessage *inMsg, int index, QTcpSocket *socket);
	void ProcessRequestData(sMessage *inMsg, int index, QTcpSocket *socket);
	void ProcessRequestStatus(sMessage *inMsg, int index, QTcpSocket *socket);
	void ProcessRequestFrameDone(sMessage *inMsg, int index, QTcpSocket *socket);
	void ProcessRequestFrameFileHeader(sMessage *inMsg, int index, QTcpSocket *socket);
	void ProcessRequestFrameFileDataChunk(sMessage *inMsg, int index, QTcpSocket *socket);
	void ProcessRequestFile(sMessage *inMsg, int index, QTcpSocket *socket);

	QList<sClient> clients;
	sClient nullClient; // dummy client for fail-safe purposes
	qint32 portNo;
	QTcpServer *server;
	sMessage msgCurrentJob;
	qint32 actualId;
	cNetRenderFileReceiver *fileReceiver;

public:
	const QStringList listOfAppSettingToTransfer = {"opencl_mode", "color_enabled", "alpha_enabled",
		"zbuffer_enabled", "normal_enabled", "normalWorld_enabled", "specular_enabled",
		"diffuse_enabled", "world_enabled", "shadow_channel_enabled", "gi_channel_enabled",
		"color_quality", "alpha_quality", "zbuffer_quality", "shadow_channel_quality",
		"gi_channel_quality", "normal_postfix", "specular_postfix", "diffuse_postfix", "world_postfix",
		"shadow_channel_postfix", "gi_channel_postfix", "append_alpha_png", "linear_colorspace",
		"jpeg_quality", "stereoscopic_in_separate_files", "save_channels_in_separate_folders",
		"optional_image_channels_enabled", "flight_animation_image_type",
		"keyframe_animation_image_type", "zbuffer_logarithmic", "zbuffer_invert",
		"zbuffer_constant_range", "zbuffer_max_depth", "zbuffer_min_depth"};
};

#endif /* MANDELBULBER2_SRC_NETRENDER_SERVER_HPP_ */
