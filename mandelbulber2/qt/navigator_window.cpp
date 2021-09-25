/*
 * navigator_window.cpp
 *
 *  Created on: 24 wrz 2021
 *      Author: krzysztof
 */

#include "navigator_window.h"

#include "ui_navigator_window.h"
#include "src/parameters.hpp"
#include "src/fractal_container.hpp"
#include "src/cimage.hpp"
#include "src/synchronize_interface.hpp"

cNavigatorWindow::cNavigatorWindow(QWidget *parent) : QDialog(parent), ui(new Ui::cNavigatorWindow)
{
	ui->setupUi(this);
	ui->widgetNavigationButtons->HideSomeButtons();
	setModal(false);

	image.reset(new cImage(800, 600, false));
	ui->widgetRenderedImage->AssignImage(image);
	image->CreatePreview(1.0, 800, 600, ui->widgetRenderedImage);
	image->UpdatePreview();
}

cNavigatorWindow::~cNavigatorWindow()
{
	delete ui;
}

void cNavigatorWindow::SetInitialParameters(
	std::shared_ptr<cParameterContainer> _params, std::shared_ptr<cFractalContainer> _fractalParams)
{
	params.reset(new cParameterContainer());
	fractalParams.reset(new cFractalContainer());

	*params = *_params;
	*fractalParams = *_fractalParams;
	ui->widgetRenderedImage->AssignParameters(params, fractalParams);
	SynchronizeInterfaceWindow(ui->frameNavigationButtons, params, qInterface::write);
}
