/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-18 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * RenderWindow class - main program window
 *
 * This file contains implementation of the button callbacks in RenderWindow class.
 * See also header render_window.hpp and whole implementation of class
 * spread over render_window_*.cpp
 */

#include "ui_render_window.h"

#include "camera_target.hpp"
#include "common_math.h"
#include "initparameters.hpp"
#include "interface.hpp"
#include "render_window.hpp"
#include "rendered_image_widget.hpp"

void RenderWindow::slotPressedButtonDeletePrimitive() const
{
	QString buttonName = this->sender()->objectName();
	QString primitiveName = buttonName.mid(buttonName.indexOf('_') + 1);
	gMainInterface->DeletePrimitive(primitiveName);
}

void RenderWindow::slotPressedButtonSetPositionPrimitive() const
{
	QString buttonName = this->sender()->objectName();
	QString primitiveName = buttonName.mid(buttonName.indexOf('_') + 1);
	QStringList split = primitiveName.split('_');
	QList<QVariant> item;
	item.append(int(RenderedImage::clickPlacePrimitive));
	item.append(int(PrimitiveNameToEnum(split.at(1))));
	item.append(split.at(2).toInt());
	item.append(primitiveName); // light number
	int index = ui->comboBox_mouse_click_function->findData(item);
	ui->comboBox_mouse_click_function->setCurrentIndex(index);
	gMainInterface->renderedImage->setClickMode(item);
}

void RenderWindow::slotPressedButtonAllignPrimitiveAngle() const
{
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::read);

	CVector3 camera = gPar->Get<CVector3>("camera");
	CVector3 target = gPar->Get<CVector3>("target");
	CVector3 cameraTopVector = gPar->Get<CVector3>("camera_top");
	cCameraTarget cameraTarget(camera, target, cameraTopVector);
	CVector3 cameraRotation = cameraTarget.GetRotation();

	CVector3 baseX = CVector3(1.0, 0.0, 0.0);
	CVector3 baseY = CVector3(0.0, 1.0, 0.0);
	CVector3 baseZ = CVector3(0.0, 0.0, 1.0);

	// calculation of inverted rotation matrix
	CRotationMatrix mRot;
	mRot.RotateY(-cameraRotation.x); // yaw
	mRot.RotateX(-cameraRotation.y); // pitch
	mRot.RotateZ(-cameraRotation.z); // roll

	baseX = mRot.RotateVector(baseX);
	baseY = mRot.RotateVector(baseY);
	baseZ = mRot.RotateVector(baseZ);

	double alpha = (atan2(baseY.z, baseY.y));
	double beta = -atan2(baseY.x, sqrt(baseY.z * baseY.z + baseY.y * baseY.y));

	CVector3 vectorTemp = baseX;
	vectorTemp = vectorTemp.RotateAroundVectorByAngle(CVector3(1.0, 0.0, 0.0), -alpha);
	vectorTemp = vectorTemp.RotateAroundVectorByAngle(CVector3(0.0, 0.0, 1.0), -beta);

	double gamma = -atan2(vectorTemp.z, vectorTemp.x); // FIXME wrong angle is calculated

	alpha = cCameraTarget::CorrectAngle(alpha);
	beta = cCameraTarget::CorrectAngle(beta);
	gamma = cCameraTarget::CorrectAngle(gamma);

	CVector3 rotationAligned(alpha * 180.0 / M_PI, beta * 180.0 / M_PI, gamma * 180.0 / M_PI);

	QString buttonName = this->sender()->objectName();
	QString primitiveName = buttonName.mid(buttonName.indexOf('_') + 1);

	gPar->Set(primitiveName + "_rotation", rotationAligned);
	gMainInterface->SynchronizeInterface(gPar, gParFractal, qInterface::write);

	//	RotateZ(gamma); xy plane
	//	RotateY(beta); xz plane
	//	RotateX(alpha); yz plane

	// camera rotation
	// preparing rotation matrix
	//	mRot.RotateZ(viewAngle.x); // yaw   xy plane
	//	mRot.RotateX(viewAngle.y); // pitch yz plane
	//	mRot.RotateY(viewAngle.z); // roll xz plane
}
