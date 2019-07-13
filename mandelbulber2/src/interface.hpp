/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include "algebra.hpp"
#include "primitives.h"
#include "synchronize_interface.hpp"

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
class cDetachedWindow;

class cInterface : public QObject
{
	Q_OBJECT

public:
	cInterface(QObject *parent = nullptr);
	~cInterface() override;
	void ShowUi();
	void ConnectSignals() const;
	void ConnectProgressAndStatisticsSignals() const;
	void SynchronizeInterface(
		cParameterContainer *par, cFractalContainer *parFractal, qInterface::enumReadWrite mode) const;
	void StartRender(bool noUndo = false);
	void MoveCamera(QString buttonName, bool synchronizeAndRender = false);
	void RotateCamera(QString buttonName, bool synchronizeAndRender = false);
	void CameraOrTargetEdited() const;
	void RotationEdited() const;
	void CameraDistanceEdited() const;
	void IFSDefaultsDodecahedron(cParameterContainer *parFractal) const;
	void IFSDefaultsIcosahedron(cParameterContainer *parFractal) const;
	static void IFSDefaultsOctahedron(cParameterContainer *parFractal);
	static void IFSDefaultsMengerSponge(cParameterContainer *parFractal);
	static void IFSDefaultsReset(cParameterContainer *parFractal);
	void RefreshMainImage();
	void RefreshPostEffects();
	void AutoFog() const;
	double GetDistanceForPoint(CVector3 point) const;
	static double GetDistanceForPoint(
		CVector3 point, cParameterContainer *par, cFractalContainer *parFractal);
	void SetByMouse(
		CVector2<double> screenPoint, Qt::MouseButton button, const QList<QVariant> &mode);
	void MouseDragStart(CVector2<double> screenPoint, Qt::MouseButtons, const QList<QVariant> &mode);
	void MouseDragFinish();
	void MouseDragDelta(int dx, int dy);
	void MovementStepModeChanged(int mode) const;
	void CameraMovementModeChanged(int index);
	void Undo();
	void Redo();
	void ResetView();
	void BoundingBoxMove(char dimension, double moveLower, double moveUpper);
	void SetBoundingBoxAsLimitsTotal();
	void SetBoundingBoxAsLimits(CVector3 outerBoundingMin, CVector3 outerBoundingMax);
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
	void DetachMainImageWidget();
	void AttachMainImageWidget();
	static void ColorizeGroupBoxes(QWidget *window, int randomSeed);
	void SaveLocalSettings(const QWidget *widget);
	void LoadLocalSettings(const QWidget *widget);
	void ResetLocalSettings(const QWidget *widget);
	QStringList CreateListOfParametersInWidget(const QWidget *widget);
	bool QuitApplicationDialog();
	void GlobalStopRequest();
	void ResetGlobalStopRequest();

	QSettings settings;

	RenderWindow *mainWindow;
	cDetachedWindow *detachedWindow;
	cHeadless *headless;
	QImage *qImage;
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
	QTimer *stopRequestPulseTimer;
	QString autoRefreshLastHash;
	bool autoRefreshLastState;
	cMaterialItemModel *materialListModel;
	cMaterialEditor *materialEditor;
	QWidget *scrollAreaMaterialEditor;
	cSystemTray *systemTray;

	CVector2<int> lockedImageResolution;
	double lockedDetailLevel;
	int lastSelectedMaterial;

	bool stopRequest;
	bool repeatRequest; // request to repeat start loop
	int numberOfStartedRenders;

	struct sCameraDragData
	{
		sCameraDragData() {}
		bool cameraDraggingStarted{false};
		CVector2<double> startScreenPoint;
		CVector2<double> startNormalizedPoint;
		double startZ{0.0};
		CVector3 startCamera;
		CVector3 startTarget;
		CVector3 startTopVector;
		CVector3 startIndicatedPoint;
		Qt::MouseButtons button{Qt::NoButton};
		QElapsedTimer lastRefreshTime;
		qint64 lastStartRenderingTime{0};
	} cameraDragData;
};

extern cInterface *gMainInterface;

#endif /* MANDELBULBER2_SRC_INTERFACE_HPP_ */
