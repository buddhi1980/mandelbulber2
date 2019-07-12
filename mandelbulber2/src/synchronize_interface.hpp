/**
 * Mandelbulber v2, a 3D fractal generator       ,=#MKNmMMKmmßMNWy,
 *                                             ,B" ]L,,p%%%,,,§;, "K
 * Copyright (C) 2016-19 Mandelbulber Team     §R-==%w["'~5]m%=L.=~5N
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
 * Reading ad writing parameters from/to selected widget to/from parameters container
 */

#ifndef MANDELBULBER2_SRC_SYNCHRONIZE_INTERFACE_HPP_
#define MANDELBULBER2_SRC_SYNCHRONIZE_INTERFACE_HPP_

#include <QtCore>

// forward declarations
class cParameterContainer;

class QWidget;
class QLineEdit;
class QGroupBox;
class QDoubleSpinBox;
class QSpinBox;
class QCheckBox;
class FileSelectWidget;
class MyColorButton;
class ColorPaletteWidget;
class QComboBox;
class cMaterialSelector;
class QPlainTextEdit;
class cGradientEditWidget;

namespace qInterface
{
enum enumReadWrite
{
	read,
	write
};
struct widgetProperties
{
	QString name;
	QString className;
	QString paramName;
	QString typeName;
	bool allowed;
	char lastChar() { return paramName.at(paramName.length() - 1).toLatin1(); }
	QString nameVect() { return paramName.left(paramName.length() - 2); }
};
} // namespace qInterface

void SynchronizeInterfaceWindow(
	QWidget *window, cParameterContainer *par, qInterface::enumReadWrite mode);

// widget specific synchronization functions
void SynchronizeInterfaceQLineEdit(
	QList<QLineEdit *> widgets, cParameterContainer *par, qInterface::enumReadWrite mode);
void SynchronizeInterfaceQDoubleSpinBox(
	QList<QDoubleSpinBox *> widgets, cParameterContainer *par, qInterface::enumReadWrite mode);
void SynchronizeInterfaceQSpinBox(
	QList<QSpinBox *> widgets, cParameterContainer *par, qInterface::enumReadWrite mode);
void SynchronizeInterfaceQCheckBox(
	QList<QCheckBox *> widgets, cParameterContainer *par, qInterface::enumReadWrite mode);
void SynchronizeInterfaceQGroupBox(
	QList<QGroupBox *> widgets, cParameterContainer *par, qInterface::enumReadWrite mode);
void SynchronizeInterfaceFileSelectWidget(
	QList<FileSelectWidget *> widgets, cParameterContainer *par, qInterface::enumReadWrite mode);
void SynchronizeInterfaceMyColorButton(
	QList<MyColorButton *> widgets, cParameterContainer *par, qInterface::enumReadWrite mode);
void SynchronizeInterfaceColorGradientWidget(
	QList<cGradientEditWidget *> widgets, cParameterContainer *par, qInterface::enumReadWrite mode);
void SynchronizeInterfaceQComboBox(
	QList<QComboBox *> widgets, cParameterContainer *par, qInterface::enumReadWrite mode);
void SynchronizeInterfaceMaterialSelector(
	QList<cMaterialSelector *> widgets, cParameterContainer *par, qInterface::enumReadWrite mode);
void SynchronizeInterfaceQPlainTextEdit(
	QList<QPlainTextEdit *> widgets, cParameterContainer *par, qInterface::enumReadWrite mode);

// utility functions
void GetNameAndType(QString name, QString *parameterName, QString *type);
void SynchronizeInterfaceReadVect3d(
	QString &nameVect, char lastChar, double value, cParameterContainer *par);
void SynchronizeInterfaceWriteVect3d(
	QString &nameVect, char lastChar, double &out, cParameterContainer *par);
void SynchronizeInterfaceReadVect4d(
	QString &nameVect, char lastChar, double value, cParameterContainer *par);
void SynchronizeInterfaceWriteVect4d(
	QString &nameVect, char lastChar, double &out, cParameterContainer *par);
qInterface::widgetProperties parseWidgetProperties(QWidget *widget, QStringList allowedClassNames);

#endif /* MANDELBULBER2_SRC_SYNCHRONIZE_INTERFACE_HPP_ */
