/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-16 Krzysztof Marczak     §R-==%w["'~5]m%=L.=~5N
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
 * cInterface class - operations connected directly with user interface
 */
#ifndef MANDELBULBER2_SRC_INTERFACE_HPP_
#define MANDELBULBER2_SRC_INTERFACE_HPP_

#include "../qt/material_editor.h"
#include "../qt/my_progress_bar.h"
#include "../qt/system_tray.hpp"
#include "fractal_container.hpp"
#include "headless.h"
#include "material_item_model.h"
#include "parameters.hpp"
#include "player_widget.hpp"
#include "primitives.h"
#include "render_window.hpp"
#include "rendered_image_widget.hpp"
#include "synchronize_interface.hpp"

class cInterface
{
public:
	enum enumCameraMovementStepMode
	{
		relative,
		absolute
	};
	enum enumCameraMovementMode
	{
		fixedDistance,
		moveCamera,
		moveTarget
	};
	enum enumCameraRotationMode
	{
		rotateCamera,
		rotateAroundTarget
	};

	cInterface();
	~cInterface();
	void ShowUi(void);
	void ConnectSignals(void);
	void SynchronizeInterface(
		cParameterContainer *par, cFractalContainer *parFractal, qInterface::enumReadWrite mode);
	void StartRender(bool noUndo = false);
	void MoveCamera(QString buttonName);
	void RotateCamera(QString buttonName);
	void CameraOrTargetEdited();
	void RotationEdited();
	void CameraDistanceEdited();
	void IFSDefaultsDodecahedron(cParameterContainer *parFractal);
	void IFSDefaultsIcosahedron(cParameterContainer *parFractal);
	void IFSDefaultsOctahedron(cParameterContainer *parFractal);
	void IFSDefaultsMengerSponge(cParameterContainer *parFractal);
	void IFSDefaultsReset(cParameterContainer *parFractal);
	void RefreshMainImage();
	void AutoFog();
	double GetDistanceForPoint(CVector3 point);
	double GetDistanceForPoint(
		CVector3 point, cParameterContainer *par, cFractalContainer *parFractal);
	void SetByMouse(
		CVector2<double> screenPoint, Qt::MouseButton button, const QList<QVariant> &mode);
	void MovementStepModeChanged(int mode);
	void Undo();
	void Redo();
	void ResetView();
	void NewPrimitive(const QString &primitiveType, int index = 0);
	void DeletePrimitive(const QString &primitiveName);
	void RebuildPrimitives(cParameterContainer *par);
	void ComboMouseClickUpdate();
	void AutoRecovery();
	void OptimizeStepFactor(double qualityTarget);
	void ResetFormula(int fractalNumber);
	void PeriodicRefresh();
	void DisablePeriodicRefresh();
	void ReEnablePeriodicRefresh();
	void InitPeriodicRefresh();
	void InitMaterialsUi();
	void MaterialSelected(int matIndex);
	void StartupDefaultSettings(void);
	void DisableJuliaPointMode();

	bool QuitApplicationDialog();

	RenderWindow *mainWindow;
	cHeadless *headless;
	QImage *qimage;
	RenderedImage *renderedImage;
	PlayerWidget *imageSequencePlayer;
	MyProgressBar *progressBar;
	MyProgressBar *progressBarAnimation;
	MyProgressBar *progressBarQueueImage;
	MyProgressBar *progressBarQueueAnimation;
	QFrame *progressBarFrame;
	QVBoxLayout *progressBarLayout;
	cImage *mainImage;
	QList<sPrimitiveItem> listOfPrimitives;
	QTimer *autoRefreshTimer;
	QString autoRefreshLastHash;
	bool autoRefreshLastState;
	cMaterialItemModel *materialListModel;
	cMaterialEditor *materialEditor;
	QWidget *scrollAreaMaterialEditor;
	cSystemTray *systemTray;

	CVector2<int> lockedImageResolution;
	double lockedDetailLevel;

	bool interfaceReady;
	bool stopRequest;
	bool repeatRequest; // request to repeat start loop
};

extern cInterface *gMainInterface;

#endif /* MANDELBULBER2_SRC_INTERFACE_HPP_ */
