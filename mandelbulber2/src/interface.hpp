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

//class for main user interface

class cInterface
{
public:
	enum enumReadWrite {read, write};
	enum enumCameraMovementStepMode {absolute, relative};
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
	void StatusText(QString &text, QString &progressText, double progress);
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
