/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2026 Mandelbulber Team        §R-==%w["'~5]m%=L.=~5N
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
 * camera path export - writes the interpolated camera of every keyframe animation frame
 * to a Nuke .chan or CSV file for use in compositing and 3D software
 */

#ifndef MANDELBULBER2_SRC_CAMERA_PATH_EXPORT_HPP_
#define MANDELBULBER2_SRC_CAMERA_PATH_EXPORT_HPP_

#include <memory>
#include <vector>

#include <QString>

#include "algebra.hpp"
#include "projection_3d.hpp"

class cKeyframes;
class cParameterContainer;
class cFractalContainer;

struct sCameraPathFrame
{
	int frame = 0;
	CVector3 position;
	CVector3 rotation;				// yaw, pitch, roll in degrees
	double verticalFov = 0.0; // degrees
};

// vertical angular extent of the image in degrees for the given "fov" parameter
double CameraPathVerticalFov(double fovDegrees, params::enumPerspectiveType perspectiveType);

// interpolates every animation frame the same way as keyframe rendering does and returns
// the camera of each frame. params and fractal are not modified.
std::vector<sCameraPathFrame> CalculateCameraPath(std::shared_ptr<cKeyframes> keyframes,
	std::shared_ptr<const cParameterContainer> params,
	std::shared_ptr<const cFractalContainer> fractal);

// writes the camera path to a file: CSV with a header when the file name ends with .csv,
// otherwise Nuke .chan rows (frame x y z yaw pitch roll vfov)
bool ExportCameraPath(const QString &fileName, const std::vector<sCameraPathFrame> &cameraPath,
	params::enumPerspectiveType perspectiveType, QString *errorMessage = nullptr);

#endif /* MANDELBULBER2_SRC_CAMERA_PATH_EXPORT_HPP_ */
