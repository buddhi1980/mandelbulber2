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
#include "render_window.hpp"
#include <QProgressBar>
#include "rendered_image_widget.hpp"

class cInterface
{
public:
	enum enumReadWrite {read, write};
	enum enumCameraMovementStepMode {relative, absolute};
	enum enumCameraMovementMode {fixedDistance, moveCamera, moveTarget};
	enum enumCameraRotationMode {rotateCamera, rotateAroundTarget};

	cInterface();
	~cInterface();
	void ShowUi(void);
	void ConnectSignals(void);
	void SynchronizeInterfaceWindow(QWidget *window, cParameterContainer *par, enumReadWrite mode);
	void SynchronizeInterface(cParameterContainer *par, cParameterContainer *parFractal, enumReadWrite mode);
	void ConnectSignalsForSlidersInWindow(QWidget *window);
	void MakeColorButtonsInWindow(QWidget *window);
	void GetNameAndType(QString name, QString *parameterName, QString *type);
	void InitializeFractalUi(QString &uiFileName);
	void StatusText(const QString &text, const QString &progressText, double progress);
	double ImageScaleComboSelection2Double(int index);
	double CalcMainImageScale(double scale, int previewWidth, int previewHeight, cImage *image);
	void StartRender(void);
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
	void ShowImageInLabel(QLabel *label, const QString &filename);
	void RefreshMainImage();
	cColorPalette GetPaletteFromImage(const QString &filename);
	void AutoFog();
	double GetDistanceForPoint(CVector3 point);
	double GetDistanceForPoint(CVector3 point, cParameterContainer *par, cParameterContainer *parFractal);
	void SetByMouse(CVector2<double> screenPoint, Qt::MouseButton button, RenderedImage::enumClickMode clickMode);
	void MovementStepModeChanged(int mode);
	void Undo();
	void Redo();
	void ResetView();

	QApplication *application;
	RenderWindow *mainWindow;
	QImage *qimage;
	RenderedImage *renderedImage;
	QProgressBar *progressBar;

	cImage *mainImage;

	bool stopRequest;
	bool repeatRequest;

private:

};

extern cInterface *mainInterface;

void MakeIconForButton(QColor &color, QPushButton *pushbutton);

#endif /* INTERFACE_HPP_ */
