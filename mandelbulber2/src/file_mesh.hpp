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

#ifndef MANDELBULBER2_SRC_FILE_MESH_HPP_
#define MANDELBULBER2_SRC_FILE_MESH_HPP_

#include <utility>

#include <QtCore>

#include "color_structures.hpp"

class MeshFileSave : public QObject
{
	Q_OBJECT

public:
	enum enumMeshFileType
	{
		MESH_FILE_TYPE_PLY = 0
	};

	enum enumMeshContentType
	{
		MESH_CONTENT_GEOMETRY = 0,

		// rgb color
		MESH_CONTENT_COLOR = 1,
	};

	enum enumMeshFileModeType
	{
		MESH_BINARY = 0,
		MESH_ASCII = 1
	};

	struct structSaveMeshConfig
	{
		structSaveMeshConfig() {}

		structSaveMeshConfig(enumMeshFileType _fileType, QList<enumMeshContentType> _contentTypes,
			enumMeshFileModeType _fileModeType)
				: fileType(_fileType), contentTypes(std::move(_contentTypes)), fileModeType(_fileModeType)
		{
		}

		enumMeshFileType fileType{MESH_FILE_TYPE_PLY};
		QList<enumMeshContentType> contentTypes{QList<enumMeshContentType>({})};
		enumMeshFileModeType fileModeType{MESH_ASCII};
	};

	struct structSaveMeshData
	{
		structSaveMeshData()
		{
			vertices = nullptr;
			polygons = nullptr;
			colorIndices = nullptr;
		}

		structSaveMeshData(std::vector<double> *_vertices, std::vector<long long> *_polygons,
			std::vector<sRGB8> *_colorIndices)
				: vertices(_vertices), polygons(_polygons), colorIndices(_colorIndices)
		{
		}
		std::vector<double> *vertices;
		std::vector<long long> *polygons;
		std::vector<sRGB8> *colorIndices;
	};

	static QString MeshFileExtension(enumMeshFileType meshFileType);
	static QString MeshNameWithoutExtension(QString path);
	static enumMeshFileType MeshFileType(QString meshFileExtension);
	static MeshFileSave *create(
		QString filename, structSaveMeshConfig meshConfig, structSaveMeshData meshData);
	virtual void SaveMesh() = 0;
	virtual QString getJobName() = 0;

protected:
	QString filename;
	structSaveMeshConfig meshConfig;
	structSaveMeshData meshData;

	MeshFileSave(QString filename, structSaveMeshConfig meshConfig, structSaveMeshData meshData);

signals:
	void updateProgressAndStatus(const QString &text, const QString &progressText, double progress);
};

class MeshFileSavePLY : public MeshFileSave
{
	Q_OBJECT
public:
	MeshFileSavePLY(QString filename, structSaveMeshConfig meshConfig, structSaveMeshData meshData)
			: MeshFileSave(filename, meshConfig, meshData)
	{
	}
	void SaveMesh() override;
	QString getJobName() override { return tr("Saving %1").arg("PLY"); }
	void SavePLY();
};

#endif /* MANDELBULBER2_SRC_FILE_MESH_HPP_ */
