/*
 * interface.hpp
 *
 *  Created on: Mar 11, 2014
 *      Author: krzysztof
 */

#ifndef INTERFACE_HPP_
#define INTERFACE_HPP_

#include "parameters.hpp"
#include "render_window.hpp"
#include <QProgressBar>
#include "rendered_image_widget.hpp"

//class for main user interface

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

	QApplication *application;
	RenderWindow *mainWindow;
	QImage *qimage;
	RenderedImage *renderedImage;
	QProgressBar *progressBar;

	cImage *mainImage;

	bool stopRequest;
	bool repeatRequest;

private:

	//TODO add Reset Camera button
	//TODO add Undo and Redo in menu
};

extern cInterface *mainInterface;

void MakeIconForButton(QColor &color, QPushButton *pushbutton);

#endif /* INTERFACE_HPP_ */
