/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-17 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Authors: Sebastian Jennen (jenzebas@gmail.com)
 *
 * ImageSaveDialog - promoted QFileDialog for selection of the image file to store the render.
 * This dialog also contains a widget to select the image quality settings
 * (same as in preferences dialog).
 */

#include "image_save_dialog.h"

#include "../src/automated_widgets.hpp"
#include "../src/initparameters.hpp"
#include "../src/interface.hpp"
#include "../src/queue.hpp"
#include "../src/settings.hpp"
#include "ui_image_save_dialog.h"

cImageSaveDialog::cImageSaveDialog(QWidget *parent)
		: QFileDialog(parent), ui(new Ui::cImageSaveDialog)
{
	setOption(QFileDialog::DontUseNativeDialog);

	// load the custom image quality ui into a container widget
	// and append it to the QFileDialog ui
	QWidget *container = new QWidget();
	QGridLayout *gridLayout = static_cast<QGridLayout *>(this->layout());
	ui->setupUi(container);
	gridLayout->addWidget(container, 1, 3, 3, 1);
	resize(900, height()); // needs more width to display all elements

	// connect signal and synchronize image quality settings
	automatedWidgets = new cAutomatedWidgets(this);
	automatedWidgets->ConnectSignalsForSlidersInWindow(this);
	SynchronizeInterfaceWindow(this, gPar, qInterface::write);
}

cImageSaveDialog::~cImageSaveDialog()
{
}

void cImageSaveDialog::accept()
{
	// save changed image quality settings before returning the accept
	SynchronizeInterfaceWindow(this, gPar, qInterface::read);
	QFileDialog::accept();
}
