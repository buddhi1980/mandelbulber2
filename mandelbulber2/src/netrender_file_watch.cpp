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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * NetrenderFileWatch class - watches for transmissable files on the client to
 * send to the server
 */

#include "netrender_file_watch.hpp"

NetrenderFileWatch::NetrenderFileWatch(const QString &_netrenderFolder) : QObject(nullptr)
{
	netrenderFolder = _netrenderFolder;
	netrenderFolderWatcher.addPath(_netrenderFolder);
	connect(&netrenderFolderWatcher, SIGNAL(directoryChanged(const QString &)), this,
		SLOT(netrenderFolderChanged()));
}

NetrenderFileWatch::~NetrenderFileWatch() = default;

void NetrenderFileWatch::netrenderFolderChanged()
{
	// get all files, which are not processing
	QStringList netrenderFolderFiles =
		QDir(netrenderFolder).entryList(QDir::NoDotAndDotDot | QDir::Files);
	// make paths absolute
	for (int i = 0; i < netrenderFolderFiles.size(); i++)
	{
		QString netrenderFile = netrenderFolder + QDir::separator() + netrenderFolderFiles.at(i);
		// skip processing files
		if (netrenderFile.endsWith(".processing")) continue;
		// mark these files as processing
		QFile::rename(netrenderFile, netrenderFile + ".processing");
		// send chunked file over netrender
		sendFileOverNetrender(netrenderFile + ".processing");
		// delete sent file
		QFile(netrenderFile + ".processing").remove();
	}
}

void NetrenderFileWatch::sendFileOverNetrender(const QString &file)
{
	int filesize = QFile(file).size();
	int chunks = ceil(filesize / NetrenderFileWatch::CHUNK_SIZE);
	for (int chunk; chunk < chunks; chunk++)
	{
		int offset = chunk * NetrenderFileWatch::CHUNK_SIZE;
		// TODO: something like this
		// emit sendFileChunk(file, chunk, offset, min(filesize - offset,
		// NetrenderFileWatch::CHUNK_SIZE));
	}
}
