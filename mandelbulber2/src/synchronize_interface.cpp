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
 * Authors: Krzysztof Marczak (buddhi1980@gmail.com)
 *
 * Reading ad writing parameters from/to selected widget to/from parameters container
 */

#include "synchronize_interface.hpp"

#include "../qt/color_palette_widget.h"
#include "../qt/file_select_widget.h"
#include "../qt/material_selector.h"
#include "../qt/my_check_box.h"
#include "../qt/my_color_button.h"
#include "../qt/my_double_spin_box.h"
#include "../qt/my_group_box.h"
#include "../qt/my_line_edit.h"
#include "../qt/my_spin_box.h"
#include "fractal_list.hpp"
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPlainTextEdit>
#include "algebra.hpp"
#include "parameters.hpp"

using namespace qInterface;

// Reading ad writing parameters from/to selected widget to/from parameters container
void SynchronizeInterfaceWindow(QWidget *window, cParameterContainer *par, enumReadWrite mode)
{
	WriteLog("cInterface::SynchronizeInterface: QLineEdit", 3);
	//----------- QLineEdit -------------------
	{
		QList<QLineEdit *> widgetListLineEdit = window->findChildren<QLineEdit *>();
		QList<QLineEdit *>::iterator it;

		for (it = widgetListLineEdit.begin(); it != widgetListLineEdit.end(); ++it)
		{
			// qDebug() << "QLineEdit:" << (*it)->objectName() << " Type:" <<
			// (*it)->metaObject()->className() << endl;

			QString name = (*it)->objectName();
			QString className = (*it)->metaObject()->className();
			if (name.length() > 1
					&& (className == QString("QLineEdit") || className == QString("MyLineEdit")))
			{
				QLineEdit *lineEdit = *it;
				// QString text = lineEdit->text();
				// qDebug() << name << " - text: " << text << endl;

				QString type, parameterName;
				GetNameAndType(name, &parameterName, &type);
				// qDebug() << name << " - type: " << type << endl;

				if (className == QString("MyLineEdit"))
				{
					MyLineEdit *mylineedit = static_cast<MyLineEdit *>(*it);
					mylineedit->AssignParameterContainer(par);
					mylineedit->AssignParameterName(parameterName);
				}

				//----- get vectors ------------
				if (type == QString("vect3") || type == QString("logvect3"))
				{
					char lastChar = parameterName.at(parameterName.length() - 1).toLatin1();
					QString nameVect = parameterName.left(parameterName.length() - 2);

					if (mode == read)
					{
						double value = systemData.locale.toDouble(lineEdit->text());
						// qDebug() << nameVect << " - " << lastChar << " axis = " << value << endl;
						CVector3 vect = par->Get<CVector3>(nameVect);

						switch (lastChar)
						{
							case 'x': vect.x = value; break;

							case 'y': vect.y = value; break;

							case 'z': vect.z = value; break;

							default:
								qWarning() << "cInterface::SynchronizeInterfaceWindow(): edit field " << nameVect
													 << " has wrong axis name (is " << lastChar << ")" << endl;
								break;
						}
						par->Set(nameVect, vect);
					}
					else if (mode == write)
					{
						CVector3 vect = par->Get<CVector3>(nameVect);
						QString qtext;

						switch (lastChar)
						{
							case 'x': qtext = QString("%L1").arg(vect.x, 0, 'g', 16); break;

							case 'y': qtext = QString("%L1").arg(vect.y, 0, 'g', 16); break;

							case 'z': qtext = QString("%L1").arg(vect.z, 0, 'g', 16); break;

							default:
								qWarning() << "cInterface::SynchronizeInterfaceWindow(): edit field " << nameVect
													 << " has wrong axis name (is " << lastChar << ")" << endl;
								break;
						}
						lineEdit->setText(qtext);
						lineEdit->setCursorPosition(0);
					}
				}

				//----- get vectors 4D  ------------
				if (type == QString("vect4"))
				{
					char lastChar = parameterName.at(parameterName.length() - 1).toLatin1();
					QString nameVect = parameterName.left(parameterName.length() - 2);

					if (mode == read)
					{
						double value = systemData.locale.toDouble(lineEdit->text());
						// qDebug() << nameVect << " - " << lastChar << " axis = " << value << endl;
						CVector4 vect = par->Get<CVector4>(nameVect);

						switch (lastChar)
						{
							case 'x': vect.x = value; break;

							case 'y': vect.y = value; break;

							case 'z': vect.z = value; break;

							case 'w': vect.w = value; break;

							default:
								qWarning() << "cInterface::SynchronizeInterfaceWindow(): edit field " << nameVect
													 << " has wrong axis name (is " << lastChar << ")" << endl;
								break;
						}
						par->Set(nameVect, vect);
					}
					else if (mode == write)
					{
						CVector4 vect = par->Get<CVector4>(nameVect);
						QString qtext;

						switch (lastChar)
						{
							case 'x': qtext = QString("%L1").arg(vect.x, 0, 'g', 16); break;

							case 'y': qtext = QString("%L1").arg(vect.y, 0, 'g', 16); break;

							case 'z': qtext = QString("%L1").arg(vect.z, 0, 'g', 16); break;

							case 'w': qtext = QString("%L1").arg(vect.w, 0, 'g', 16); break;

							default:
								qWarning() << "cInterface::SynchronizeInterfaceWindow(): edit field " << nameVect
													 << " has wrong axis name (is " << lastChar << ")" << endl;
								break;
						}
						lineEdit->setText(qtext);
						lineEdit->setCursorPosition(0);
					}
				}

				//---------- get double scalars --------
				else if (type == QString("edit") || type == QString("logedit"))
				{
					if (mode == read)
					{
						double value = systemData.locale.toDouble(lineEdit->text());
						par->Set(parameterName, value);
					}
					else if (mode == write)
					{
						double value = par->Get<double>(parameterName);
						lineEdit->setText(QString("%L1").arg(value, 0, 'g', 16));
						lineEdit->setCursorPosition(0);
					}
				}

				//----------- get texts ------------
				else if (type == QString("text"))
				{
					if (mode == read)
					{
						QString value = lineEdit->text();
						par->Set(parameterName, value);
					}
					else if (mode == write)
					{
						QString value = par->Get<QString>(parameterName);
						lineEdit->setText(value);
					}
				}
			}
		} // end foreach
	}

	WriteLog("cInterface::SynchronizeInterface: QDoubleSpinBox", 3);
	//------------ Double spin-box --------------
	{
		QList<QDoubleSpinBox *> widgetListDoubleSpinBox = window->findChildren<QDoubleSpinBox *>();
		QList<QDoubleSpinBox *>::iterator it;
		for (it = widgetListDoubleSpinBox.begin(); it != widgetListDoubleSpinBox.end(); ++it)
		{
			QString name = (*it)->objectName();
			// qDebug() << "QDoubleSpinBox:" << (*it)->objectName() << " Type:" <<
			// (*it)->metaObject()->className() << endl;
			QString className = (*it)->metaObject()->className();
			if (name.length() > 1
					&& (className == QString("QDoubleSpinBox") || className == QString("MyDoubleSpinBox")))
			{
				QDoubleSpinBox *spinbox = *it;

				QString type, parameterName;
				GetNameAndType(name, &parameterName, &type);

				if (className == QString("MyDoubleSpinBox"))
				{
					MyDoubleSpinBox *mydoublespinbox = static_cast<MyDoubleSpinBox *>(*it);
					mydoublespinbox->AssignParameterContainer(par);
					mydoublespinbox->AssignParameterName(parameterName);
				}

				if (type == QString("spinbox") || type == QString("spinboxd"))
				{
					if (mode == read)
					{
						double value = spinbox->value();
						par->Set(parameterName, value);
					}
					else if (mode == write)
					{
						double value = par->Get<double>(parameterName);
						spinbox->setValue(value);
					}
				}
				else if (type == QString("spinbox3") || type == QString("spinboxd3"))
				{
					char lastChar = parameterName.at(parameterName.length() - 1).toLatin1();
					QString nameVect = parameterName.left(parameterName.length() - 2);
					if (mode == read)
					{
						double value = spinbox->value();
						CVector3 vect = par->Get<CVector3>(nameVect);

						switch (lastChar)
						{
							case 'x': vect.x = value; break;

							case 'y': vect.y = value; break;

							case 'z': vect.z = value; break;

							default:
								qWarning() << "cInterface::SynchronizeInterfaceWindow(): " << type << " "
													 << nameVect << " has wrong axis name (is " << lastChar << ")" << endl;
								break;
						}
						par->Set(nameVect, vect);
					}
					else if (mode == write)
					{
						CVector3 vect = par->Get<CVector3>(nameVect);
						double value = 0;

						switch (lastChar)
						{
							case 'x': value = vect.x; break;

							case 'y': value = vect.y; break;

							case 'z': value = vect.z; break;

							default:
								qWarning() << "cInterface::SynchronizeInterfaceWindow(): " << type << " "
													 << nameVect << " has wrong axis name (is " << lastChar << ")" << endl;
								break;
						}
						spinbox->setValue(value);
					}
				}
				else if (type == QString("spinbox4") || type == QString("spinboxd4"))
				{
					char lastChar = parameterName.at(parameterName.length() - 1).toLatin1();
					QString nameVect = parameterName.left(parameterName.length() - 2);
					if (mode == read)
					{
						double value = spinbox->value();
						CVector4 vect = par->Get<CVector4>(nameVect);

						switch (lastChar)
						{
							case 'x': vect.x = value; break;

							case 'y': vect.y = value; break;

							case 'z': vect.z = value; break;

							case 'w': vect.w = value; break;

							default:
								qWarning() << "cInterface::SynchronizeInterfaceWindow(): " << type << " "
													 << nameVect << " has wrong axis name (is " << lastChar << ")" << endl;
								break;
						}
						par->Set(nameVect, vect);
					}
					else if (mode == write)
					{
						CVector4 vect = par->Get<CVector4>(nameVect);
						double value = 0;

						switch (lastChar)
						{
							case 'x': value = vect.x; break;

							case 'y': value = vect.y; break;

							case 'z': value = vect.z; break;

							case 'w': value = vect.w; break;

							default:
								qWarning() << "cInterface::SynchronizeInterfaceWindow(): " << type << " "
													 << nameVect << " has wrong axis name (is " << lastChar << ")" << endl;
								break;
						}
						spinbox->setValue(value);
					}
				}
			}
		}
	}

	WriteLog("cInterface::SynchronizeInterface: QSpinBox", 3);
	//------------ integer spin-box --------------
	{
		QList<QSpinBox *> widgetListDoubleSpinBox = window->findChildren<QSpinBox *>();
		QList<QSpinBox *>::iterator it;
		for (it = widgetListDoubleSpinBox.begin(); it != widgetListDoubleSpinBox.end(); ++it)
		{
			QString name = (*it)->objectName();
			// qDebug() << "QDoubleSpinBox:" << (*it)->objectName() << " Type:" <<
			// (*it)->metaObject()->className() << endl;
			QString className = (*it)->metaObject()->className();
			if (name.length() > 1
					&& (className == QString("QSpinBox") || className == QString("MySpinBox")))
			{
				QSpinBox *spinbox = *it;
				QString type, parameterName;
				GetNameAndType(name, &parameterName, &type);

				if (className == QString("MySpinBox"))
				{
					MySpinBox *myspinbox = static_cast<MySpinBox *>(*it);
					myspinbox->AssignParameterContainer(par);
					myspinbox->AssignParameterName(parameterName);
				}

				if (type == QString("spinboxInt"))
				{
					if (mode == read)
					{
						int value = spinbox->value();
						par->Set(parameterName, value);
					}
					else if (mode == write)
					{
						int value = par->Get<int>(parameterName);
						spinbox->setValue(value);
					}
				}
			}
		}
	}

	WriteLog("cInterface::SynchronizeInterface: QCheckBox", 3);
	// checkboxes
	{
		QList<QCheckBox *> widgetListDoubleSpinBox = window->findChildren<QCheckBox *>();
		QList<QCheckBox *>::iterator it;
		for (it = widgetListDoubleSpinBox.begin(); it != widgetListDoubleSpinBox.end(); ++it)
		{
			QString name = (*it)->objectName();
			// qDebug() << "QCheckBox:" << (*it)->objectName() << " Type:" <<
			// (*it)->metaObject()->className() << endl;
			QString className = (*it)->metaObject()->className();
			if (name.length() > 1
					&& (className == QString("QCheckBox") || className == QString("MyCheckBox")))
			{
				QCheckBox *checkbox = *it;

				QString type, parameterName;
				GetNameAndType(name, &parameterName, &type);

				if (className == QString("MyCheckBox"))
				{
					MyCheckBox *mycheckbox = static_cast<MyCheckBox *>(*it);
					mycheckbox->AssignParameterContainer(par);
					mycheckbox->AssignParameterName(parameterName);
				}

				if (type == QString("checkBox"))
				{
					if (mode == read)
					{
						bool value = checkbox->isChecked();
						par->Set(parameterName, value);
					}
					else if (mode == write)
					{
						bool value = par->Get<bool>(parameterName);
						checkbox->setChecked(value);
					}
				}
			}
		}
	}

	WriteLog("cInterface::SynchronizeInterface: QGroupBox", 3);
	// groupsBox with checkbox
	{
		QList<QGroupBox *> widgetListDoubleSpinBox = window->findChildren<QGroupBox *>();
		QList<QGroupBox *>::iterator it;
		for (it = widgetListDoubleSpinBox.begin(); it != widgetListDoubleSpinBox.end(); ++it)
		{
			QString name = (*it)->objectName();
			// qDebug() << "QGroupBox:" << (*it)->objectName() << " Type:" <<
			// (*it)->metaObject()->className() << endl;
			QString className = (*it)->metaObject()->className();
			if (name.length() > 1
					&& (className == QString("QGroupBox") || className == QString("MyGroupBox")))
			{
				QGroupBox *groupbox = *it;

				QString type, parameterName;
				GetNameAndType(name, &parameterName, &type);

				if (className == QString("MyGroupBox"))
				{
					MyGroupBox *mygroupbox = static_cast<MyGroupBox *>(*it);
					mygroupbox->AssignParameterContainer(par);
					mygroupbox->AssignParameterName(parameterName);
				}

				if (type == QString("groupCheck"))
				{
					if (mode == read)
					{
						bool value = groupbox->isChecked();
						par->Set(parameterName, value);
					}
					else if (mode == write)
					{
						bool value = par->Get<bool>(parameterName);
						groupbox->setChecked(value);
					}
				}
			}
		}
	}

	WriteLog("cInterface::SynchronizeInterface: FileSelectWidget", 3);
	//---------- file select widgets -----------
	{
		QList<FileSelectWidget *> widgetListPushButton = window->findChildren<FileSelectWidget *>();
		QList<FileSelectWidget *>::iterator it;
		for (it = widgetListPushButton.begin(); it != widgetListPushButton.end(); ++it)
		{
			QString name = (*it)->objectName();
			// QString className = (*it)->metaObject()->className();
			if (name.length() > 1 && (*it)->metaObject()->className() == QString("FileSelectWidget"))
			{
				QString type, parameterName;
				GetNameAndType(name, &parameterName, &type);

				FileSelectWidget *fileSelectWidget = *it;
				fileSelectWidget->AssignParameterContainer(par);
				fileSelectWidget->AssignParameterName(parameterName);

				if (mode == read)
				{
					par->Set(parameterName, fileSelectWidget->GetPath());
				}
				else if (mode == write)
				{
					fileSelectWidget->SetPath(par->Get<QString>(parameterName));
				}
			}
		}
	}

	WriteLog("cInterface::SynchronizeInterface: MyColorButton", 3);
	//---------- color buttons -----------
	{
		QList<MyColorButton *> widgetListPushButton = window->findChildren<MyColorButton *>();
		QList<MyColorButton *>::iterator it;
		for (it = widgetListPushButton.begin(); it != widgetListPushButton.end(); ++it)
		{
			QString name = (*it)->objectName();
			// QString className = (*it)->metaObject()->className();
			if (name.length() > 1 && (*it)->metaObject()->className() == QString("MyColorButton"))
			{
				QString type, parameterName;
				GetNameAndType(name, &parameterName, &type);

				MyColorButton *colorButton = *it;
				colorButton->AssignParameterContainer(par);
				colorButton->AssignParameterName(parameterName);

				if (mode == read)
				{
					par->Set(parameterName, colorButton->GetColor());
				}
				else if (mode == write)
				{
					colorButton->setText("");
					colorButton->SetColor(par->Get<sRGB>(parameterName));
				}
			}
		}
	}

	WriteLog("cInterface::SynchronizeInterface: ColorPaletteWidget", 3);
	//---------- colorpalette -----------
	{
		QList<ColorPaletteWidget *> widgetListColorPalette =
			window->findChildren<ColorPaletteWidget *>();
		QList<ColorPaletteWidget *>::iterator it;
		for (it = widgetListColorPalette.begin(); it != widgetListColorPalette.end(); ++it)
		{
			QString name = (*it)->objectName();
			// qDebug() << "ColorPalette:" << (*it)->objectName() << " Type:" <<
			// (*it)->metaObject()->className() << endl;
			if (name.length() > 1 && (*it)->metaObject()->className() == QString("ColorPaletteWidget"))
			{
				ColorPaletteWidget *colorPaletteWidget = *it;

				QString type, parameterName;
				GetNameAndType(name, &parameterName, &type);

				colorPaletteWidget->AssignParameterContainer(par);
				colorPaletteWidget->AssignParameterName(parameterName);

				if (type == QString("colorpalette"))
				{
					if (mode == read)
					{
						cColorPalette palette = colorPaletteWidget->GetPalette();
						par->Set(parameterName, palette);
					}
					else if (mode == write)
					{
						cColorPalette palette = par->Get<cColorPalette>(parameterName);
						colorPaletteWidget->SetPalette(palette);
					}
				}
			}
		}
	}

	WriteLog("cInterface::SynchronizeInterface: QComboBox", 3);
	// combo boxes
	{
		QList<QComboBox *> widgetListPushButton = window->findChildren<QComboBox *>();
		QList<QComboBox *>::iterator it;
		for (it = widgetListPushButton.begin(); it != widgetListPushButton.end(); ++it)
		{
			QString name = (*it)->objectName();
			// qDebug() << "QComboBox:" << (*it)->objectName() << " Type:" <<
			// (*it)->metaObject()->className() << endl;
			if (name.length() > 1 && (*it)->metaObject()->className() == QString("QComboBox"))
			{
				QComboBox *comboBox = *it;

				QString type, parameterName;
				GetNameAndType(name, &parameterName, &type);

				if (type == QString("comboBox"))
				{
					if (mode == read)
					{
						int selection = comboBox->currentIndex();
						if (parameterName.left(7) == QString("formula"))
						{
							selection = fractalList[comboBox->itemData(selection).toInt()].internalID;
						}
						par->Set(parameterName, selection);
					}
					else if (mode == write)
					{
						int selection = par->Get<int>(parameterName);
						if (parameterName.left(7) == QString("formula"))
						{
							for (int i = 0; i < fractalList.size(); i++)
							{
								if (fractalList[i].internalID == selection)
								{
									selection = comboBox->findData(i);
									break;
								}
							}
						}
						comboBox->setCurrentIndex(selection);
					}
				}
			}
		}
	}

	WriteLog("cInterface::SynchronizeInterface: cMaterialSelector", 3);
	//---------- material selector -----------
	{
		QList<cMaterialSelector *> widgetListMaterialSelector =
			window->findChildren<cMaterialSelector *>();
		QList<cMaterialSelector *>::iterator it;
		for (it = widgetListMaterialSelector.begin(); it != widgetListMaterialSelector.end(); ++it)
		{
			QString name = (*it)->objectName();
			// QString className = (*it)->metaObject()->className();
			if (name.length() > 1 && (*it)->metaObject()->className() == QString("cMaterialSelector"))
			{
				QString type, parameterName;
				GetNameAndType(name, &parameterName, &type);

				cMaterialSelector *materialSelector = *it;
				materialSelector->AssignParameterContainer(par);
				materialSelector->AssignParameterName(parameterName);

				if (type == QString("materialselector"))
				{
					if (mode == read)
					{
						par->Set(parameterName, materialSelector->GetMaterialIndex());
					}
					else if (mode == write)
					{
						materialSelector->SetMaterialIndex(par->Get<int>(parameterName));
					}
				}
			}
		}
	}

	WriteLog("cInterface::SynchronizeInterface: QPlainTextEdit", 3);
	//----------- QPlainTextEdit -------------------
	{
		QList<QPlainTextEdit *> widgetListLineEdit = window->findChildren<QPlainTextEdit *>();
		QList<QPlainTextEdit *>::iterator it;

		for (it = widgetListLineEdit.begin(); it != widgetListLineEdit.end(); ++it)
		{
			QString name = (*it)->objectName();
			QString className = (*it)->metaObject()->className();
			if (name.length() > 1 && className == QString("QPlainTextEdit"))
			{
				QPlainTextEdit *textEdit = *it;

				QString type, parameterName;
				GetNameAndType(name, &parameterName, &type);

				//----------- get texts ------------
				if (type == QString("text"))
				{
					if (mode == read)
					{
						QString value = textEdit->toPlainText();
						par->Set(parameterName, value);
					}
					else if (mode == write)
					{
						QString value = par->Get<QString>(parameterName);
						textEdit->setPlainText(value);
					}
				}
			}
		} // end foreach
	}

	WriteLog("cInterface::SynchronizeInterface: Done", 3);
}

// extract name and type string from widget name
void GetNameAndType(QString name, QString *parameterName, QString *type)
{
	int firstDashPosition = name.indexOf("_");
	*type = name.left(firstDashPosition);
	*parameterName = name.mid(firstDashPosition + 1);
}
