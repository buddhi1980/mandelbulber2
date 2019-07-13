/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * file mesh class to store different mesh file formats
 *
 * Each mesh file type derives MeshFileSave and implements the SaveMesh
 * method to store the mesh data with the corresponding file format
 */

#include "file_mesh.hpp"

#include "error_message.hpp"
#include "files.h"
#include "initparameters.hpp"
#include "parameters.hpp"

MeshFileSave::MeshFileSave(
	QString filename, structSaveMeshConfig meshConfig, structSaveMeshData meshData)
{
	this->filename = filename;
	this->meshConfig = meshConfig;
	this->meshData = meshData;
}

MeshFileSave *MeshFileSave::create(
	QString filename, structSaveMeshConfig meshConfig, structSaveMeshData meshData)
{
	switch (meshConfig.fileType)
	{
		case MESH_FILE_TYPE_PLY: return new MeshFileSavePLY(filename, meshConfig, meshData);
	}
	qCritical() << "fileType " << MeshFileExtension(meshConfig.fileType) << " not supported!";
	return nullptr;
}

QString MeshFileSave::MeshFileExtension(enumMeshFileType meshFileType)
{
	switch (meshFileType)
	{
		case MESH_FILE_TYPE_PLY: return "ply";
	}
	return "";
}

MeshFileSave::enumMeshFileType MeshFileSave::MeshFileType(QString meshFileExtension)
{
	if (meshFileExtension == "ply")
		return MESH_FILE_TYPE_PLY;
	else
		return MESH_FILE_TYPE_PLY;
}

QString MeshFileSave::MeshNameWithoutExtension(QString path)
{
	QFileInfo fi(path);
	QString fileName = fi.completeBaseName();
	if (!QStringList({"ply"}).contains(fi.suffix()))
	{
		fileName += "." + fi.suffix();
	}
	return fi.path() + QDir::separator() + fileName;
}

void MeshFileSavePLY::SaveMesh()
{
	emit updateProgressAndStatus(getJobName(), QString("Started"), 0.0);
	SavePLY();
	emit updateProgressAndStatus(getJobName(), QString("Finished"), 1.0);
}

void MeshFileSavePLY::SavePLY()
{
	QFile qFile(filename);

	bool withColor = meshConfig.contentTypes.contains(MESH_CONTENT_COLOR);
	bool isBinary = meshConfig.fileModeType == MESH_BINARY;

	double alpha = 1.0;
	char polygonSize = 3;
	QString plyFormat = isBinary ? "binary_little_endian" : "ascii";

	if (!qFile.open(QFile::WriteOnly))
	{
		QString statusText = tr("Mesh Export - Failed to open output file!");
		emit updateProgressAndStatus(statusText, "", 1.0);
		return;
	}
	QDataStream oB(&qFile);
	QTextStream oT(&qFile);

	// write the file header
	oT << QString("ply\n").toLatin1();
	oT << QString("format %1 1.0\n").arg(plyFormat).toLatin1();
	oT << QString("comment Mandelbulber Exported Mesh\n").toLatin1();
	oT << QString("element vertex %1\n").arg(meshData.vertices->size() / 3).toLatin1();
	oT << QString("property double x\n").toLatin1();
	oT << QString("property double y\n").toLatin1();
	oT << QString("property double z\n").toLatin1();
	oT << QString("property double s\n").toLatin1();
	oT << QString("property double t\n").toLatin1();
	if (withColor)
	{
		oT << QString("property uchar red\n").toLatin1();
		oT << QString("property uchar green\n").toLatin1();
		oT << QString("property uchar blue\n").toLatin1();
	}
	oT << QString("element face %1\n").arg(meshData.polygons->size() / 3).toLatin1();
	oT << QString("property list uchar int vertex_index\n").toLatin1();
	oT << QString("end_header\n").toLatin1();
	oT.flush();

	// write vertices
	for (unsigned long long i = 0; i < meshData.vertices->size() / 3; i++)
	{
		if (isBinary)
		{
			oB.writeRawData(reinterpret_cast<char *>(&meshData.vertices->at(i * 3)), sizeof(double) * 3);
			oB.writeRawData(reinterpret_cast<char *>(&meshData.colorIndices->at(i)), sizeof(double) * 1);
			oB.writeRawData(reinterpret_cast<char *>(&alpha), sizeof(double) * 1);
		}
		else
		{
			oT << QString("%1 %2 %3")
							.arg(meshData.vertices->at(i * 3))
							.arg(meshData.vertices->at(i * 3 + 1))
							.arg(meshData.vertices->at(i * 3 + 2))
							.toLatin1();
			oT << QString(" %1 %2").arg(meshData.colorIndices->at(i).R).arg(alpha).toLatin1();
		}

		if (withColor)
		{
			sRGB8 colour = meshData.colorIndices->at(i);
			if (isBinary)
			{
				oB.writeRawData((char *)&colour, sizeof(sRGB8));
			}
			else
			{
				oT << QString(" %1 %2 %3").arg(colour.R).arg(colour.G).arg(colour.B).toLatin1();
			}
		}
		if (!isBinary) oT << QString("\n").toLatin1();
	}

	// write polygons
	for (unsigned long long i = 0; i < meshData.polygons->size(); i += 3)
	{
		if (isBinary)
		{
			qint64 p1 = meshData.polygons->at(i + 2);
			qint64 p2 = meshData.polygons->at(i + 1);
			qint64 p3 = meshData.polygons->at(i + 0);
			oB.writeRawData(reinterpret_cast<char *>(&polygonSize), sizeof(char) * 1);
			oB.writeRawData(reinterpret_cast<char *>(&p1), sizeof(int) * 1);
			oB.writeRawData(reinterpret_cast<char *>(&p2), sizeof(int) * 1);
			oB.writeRawData(reinterpret_cast<char *>(&p3), sizeof(int) * 1);
		}
		else
		{
			oT << QString("%1 %2 %3 %4\n")
							.arg(polygonSize)
							.arg(meshData.polygons->at(i + 2))
							.arg(meshData.polygons->at(i + 1))
							.arg(meshData.polygons->at(i + 0))
							.toLatin1();
		}
	}
	qFile.close();
}
