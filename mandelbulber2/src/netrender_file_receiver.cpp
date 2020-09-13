/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2019-20 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * TODO: description
 */

#include "netrender_file_receiver.hpp"

#include <QDebug>
#include <QDir>

#include "initparameters.hpp"
#include "system_directories.hpp"

cNetRenderFileReceiver::cNetRenderFileReceiver(QObject *parent) : QObject(parent) {}

cNetRenderFileReceiver::~cNetRenderFileReceiver() = default;

void cNetRenderFileReceiver::ReceiveHeader(int clientIndex, qint64 _size, QString _fileName)
{
	if (!fileInfos.contains(clientIndex)) fileInfos.insert(clientIndex, sFileInfo());

	QString receivedFileName = _fileName;

	QString dirName;

	if (receivedFileName.left(4) == "DIR[")
	{
		int posOfBracket = receivedFileName.indexOf("]");
		dirName = receivedFileName.mid(4, posOfBracket - 1 - 3);
		receivedFileName = receivedFileName.mid(posOfBracket + 1);
	}

	int firstDash = receivedFileName.indexOf('_');
	QString fileName = receivedFileName.mid(firstDash + 1);

	QString fullFilePath =
		systemDirectories.GetNetrenderFolder() + QDir::separator() + dirName + fileName;

	QFile file(fullFilePath);
	if (file.open(QIODevice::WriteOnly))
	{
		sFileInfo fileInfo;

		fileInfo.fileSize = _size;
		fileInfo.receivingStarted = true;
		fileInfo.dirName = dirName;
		fileInfo.chunkIndex = 0;
		fileInfo.fileName = fileName;
		fileInfo.fullFilePathInCache = fullFilePath;

		fileInfos[clientIndex] = fileInfo;

		file.close();
	}
	else
	{
		qCritical() << "Can't open file for write to NetRender cache " << fullFilePath;
	}
}

void cNetRenderFileReceiver::ReceiveChunk(int clientIndex, int chunkIndex, QByteArray data)
{
	if (fileInfos.contains(clientIndex))
	{
		sFileInfo fileInfo = fileInfos[clientIndex];
		if (chunkIndex == fileInfo.chunkIndex + 1)
		{
			qint64 bytesLeft =
				fileInfo.fileSize - fileInfo.chunkIndex * cNetRenderFileReceiver::CHUNK_SIZE;
			qint64 expectedChunkSize = qMin(bytesLeft, cNetRenderFileReceiver::CHUNK_SIZE);
			if (data.size() == expectedChunkSize)
			{
				QFile file(fileInfo.fullFilePathInCache);

				if (file.open(QIODevice::Append))
				{
					file.write(data);
					file.close();
					fileInfo.chunkIndex++;
				}
				else
				{
					qCritical() << "Can't open file for write to NetRender cache "
											<< fileInfo.fullFilePathInCache;
				}

				// last chunk
				if (bytesLeft == expectedChunkSize)
				{
					fileInfo.receivingStarted = false;

					QString destFileName;

					if (fileInfo.dirName.isEmpty())
					{
						destFileName = gPar->Get<QString>("anim_keyframe_dir") + fileInfo.fileName;
					}
					else
					{
						QDir dir(gPar->Get<QString>("anim_keyframe_dir"));
						if (dir.exists())
						{
							if (!dir.exists(fileInfo.dirName))
							{
								dir.mkdir(fileInfo.dirName);
							}
						}
						destFileName = gPar->Get<QString>("anim_keyframe_dir") + fileInfo.dirName
													 + QDir::separator() + fileInfo.fileName;
					}

					file.copy(destFileName);
					file.remove();
				}
			}
			else
			{
				qCritical() << "ReceiveChunk(): Wrong chunk size" << data.size() << expectedChunkSize;
			}
		}
		else
		{
			qCritical() << "ReceiveChunk(): Wrong chunk index" << chunkIndex;
		}

		fileInfos[clientIndex] = fileInfo;
	}
	else
	{
		qCritical() << "ReceiveChunk(): Unknown client index" << clientIndex;
	}
}
