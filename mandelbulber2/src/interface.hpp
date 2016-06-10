/**
 * Mandelbulber v2, a 3D fractal generator
 *
 * cInterface class - operations connected directly with user interface
 *
 * Copyright (C) 2014 Krzysztof Marczak
 *
 * This file is part of Mandelbulber.
 *
 * Mandelbulber is free software: you can redistribute it and/or modify it under the terms of the
 * GNU General Public License as published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * Mandelbulber is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 * without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 * See the GNU General Public License for more details. You should have received a copy of the GNU
 * General Public License along with Mandelbulber. If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 */
#ifndef INTERFACE_HPP_
#define INTERFACE_HPP_

#include "parameters.hpp"
#include "fractal_container.hpp"
#include "render_window.hpp"
#include "headless.h"
#include <QProgressBar>
#include "rendered_image_widget.hpp"
#include "player_widget.hpp"
#include "primitives.h"
#include "synchronize_interface.hpp"
#include "material_item_model.h"
#include "../qt/material_editor.h"

class cInterface
{
public:
	enum enumCameraMovementStepMode
	{
		relative, absolute
	};
	enum enumCameraMovementMode
	{
		fixedDistance, moveCamera, moveTarget
	};
	enum enumCameraRotationMode
	{
		rotateCamera, rotateAroundTarget
	};

	cInterface();
	~cInterface();
	void ShowUi(void);
	void ConnectSignals(void);
	void SynchronizeInterface(cParameterContainer *par, cFractalContainer *parFractal, qInterface::enumReadWrite mode);
	void InitializeFractalUi(QString &uiFileName);
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
	double GetDistanceForPoint(CVector3 point, cParameterContainer *par, cFractalContainer *parFractal);
	void SetByMouse(CVector2<double> screenPoint, Qt::MouseButton button, const QList<QVariant> &mode);
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

	bool QuitApplicationDialog();

	RenderWindow *mainWindow;
	cHeadless *headless;
	QImage *qimage;
	RenderedImage *renderedImage;
	PlayerWidget *imageSequencePlayer;
	QProgressBar *progressBar;
	QProgressBar *progressBarAnimation;
	QProgressBar *progressBarQueueImage;
	QProgressBar *progressBarQueueAnimation;
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

	bool interfaceReady;
	bool stopRequest;
	bool repeatRequest; //request to repeat start loop
};

extern cInterface *gMainInterface;

#endif /* INTERFACE_HPP_ */
