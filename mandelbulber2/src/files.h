/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * file operation functions
 */

#ifndef MANDELBULBER2_SRC_FILES_H_
#define MANDELBULBER2_SRC_FILES_H_

#include <string>

#include <QtCore>

#include "file_image.hpp"
#include "system.hpp"

// forward declarations
class cImage;

extern std::string logfileName;

std::string IndexFilename(const char *filename, const char *extension, int number);
bool FileIfExists(const char *filename);
int fcopy(const char *source, const char *dest);
std::string removeFileExtension(const std::string &filename);
void BufferNormalize16(sRGB16 *buffer, unsigned int size);
// void SaveAllImageLayers(const char *filename, cImage *image);

// SaveImage() returns list of saved files
QStringList SaveImage(QString filename, ImageFileSave::enumImageFileType fileType, cImage *image,
	QObject *updateReceiver = nullptr);
sRGBA16 *LoadPNG(QString filename, int &outWidth, int &outHeight);

bool FileExists(const QString &path);
QString FilePathHelper(const QString &path, const QStringList &pathList);
QString FilePathHelperTextures(const QString &path);
QString FilePathHelperSounds(const QString &path);
QByteArray LoadUtf8TextFromFile(const QString &fileName);
QString AnimatedFileName(
	const QString &filenameString, int frame, const QList<QString> *netRenderTextureList = nullptr);

#endif /* MANDELBULBER2_SRC_FILES_H_ */
