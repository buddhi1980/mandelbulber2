/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2017 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * file image class to store different image file formats
 *
 * Each image file type derives ImageFileSave and implements the SaveImage
 * method to store the image data with the corresponding file format
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
	return SavePLY();
	emit updateProgressAndStatus(getJobName(), QString("Finished"), 1.0);
}

void MeshFileSavePLY::SavePLY()
{
	QFile f(filename);
	bool withColor = meshConfig.contentTypes.contains(MESH_CONTENT_COLOR);
	bool isBinary = meshConfig.fileModeType == MESH_BINARY;

	// TODO binary
	// TODO color

	if (!f.open(QFile::WriteOnly))
	{
		QString statusText = tr("Mesh Export - Failed to open output file!");
		emit updateProgressAndStatus(statusText, "", 1.0);
		return;
	}

	// write the file header
	f.write(QString("ply\n").toLatin1());
	f.write(QString("format ascii 1.0\n").toLatin1());
	f.write(QString("comment Mandelbulber Exported Mesh\n").toLatin1());
	f.write(QString("element vertex %1\n").arg(meshData.vertices.size() / 3).toLatin1());
	f.write(QString("property float x\n").toLatin1());
	f.write(QString("property float y\n").toLatin1());
	f.write(QString("property float z\n").toLatin1());
	f.write(QString("property float s\n").toLatin1());
	f.write(QString("property float t\n").toLatin1());
	if (withColor)
	{
		f.write(QString("property float red\n").toLatin1());
		f.write(QString("property float green\n").toLatin1());
		f.write(QString("property float blue\n").toLatin1());
	}
	f.write(QString("element face %1\n").arg(meshData.polygons.size() / 3).toLatin1());
	f.write(QString("property list uchar int vertex_indices\n").toLatin1());
	f.write(QString("end_header\n").toLatin1());

	// write vertices
	for (unsigned int i = 0; i < meshData.vertices.size() / 3; i++)
	{

		// float v[] = {(float) vertices[i * 3], (float) vertices[i * 3 + 1],
		//(float) vertices[i * 3 + 2]};

		// f.write((char*) &v[0], sizeof(float) * 3);
		f.write(QString("%1 %2 %3")
							.arg(meshData.vertices[i * 3])
							.arg(meshData.vertices[i * 3 + 1])
							.arg(meshData.vertices[i * 3 + 2])
							.toLatin1());
		f.write(QString(" %1 %2").arg(meshData.colorIndices[i]).arg(255).toLatin1());

		if (withColor)
		{
			double colorIndex = meshData.colorIndices[i];
			int nrCol = floor(colorIndex);
			nrCol = abs(nrCol) % (248 * 256);

			int color_number;
			if (nrCol >= 248 * 256)
			{
				color_number = nrCol;
			}
			else
			{
				// color_number = (int)(nrCol * input.material->coloring_speed + 256 *
				// input.material->paletteOffset) % 65536;
			}
			// sRGB colour = input.material->palette.IndexToColour(color_number);
			sRGB colour(255, 0, 0);
			// uchar rgb[3] = {color, color, color};
			// f.write((char*) &rgb[0], sizeof(char) * 3);
			f.write(QString(" %1 %2 %3").arg(colour.R).arg(colour.G).arg(colour.B).toLatin1());
		}
		f.write(QString("\n").toLatin1());
	}

	// write polygons
	for (unsigned int i = 0; i < meshData.polygons.size(); i += 3)
	{
		// uchar n = 3;
		// int p[] = {polygons[i], polygons[i + 1], polygons[i + 2]};
		// f.write((char*) &n, sizeof(uchar));
		// f.write((char*) &polygons[i], 3 * sizeof(int));

		if (withColor)
		{
			f.write(QString("6 %1 %2 %3 %4 %5 %6\n")
								.arg(meshData.polygons[i + 2])
								.arg(meshData.polygons[i + 1])
								.arg(meshData.polygons[i + 0])
								.arg(meshData.colorIndices[meshData.polygons[i + 2]])
								.arg(meshData.colorIndices[meshData.polygons[i + 1]])
								.arg(meshData.colorIndices[meshData.polygons[i + 0]])
								.toLatin1());
		}
		else
		{
			f.write(QString("3 %1 %2 %3\n")
								.arg(meshData.polygons[i + 2])
								.arg(meshData.polygons[i + 1])
								.arg(meshData.polygons[i + 0])
								.toLatin1());
		}
	}
	f.close();
}
