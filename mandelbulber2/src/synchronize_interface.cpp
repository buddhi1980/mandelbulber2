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

#include "synchronize_interface.hpp"

#include <QtWidgets/QComboBox>
#include <QtWidgets/QPlainTextEdit>

#include "algebra.hpp"
#include "fractal_list.hpp"
#include "global_data.hpp"
#include "parameters.hpp"

#include "qt/file_select_widget.h"
#include "qt/formula_combo_box.h"
#include "qt/gradient_edit_widget.h"
#include "qt/material_selector.h"
#include "qt/my_check_box.h"
#include "qt/my_color_button.h"
#include "qt/my_combo_box.h"
#include "qt/my_double_spin_box.h"
#include "qt/my_group_box.h"
#include "qt/my_line_edit.h"
#include "qt/my_spin_box.h"

using namespace qInterface;

// Reading ad writing parameters from/to selected widget to/from parameters container
void SynchronizeInterfaceWindow(QWidget *window, cParameterContainer *par, enumReadWrite mode)
{
	if (!gInterfaceReadyForSynchronization && mode == qInterface::read) return;

	WriteLog("cInterface::SynchronizeInterface: QLineEdit", 3);
	SynchronizeInterfaceQLineEdit(window->findChildren<QLineEdit *>(), par, mode);

	WriteLog("cInterface::SynchronizeInterface: QDoubleSpinBox", 3);
	SynchronizeInterfaceQDoubleSpinBox(window->findChildren<QDoubleSpinBox *>(), par, mode);

	WriteLog("cInterface::SynchronizeInterface: QSpinBox", 3);
	SynchronizeInterfaceQSpinBox(window->findChildren<QSpinBox *>(), par, mode);

	WriteLog("cInterface::SynchronizeInterface: QCheckBox", 3);
	SynchronizeInterfaceQCheckBox(window->findChildren<QCheckBox *>(), par, mode);

	WriteLog("cInterface::SynchronizeInterface: QGroupBox", 3);
	SynchronizeInterfaceQGroupBox(window->findChildren<QGroupBox *>(), par, mode);

	WriteLog("cInterface::SynchronizeInterface: FileSelectWidget", 3);
	SynchronizeInterfaceFileSelectWidget(window->findChildren<FileSelectWidget *>(), par, mode);

	WriteLog("cInterface::SynchronizeInterface: MyColorButton", 3);
	SynchronizeInterfaceMyColorButton(window->findChildren<MyColorButton *>(), par, mode);

	WriteLog("cInterface::SynchronizeInterface: cGradientEditWidget", 3);
	SynchronizeInterfaceColorGradientWidget(window->findChildren<cGradientEditWidget *>(), par, mode);

	WriteLog("cInterface::SynchronizeInterface: QComboBox", 3);
	SynchronizeInterfaceQComboBox(window->findChildren<QComboBox *>(), par, mode);

	WriteLog("cInterface::SynchronizeInterface: cMaterialSelector", 3);
	SynchronizeInterfaceMaterialSelector(window->findChildren<cMaterialSelector *>(), par, mode);

	WriteLog("cInterface::SynchronizeInterface: QPlainTextEdit", 3);
	SynchronizeInterfaceQPlainTextEdit(window->findChildren<QPlainTextEdit *>(), par, mode);

	WriteLog("cInterface::SynchronizeInterface: Done", 3);
}

// widget specific synchronization functions
void SynchronizeInterfaceQLineEdit(
	QList<QLineEdit *> widgets, cParameterContainer *par, enumReadWrite mode)
{
	for (auto &widget : widgets)
	{
		widgetProperties props = parseWidgetProperties(widget, {"MyLineEdit", "QLineEdit"});
		if (props.allowed)
		{
			QLineEdit *lineEdit = widget;

			if (props.className == QString("MyLineEdit"))
			{
				MyLineEdit *myLineEdit = static_cast<MyLineEdit *>(widget);
				myLineEdit->AssignParameterContainer(par);
				myLineEdit->AssignParameterName(props.paramName);
			}

			//----- get vectors ------------
			if (props.typeName == QString("vect3") || props.typeName == QString("logvect3"))
			{
				char lastChar = props.lastChar();
				QString nameVect = props.nameVect();

				if (mode == qInterface::read)
				{
					double value = systemData.locale.toDouble(lineEdit->text());
					SynchronizeInterfaceReadVect3d(nameVect, lastChar, value, par);
				}
				else if (mode == qInterface::write)
				{
					double value;
					SynchronizeInterfaceWriteVect3d(nameVect, lastChar, value, par);
					lineEdit->setText(QString("%L1").arg(value, 0, 'g', 16));
					lineEdit->setCursorPosition(0);
				}
			}

			//----- get vectors 4D  ------------
			else if (props.typeName == QString("vect4"))
			{
				char lastChar = props.lastChar();
				QString nameVect = props.nameVect();

				if (mode == qInterface::read)
				{
					double value = systemData.locale.toDouble(lineEdit->text());
					SynchronizeInterfaceReadVect4d(nameVect, lastChar, value, par);
				}
				else if (mode == qInterface::write)
				{
					double value;
					SynchronizeInterfaceWriteVect4d(nameVect, lastChar, value, par);
					lineEdit->setText(QString("%L1").arg(value, 0, 'g', 16));
					lineEdit->setCursorPosition(0);
				}
			}

			//---------- get double scalars --------
			else if (props.typeName == QString("edit") || props.typeName == QString("logedit"))
			{
				if (mode == qInterface::read)
				{
					double value = systemData.locale.toDouble(lineEdit->text());
					par->Set(props.paramName, value);
				}
				else if (mode == qInterface::write)
				{
					double value = par->Get<double>(props.paramName);
					lineEdit->setText(QString("%L1").arg(value, 0, 'g', 16));
					lineEdit->setCursorPosition(0);
				}
			}

			//----------- get texts ------------
			else if (props.typeName == QString("text"))
			{
				if (mode == qInterface::read)
				{
					QString value = lineEdit->text();
					par->Set(props.paramName, value);
				}
				else if (mode == qInterface::write)
				{
					QString value = par->Get<QString>(props.paramName);
					lineEdit->setText(value);
				}
			}
		}
	} // end foreach
}

void SynchronizeInterfaceQDoubleSpinBox(
	QList<QDoubleSpinBox *> widgets, cParameterContainer *par, enumReadWrite mode)
{
	QList<QDoubleSpinBox *>::iterator it;
	for (it = widgets.begin(); it != widgets.end(); ++it)
	{
		widgetProperties props = parseWidgetProperties((*it), {"MyDoubleSpinBox", "QDoubleSpinBox"});
		if (props.allowed)
		{
			QDoubleSpinBox *spinbox = *it;

			if (props.className == QString("MyDoubleSpinBox"))
			{
				MyDoubleSpinBox *myDoubleSpinBox = static_cast<MyDoubleSpinBox *>(*it);
				myDoubleSpinBox->AssignParameterContainer(par);
				myDoubleSpinBox->AssignParameterName(props.paramName);
			}

			if (props.typeName == QString("spinbox") || props.typeName == QString("spinboxd"))
			{
				if (mode == qInterface::read)
				{
					double value = spinbox->value();
					par->Set(props.paramName, value);
				}
				else if (mode == qInterface::write)
				{
					double value = par->Get<double>(props.paramName);
					spinbox->setValue(value);
				}
			}
			else if (props.typeName == QString("spinbox3") || props.typeName == QString("spinboxd3"))
			{
				char lastChar = props.lastChar();
				QString nameVect = props.nameVect();
				if (mode == qInterface::read)
				{
					double value = spinbox->value();
					SynchronizeInterfaceReadVect3d(nameVect, lastChar, value, par);
				}
				else if (mode == qInterface::write)
				{
					double value = 0;
					SynchronizeInterfaceWriteVect3d(nameVect, lastChar, value, par);
					spinbox->setValue(value);
				}
			}
			else if (props.typeName == QString("spinbox4") || props.typeName == QString("spinboxd4"))
			{
				char lastChar = props.lastChar();
				QString nameVect = props.nameVect();
				if (mode == qInterface::read)
				{
					double value = spinbox->value();
					SynchronizeInterfaceReadVect4d(nameVect, lastChar, value, par);
				}
				else if (mode == qInterface::write)
				{
					double value = 0;
					SynchronizeInterfaceWriteVect4d(nameVect, lastChar, value, par);
					spinbox->setValue(value);
				}
			}
		}
	}
}

void SynchronizeInterfaceQSpinBox(
	QList<QSpinBox *> widgets, cParameterContainer *par, enumReadWrite mode)
{
	QList<QSpinBox *>::iterator it;
	for (it = widgets.begin(); it != widgets.end(); ++it)
	{
		widgetProperties props = parseWidgetProperties((*it), {"QSpinBox", "MySpinBox"});
		if (props.allowed)
		{
			QSpinBox *spinbox = *it;
			if (props.className == QString("MySpinBox"))
			{
				MySpinBox *mySpinBox = static_cast<MySpinBox *>(*it);
				mySpinBox->AssignParameterContainer(par);
				mySpinBox->AssignParameterName(props.paramName);
			}

			if (props.typeName == QString("spinboxInt"))
			{
				if (mode == qInterface::read)
				{
					int value = spinbox->value();
					par->Set(props.paramName, value);
				}
				else if (mode == qInterface::write)
				{
					int value = par->Get<int>(props.paramName);
					spinbox->setValue(value);
				}
			}
		}
	}
}

void SynchronizeInterfaceQCheckBox(
	QList<QCheckBox *> widgets, cParameterContainer *par, enumReadWrite mode)
{
	QList<QCheckBox *>::iterator it;
	for (it = widgets.begin(); it != widgets.end(); ++it)
	{
		widgetProperties props = parseWidgetProperties((*it), {"QCheckBox", "MyCheckBox"});
		if (props.allowed)
		{
			QCheckBox *checkbox = *it;
			if (props.className == QString("MyCheckBox"))
			{
				MyCheckBox *myCheckBox = static_cast<MyCheckBox *>(*it);
				myCheckBox->AssignParameterContainer(par);
				myCheckBox->AssignParameterName(props.paramName);
			}

			if (props.typeName == QString("checkBox"))
			{
				if (mode == qInterface::read)
				{
					bool value = checkbox->isChecked();
					par->Set(props.paramName, value);
				}
				else if (mode == qInterface::write)
				{
					bool value = par->Get<bool>(props.paramName);
					checkbox->setChecked(value);
				}
			}
		}
	}
}

void SynchronizeInterfaceQGroupBox(
	QList<QGroupBox *> widgets, cParameterContainer *par, enumReadWrite mode)
{
	QList<QGroupBox *>::iterator it;
	for (it = widgets.begin(); it != widgets.end(); ++it)
	{
		widgetProperties props = parseWidgetProperties((*it), {"QGroupBox", "MyGroupBox"});
		if (props.allowed)
		{
			QGroupBox *groupBbox = *it;
			if (props.className == QString("MyGroupBox"))
			{
				MyGroupBox *myGroupBox = static_cast<MyGroupBox *>(*it);
				myGroupBox->AssignParameterContainer(par);
				myGroupBox->AssignParameterName(props.paramName);
			}

			if (props.typeName == QString("groupCheck"))
			{
				if (mode == qInterface::read)
				{
					bool value = groupBbox->isChecked();
					par->Set(props.paramName, value);
				}
				else if (mode == qInterface::write)
				{
					bool value = par->Get<bool>(props.paramName);
					groupBbox->setChecked(value);
				}
			}
		}
	}
}

void SynchronizeInterfaceFileSelectWidget(
	QList<FileSelectWidget *> widgets, cParameterContainer *par, enumReadWrite mode)
{
	QList<FileSelectWidget *>::iterator it;
	for (it = widgets.begin(); it != widgets.end(); ++it)
	{
		widgetProperties props = parseWidgetProperties((*it), {"FileSelectWidget"});
		if (props.allowed)
		{
			FileSelectWidget *fileSelectWidget = *it;
			fileSelectWidget->AssignParameterContainer(par);
			fileSelectWidget->AssignParameterName(props.paramName);

			if (mode == qInterface::read)
			{
				par->Set(props.paramName, fileSelectWidget->GetPath());
			}
			else if (mode == qInterface::write)
			{
				fileSelectWidget->SetPath(par->Get<QString>(props.paramName));
			}
		}
	}
}

void SynchronizeInterfaceMyColorButton(
	QList<MyColorButton *> widgets, cParameterContainer *par, enumReadWrite mode)
{
	QList<MyColorButton *>::iterator it;
	for (it = widgets.begin(); it != widgets.end(); ++it)
	{
		widgetProperties props = parseWidgetProperties((*it), {"MyColorButton"});
		if (props.allowed)
		{
			MyColorButton *colorButton = *it;
			colorButton->AssignParameterContainer(par);
			colorButton->AssignParameterName(props.paramName);

			if (mode == qInterface::read)
			{
				par->Set(props.paramName, colorButton->GetColor());
			}
			else if (mode == qInterface::write)
			{
				colorButton->setText("");
				colorButton->SetColor(par->Get<sRGB>(props.paramName));
			}
		}
	}
}

void SynchronizeInterfaceQComboBox(
	QList<QComboBox *> widgets, cParameterContainer *par, enumReadWrite mode)
{
	QList<QComboBox *>::iterator it;
	for (it = widgets.begin(); it != widgets.end(); ++it)
	{
		widgetProperties props =
			parseWidgetProperties((*it), {"QComboBox", "MyComboBox", "cFormulaComboBox"});
		if (props.allowed)
		{
			QComboBox *comboBox = *it;

			if (props.className == QString("MyComboBox"))
			{
				MyComboBox *myComboBox = static_cast<MyComboBox *>(*it);
				myComboBox->AssignParameterContainer(par);
				myComboBox->AssignParameterName(props.paramName);
			}

			if (props.typeName == QString("comboBox"))
			{
				if (mode == qInterface::read)
				{
					int selection = comboBox->currentIndex();
					if (props.paramName.left(7) == QString("formula"))
					{
						selection = fractalList[comboBox->itemData(selection).toInt()].internalID;
					}
					par->Set(props.paramName, selection);
				}
				else if (mode == qInterface::write)
				{
					int selection = par->Get<int>(props.paramName);
					if (props.paramName.left(7) == QString("formula"))
					{
						cFormulaComboBox *formulaComboBox = dynamic_cast<cFormulaComboBox *>(comboBox);
						if (formulaComboBox)
						{
							formulaComboBox->AssignParameterContainer(par);
							formulaComboBox->AssignParameterName(props.paramName);
							for (int i = 0; i < fractalList.size(); i++)
							{
								if (fractalList[i].internalID == selection)
								{
									selection = comboBox->findData(i);
									break;
								}
							}
						}
					}
					comboBox->setCurrentIndex(selection);
				}
			}
		}
	}
}

void SynchronizeInterfaceMaterialSelector(
	QList<cMaterialSelector *> widgets, cParameterContainer *par, enumReadWrite mode)
{
	QList<cMaterialSelector *>::iterator it;
	for (it = widgets.begin(); it != widgets.end(); ++it)
	{
		widgetProperties props = parseWidgetProperties((*it), {"cMaterialSelector"});
		if (props.allowed)
		{
			cMaterialSelector *materialSelector = *it;
			materialSelector->AssignParameterContainer(par);
			materialSelector->AssignParameterName(props.paramName);

			if (props.typeName == QString("materialselector"))
			{
				if (mode == qInterface::read)
				{
					par->Set(props.paramName, materialSelector->GetMaterialIndex());
				}
				else if (mode == qInterface::write)
				{
					materialSelector->SetMaterialIndex(par->Get<int>(props.paramName));
				}
			}
		}
	}
}

void SynchronizeInterfaceQPlainTextEdit(
	QList<QPlainTextEdit *> widgets, cParameterContainer *par, enumReadWrite mode)
{
	QList<QPlainTextEdit *>::iterator it;
	for (it = widgets.begin(); it != widgets.end(); ++it)
	{
		widgetProperties props = parseWidgetProperties((*it), {"QPlainTextEdit"});
		if (props.allowed)
		{
			QPlainTextEdit *textEdit = *it;

			//----------- get texts ------------
			if (props.typeName == QString("text"))
			{
				if (mode == qInterface::read)
				{
					QString value = textEdit->toPlainText();
					par->Set(props.paramName, value);
				}
				else if (mode == qInterface::write)
				{
					QString value = par->Get<QString>(props.paramName);
					textEdit->setPlainText(value);
				}
			}
		}
	} // end foreach
}

void SynchronizeInterfaceColorGradientWidget(
	QList<cGradientEditWidget *> widgets, cParameterContainer *par, qInterface::enumReadWrite mode)
{
	QList<cGradientEditWidget *>::iterator it;
	for (it = widgets.begin(); it != widgets.end(); ++it)
	{
		widgetProperties props = parseWidgetProperties((*it), {"cGradientEditWidget"});
		if (props.allowed)
		{
			cGradientEditWidget *colorGradientWidget = static_cast<cGradientEditWidget *>(*it);
			colorGradientWidget->AssignParameterContainer(par);
			colorGradientWidget->AssignParameterName(props.paramName);

			if (props.typeName == QString("colorpalette"))
			{
				if (mode == qInterface::read)
				{
					QString palette = colorGradientWidget->GetColors();
					par->Set(props.paramName, palette);
				}
				else if (mode == qInterface::write)
				{
					QString palette = par->Get<QString>(props.paramName);
					colorGradientWidget->SetColors(palette);
				}
			}
		}
	}
}

// utility functions

// extract name and type string from widget name
void GetNameAndType(QString name, QString *parameterName, QString *type)
{
	int firstDashPosition = name.indexOf("_");
	*type = name.left(firstDashPosition);
	*parameterName = name.mid(firstDashPosition + 1);
}

void SynchronizeInterfaceReadVect3d(
	QString &nameVect, char lastChar, double value, cParameterContainer *par)
{
	CVector3 vect = par->Get<CVector3>(nameVect);
	switch (lastChar)
	{
		case 'x': vect.x = value; break;
		case 'y': vect.y = value; break;
		case 'z': vect.z = value; break;
		default:
			qWarning() << "cInterface::SynchronizeInterfaceReadVect3d(): edit field " << nameVect
								 << " has wrong axis name (is " << lastChar << ")" << endl;
			break;
	}
	par->Set(nameVect, vect);
}

void SynchronizeInterfaceWriteVect3d(
	QString &nameVect, char lastChar, double &out, cParameterContainer *par)
{
	CVector3 vect = par->Get<CVector3>(nameVect);
	switch (lastChar)
	{
		case 'x': out = vect.x; break;
		case 'y': out = vect.y; break;
		case 'z': out = vect.z; break;
		default:
			qWarning() << "cInterface::SynchronizeInterfaceWriteVect3d(): edit field " << nameVect
								 << " has wrong axis name (is " << lastChar << ")" << endl;
			break;
	}
}

void SynchronizeInterfaceReadVect4d(
	QString &nameVect, char lastChar, double value, cParameterContainer *par)
{
	CVector4 vect = par->Get<CVector4>(nameVect);
	switch (lastChar)
	{
		case 'x': vect.x = value; break;
		case 'y': vect.y = value; break;
		case 'z': vect.z = value; break;
		case 'w': vect.w = value; break;
		default:
			qWarning() << "cInterface::SynchronizeInterfaceReadVect4d(): edit field " << nameVect
								 << " has wrong axis name (is " << lastChar << ")" << endl;
			break;
	}
	par->Set(nameVect, vect);
}

void SynchronizeInterfaceWriteVect4d(
	QString &nameVect, char lastChar, double &out, cParameterContainer *par)
{
	CVector4 vect = par->Get<CVector4>(nameVect);
	switch (lastChar)
	{
		case 'x': out = vect.x; break;
		case 'y': out = vect.y; break;
		case 'z': out = vect.z; break;
		case 'w': out = vect.w; break;
		default:
			qWarning() << "cInterface::SynchronizeInterfaceWriteVect4d(): edit field " << nameVect
								 << " has wrong axis name (is " << lastChar << ")" << endl;
			break;
	}
}

widgetProperties parseWidgetProperties(QWidget *widget, QStringList allowedClassNames)
{
	widgetProperties props;
	props.name = widget->objectName();
	props.className = widget->metaObject()->className();
	props.allowed = (props.name.length() > 1 && (allowedClassNames.contains(props.className)));
	int firstDashPosition = props.name.indexOf("_");
	props.typeName = props.name.left(firstDashPosition);
	props.paramName = props.name.mid(firstDashPosition + 1);
	return props;
}
