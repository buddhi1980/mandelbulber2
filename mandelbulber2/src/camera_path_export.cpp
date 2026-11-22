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

#include "camera_path_export.hpp"

#include <QFile>
#include <QTextStream>

#include "camera_target.hpp"
#include "fractal_container.hpp"
#include "keyframes.hpp"
#include "parameters.hpp"
#include "scripts.h"

double CameraPathVerticalFov(double fovDegrees, params::enumPerspectiveType perspectiveType)
{
	// the image plane spans -0.5...0.5 vertically (see CalculateViewVector()). For three-point
	// perspective tan(vfov / 2) = 0.5 * 2 * tan(fov / 2), so the vertical field of view equals fov.
	// Fish eye maps the radius linearly to the angle, so the vertical extent is also fov.
	// Equirectangular scales the image point by 0.5, so the vertical extent is fov / 2.
	switch (perspectiveType)
	{
		case params::perspEquirectangular: return fovDegrees * 0.5;
		case params::perspThreePoint:
		case params::perspFishEye:
		case params::perspFishEyeCut:
		default: return fovDegrees;
	}
}

std::vector<sCameraPathFrame> CalculateCameraPath(std::shared_ptr<cKeyframes> keyframes,
	std::shared_ptr<const cParameterContainer> params,
	std::shared_ptr<const cFractalContainer> fractal)
{
	std::vector<sCameraPathFrame> cameraPath;

	auto tempPar = std::make_shared<cParameterContainer>();
	*tempPar = *params;
	auto tempFractPar = std::make_shared<cFractalContainer>();
	*tempFractPar = *fractal;

	keyframes->UpdateFramesIndexesTable();
	keyframes->ClearMorphCache();

	const int numberOfFrames = keyframes->GetTotalNumberOfFrames();
	cameraPath.reserve(numberOfFrames);

	for (int frameIndex = 0; frameIndex < numberOfFrames; frameIndex++)
	{
		// the same steps as in cKeyframeAnimation::RenderKeyframes()
		tempPar->Set("frame_no", frameIndex);
		keyframes->GetInterpolatedFrameAndConsolidate(frameIndex, tempPar, tempFractPar);
		cScripts::EvaluateAll(tempPar, tempFractPar);

		// the renderer computes the camera rotation from camera, target and top vector
		// (cRenderWorker::PrepareMainVectors())
		const cCameraTarget cameraTarget(tempPar->Get<CVector3>("camera"),
			tempPar->Get<CVector3>("target"), tempPar->Get<CVector3>("camera_top"));

		sCameraPathFrame cameraFrame;
		cameraFrame.frame = frameIndex;
		cameraFrame.position = cameraTarget.GetCamera();
		cameraFrame.rotation = cameraTarget.GetRotation() * (180.0 / M_PI);
		cameraFrame.verticalFov = CameraPathVerticalFov(tempPar->Get<double>("fov"),
			params::enumPerspectiveType(tempPar->Get<int>("perspective_type")));
		cameraPath.push_back(cameraFrame);
	}

	return cameraPath;
}

bool ExportCameraPath(const QString &fileName, const std::vector<sCameraPathFrame> &cameraPath,
	params::enumPerspectiveType perspectiveType, QString *errorMessage)
{
	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
	{
		if (errorMessage) *errorMessage = file.errorString();
		return false;
	}

	const bool csv = fileName.endsWith(".csv", Qt::CaseInsensitive);
	const QString separator = csv ? "," : " ";
	QTextStream out(&file);

	if (csv)
	{
		// a Nuke .chan file has to contain only numbers, so the conventions are documented here
		out << "# Mandelbulber camera path, one row per animation frame (frame 0 = first image)\n";
		out << "# coordinates: right-handed, Z axis up; with zero rotation the camera looks along +Y,"
					 " +X is right\n";
		out << "# rotation in degrees: camera to world matrix = Rz(yaw) * Rx(pitch) * Ry(roll)\n";
		out << "#   yaw: around world Z, positive turns from +Y towards -X\n";
		out << "#   pitch: positive looks up\n";
		out << "#   roll: around the view direction, positive tilts the top of the image towards +X\n";
		out << "# vfov: vertical field of view in degrees";
		if (perspectiveType != params::perspThreePoint)
			out << " (angular extent, the perspective type is not three-point)";
		out << "\n";
		out << "frame,x,y,z,yaw,pitch,roll,vfov\n";
	}

	for (const sCameraPathFrame &cameraFrame : cameraPath)
	{
		const double values[] = {cameraFrame.position.x, cameraFrame.position.y, cameraFrame.position.z,
			cameraFrame.rotation.x, cameraFrame.rotation.y, cameraFrame.rotation.z,
			cameraFrame.verticalFov};
		out << cameraFrame.frame;
		for (double value : values)
			out << separator << QString::number(value, 'g', 15);
		out << "\n";
	}

	out.flush();
	file.close();
	if (file.error() != QFileDevice::NoError)
	{
		if (errorMessage) *errorMessage = file.errorString();
		return false;
	}
	return true;
}
