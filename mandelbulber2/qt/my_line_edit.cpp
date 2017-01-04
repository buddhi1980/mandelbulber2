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
 * MyLineEdit class - promoted MyLineEdit widget with context menu
 */

#include "my_line_edit.h"

#include <qapplication.h>
#include <QMenu>

#include "../src/animation_flight.hpp"
#include "../src/animation_keyframes.hpp"

MyLineEdit::MyLineEdit(QWidget *parent) : QLineEdit(parent), CommonMyWidgetWrapper(this)
{
	actionResetVectorToDefault = nullptr;
	actionCopyVectorToClipboard = nullptr;
	actionPasteVectorFromClipboard = nullptr;
}

void MyLineEdit::paintEvent(QPaintEvent *event)
{
	QFont f = font();
	f.setBold(text() != GetDefault());
	setFont(f);
	QLineEdit::paintEvent(event);
}

QString MyLineEdit::GetDefault()
{
	if (parameterContainer && !gotDefault)
	{
		QString type = GetType(objectName());
		if (type == QString("vect3") || type == QString("logvect3"))
		{
			char lastChar = (parameterName.at(parameterName.length() - 1)).toLatin1();
			QString nameVect = parameterName.left(parameterName.length() - 2);
			CVector3 val = parameterContainer->GetDefault<CVector3>(nameVect);
			QString valS = QString("%L1").arg(val.itemByName(lastChar), 0, 'g', 16);
			defaultText = valS;
			gotDefault = true;
		}
		else if (type == QString("vect4") || type == QString("logvect3"))
		{
			char lastChar = (parameterName.at(parameterName.length() - 1)).toLatin1();
			QString nameVect = parameterName.left(parameterName.length() - 2);
			CVector4 val = parameterContainer->GetDefault<CVector4>(nameVect);
			QString valS = QString("%L1").arg(val.itemByName(lastChar), 0, 'g', 16);
			defaultText = valS;
			gotDefault = true;
		}
		else
		{
			QString val = parameterContainer->GetDefault<QString>(parameterName);
			defaultText = val;
			gotDefault = true;
		}

		setToolTipText();
	}
	return defaultText;
}

void MyLineEdit::CopyToClipboard() const
{
	if (parameterContainer)
	{
		QString type = GetType(objectName());
		if (type == QString("vect3") || type == QString("vect4") || type == QString("logvect3"))
		{
			QString nameVect = parameterName.left(parameterName.length() - 2);
			QString valS = parameterContainer->Get<QString>(nameVect);
			QClipboard *clipboard = QApplication::clipboard();
			clipboard->setText(valS);
		}
	}
}

void MyLineEdit::PasteFromClipboard()
{
	QClipboard *clipboard = QApplication::clipboard();
	QString text = clipboard->text();

	if (parameterContainer)
	{
		QString type = GetType(objectName());
		if (type == QString("vect3") || type == QString("logvect3"))
		{
			QStringList numbersStr = text.split(' ');
			if (numbersStr.length() == 3)
			{
				QString nameVect = parameterName.left(parameterName.length() - 2);
				QString lineEditNameX, lineEditNameY, lineEditNameZ;
				if (type == QString("vect3"))
				{
					lineEditNameX = QString("vect3_%1_x").arg(nameVect);
					lineEditNameY = QString("vect3_%1_y").arg(nameVect);
					lineEditNameZ = QString("vect3_%1_z").arg(nameVect);
				}
				else if (type == QString("logvect3"))
				{
					lineEditNameX = QString("logvect3_%1_x").arg(nameVect);
					lineEditNameY = QString("logvect3_%1_y").arg(nameVect);
					lineEditNameZ = QString("logvect3_%1_z").arg(nameVect);
				}
				MyLineEdit *lineEditX = parent()->findChild<MyLineEdit *>(lineEditNameX);
				MyLineEdit *lineEditY = parent()->findChild<MyLineEdit *>(lineEditNameY);
				MyLineEdit *lineEditZ = parent()->findChild<MyLineEdit *>(lineEditNameZ);
				if (lineEditX) lineEditX->setText(numbersStr[0]);
				if (lineEditY) lineEditY->setText(numbersStr[1]);
				if (lineEditZ) lineEditZ->setText(numbersStr[2]);
			}
			emit editingFinished();
		}

		if (type == QString("vect4"))
		{
			QStringList numbersStr = text.split(' ');
			if (numbersStr.length() == 4)
			{
				QString nameVect = parameterName.left(parameterName.length() - 2);
				QString lineEditNameX = QString("vect4_%1_x").arg(nameVect);
				QString lineEditNameY = QString("vect4_%1_y").arg(nameVect);
				QString lineEditNameZ = QString("vect4_%1_z").arg(nameVect);
				QString lineEditNameW = QString("vect4_%1_w").arg(nameVect);
				MyLineEdit *lineEditX = parent()->findChild<MyLineEdit *>(lineEditNameX);
				MyLineEdit *lineEditY = parent()->findChild<MyLineEdit *>(lineEditNameY);
				MyLineEdit *lineEditZ = parent()->findChild<MyLineEdit *>(lineEditNameZ);
				MyLineEdit *lineEditW = parent()->findChild<MyLineEdit *>(lineEditNameW);
				if (lineEditX) lineEditX->setText(numbersStr[0]);
				if (lineEditY) lineEditY->setText(numbersStr[1]);
				if (lineEditZ) lineEditZ->setText(numbersStr[2]);
				if (lineEditW) lineEditW->setText(numbersStr[3]);
			}
			emit editingFinished();
		}
	}
}

void MyLineEdit::ResetVectorToDefault()
{
	if (parameterContainer)
	{
		QString type = GetType(objectName());
		if (type == QString("vect3") || type == QString("vect4") || type == QString("logvect3"))
		{
			QString nameVect = parameterName.left(parameterName.length() - 2);
			QStringList numbersStr = parameterContainer->GetDefault<QString>(nameVect).split(' ');
			if (numbersStr.length() >= 3)
			{
				QString lineEditNameX, lineEditNameY, lineEditNameZ;
				if (type == QString("vect3"))
				{
					lineEditNameX = QString("vect3_%1_x").arg(nameVect);
					lineEditNameY = QString("vect3_%1_y").arg(nameVect);
					lineEditNameZ = QString("vect3_%1_z").arg(nameVect);
				}
				else if (type == QString("logvect3"))
				{
					lineEditNameX = QString("logvect3_%1_x").arg(nameVect);
					lineEditNameY = QString("logvect3_%1_y").arg(nameVect);
					lineEditNameZ = QString("logvect3_%1_z").arg(nameVect);
				}
				MyLineEdit *lineEditX = parent()->findChild<MyLineEdit *>(lineEditNameX);
				MyLineEdit *lineEditY = parent()->findChild<MyLineEdit *>(lineEditNameY);
				MyLineEdit *lineEditZ = parent()->findChild<MyLineEdit *>(lineEditNameZ);
				if (lineEditX) lineEditX->setText(numbersStr[0]);
				if (lineEditY) lineEditY->setText(numbersStr[1]);
				if (lineEditZ) lineEditZ->setText(numbersStr[2]);
			}
			if (type == QString("vect4") && numbersStr.length() == 4)
			{
				QString lineEditNameW = QString("vect3_%1_w").arg(nameVect);
				MyLineEdit *lineEditW = parent()->findChild<MyLineEdit *>(lineEditNameW);
				if (lineEditW) lineEditW->setText(numbersStr[3]);
			}
			emit editingFinished();
		}
	}
}

void MyLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
	QMenu *menu = createStandardContextMenu();

	QString type = GetType(objectName());
	if (type == QString("vect3") || type == QString("vect4") || type == QString("logvect3"))
	{
		menu->addSeparator();
		actionCopyVectorToClipboard = menu->addAction(tr("Copy vector"));
		actionPasteVectorFromClipboard = menu->addAction(tr("Paste vector"));
		actionResetVectorToDefault = menu->addAction(tr("Reset vector to default"));

		QIcon iconReset = QIcon(":system/icons/edit-undo.png");
		actionResetVectorToDefault->setIcon(iconReset);
		QIcon iconCopy = QIcon::fromTheme("edit-copy");
		actionCopyVectorToClipboard->setIcon(iconCopy);
		QIcon iconPaste = QIcon::fromTheme("edit-paste");
		actionPasteVectorFromClipboard->setIcon(iconPaste);

		connect(actionCopyVectorToClipboard, SIGNAL(triggered()), this, SLOT(slotCopyVector()));
		connect(actionPasteVectorFromClipboard, SIGNAL(triggered()), this, SLOT(slotPasteVector()));
		connect(actionResetVectorToDefault, SIGNAL(triggered()), this, SLOT(slotResetVector()));
	}
	CommonMyWidgetWrapper::contextMenuEvent(event, menu);
}

void MyLineEdit::resetToDefault()
{
	setText(defaultText);
	emit editingFinished();
}

QString MyLineEdit::getDefaultAsString()
{
	return defaultText;
}

QString MyLineEdit::getFullParameterName()
{
	QString type = GetType(objectName());
	QString parName = parameterName;
	if (type == QString("vect3") || type == QString("vect4") || type == QString("logvect3"))
		parName = parameterName.left(parameterName.length() - 2);
	return parName;
}

void MyLineEdit::slotCopyVector() const
{
	CopyToClipboard();
}

void MyLineEdit::slotPasteVector()
{
	PasteFromClipboard();
}

void MyLineEdit::slotResetVector()
{
	ResetVectorToDefault();
}
