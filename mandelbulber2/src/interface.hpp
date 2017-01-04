/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <QtWidgets/QtWidgets>
#include "primitives.h"
#include "synchronize_interface.hpp"
#include "algebra.hpp"

// forward declarations
class cParameterContainer;
class cFractalContainer;
class RenderWindow;
class RenderedImage;
class cHeadless;
class MyProgressBar;
class PlayerWidget;
class cMaterialItemModel;
class cMaterialEditor;
class cSystemTray;
class cImage;

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
	void ShowUi();
	void ConnectSignals() const;
	void ConnectProgressAndStatisticsSignals() const;
	void SynchronizeInterface(
		cParameterContainer *par, cFractalContainer *parFractal, qInterface::enumReadWrite mode) const;
	void StartRender(bool noUndo = false);
	void MoveCamera(QString buttonName);
	void RotateCamera(QString buttonName);
	void CameraOrTargetEdited() const;
	void RotationEdited() const;
	void CameraDistanceEdited() const;
	void IFSDefaultsDodecahedron(cParameterContainer *parFractal) const;
	void IFSDefaultsIcosahedron(cParameterContainer *parFractal) const;
	static void IFSDefaultsOctahedron(cParameterContainer *parFractal);
	static void IFSDefaultsMengerSponge(cParameterContainer *parFractal);
	static void IFSDefaultsReset(cParameterContainer *parFractal);
	void RefreshMainImage();
	void AutoFog() const;
	double GetDistanceForPoint(CVector3 point) const;
	static double GetDistanceForPoint(
		CVector3 point, cParameterContainer *par, cFractalContainer *parFractal);
	void SetByMouse(
		CVector2<double> screenPoint, Qt::MouseButton button, const QList<QVariant> &mode);
	void MovementStepModeChanged(int mode) const;
	void Undo();
	void Redo();
	void ResetView();
	void SetBoundingBoxAsLimits();
	void NewPrimitive(const QString &primitiveType, int index = 0);
	void DeletePrimitive(const QString &primitiveName);
	void RebuildPrimitives(cParameterContainer *par);
	void ComboMouseClickUpdate() const;
	void AutoRecovery() const;
	bool DataFolderUpgrade() const;
	void OptimizeStepFactor(double qualityTarget);
	void ResetFormula(int fractalNumber) const;
	void PeriodicRefresh();
	void DisablePeriodicRefresh();
	void ReEnablePeriodicRefresh();
	void InitPeriodicRefresh();
	void InitMaterialsUi();
	void MaterialSelected(int matIndex);
	static void StartupDefaultSettings();
	void DisableJuliaPointMode() const;

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
