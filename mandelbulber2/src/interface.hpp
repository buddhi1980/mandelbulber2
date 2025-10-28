/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2014-24 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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

#include <memory>

#include <QtWidgets/QtWidgets>

#include "algebra.hpp"
#include "camera_target.hpp"
#include "primitives.h"
#include "projection_3d.hpp"
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
	void SynchronizeInterface(std::shared_ptr<cParameterContainer> par,
		std::shared_ptr<cFractalContainer> parFractal, qInterface::enumReadWrite mode) const;
	void StartRender(bool noUndo = false);
	void RenderFlame();
	bool StopRender();
	void RefreshMainImage();
	void RefreshImageAdjustments();

	void IFSDefaultsDodecahedron(std::shared_ptr<cParameterContainer> parFractal) const;
	void IFSDefaultsIcosahedron(std::shared_ptr<cParameterContainer> parFractal) const;
	static void IFSDefaultsOctahedron(std::shared_ptr<cParameterContainer> parFractal);
	static void IFSDefaultsMengerSponge(std::shared_ptr<cParameterContainer> parFractal);
	static void IFSDefaultsReset(std::shared_ptr<cParameterContainer> parFractal);
	void AutoFog(std::shared_ptr<cParameterContainer> _params,
		std::shared_ptr<cFractalContainer> _fractalParams) const;
	double GetDistanceForPoint(CVector3 point) const;
	static double GetDistanceForPoint(CVector3 point, std::shared_ptr<cParameterContainer> par,
		std::shared_ptr<cFractalContainer> parFractal);

	void Undo();
	void Redo();
	static void ResetView(QWidget *navigationWidget, std::shared_ptr<cParameterContainer> params,
		std::shared_ptr<cFractalContainer> parFractal);
	void BoundingBoxMove(char dimension, double moveLower, double moveUpper);
	void SetBoundingBoxAsLimitsTotal();
	void SetBoundingBoxAsLimits(CVector3 outerBoundingMin, CVector3 outerBoundingMax,
		std::shared_ptr<cParameterContainer> params, std::shared_ptr<cFractalContainer> parFractal);
	void ComboMouseClickUpdate();
	static void ComboMouseClickUpdate(
		QComboBox *comboBox, std::shared_ptr<cParameterContainer> params);
	bool AutoRecovery() const;
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
	static void AdjustLayoutSpacing(QWidget *window, int spacing);

	void SaveLocalSettings(const QWidget *widget);
	void LoadLocalSettings(const QWidget *widget);
	void ResetLocalSettings(const QWidget *widget);
	void RandomizeLocalSettings(const QWidget *widget);
	QStringList CreateListOfParametersInWidget(const QWidget *widget);
	bool QuitApplicationDialog();
	void GlobalStopRequest();
	void ResetGlobalStopRequest();
	void CleanSettings();
	void UpdateMainImagePreview();
	void ChangeLightWireframeVisibility(bool enable);
	QList<QVariant> GetMouseClickFunction();
	static void SetQuickRenderParameters(const std::shared_ptr<cParameterContainer> &tempParam);
	void LoadDefaultSettings();
	bool CheckForMissingTextures();
	static void UpdateCameraRotation(std::shared_ptr<cParameterContainer> params);

	QSettings settings;

	RenderWindow *mainWindow;
	cDetachedWindow *detachedWindow;
	cHeadless *headless;
	RenderedImage *renderedImage;
	PlayerWidget *imageSequencePlayer;
	MyProgressBar *progressBar;
	MyProgressBar *progressBarAnimation;
	QFrame *progressBarFrame;
	QVBoxLayout *progressBarLayout;
	std::shared_ptr<cImage> mainImage;
	QTimer *autoRefreshTimer;
	QTimer *stopRequestPulseTimer;
	QElapsedTimer timerForAbortWarnings;
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

private slots:
	void slotAutoSaveImage(double timeSeconds);
	void slotDisablePeriodicRefresh();
	void slotReEnablePeriodicRefresh();
	void slotRefreshPostEffects();
};

extern cInterface *gMainInterface;

#endif /* MANDELBULBER2_SRC_INTERFACE_HPP_ */
